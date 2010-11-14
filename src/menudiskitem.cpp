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

#include <iostream>

#include "menudiskitem.h"

MenuDiskItem::MenuDiskItem(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    setMountStatus(false);
}

MenuDiskItem::MenuDiskItem(QString device, QString name, bool is_mount, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _device = device;
    _name   = name;

    setLabel(_name);
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

void MenuDiskItem::setLabel(QString text)
{
    if (text.isEmpty())
    {
        text = _device;
    }

    text = QString("<a href=\"%1\">%1</a>").arg(text);
    diskLabel->setText(text);
}

void MenuDiskItem::setMountStatus(bool is_mount)
{
    eject->setEnabled(is_mount);
}

void MenuDiskItem::on_eject_clicked()
{
    //std::cout << "Eject: " << qPrintable(_device) << "\n";
    //emit ejectMedia(_device);
    emit ejectMedia(_device);
}

void MenuDiskItem::on_diskLabel_linkActivated(QString /*link*/)
{
    //std::cout << "Link: " << qPrintable(link) << "\n";
    emit mountMedia(_device);
}
