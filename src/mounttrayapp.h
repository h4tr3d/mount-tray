/** =============================================================================================

    This file is a part of "%ProjectName%" project
    http://hatred.homelinux.net

    @date   2010-%MONTH%-%DAY%
    @brief

    Copyright (C) 2010 by hatred <>

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

signals:

public slots:

private slots:
    void onDiskAdded(DiskInfo info);
    void onDiskRemoved(DiskInfo info);

    void onDbusDeviceChangesMessage(QDBusObjectPath device);

    void onMediaMount(QString device);
    void onMediaEject(QString device);

private:
    void initialScanDevices();
    void addMenuItem(QString device, QString name);
    void removeMenuItem(QString device);
    void updateMenuItem(QString device, QString name, bool is_mounted);

    void showMessage(QString text);
    void showError(QString text);

private:
    QSystemTrayIcon                 _tray_icon;
    QMenu                          *_menu;
    QHash<QString, QWidgetAction*>  _menu_items;

    DiskMonitor    _dm;
    StorageManager _sm;

};

#endif // MOUNTTRAYAPP_H
