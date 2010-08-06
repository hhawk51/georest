//
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

#ifndef _c_FdoFilter_h
#define _c_FdoFilter_h

// class c_FdoFilter contains functions to create FDO filter string from property values
//
class REST_REQUEST_HANDLER_API c_FdoFilter
{
public:
  c_FdoFilter(void);
  ~c_FdoFilter(void);
  
public:
  // appends to string FDO filter for properties ( using AND if more then one property)
  static void CreateFdoFilterString(MgPropertyCollection* Props,std::wstring& FdoFilter);
  
  // appends fdo filter representation of property value
  static void Property2FdoFilter(MgProperty* Prop,std::wstring& FdoFilter);
};


#endif