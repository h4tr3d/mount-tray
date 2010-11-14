/** =============================================================================================

    This file is a part of "MountTray" project
    http://hatred.homelinux.net

    @date   2010-06-06
    @brief  Class for iterate with one device: mount, unmount them, take information about
            mount status, mount point

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files

   ============================================================================================== */

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QProcess>

#include "storageitem.h"
#include "mount.h"

StorageItem::StorageItem(DiskInfo info, QObject *parent) :
    QObject(parent)
{
    _udev_info     = info;
    _mount_options = "defaults";
    _is_mounted    = false;

    QStringList mount = ::isMounted(info.device_name, MC_DEVICE);

    // Если примонтирован уже, то точку монтирования назначить на указанную
    if (mount.count() > 0)
    {
        _is_mounted    = true;
        _mount_point = mount[1];
    }
}

StorageItem::~StorageItem()
{
}

QString StorageItem::getMountPoint()
{
    return _mount_point;
}

bool StorageItem::isMounted()
{
    return _is_mounted;
}

void StorageItem::mount()
{
    bool status = ::diskMount(UDISKS, _udev_info.device_name, _mount_point);
    if (status)
    {
        _is_mounted = true;
        if (_mount_point.isEmpty())
        {
            QStringList mounts = ::isMounted(_udev_info.device_name, MC_DEVICE);
            if (mounts.count() > 0)
            {
                _mount_point = mounts.at(1);
            }
            else
            {
                _is_mounted = false;
            }
        }
    }
}

void StorageItem::unmount()
{
    bool status = ::diskUnMount(UDISKS, _udev_info.device_name);
    if (status)
    {
        _is_mounted  = false;
        _mount_point = QString();
    }
}

void StorageItem::setMountStatus(bool is_mounted, QString mount_point)
{
    _is_mounted = is_mounted;

    if (is_mounted)
    {
        if (mount_point.isEmpty())
        {
            QStringList mounts = ::isMounted(_udev_info.device_name, MC_DEVICE);
            if (mounts.count() > 0)
            {
                _mount_point = mounts.at(1);
            }
            else
            {
                // Ибо нефиг говорить, что примонтированы, когда это не так
                _is_mounted = false;
            }
        }
        else
        {
            _mount_point = mount_point;
        }
    }
    else
    {
        _mount_point = QString();
    }
}

