/** =============================================================================================

    This file is a part of "MountTray" project
    http://hatred.homelinux.net

    @date   2010-06-06
    @brief  Widget for context menu for iterate with disks: mount/open/unmount

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files

    ============================================================================================= */

#include <QDebug>

#include <iostream>

#include "menudiskitem.h"

MenuDiskItem::MenuDiskItem(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    setMountStatus(false);
}

MenuDiskItem::MenuDiskItem(const QString &device, const QString &name, bool is_mount, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    iconMounted = QIcon(":/ui/images/eject.png");
    iconUnmounted = QIcon(":/ui/images/inject.png");

    this->device = device;
    this->name   = name;

    setLabel(name);
    setMountStatus(is_mount);
}

void MenuDiskItem::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void MenuDiskItem::setLabel(const QString &text)
{
    QString label = text;
    if (label.isEmpty())
    {
        label = device;
    }

    label = QString("<a href=\"%1\">%1</a>").arg(label);
    diskLabel->setText(label);
}

void MenuDiskItem::setMountStatus(bool is_mount)
{
    isMountedFlag = is_mount;
    if (isMountedFlag)
    {
        eject->setIcon(iconMounted);
    }
    else
    {
        eject->setIcon(iconUnmounted);
    }
}

void MenuDiskItem::setIconMounted(const QIcon &icon)
{
    iconMounted = icon;
}

void MenuDiskItem::setIconUnmounted(const QIcon &icon)
{
    iconUnmounted = icon;
}

void MenuDiskItem::on_eject_clicked()
{
    if (isMountedFlag)
        emit ejectMedia(device);
    else
        emit mountMedia(device);
}

void MenuDiskItem::on_diskLabel_linkActivated(QString /*link*/)
{
    emit openMedia(device);
}
