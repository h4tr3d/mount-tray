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

#include <QDebug>
#include <QProcess>

#include "mount.h"

static bool mountClassic(const QString &device, QString &mount_point, QString &status, const QString &fs, const QString &options)
{
    Q_UNUSED(device);
    Q_UNUSED(mount_point);
    Q_UNUSED(status);
    Q_UNUSED(fs);
    Q_UNUSED(options);

    // TODO
    return false;
}

static bool unmountClassic(const QString &device, QString &status)
{
    Q_UNUSED(device);
    Q_UNUSED(status);
    // TODO
    return false;
}

static bool mountUdisks(const QString &device, QString &mount_point, QString &status, const QString &fs, const QString &options)
{
    QProcess    mount;
    QString     command = "udisks";
    QStringList args;

    mount.setProcessChannelMode(QProcess::MergedChannels);

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
        status = "Trouble with mount: start issue";
        return false;
    }

    if (!mount.waitForFinished())
    {
        status = "Trouble with mount: finish issue";
        return false;
    }

    //int code       = mount.exitCode();
    bool is_ok     = false;
    QString buffer = mount.readAll();


    // Stupid 'udisks' in any cases return 0 exit status!
    if (buffer.contains(QRegExp("^Mount failed:", Qt::CaseInsensitive)) == false)
    {
        is_ok = true;
    }

    QRegExp reg("^Mounted (.+) at (.+)$", Qt::CaseSensitive);
    if (is_ok && reg.exactMatch(buffer))
    {
        mount_point = reg.cap(2).trimmed();
        std::cout << "Mount point: " << mount_point.toStdString() << std::endl;
        status = "Ok";
    }
    else
    {
        std::cout << "mount dir can't be match" << std::endl;
        status = buffer;
    }

    return is_ok;
}

static bool unmountUdisks(const QString &device, QString &status)
{
    QProcess    unmount;
    QString     command = "udisks";
    QStringList args;

    unmount.setProcessChannelMode(QProcess::MergedChannels);

    args << "--unmount" << device;

    unmount.start(command, args);
    if (!unmount.waitForStarted())
    {
        status = "Trouble with unmount: start issue";
        return false;
    }

    if (!unmount.waitForFinished())
    {
        status = "Trouble with unmount: finish issue";
        return false;
    }

    //int code       = unmount.exitCode();
    bool is_ok     = false;
    QString buffer = unmount.readAll();


    // Stupid 'udisks' in any cases return 0 exit status!
    if (buffer.contains(QRegExp("^Unmount failed:", Qt::CaseInsensitive)) == false)
    {
        is_ok = true;
    }

    if (is_ok == true)
    {
        status = "Ok";
    }
    else
    {
        status = buffer;
    }

    return is_ok;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


bool diskMount(MountingType  type,
               const QString      &device,
               QString            &mount_point,
               QString            &status,
               const QString      &fs,
               const QString      &options )
{
    bool result = false;
    switch (type)
    {
        case CLASSIC:
        {
            result = mountClassic(device, mount_point, status, fs, options);
            break;
        }

        case UDISKS:
        {
            result = mountUdisks(device, mount_point, status, fs, options);
            break;
        }
    }

    return result;
}

bool diskUnMount(MountingType type, const QString &device, QString &status)
{
    bool result = false;
    switch (type)
    {
        case CLASSIC:
        {
            result = unmountClassic(device, status);
            break;
        }

        case UDISKS:
        {
            result = unmountUdisks(device, status);
        }
    }

    return result;
}

QString &replaceOctalEscapes(QString &text)
{
    QRegExp oct("\\\\0([0-9]{1,})");
    QMap<QString, int> replaceMap;

    qDebug() << "Input string: " << text;

    int pos = 0;
    while ((pos = oct.indexIn(text, pos)) != -1)
    {
        QString octalValueStr = oct.cap(1);
        pos += oct.matchedLength();
        int octalValue = octalValueStr.toInt(0, 8);

        replaceMap.insert(QString("\\0%1").arg(octalValue, 0, 8), octalValue);
    }

    QMap<QString, int>::iterator i;
    for (i = replaceMap.begin(); i != replaceMap.end(); ++i)
    {
        text.replace(i.key(), QString("%1").arg(QChar(i.value())));
    }

    qDebug() << "Output string: " << text;

    return text;
}

QStringList isMounted(const QString &name, MountCheck check)
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

        QStringList::iterator it;
        for (it = parts.begin(); it != parts.end(); ++it)
        {
            (*it) = replaceOctalEscapes(*it);
        }

        switch(check)
        {
            case MC_DEVICE:
                value = parts[0];
                break;

            case MC_DIR:
                value = parts[1];
                break;
        }

        value = replaceOctalEscapes(value);

        if (value == name)
        {
            return_value = parts;
            break;
        }
    }

    return return_value;
}
