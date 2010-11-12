/** =============================================================================================

    This file is a part of "%ProjectName%" project
    http://hatred.homelinux.net

    @date   2010-11-12
    @brief

    Copyright (C) 2010 by hatred <>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files

   ============================================================================================== */

#ifndef MENUDISKITEM_H
#define MENUDISKITEM_H

#include "ui_menudiskitem.h"

class MenuDiskItem : public QWidget, private Ui::MenuDiskItem
{
    Q_OBJECT

public:
    explicit MenuDiskItem(QWidget *parent = 0);
    explicit MenuDiskItem(QString device, QString name = "", bool is_mount = false, QWidget *parent = 0);

    void     setLabel(QString text);
    void     setMountStatus(bool is_mount);

signals:
    void ejectMedia(QString media_dev);
    void mountMedia(QString media_dev);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_eject_clicked();
    void on_diskLabel_linkActivated(QString link);

private:
    QString _device;
    QString _name;
};

#endif // MENUDISKITEM_H
