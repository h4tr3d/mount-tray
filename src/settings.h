/** =============================================================================================
  
    This file is a part of "Mount Tray" project
    http://htrd.su
    
    @date   2013-07-19
    @brief
    
    Copyright (C) 2002-2013 by Alexander 'hatred' Drozdov <adrozdoff@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.
    
    For more information see LICENSE and LICENSE.ru files
    
   ============================================================================================== */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{
public:
    Settings();

    QSettings* operator->();

    operator QSettings&();

private:
    QSettings _settings;
};

#endif // SETTINGS_H
