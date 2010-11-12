/**************************************************************************
**   Copyright (C) 2010 by hatred
**   hatred@inbox.ru
**   http://hatred.homelinux.net
**
**   This file is a part of "%ProjectName%" application
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the version 2 of GNU General Public License as
**   published by the Free Software Foundation.
**
**   For more information see LICENSE and LICENSE.ru files
**
**   @file   %FileName%
**   @date   %DATE%
**   @author hatred
**   @brief
**
**************************************************************************/

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

//    // Проверить, есть ли метра, если есть, назначить точку по ней
//    if (tmp_point.isEmpty() && !info.file_system_label.isEmpty())
//    {
//        tmp_point = QString("/media/") + info.file_system_label;
//    }

//    // Если ничего не подошло, использовать стандартную точку
//    if (tmp_point.isEmpty())
//    {
//        tmp_point = QString("/media/disk");
//    }

//    // Проверить, подмонтировано ли в эту точку, если да, сделать нумерные
//    if (!tmp_point.isEmpty() && ::isMounted(tmp_point, MC_DIR).count() > 0)
//    {
//        for (int i = 1; ; i++)
//        {
//            QString new_point = tmp_point + QString("-%1").arg(i);
//            if (::isMounted(new_point, MC_DIR).count() == 0)
//            {
//                tmp_point = new_point;
//                break;
//            }
//        }
//    }

//    _mount_point = tmp_point;
}

StorageItem::~StorageItem()
{
    // TODO: must be created!!!
    //if (_is_mounted)
    //{
    //    unmount();
    //}

    //if (_sudo)
    //{
    //    while (_sudo->isRunning()){sleep(1);}
    //}
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

//void StorageItem::runCommand(RunCommand command)
//{
//    if (_sudo->isRunning())
//    {
//        _sudo->stop();
//    }

//    QString     cmd;
//    QStringList args;

//    switch (command)
//    {
//        case RC_MOUNT:
//            cmd  = _mount_cmd;

//            args << _udev_info.device_name
//                 << _mount_point;

//            if (!_mount_options.isEmpty())
//            {
//                args << "-o" << _mount_options;
//            }

//            break;
//        case RC_UNMOUNT:
//            cmd  = _unmount_cmd;
//            args << _mount_point;
//            break;
//    }

//    _sudo->start(cmd, args);
//}

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

