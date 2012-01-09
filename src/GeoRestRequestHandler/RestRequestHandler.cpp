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

// RestRequestHandler.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "Poco\UnicodeConverter.h"
#include "Poco\Path.h"


wchar_t g_AppFileName[MAX_PATH+2];
string g_HtmlTemplatePath;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
  BOOL fReturn = TRUE;

  switch (ul_reason_for_call) 
  {

    case DLL_PROCESS_ATTACH :
    {
      wchar_t buff[MAX_PATH+2];
      GetModuleFileNameW ((HINSTANCE)hModule , buff, MAX_PATH);    
      if( wcsncmp(buff,L"\\\\?\\",4) == 0 )
      {
        wcscpy(g_AppFileName,&buff[4]);
      }
      else
      {
        wcscpy(g_AppFileName,buff);
      }
      
      std::string s1;
      Poco::UnicodeConverter::toUTF8(g_AppFileName,s1);
      
      Poco::Path dllpath(s1);  
      dllpath.setFileName("");
      dllpath.pushDirectory("HtmlTemplates");
      g_HtmlTemplatePath = dllpath.toString();
      
      
      ACE::init();
    }
    break;
    case DLL_PROCESS_DETACH:
    {
      ACE::fini();
    }
    break;
  }

  return fReturn;
    
}



