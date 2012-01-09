//
//  Copyright (C) 2009 by  Haris Kurtagic  www.sl-king.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
#ifndef _RestConfigInclude_h
#define _RestConfigInclude_h

#ifdef _WIN32
#ifdef RESTSERVICECONFIG_EXPORTS
#    define REST_CONFIG_API __declspec(dllexport)
#else
#    define REST_CONFIG_API __declspec(dllimport)
#endif
#else
#define REST_CONFIG_API
#endif


enum e_REST_AccessCodes
{
  e_Unknown, // 
  e_AccessOK,
  e_NoAccess_Format,   // format (html,xml,json,..) is not allowed
  e_NoAccess_FormatMethod, // format is allowed but not the http method on that format
  e_NoAccess_FormatMethodUser, // format and method are allowed but not for that user
};


#include "c_RestConfig.h"

#endif