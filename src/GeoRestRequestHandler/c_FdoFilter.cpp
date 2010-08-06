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
#include "stdafx.h"

#include "c_FdoFilter.h"
#include "Poco\NumberFormatter.h"

c_FdoFilter::c_FdoFilter(void)
{
}

c_FdoFilter::~c_FdoFilter(void)
{
}


void c_FdoFilter::Property2FdoFilter(MgProperty* Prop,std::wstring& FdoFilter)
{
  switch(Prop->GetPropertyType())
  {
    case MgPropertyType::Int16:
    {
      wchar_t buff[64];
      wsprintf(buff,L"%d",((MgInt16Property*)Prop)->GetValue());
      FdoFilter.append(buff);
    }
    break;
    case MgPropertyType::Int32:
    {
      wchar_t buff[64];
      wsprintf(buff,L"%ld",((MgInt32Property*)Prop)->GetValue());
      FdoFilter.append(buff);
    }
    break;
    case MgPropertyType::Int64:
    {
      wchar_t buff[64];
      wsprintf(buff,L"%I64d",((MgInt64Property*)Prop)->GetValue());
      FdoFilter.append(buff);
    }
    break;
    case MgPropertyType::Single:
    {
      wchar_t buff[64];
      wsprintf(buff,L"%f",((MgSingleProperty*)Prop)->GetValue());
      FdoFilter.append(buff);
    }
    break;
    case MgPropertyType::Double:
    {
      wchar_t buff[64];
      wsprintf(buff,L"%lf",((MgDoubleProperty*)Prop)->GetValue());
      FdoFilter.append(buff);
    }
    break;
    case MgPropertyType::String:
    {
      FdoFilter.append(L"'");
      FdoFilter.append(((MgStringProperty*)Prop)->GetValue());
      FdoFilter.append(L"'");
    }
    break;
    case MgPropertyType::DateTime:
    {
      MgDateTimeProperty *dt_prop = (MgDateTimeProperty *)Prop;
      Ptr<MgDateTime> dt = dt_prop->GetValue();
      FdoFilter.append(dt->ToString());      
    }
    break;
  }
}

void c_FdoFilter::CreateFdoFilterString( MgPropertyCollection* Props,std::wstring& FdoFilter )
{
  Ptr<MgProperty> prop;
  int count=Props->GetCount();
  for(int ind=0;ind<count;ind++)
  {
    prop = Props->GetItem(ind);
    FdoFilter.append(prop->GetName());
    FdoFilter.append(L"=");
    Property2FdoFilter(prop,FdoFilter);
  }
}