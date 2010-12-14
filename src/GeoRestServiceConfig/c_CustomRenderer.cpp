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
#include <iostream>
#include "c_CustomRenderer.h"
#include "Poco\UnicodeConverter.h"



c_CustomRendererVector g_CustomRendererVector;

c_CustomRenderer::c_CustomRenderer(const wchar_t* Name)
{
  m_Name = Name;  
  m_FuncHeader2Stream=NULL;
  m_FuncContent2Stream=NULL;
}

c_CustomRenderer::~c_CustomRenderer(void)
{
  
}

void c_CustomRenderer::LoadLibrary(const wchar_t* Library)
{
  HMODULE providerLibrary = NULL;
  std::wstring libraryLocation;
  
  providerLibrary = LoadLibraryW(Library);
  
  if (providerLibrary == NULL) 
  {
    wstring reason;
    LPVOID lpMsgBuf;
    wchar_t szBuf[1024];
    DWORD dw = GetLastError();
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &lpMsgBuf, 0, NULL);
    swprintf(szBuf, sizeof(szBuf), L"%hs", lpMsgBuf);
    reason = szBuf;
    LocalFree(lpMsgBuf);
    
    wstring errstr;
    errstr.append(L"Unable to load library: ");
    errstr.append(Library);

    std::wcout << std::endl << errstr << L" Reason:" << reason;

    throw c_ExceptionRestConfig(errstr.c_str());
  }
  
  
  m_FuncHeader2Stream = (FuncHeader2Stream) GetProcAddress(providerLibrary, "Header2Stream");
  m_FuncContent2Stream = (FuncContent2Stream) GetProcAddress(providerLibrary, "Content2Stream");
  
  if( !m_FuncHeader2Stream || !m_FuncContent2Stream )
  {
    wstring errstr;
    errstr.append(L"Missing entry functions in library: ");
    errstr.append(Library);
    
    std::wcout << std::endl << errstr; 
    
    throw c_ExceptionRestConfig(errstr.c_str());
  }
  
}




c_CustomRendererVector::~c_CustomRendererVector( void )
{
  Clear();
}

void c_CustomRendererVector::Clear()
{
  t_CustomRendererVector::iterator iter;
  for ( iter = m_CustomRendererVector.begin( ) ; iter != m_CustomRendererVector.end( ) ; iter++ )
  {
    c_CustomRenderer * datalayer = *iter;
    delete datalayer;
  }
  m_CustomRendererVector.clear();
}

c_CustomRenderer* c_CustomRendererVector::FindName( const std::wstring& Name ) 
{
  t_CustomRendererVector::iterator iter;
  for ( iter = m_CustomRendererVector.begin( ) ; iter != m_CustomRendererVector.end( ) ; iter++ )
  {
    if( wcsicmp(Name.c_str(),(*iter)->m_Name.c_str()) == 0 ) return *iter;
  }
  
  return NULL;
}


bool c_CustomRendererVector::Add( c_CustomRenderer* DataLayer )
{
  if( FindName(DataLayer->m_Name.c_str()) )
  {
    return false;
  }
  
  m_CustomRendererVector.push_back(DataLayer);
  return true;
}

int c_CustomRendererVector::GetCount()
{
  return m_CustomRendererVector.size();
}

const c_CustomRenderer* c_CustomRendererVector::GetRenderer( int Index )
{
  return m_CustomRendererVector.at(Index);
}

c_CustomRendererVector* c_CustomRendererVector::GetInstance()
{
  return &g_CustomRendererVector;
}
