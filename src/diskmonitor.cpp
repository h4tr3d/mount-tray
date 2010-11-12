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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#include <iostream>
#include <QStringList>
#include <QFileInfo>
#include <QMetaType>

#include "diskmonitor.h"

DiskMonitor::DiskMonitor(QObject *parent) :
    QThread(parent)
{
    _udev    = NULL;
    _monitor = NULL;

    qRegisterMetaType<DiskInfo>("DiskInfo");

    init();
}

DiskMonitor::~DiskMonitor()
{
    udev_monitor_unref(_monitor);
    udev_unref(_udev);
}

void DiskMonitor::init()
{
    int status = -1;

    if (_udev == NULL)
        _udev    = udev_new();

    if (_udev == NULL)
    {
        std::cout << "Can't init udev" << std::endl;
        return;
    }

    if (_monitor == NULL)
    {
        _monitor = udev_monitor_new_from_netlink(_udev, "udev");
        if (_monitor == NULL)
        {
            std::cout << "Can't init monitor" << std::endl;
            return;
        }

        status = udev_monitor_filter_add_match_subsystem_devtype(_monitor, "block", NULL);
        if (status < 0)
        {
            std::cout << "Can't add filter for block" << std::endl;
        }
    }
}

void DiskMonitor::run()
{
    std::cout << "Disk Monitor started" << std::endl;

    int    enable_status = -1;
    fd_set read_fds;

    while(1)
    {
        int                 fd_count;
        int                 monitor_fd;
        struct udev_device *device;

        if (_udev == NULL || _monitor == NULL)
        {
            std::cout << "Monitor is not initialized... try init...\n";
            init();
            continue;
        }

        if (enable_status == -1)
        {
            enable_status = udev_monitor_enable_receiving(_monitor);
            if (enable_status == -1)
            {
                std::cout << "Can't enable receiving... try later...\n";
            }
            continue;
        }

        monitor_fd = udev_monitor_get_fd(_monitor);
        FD_ZERO(&read_fds);
        FD_SET(monitor_fd, &read_fds);

        fd_count = select(monitor_fd + 1, &read_fds, NULL, NULL, NULL);
        if (fd_count < 0)
        {
            continue;
        }

        if (!FD_ISSET(monitor_fd, &read_fds))
        {
            continue;
        }

        device = udev_monitor_receive_device(_monitor);
        if (device == NULL)
        {
            continue;
        }

        QString action  = udev_device_get_action(device);
        QString devnode = udev_device_get_devnode(device);

        fillDiskInfo(device);

        if (_disk_info.raw_info["DEVTYPE"] == "partition")
        {
            //std::cout << action.toAscii().data() << ": "
            //          << devnode.toAscii().data() << std::endl;

            if (action == "add")
            {
                // emit signal
                emit deviceConnected(_disk_info);
            }
            else if (action == "remove")
            {
                // emit signal
                emit deviceDisconnected(_disk_info);
            }
        }

        udev_device_unref(device);
    }
}

void DiskMonitor::fillDiskInfo(struct udev_device *device)
{
    fillDiskInfo(device, _disk_info);
}

void DiskMonitor::fillDiskInfo(struct udev_device *device, DiskInfo &info)
{
    struct udev_list_entry *property_list = NULL;
    struct udev_list_entry *entry         = NULL;

    info.raw_info.clear();

    property_list = udev_device_get_properties_list_entry(device);
    udev_list_entry_foreach(entry, property_list)
    {
        QString property_name  = udev_list_entry_get_name(entry);
        QString property_value = udev_device_get_property_value(device,
                                                                property_name.toAscii().data());

        info.raw_info[property_name] = property_value;
    }

    info.device_name       = info.raw_info["DEVNAME"];
    info.model_name        = info.raw_info["ID_MODEL"];
    info.file_system_type  = info.raw_info["ID_FS_TYPE"];
    info.file_system_label = info.raw_info["ID_FS_LABEL"];
    info.file_system_size  = info.raw_info["UDISKS_PARTITION_SIZE"];

    if (!info.file_system_label.isEmpty())
    {
        info.name = info.file_system_label;
    }
    else
    {
        info.name = QString("%1 %2")
                        .arg(info.raw_info["ID_VENDOR"])
                        .arg(info.raw_info["ID_MODEL"]);
    }
}

QList<DiskInfo *> DiskMonitor::scanDevices()
{
    DiskInfo          *disk;
    QList<DiskInfo *>  disks;

    struct udev_enumerate  *enumerate  = udev_enumerate_new(_udev);

    if (enumerate == NULL)
    {
        std::cout << "Can't init enumerate" << std::endl;
        return disks;
    }

    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);

    struct udev_device     *device     = NULL;
    struct udev_list_entry *entryes    = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, entryes)
    {
        device = udev_device_new_from_syspath(_udev, udev_list_entry_get_name(entry));
        if (device == NULL)
        {
            continue;
        }

        disk = new DiskInfo();

        fillDiskInfo(device, *disk);

        if (disk->raw_info["DEVTYPE"] == "partition")
        {
            struct udev_device *parent = udev_device_get_parent(device);
            if (parent == NULL)
            {
                delete disk;
                continue;
            }

            const char *value = udev_device_get_sysattr_value(parent, "removable");
            std::cout << "isRemovable: " << (value != NULL ? value : "unknown") << std::endl;
            if (value != NULL && value[0] == '1')
            {
                const char *name = udev_list_entry_get_name(entry);
                std::cout << "Entry: " << name << std::endl;
                disks.append(disk);
            }
            else
            {
                delete disk;
            }
        }

        udev_device_unref(device);
    }

    udev_enumerate_unref(enumerate);

    return disks;
}

