/** =============================================================================================

    This file is a part of "MountTray" project
    http://hatred.homelinux.net

    @date   2010-11-11
    @brief  Mount and UnMount functions

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files

   ============================================================================================== */

#ifndef MOUNT_H
#define MOUNT_H

#include <QtCore>

/**
 * TBD
 */
enum MountInfoRequestType
{
    MC_DEVICE,
    MC_DIR
};

/**
 * Mounter interface class
 */
struct Mounter
{
    virtual ~Mounter() {}

    virtual bool mount(const QString &device,
                       QString &mount_point,
                       QString &status,
                       const QString &fs,
                       const QString &options) = 0;
    virtual bool unmount(const QString &device, QString &status) = 0;
};

/**
 * Init mounters, this method also load mounters plugins
 */
void initMounters();

/**
 * Mount device with default mount method
 *
 * @param device      device to mount
 * @param mount_point mount point, can be overriden by some mounters.
 * @param status      mounting status text
 * @param fs          optional, filesystem type
 * @param options     optional, mounting options
 * @return
 */
bool diskMount(const QString      &device,
               QString            &mount_point,
               QString            &status,
               const QString      &fs      = QString(),
               const QString      &options = QString());

/**
 * UnMount device with default unmount method
 * @param device  device to unmount
 * @param status  unmount status text
 * @return
 */
bool diskUnMount(const QString      &device,
                 QString            &status);

/**
 * Get mount info (device, mount point, mount options and so on) by device or mount point
 *
 * @param name         device or mount point
 * @param requestType  type of the 'name' argument: mount point or device
 * @return
 */
QStringList getMountInfo(const QString &name, MountInfoRequestType requestType);

#endif // MOUNT_H
