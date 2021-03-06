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

#ifndef MOUNTTRAYAPP_H
#define MOUNTTRAYAPP_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <QtDBus/QtDBus>
#include <QWidgetAction>
#include <QMenu>
#include <QHash>

#include "menudiskitem.h"
#include "diskmonitor.h"
#include "storagemanager.h"

class MountTrayApp : public QApplication
{
    Q_OBJECT
public:
    explicit MountTrayApp(int &argc, char **argv);
    ~MountTrayApp();

private slots:
    void onDiskAdded(DiskInfo info);
    void onDiskRemoved(DiskInfo info);

    void onDbusDeviceChangesMessage(QDBusObjectPath device);

    void onMediaOpen(const QString &device);
    void onMediaMount(const QString &device);
    void onMediaEject(const QString &device);

    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

    void onAbout();

private:
    void initialScanDevices();
    void addMenuItem(const QString &device, const QString &name);
    void removeMenuItem(const QString &device);
    void updateMenuItem(const QString &device, const QString &name, bool is_mounted, MediaType mediaType);

    void showMessage(const QString &text);
    void showError(const QString &text);

private:
    QSystemTrayIcon                 _tray_icon;
    QMenu                          *_main_menu;
    QMenu                          *_disk_menu;
    QHash<QString, QWidgetAction*>  _disk_menu_items;

    DiskMonitor    _dm;
    StorageManager _sm;

};

#endif // MOUNTTRAYAPP_H
