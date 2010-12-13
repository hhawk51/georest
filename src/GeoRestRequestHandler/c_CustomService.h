#//
//  Copyright (C) 2010 by  Haris Kurtagic  www.sl-king.com
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

#ifndef _c_CustomService_h
#define _c_CustomService_h

#include "c_RestHandler.h"
#include "c_RestRequest.h"

class c_CustomService
{
public:
  c_CustomService(void);
  ~c_CustomService(void);
  
  static c_RestHandler* CreateRequestHandler(const wchar_t* UriPart,c_RestRequest* Request);
};


#endif