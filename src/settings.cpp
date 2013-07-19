/** =============================================================================================
  
    This file is a part of "%ProjectName%" project
    http://htrd.su
    
    @date   2013-7-19
    @brief
    
    Copyright (C) 2002-2013 by Alexander 'hatred' Drozdov <adrozdoff@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.
    
    For more information see LICENSE and LICENSE.ru files
    
   ============================================================================================== */

#include <QApplication>

#include "settings.h"

Settings::Settings()
    : _settings(QSettings::IniFormat,
                QSettings::UserScope,
                QApplication::organizationName(),
                QApplication::applicationName())
{
}

QSettings *Settings::operator->()
{
    return &_settings;
}

Settings::operator QSettings &()
{
    return _settings;
}
