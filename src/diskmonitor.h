/** =============================================================================================

    This file is a part of "MountTray" project
    http://hatred.homelinux.net

    @date   2010-06-06
    @brief  DiskMonitor - watch udev and detect adding and removing block devices

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files

   ============================================================================================== */

#ifndef DISKMONITOR_H
#define DISKMONITOR_H

#include <libudev.h>

#include <QThread>
#include <QMap>

typedef QMap<QString, QString> QUdevInfo;

typedef struct _DiskInfo {
    _DiskInfo()
    {
    }

    QString name;               // Unified field for display device name
    QString device_name;        // Full device name with path like '/dev/sda1'
    QString model_name;         // Hardware model like 'ST9160827AS'
    QString file_system_type;   // File system name like 'vfat'
    QString file_system_label;  // Label, if set
    QString file_system_size;   // Only if found udisks entries

    QUdevInfo raw_info;         // Associtive massive of all udev's ops
} DiskInfo;

class DiskMonitor : public QThread
{
    Q_OBJECT
public:
    explicit DiskMonitor(QObject *parent = 0);
    ~DiskMonitor();

    QList<DiskInfo*> scanDevices();

signals:
    void deviceConnected(DiskInfo info);
    void deviceDisconnected(DiskInfo info);

public slots:

private:
    DiskInfo             _disk_info;
    struct udev         *_udev;
    struct udev_monitor *_monitor;

private:
    void run();
    void init();
    void fillDiskInfo(struct udev_device *device);
    void fillDiskInfo(struct udev_device *device, DiskInfo &info);

};

#endif // DISKMONITOR_H
