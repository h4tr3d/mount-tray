/**************************************************************************
**   Copyright (C) 2010 by hatred
**   hatred@inbox.ru
**   http://hatred.homelinux.net
**
**   This file is a part of "Qmnt" application
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the version 2 of GNU General Public License as
**   published by the Free Software Foundation.
**
**   For more information see LICENSE and LICENSE.ru files
**
**   @file   storafemanager.h
**   @date   2010-06-06
**   @author hatred
**   @brief
**
**************************************************************************/

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

    void addDevice(DiskInfo info);
    void removeDevice(DiskInfo info);
    void removeDevice(QString dev_name);

    QList<StorageItem *> getDevices();
    StorageItem         *getDevice(QString dev_name);
    StorageItem         *getDevice(DiskInfo info);

signals:

public slots:

private:
    QMap<QString, StorageItem *> _storage_items;
};

#endif // STORAGEMANAGER_H
