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

#ifndef _c_OData_ValueParser
#define _c_OData_ValueParser


class c_OData_ValueParser
{
public:
  static bool ParseBoolean( std::string& ODataStrVal, bool& Val,bool IsUri = false );
  static bool ParseByte( std::string& ODataStrVal, unsigned char& Val ,bool IsUri = false );
  static bool ParseInt16( std::string& ODataStrVal, short& Val ,bool IsUri = false );
  static bool ParseInt32( std::string& ODataStrVal, int& Val ,bool IsUri = false );
  static bool ParseInt64( std::string& ODataStrVal, INT64& Val ,bool IsUri = false );
  static bool ParseSingle( std::string& ODataStrVal, float& Val ,bool IsUri = false );
  static bool ParseDouble( std::string& ODataStrVal, double& Val ,bool IsUri = false );
  static bool ParseString( std::string& ODataStrVal, std::string& Val,bool IsUri = false );
  static bool ParseDateTime( std::string& ODataStrVal, MgDateTime* Val,bool IsUri = false );
  
  static bool EdmDataType2MgPropertyType(const std::string& TypeVal,int& MGType,int EmptyString=MgPropertyType::String);  
};

#endif