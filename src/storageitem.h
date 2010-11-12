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

#ifndef STORAGEITEM_H
#define STORAGEITEM_H

#include <QObject>

#include "diskmonitor.h"
//#include "qsudo.h"

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

    void   setMountStatus(bool is_mounted, QString mount_point = QString());

    void    mount();
    void    unmount();

signals:
    void deviceMounted(QString device_name);
    void deviceUnmounted(QString device_name);
    void deviceError(QString device_name, QString error_text);

public slots:

private slots:
    //

private:
    //void runCommand(RunCommand command);

private:
    //QSudo    *_sudo;
    DiskInfo  _udev_info;
    QString   _mount_point;
    QString   _mount_options;
    bool      _is_mounted;

    //QString   _mount_cmd;
    //QString   _unmount_cmd;
};

#endif // STORAGEITEM_H
