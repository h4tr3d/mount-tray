/** =============================================================================================

    This file is a part of "MountTray" project
    http://hatred.homelinux.net

    @date   2010-11-11
    @brief  Main application class: integrate all components

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files

   ============================================================================================== */

#include <iostream>

#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

#include "mounttrayapp.h"
#include "version.h"

MountTrayApp::MountTrayApp(int & argc, char ** argv) :
    QApplication(argc, argv)
{
    if (!QDBusConnection::systemBus().isConnected())
    {
        std::cout << "Can't connect to dbus daemon. Some functions will be omited\n";
    }

    setWindowIcon(QIcon(":/ui/images/diskette.png"));
    setQuitOnLastWindowClosed(false);

    // Display disk menu
    _disk_menu            = new QMenu();
    QAction *empty_action = _disk_menu->addAction(tr("Empty"));
    empty_action->setEnabled(false);

    // Exit, config and etc menu
    _main_menu = new QMenu();
    _main_menu->addAction(tr("About"), this, SLOT(onAbout()));
    _main_menu->addSeparator();
    _main_menu->addAction(tr("Exit"), this, SLOT(quit()));

    _tray_icon.setIcon(QIcon(":/ui/images/diskette.png"));
    _tray_icon.show();
    _tray_icon.setContextMenu(_main_menu);

    connect(&_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,        SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));

    QDBusConnection conn = QDBusConnection::systemBus();
    // TODO: Check for connection, timer for reconect
    /*bool connected =*/ conn.connect("org.freedesktop.UDisks",
                                  "/org/freedesktop/UDisks",
                                  "org.freedesktop.UDisks",
                                  "DeviceChanged",
                                  this,
                                  SLOT(onDbusDeviceChangesMessage(QDBusObjectPath)));

    // Scan already connected devices
    initialScanDevices();

    connect(&_dm, SIGNAL(deviceConnected(DiskInfo)),
            this, SLOT(onDiskAdded(DiskInfo)));
    connect(&_dm, SIGNAL(deviceDisconnected(DiskInfo)),
            this, SLOT(onDiskRemoved(DiskInfo)));
    _dm.start();
}

MountTrayApp::~MountTrayApp()
{
    _dm.exit(0);
    _dm.wait(1000);
    _dm.terminate();
    _dm.wait();
}

void MountTrayApp::initialScanDevices()
{
    QList<DiskInfo*> devices = _dm.scanDevices();
    for (int i = 0; i < devices.count(); i++)
    {
        DiskInfo *disk = devices.at(i);

        // add device
        _sm.addDevice(*disk);
        addMenuItem(disk->device_name, disk->name);

        StorageItem *sitem = _sm.getDevice(*disk);
        updateMenuItem(disk->device_name, disk->file_system_label, sitem->isMounted());

        // clear
        delete disk;
    }
}

void MountTrayApp::addMenuItem(QString device, QString name)
{
    MenuDiskItem  *item   = new MenuDiskItem(device, name);
    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(item);

    if (_disk_menu_items.count() == 0)
    {
        // Clear 'Empty' item
        _disk_menu->clear();
        _disk_menu->addAction(action);
    }
    else
    {
        // Insert on Top
        _disk_menu->insertAction(_disk_menu->actions().at(0), action);
    }

    _disk_menu_items.insert(device, action);

    // Connect signals
    connect(item, SIGNAL(mountMedia(QString)),
            this, SLOT(onMediaMount(QString)));

    connect(item, SIGNAL(ejectMedia(QString)),
            this, SLOT(onMediaEject(QString)));
}

void MountTrayApp::removeMenuItem(QString device)
{
    QWidgetAction *action = 0;
    if (_disk_menu_items.contains(device))
    {
        action = _disk_menu_items[device];
        _disk_menu->removeAction(action);
        _disk_menu_items.remove(device);
        delete action;
    }

    if (_disk_menu_items.count() == 0)
    {
        QAction *empty_action = _disk_menu->addAction(tr("Empty"));
        empty_action->setEnabled(false);
    }
}

void MountTrayApp::updateMenuItem(QString device, QString name, bool is_mounted)
{
    QWidgetAction *action = 0;
    if (_disk_menu_items.contains(device))
    {
        action = _disk_menu_items[device];
        MenuDiskItem *item = static_cast<MenuDiskItem*>(action->defaultWidget());

        if (item == 0)
        {
            return;
        }

        if (!name.isEmpty())
        {
            item->setLabel(name);
        }

        item->setMountStatus(is_mounted);
    }
}

void MountTrayApp::showMessage(QString text)
{
    _tray_icon.showMessage("MountTray", text);
}

void MountTrayApp::showError(QString text)
{
    _tray_icon.showMessage("MountTray Error", text, QSystemTrayIcon::Critical);
}

/**************************************************************************************************/
/* Signals ---------------------------------------------------------------------------------------*/
/**************************************************************************************************/

void MountTrayApp::onDiskAdded(DiskInfo info)
{
    _sm.addDevice(info);
    addMenuItem(info.device_name, info.name);
    showMessage(tr("Device connected: %1").arg(info.device_name));
}

void MountTrayApp::onDiskRemoved(DiskInfo info)
{
    _sm.removeDevice(info);
    removeMenuItem(info.device_name);
    showMessage(tr("Device removed: %1").arg(info.device_name));
}

void MountTrayApp::onDbusDeviceChangesMessage(QDBusObjectPath device)
{
    QString path = device.path();
    std::cout << "Changed: " << qPrintable(path) << std::endl;

    QDBusInterface iface("org.freedesktop.UDisks",
                         path,
                         "org.freedesktop.UDisks.Device",
                         QDBusConnection::systemBus());

    //qDebug() << "Device name: " << iface.property("DeviceFile");
    //qDebug() << "Is mounted:  " << iface.property("DeviceIsMounted");

    QString dev_name   = iface.property("DeviceFile").toString();
    bool    is_mounted = iface.property("DeviceIsMounted").toBool();

    StorageItem *item = _sm.getDevice(dev_name);
    if (item == NULL)
    {
        return;
    }

    bool old_state = item->isMounted();
    item->setMountStatus(is_mounted);
    updateMenuItem(dev_name, QString(), item->isMounted());

    if (item->isMounted() != old_state)
    {
        if (item->isMounted())
        {
            QString mount_point = item->getMountPoint();
            showMessage(tr("Device '%1' is mounted to %2").arg(dev_name).arg(mount_point));
        }
        else
        {
            showMessage(tr("Device '%1' is unmounted\nNow you can eject USB Flash or CD/DVD Disk").arg(dev_name));
        }
    }
}

void MountTrayApp::onMediaMount(QString device)
{
    std::cout << "Mount media: " << qPrintable(device) << "\n";
    _disk_menu->hide();

    StorageItem *item = _sm.getDevice(device);
    if (item == NULL)
    {
        return;
    }

    bool    old_state   = item->isMounted();
    QString status_text;
    if (!item->isMounted())
    {
        item->mount(status_text);
    }

    QString mount_point = item->getMountPoint();

    if (!item->isMounted())
    {
        // Error
        showError(tr("Can't mount device: %1\n%2").arg(device).arg(status_text));
        return;
    }

    if (item->isMounted() != old_state)
    {
        showMessage(tr("Device '%1' is mounted to %2").arg(device).arg(mount_point));
        updateMenuItem(device, "", true);
    }

    // Run manager
    QDesktopServices::openUrl(QUrl(mount_point));

}

void MountTrayApp::onMediaEject(QString device)
{
    std::cout << "UnMount media: " << qPrintable(device) << "\n";
    _disk_menu->hide();

    StorageItem *item = _sm.getDevice(device);
    if (item == NULL)
    {
        return;
    }

    QString status_text;
    if (item->isMounted())
    {
        item->unmount(status_text);
    }

    if (item->isMounted())
    {
        // Error
        showError(tr("Can't unmount device: %1\n%2").arg(device).arg(status_text));
        return;
    }

    showMessage(tr("Device '%1' is unmounted").arg(device));
    updateMenuItem(device, "", false);
}

void MountTrayApp::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        {
            _disk_menu->popup(QCursor::pos());
            break;
        }

        default:
        {
            return;
        }
    }
}

void MountTrayApp::onAbout()
{
    int     develop_begin = 2010;
    QDate   date          = QDate::currentDate();
    QString date_text     = QString("%1").arg(develop_begin);

    if (date.year() > develop_begin)
    {
        date_text += QString("-%1").arg(date.year());
    }

    QMessageBox::about(0, tr("About MountTray"),
                       tr("<h2>About MountTray</h2>"
                          "<p>Version: " APP_VERSION_FULL "</p>"
                          "<p>Application for mounting and unmounting removable storages via system tray using udisks</p>\n"
                          "<p>This programm distributed under GPLv2 terms. Full license text "
                          "see in LICENSE file</p>\n"
                          "<p>Site: <a href=http://gitorious.org/h4tr3d-utils/pages/MountTray>http://gitorious.org/h4tr3d-utils/pages/MountTray</a></p>\n"
                          "<p>Copyright %1 Alexander 'hatred' Drozdov</p>"
                          "<p><strong>Authors:</strong></p>"
                          "<p>Alexander Drozdov - idea, programming</p>")
                          .arg(date_text));
}

