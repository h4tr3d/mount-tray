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

   ============================================================================================== */

#ifndef MENUDISKITEM_H
#define MENUDISKITEM_H

#include "ui_menudiskitem.h"

class MenuDiskItem : public QWidget, private Ui::MenuDiskItem
{
    Q_OBJECT

public:
    explicit MenuDiskItem(QWidget *parent = 0);
    explicit MenuDiskItem(const QString &device, const QString &name = "", bool is_mount = false, QWidget *parent = 0);

    void     setLabel(const QString &text);
    void     setMountStatus(bool is_mount);
    void     setIconMounted(const QIcon &icon);
    void     setIconUnmounted(const QIcon &icon);
    void     setMediaIcon(const QIcon &icon = QIcon());

signals:
    void ejectMedia(QString media_dev);
    void openMedia(QString media_dev);
    void mountMedia(QString media_dev);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_eject_clicked();
    void on_diskLabel_linkActivated(QString link);

private:
    QString device;
    QString name;
    QIcon   iconMounted;
    QIcon   iconUnmounted;
    QIcon   diskIcon;
    bool    isMountedFlag;
};

#endif // MENUDISKITEM_H
