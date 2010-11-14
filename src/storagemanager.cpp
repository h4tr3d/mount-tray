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

#include "storagemanager.h"

StorageManager::StorageManager(QObject *parent) :
    QObject(parent)
{
}

void StorageManager::addDevice(DiskInfo info)
{
    StorageItem *item = new StorageItem(info);
    if (item == NULL)
        return;

    // TODO: additional settings
    QString name = info.device_name;

    _storage_items.insert(name, item);
}

void StorageManager::removeDevice(DiskInfo info)
{
    QString name = info.device_name;
    removeDevice(name);
}

void StorageManager::removeDevice(QString dev_name)
{
    StorageItem *item = getDevice(dev_name);

    if (item == NULL)
        return;

    _storage_items.remove(dev_name);

    // All needed operations like unmount should be made in dtor
    delete item;
}

QList<StorageItem *> StorageManager::getDevices()
{
    return _storage_items.values();
}

StorageItem * StorageManager::getDevice(QString dev_name)
{
    StorageItem *item = 0;

    if (_storage_items.contains(dev_name))
    {
        item = _storage_items[dev_name];
    }

    return item;
}

StorageItem * StorageManager::getDevice(DiskInfo info)
{
    return getDevice(info.device_name);
}
