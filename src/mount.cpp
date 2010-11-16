/** =============================================================================================

    This file is a part of "%ProjectName%" project
    http://hatred.homelinux.net

    @date   2010-11-10
    @brief  Mount and UnMount functions

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files

    ============================================================================================= */

#include <iostream>

#include <QProcess>

#include "mount.h"

static bool mountClassic(const QString &device, QString &mount_point, const QString &fs, const QString &options)
{
    Q_UNUSED(device);
    Q_UNUSED(mount_point);
    Q_UNUSED(fs);
    Q_UNUSED(options);

    // TODO
    return false;
}

static bool unmountClassic(const QString &device)
{
    Q_UNUSED(device);
    // TODO
    return false;
}

static bool mountUdisks(const QString &device, QString &mount_point, const QString &fs, const QString &options)
{
    QProcess    mount;
    QString     command = "udisks";
    QStringList args;

    args << "--mount";

    if (!fs.isEmpty())
    {
        args << "--mount-fstype" << fs;
    }

    if (!options.isEmpty())
    {
        args << "--mount-options" << options;
    }

    args << device;

    mount.start(command, args);
    if (!mount.waitForStarted())
    {
        return false;
    }

    if (!mount.waitForFinished())
    {
        return false;
    }

    QString buffer = mount.readAll();
    QStringList list = buffer.trimmed().split(" ", QString::SkipEmptyParts);
    if (list.count() == 4)
    {
        mount_point = list.at(3);
    }

    int code = mount.exitCode();
    return (code == 0 ? true : false);
}

static bool unmountUdisks(QString &device)
{
    QProcess    unmount;
    QString     command = "udisks";
    QStringList args;

    args << "--unmount" << device;

    unmount.start(command, args);
    if (!unmount.waitForStarted())
    {
        std::cout << "Trouble with unmount: start issue" << std::endl;
        return false;
    }

    if (!unmount.waitForFinished())
    {
        std::cout << "Trouble with unmount: finish issue" << std::endl;
        return false;
    }

    int code = unmount.exitCode();
    return (code == 0 ? true : false);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


bool diskMount(MountingType  type,
               const QString      &device,
               QString            &mount_point,
               const QString      &fs,
               const QString      &options )
{
    bool result = false;
    switch (type)
    {
        case CLASSIC:
        {
            result = mountClassic(device, mount_point, fs, options);
            break;
        }

        case UDISKS:
        {
            result = mountUdisks(device, mount_point, fs, options);
            break;
        }
    }

    return result;
}

bool diskUnMount(MountingType type, QString &device)
{
    bool result = false;
    switch (type)
    {
        case CLASSIC:
        {
            result = unmountClassic(device);
            break;
        }

        case UDISKS:
        {
            result = unmountUdisks(device);
        }
    }

    return result;
}

QStringList isMounted(QString name, MountCheck check)
{
    QStringList return_value;

    QFile file("/etc/mtab");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return return_value;
    }

    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString     line  = in.readLine();
        QStringList parts = line.split(QRegExp("\\s+"));
        QString     value;

        switch(check)
        {
            case MC_DEVICE:
                value = parts[0];
                break;

            case MC_DIR:
                value = parts[1];
                break;
        }

        if (value == name)
        {
            return_value = parts;
            break;
        }
    }

    return return_value;
}
