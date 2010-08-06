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
#include "StdAfx.h"
#include "c_OData_ValueParser.h"
#include "Poco\DateTimeParser.h"
#include "Poco\DateTime.h"
#include "Poco\DateTimeFormat.h"



bool c_OData_ValueParser::ParseBoolean( std::string& ODataStrVal, bool& Val,bool IsUri /*= false*/ )
{
  
  if( ODataStrVal.compare("true") == 0 ) { Val=true; return true; }
  if( ODataStrVal.compare("false") == 0 ) { Val=false; return true; }
  if( ODataStrVal.compare("1") == 0 ) { Val=true; return true; }
  if( ODataStrVal.compare("0") == 0 ) { Val=false; return true; }
  
  return false;  
}

bool c_OData_ValueParser::ParseByte( std::string& ODataStrVal, unsigned char& Val,bool IsUri /*= false*/ )
{
  
  int endind;
  int ival;
  int nump = scanf_s(ODataStrVal.c_str(),L"%x%n",&ival,&endind);
  if( (nump != 1) || (endind != ODataStrVal.length()) || (Val<0) || (Val>255) )
  {
    return false;
  }
  
  Val = (unsigned char)ival;
  
  return true;;  
}
bool c_OData_ValueParser::ParseInt16( std::string& ODataStrVal, short& Val,bool IsUri /*= false*/ )
{

  int endind;
  int nump = scanf_s(ODataStrVal.c_str(),L"%hd%n",&Val,&endind);
  if( (nump != 1) || (endind != ODataStrVal.length())  )
  {
    return false;
  }

  return true;;  
}

bool c_OData_ValueParser::ParseInt32( std::string& ODataStrVal, int& Val,bool IsUri /*= false*/ )
{

  int endind;
  int nump = scanf_s(ODataStrVal.c_str(),L"%d%n",&Val,&endind);
  if( (nump != 1) || (endind != ODataStrVal.length())  )
  {
    return false;
  }

  return true;;  
}
bool c_OData_ValueParser::ParseInt64( std::string& ODataStrVal, INT64& Val ,bool IsUri /*= false*/)
{

  int endind;
  int nump = scanf_s(ODataStrVal.c_str(),L"%I64dL%n",&Val,&endind);
  if( (nump != 1) || (endind != ODataStrVal.length())  )
  {
    return false;
  }

  return true;;  
}
bool c_OData_ValueParser::ParseSingle( std::string& ODataStrVal, float& Val,bool IsUri /*= false*/ )
{

  int endind;
  int nump;
  if( IsUri )
    nump = scanf_s(ODataStrVal.c_str(),L"%ff%n",&Val,&endind);
  else
    nump = scanf_s(ODataStrVal.c_str(),L"%f%n",&Val,&endind);
  if( (nump != 1) || (endind != ODataStrVal.length())  )
  {
    return false;
  }

  return true;;  
}
bool c_OData_ValueParser::ParseDouble( std::string& ODataStrVal, double& Val,bool IsUri /*= false*/ )
{

  int endind;
  int nump = scanf_s(ODataStrVal.c_str(),L"%lf%n",&Val,&endind);
  if( (nump != 1) || (endind != ODataStrVal.length())  )
  {
    return false;
  }

  return true;;  
}
bool c_OData_ValueParser::ParseString( std::string& ODataStrVal, std::string& Val,bool IsUri /*= false*/ )
{
  if( IsUri )
  {
    int len = ODataStrVal.length();
    if( len < 2 ) return false;

    if( ODataStrVal[0]!='\'' || ODataStrVal[len-1]!='\'') return false;
    
    Val = ODataStrVal.substr(1,len-2);
  }
  
  
  Val = ODataStrVal;

  return true;;  
}
bool c_OData_ValueParser::ParseDateTime( std::string& ODataStrVal, MgDateTime* Val,bool IsUri /*= false*/ )
{
  std::string tmpstr;
  
  if( IsUri )
  {
    int len = ODataStrVal.length();
    if( len < 10 ) return false;

    if( ODataStrVal[len-1]!='\'') return false;

    tmpstr = ODataStrVal.substr(0,9);
    if( tmpstr.compare("datetime'") != 0 ) return false;
    tmpstr = ODataStrVal.substr(9,len-10);
  }
  else
    tmpstr = ODataStrVal;
    
  Poco::DateTime dt;
  int tz;
  
  if( !Poco::DateTimeParser::tryParse(Poco::DateTimeFormat::ISO8601_FORMAT,tmpstr,dt,tz) )
  {
    return false;
  }

  Val->SetYear(dt.year());
  Val->SetMonth(dt.month());
  Val->SetDay(dt.day());
  Val->SetHour(dt.hour());
  Val->SetMinute(dt.minute());
  Val->SetSecond(dt.second());
    
  return true;
  
  
}

bool c_OData_ValueParser::EdmDataType2MgPropertyType( const std::string& TypeStr,int& MGType
        ,int EmptyStringType/*=MgPropertyType::String*/ )
{
  if( TypeStr.length() == 0 ) { MGType=EmptyStringType; return true; }
  
  if( TypeStr.compare("Edm.Int16") == 0 ) { MGType=MgPropertyType::Int16; return true; }
  if( TypeStr.compare("Edm.Int32") == 0 ) { MGType=MgPropertyType::Int32; return true; }
  if( TypeStr.compare("Edm.Int64") == 0 ) { MGType=MgPropertyType::Int64; return true; }
  
  if( TypeStr.compare("Edm.Single") == 0 ) { MGType=MgPropertyType::Single; return true; }
  if( TypeStr.compare("Edm.Double") == 0 ) { MGType=MgPropertyType::Double; return true; }
  
  if( TypeStr.compare("Edm.Boolean") == 0 ) { MGType=MgPropertyType::Boolean; return true; }
  if( TypeStr.compare("Edm.Byte") == 0 ) { MGType=MgPropertyType::Byte; return true; }
  
  if( TypeStr.compare("Edm.DateTime") == 0 ) { MGType=MgPropertyType::DateTime; return true; }
  
  if( TypeStr.compare("Edm.String") == 0 ) { MGType=MgPropertyType::String; return true; }
  
  return false;
}