/** =============================================================================================

    This file is a part of "%ProjectName%" project
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

typedef enum {
    MC_DEVICE,
    MC_DIR
} MountCheck;

typedef enum {
    CLASSIC,
    UDISKS
} MountingType;

bool diskMount(MountingType  type,
               QString      &device,
               QString      &mount_point,
               QString       fs = QString(),
               QString       options = QString());

bool diskUnMount(MountingType  type,
                 QString      &device);

QStringList isMounted(QString name, MountCheck check);

#endif // MOUNT_H
