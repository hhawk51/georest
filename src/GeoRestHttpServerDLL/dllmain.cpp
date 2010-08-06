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
// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Poco\UnicodeConverter.h"
#include "Poco\Path.h"
#include "Poco\File.h"

wchar_t g_AppFileName[MAX_PATH+2];
Poco::Path g_AppFolder;
Poco::Path g_WWWFolder;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
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
    
    std::string buff_utf8;
    Poco::UnicodeConverter::toUTF8(buff,buff_utf8);
    
    g_AppFolder = buff_utf8;
    g_AppFolder.setFileName("");
    
    g_WWWFolder = g_AppFolder;
    g_WWWFolder.popDirectory();
    g_WWWFolder.pushDirectory("www");
    Poco::File f(g_WWWFolder);
    if( !f.exists() || !f.isDirectory() )
    {
      g_WWWFolder = g_AppFolder;
      g_WWWFolder.pushDirectory("www");
    }
  }
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
    
		break;
	}
	return TRUE;
}

