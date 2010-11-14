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

#include <QDesktopServices>
#include <QUrl>

#include "mounttrayapp.h"

MountTrayApp::MountTrayApp(int & argc, char ** argv) :
    QApplication(argc, argv)
{
    if (!QDBusConnection::systemBus().isConnected())
    {
        std::cout << "Can't connect to dbus daemon. Some functions will be omited\n";
    }

    _tray_icon.setIcon(QIcon(":/ui/images/diskette.png"));
    _tray_icon.show();

    _menu = new QMenu();
    _menu->addSeparator();
    _menu->addAction("Exit", this, SLOT(quit()));
    _tray_icon.setContextMenu(_menu);

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

    // Insert on Top
    _menu->insertAction(_menu->actions().at(0), action);
    _menu_items.insert(device, action);

    // Connect signals
    connect(item, SIGNAL(mountMedia(QString)),
            this, SLOT(onMediaMount(QString)));

    connect(item, SIGNAL(ejectMedia(QString)),
            this, SLOT(onMediaEject(QString)));
}

void MountTrayApp::removeMenuItem(QString device)
{
    QWidgetAction *action = 0;
    if (_menu_items.contains(device))
    {
        action = _menu_items[device];
        _menu->removeAction(action);
        _menu_items.remove(device);
        delete action;
    }
}

void MountTrayApp::updateMenuItem(QString device, QString name, bool is_mounted)
{
    QWidgetAction *action = 0;
    if (_menu_items.contains(device))
    {
        action = _menu_items[device];
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
            showMessage(tr("Device '%1' is unmounted").arg(dev_name));
        }
    }
}

void MountTrayApp::onMediaMount(QString device)
{
    std::cout << "Mount media: " << qPrintable(device) << "\n";
    _menu->hide();

    StorageItem *item = _sm.getDevice(device);
    if (item == NULL)
    {
        return;
    }

    bool old_state = item->isMounted();
    if (!item->isMounted())
    {
        item->mount();
    }

    QString mount_point = item->getMountPoint();

    if (!item->isMounted())
    {
        // Error
        showError(tr("Can't mount device: %1").arg(device));
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
    _menu->hide();

    StorageItem *item = _sm.getDevice(device);
    if (item == NULL)
    {
        return;
    }

    if (item->isMounted())
    {
        item->unmount();
    }

    if (item->isMounted())
    {
        // Error
        showError(tr("Can't unmount device: %1").arg(device));
        return;
    }

    showMessage(tr("Device '%1' is unmounted").arg(device));
    updateMenuItem(device, "", false);
}

