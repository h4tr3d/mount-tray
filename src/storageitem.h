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

#ifndef STORAGEITEM_H
#define STORAGEITEM_H

#include <QObject>

#include "diskmonitor.h"

typedef enum {
    RC_MOUNT,
    RC_UNMOUNT
} RunCommand;

class StorageItem : public QObject
{
    Q_OBJECT
public:
    explicit StorageItem(DiskInfo info, QObject *parent = 0);
    ~StorageItem();

    bool    isMounted();
    QString getMountPoint();

    void   setMountStatus(bool is_mounted, const QString &mount_point = QString());

    bool    mount(QString &text_status);
    bool    unmount(QString &text_status);

signals:
    void deviceMounted(QString device_name);
    void deviceUnmounted(QString device_name);
    void deviceError(QString device_name, QString error_text);

private:
    DiskInfo  _udev_info;
    QString   _mount_point;
    QString   _mount_options;
    bool      _is_mounted;
};

#endif // STORAGEITEM_H
