/** =============================================================================================

    This file is a part of "MountTray" project
    http://hatred.homelinux.net

    @date   2010-06-06
    @brief  Store information about all disks

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files

   ============================================================================================== */

#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QObject>
#include <QMap>

#include "storageitem.h"
#include "diskmonitor.h"

class StorageManager : public QObject
{
    Q_OBJECT
public:
    explicit StorageManager(QObject *parent = 0);

    void addDevice(const    DiskInfo &info);
    void removeDevice(const DiskInfo &info);
    void removeDevice(const QString  &dev_name);

    QList<StorageItem *> getDevices();
    StorageItem         *getDevice(const QString  &dev_name);
    StorageItem         *getDevice(const DiskInfo &info);

signals:

public slots:

private:
    QMap<QString, StorageItem *> _storage_items;
};

#endif // STORAGEMANAGER_H
