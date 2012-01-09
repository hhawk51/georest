#include "StdAfx.h"
#include "c_CustomService.h"
#include "..\GeoRestServiceConfig\c_CfgService.h"


c_CustomService::c_CustomService(void)
{
  
}

c_CustomService::~c_CustomService(void)
{
}

c_RestHandler* c_CustomService::CreateRequestHandler( const wchar_t* UriPart,c_RestRequest* Request )
{
  c_CfgService* service = c_RestConfig::GetInstance()->FindService(UriPart);
  if( !service ) return NULL;
  
  switch(service->m_Type)
  {
    case c_CfgService::e_Custom_Service:
    {
      c_CfgCustomService* cservice = (c_CfgCustomService* )service;
      
      if(cservice->m_LibraryLoaded == 0)
      {
        HMODULE providerLibrary = NULL;
        providerLibrary = LoadLibraryW(cservice->m_Library.c_str());
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
          errstr.append(cservice->m_Library);

          std::wcout << std::endl << errstr << L" Reason:" << reason;

          throw c_ExceptionRestConfig(errstr.c_str());
        }
        
        cservice->m_CreateRequestHandler = (t_CreateRequestHandler) GetProcAddress(providerLibrary, "CreateRequestHandler");        

        if( !cservice->m_CreateRequestHandler )
        {
          wstring errstr;
          errstr.append(L"Missing entry functions in library: ");
          errstr.append(cservice->m_Library);

          std::wcout << std::endl << errstr; 

          throw c_ExceptionRestConfig(errstr.c_str());
        }
        
        cservice->m_LibraryLoaded = 1;
      }
      if(cservice->m_LibraryLoaded == 1)
      {
        c_RestHandler* handler = (c_RestHandler*)(*(cservice->m_CreateRequestHandler))(Request);
        
        return handler;
      }
    }
    break;      
  }
  
  return NULL;
}