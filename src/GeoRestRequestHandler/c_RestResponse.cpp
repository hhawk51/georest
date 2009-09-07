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

#include "stdafx.h"
#include "c_RestRequest.h"
#include "c_RestHeader.h"
#include "c_RestResult.h"
#include "c_RestResponse.h"
#include "c_RestPrimitiveValue.h"
#include "RestConfigInclude.h"

#include "MapGuideCommon.h"
//#include "HttpHandler.h"
//#include "HttpPrimitiveValue.h"
//#include "IsapiResponseHandler.h"
#include "MapAgentStrings.h"
#include "XmlJsonConvert.h"

#include "FeatureReaderToGeoJson.h"
#include "FeatureReaderToXml.h"
#include "FeatureReaderToHtml.h"
//#include <stdlib.h>



/// <summary>
/// Constructor. Initialize member variables.
/// </summary>
c_RestResponse::c_RestResponse()
{
    m_header = new c_RestHeader();
    m_result = new c_RestResult();
}

/// <summary>
/// Makes available the pointer to header.
/// User will use header class instance to retrieve
/// header information for a response.
/// </summary>
/// <returns>Pointer to c_RestHeader class</returns>
c_RestHeader* c_RestResponse::GetHeader()
{
    return SAFE_ADDREF((c_RestHeader*)m_header);
}

/// <summary>
/// Makes available the pointer to c_RestResult.
/// User will retrieve the actual contents
/// from this result instance.
/// </summary>
/// <returns>Pointer to c_RestResult or NULL</returns>
c_RestResult* c_RestResponse::GetResult()
{
    return SAFE_ADDREF((c_RestResult*)m_result);
}

void c_RestResponse::Dispose()
{
    delete this;
}

INT32 c_RestResponse::GetClassId()
{
    return 0;
}

c_RestResponse::~c_RestResponse()
{
}

void c_RestResponse::SetJsonpCallbackName( const std::string& val )
{
  m_JsonpCallbackName = val;
  m_ContentType = RestMimeType::JsonP;
}







c_RestResponse_HttpData* c_RestResponse::PrepareHttpData(c_RestRequest* RestRequest)
{
  
try
{


    
  char tempHeader[4096];
  
  //string sResponseHeader;
  Ptr<MgByteReader> outputReader;

  Ptr<c_RestResult> result = GetResult();
  STATUS status = result->GetStatusCode();
  STRING statusMessage = result->GetHttpStatusMessage();
  if (status != 200)
  {
    
    if (statusMessage == MapAgentStrings::FailedAuth1 ||
      statusMessage == MapAgentStrings::FailedAuth2
      )
    {
      RequestAuth(RestRequest);
    }
    else
    {
      //TODO: Use a resource for the HTML error message
      char content[4096];

      STRING shortError = result->GetErrorMessage();
      STRING longError = result->GetDetailedErrorMessage();

      sprintf(content,"\r\n"
        "<html>\n<head>\n"
        "<title>%s</title>\n"
        "<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\">\n"
        "</head>\n"
        "<body>\n<h2>%s</h2>\n%s\n</body>\n</html>\n",
        MG_WCHAR_TO_CHAR(statusMessage),
        MG_WCHAR_TO_CHAR(shortError),
        MG_WCHAR_TO_CHAR(longError));

      DWORD  content_length = strlen(content);

      /*
      sprintf(tempHeader, MapAgentStrings::StatusHeader, status, MG_WCHAR_TO_CHAR(statusMessage));
      sResponseHeader.append(tempHeader);

      sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, (int)content_length);
      sResponseHeader.append(tempHeader);

      sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextHtml, "");
      sResponseHeader.append(tempHeader);
      */
      
      m_HttpData.SetStatusAndReason(status,MG_WCHAR_TO_CHAR(statusMessage));      
      m_HttpData.AddHeader(MapAgentStrings::ContentTypeKey,MapAgentStrings::TextHtml);
      sprintf(tempHeader, "%d", (int)content_length);
      m_HttpData.AddHeader(MapAgentStrings::ContentLengthKey,tempHeader);
      
      
      m_HttpData.SetContent(content);
      




    }
  }
  else
  {

    // Status was ok.  Send the real result back.
    STRING result_contentType = result->GetResultContentType();
    STRING response_contentType = GetContentType();
    std::string stringval_utf8;

    //sprintf(tempHeader, MapAgentStrings::StatusOkHeader);
    //sResponseHeader.append(tempHeader);
    
    m_HttpData.SetStatusAndReason(200,"OK");
    

    if (response_contentType.length() > 0)
    {
      // If we are returning text, state that it is utf-8.
      string charSet = "";
      if (response_contentType.find(L"text") != response_contentType.npos)  //NOXLATE
      {
        charSet = MapAgentStrings::Utf8Text;
      }
      //sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MG_WCHAR_TO_CHAR(response_contentType), charSet.c_str());
      //sResponseHeader.append(tempHeader);
      sprintf(tempHeader, "%s%s", MG_WCHAR_TO_CHAR(response_contentType), charSet.c_str());
      m_HttpData.AddHeader(MapAgentStrings::ContentTypeKey,tempHeader);
    }
    else
    {
      //sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
      //sResponseHeader.append(tempHeader);
      sprintf(tempHeader, "%s%s", MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
      m_HttpData.AddHeader(MapAgentStrings::ContentTypeKey,tempHeader);
    }
    



    Ptr<MgDisposable> resultObj = result->GetResultObject();
    MgDisposable* pResultObj = (MgDisposable*)resultObj;

    // Test type of response needed
    Ptr<c_RestDataReader> restreader = NULL;

    MgProxyFeatureReader* proxyfreader = dynamic_cast<MgProxyFeatureReader*>(pResultObj);
    if( proxyfreader )
    {
      restreader = new c_RestDataReader_MgFeatureReader(proxyfreader);
      //c_FeatureReaderToHtml::ToHtml((MgFeatureReader*)pResultObj,RestRequest,RestRequest->GetAgentUri(),RestRequest->GetBaseUri(),stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);            
    }
    else
    {
      restreader = SAFE_ADDREF(dynamic_cast<c_RestDataReader*>(pResultObj));
    }
    
    
    if( RestRequest->m_CfgRepresentation && RestRequest->m_CfgRepresentation->GetType() == c_CfgRepresentation::e_Template)
    {
      if( response_contentType == RestMimeType::Html  )
      {      
        c_FeatureReaderToHtml::ToHtml(restreader,RestRequest,RestRequest->GetAgentUri(),RestRequest->GetBaseUri(),stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);            
        
      }
      else
      {
        if( response_contentType == RestMimeType::Kml  )
        {
            c_FeatureReaderToHtml::ToKml(restreader.p,RestRequest,RestRequest->GetAgentUri(),RestRequest->GetBaseUri(),stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);            
        }
        else
        {
          if( response_contentType == RestMimeType::Kmz  )
          {
              outputReader = c_FeatureReaderToHtml::ToKmz(restreader.p,RestRequest,RestRequest->GetAgentUri(),RestRequest->GetBaseUri(),result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);
          }
          else
          {
            c_FeatureReaderToHtml::ToTemplate(false,restreader,RestRequest,RestRequest->GetAgentUri(),RestRequest->GetBaseUri(),stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);              
            
          }
        }
      }
    }
    
    if( RestRequest->m_CfgRepresentation && RestRequest->m_CfgRepresentation->GetType() == c_CfgRepresentation::e_FeaturesXML)
    {
      if( restreader.p )
      {
        c_FeatureReaderToXML::ToXml(restreader.p,stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);
      }
    }
    if( RestRequest->m_CfgRepresentation && RestRequest->m_CfgRepresentation->GetType() == c_CfgRepresentation::e_Sitemap)
    {
      if( restreader.p )
      {
        c_FeatureReaderToXML::ToSitemapXml(restreader.p,RestRequest,RestRequest->GetBaseUri(),stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);
      }
    }
    if( RestRequest->m_CfgRepresentation && RestRequest->m_CfgRepresentation->GetType() == c_CfgRepresentation::e_FeaturesJSON)
    {
      if( restreader.p )
      {
        c_FeatureReaderToGeoJson::ToGeoJson(restreader.p,stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);
      }
    }
    if( restreader.p )
    {
      restreader->Close();
    }
    


    if (NULL != dynamic_cast<MgByteReader*>(pResultObj))
    {
      outputReader = (MgByteReader*) SAFE_ADDREF(pResultObj);

    }
    else if (NULL != dynamic_cast<MgStringCollection*>(pResultObj))
    {
      outputReader = ((MgStringCollection*)pResultObj)->ToXml();

      /*
      outputReader = contentType == RestMimeType::Json ? 
      ((MgStringCollection*)pResultObj)->ToJson() :
      ((MgStringCollection*)pResultObj)->ToXml();
      */
    }
    else if (NULL != dynamic_cast<MgSqlDataReader*>(pResultObj))
    {
      outputReader = ((MgSqlDataReader*)pResultObj)->ToXml();

      /*
      outputReader = contentType == RestMimeType::Json ? 
      ((MgSqlDataReader*)pResultObj)->ToJson() :
      ((MgSqlDataReader*)pResultObj)->ToXml();
      */
    }
    else if (NULL != dynamic_cast<MgDataReader*>(pResultObj))
    {
      outputReader = ((MgDataReader*)pResultObj)->ToXml();

      /*
      outputReader = contentType == RestMimeType::Json ? 
      ((MgDataReader*)pResultObj)->ToJson() :
      ((MgDataReader*)pResultObj)->ToXml();
      */
    }
    else if (NULL != dynamic_cast<MgSpatialContextReader*>(pResultObj))
    {
      outputReader = ((MgSpatialContextReader*)pResultObj)->ToXml();

      /*
      outputReader = contentType == RestMimeType::Json ? 
      ((MgSpatialContextReader*)pResultObj)->ToJson() :
      ((MgSpatialContextReader*)pResultObj)->ToXml();
      */
    }
    else if (NULL != dynamic_cast<MgLongTransactionReader*>(pResultObj))
    {
      outputReader = ((MgSpatialContextReader*)pResultObj)->ToXml();

      /*
      outputReader = contentType == RestMimeType::Json ? 
      ((MgLongTransactionReader*)pResultObj)->ToJson() :
      ((MgLongTransactionReader*)pResultObj)->ToXml();
      */
    }
    else if (NULL != dynamic_cast<c_RestPrimitiveValue*>(pResultObj))
    { 
      STRING temp = ((c_RestPrimitiveValue*)pResultObj)->ToString();
      stringval_utf8 = MG_WCHAR_TO_CHAR(temp);
    }

    if (stringval_utf8.length() > 0)
    {

      if( response_contentType == RestMimeType::JsonP || 
        ( response_contentType == RestMimeType::Json && (GetJsonpCallbackName().length()>0))
        )
      {
        //utf8 = response->GetJsonpCallbackName() + "( \"" + utf8 + "\" )"; ...
        std::string jsonp;
        CreateJsonpCallbackString(GetJsonpCallbackName(),stringval_utf8,jsonp);
        stringval_utf8 = jsonp;
      }

      sprintf(tempHeader, "%d", stringval_utf8.length());
      m_HttpData.AddHeader(MapAgentStrings::ContentLengthKey,tempHeader);
      

      m_HttpData.SetContent(stringval_utf8);
      
    }
    else if (outputReader != NULL)
    {
      if( response_contentType == RestMimeType::Json || response_contentType == RestMimeType::JsonP )
      {
        MgXmlJsonConvert convert;
        convert.ToJson(outputReader);

        if( response_contentType == RestMimeType::JsonP )
        {
          string json;
          // set read back to text from Jason so it can be converted to string with function 'ToStringUtf8'
          outputReader->GetByteSource()->SetMimeType(RestMimeType::Text);
          outputReader->ToStringUtf8(json);

          string jsonp;
          CreateJsonpCallbackString(GetJsonpCallbackName(),json,jsonp);

          Ptr<MgByteSource> byteSource = new MgByteSource(
            (unsigned char*)jsonp.c_str(), (INT32)jsonp.length());
          byteSource->SetMimeType(RestMimeType::JsonP);
          outputReader.Attach(byteSource->GetReader());   
        }
      }

      INT64 outLen = outputReader->GetLength();
      sprintf(tempHeader, "%d", (INT32)outLen);
      
      m_HttpData.AddHeader(MapAgentStrings::ContentLengthKey,tempHeader);
      
      /*
      unsigned char buf[4096];
      DWORD dwSize;
      int nBytes = outputReader->Read(buf,4096);
      while (nBytes > 0)
      {
        dwSize = nBytes;
        m_pECB->WriteClient(m_pECB->ConnID, buf, &dwSize, 0);
        nBytes = outputReader->Read(buf,4096);                                
      }
      */
      m_HttpData.SetContent(outputReader);

      // Tell IIS to keep the connection open 
      //DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
      //m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
    }
    else
    {
      
      sprintf(tempHeader, "%d", 0);

      m_HttpData.AddHeader(MapAgentStrings::ContentLengthKey,tempHeader);
      

      // Tell IIS to keep the connection open 
      //DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
      //m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
    }
  }
  
  return &m_HttpData;
}                                                                         
catch (MgException* exc)                                                    
{       
  SendError(exc);
  exc->Release();                                                                
}                                                                        
catch (exception& e)                                                     
{                                                                         
  Ptr<MgException> mgException = MgSystemException::Create(e, L"c_RestResponse::PrepareHttpData", __LINE__, __WFILE__); 
  SendError(mgException);
}                                                                         
catch (...)                                                               
{                                                                         
  Ptr<MgException> mgException = new MgUnclassifiedException(L"c_RestResponse::PrepareHttpData", __LINE__, __WFILE__, NULL, L"", NULL); 
  SendError(mgException);
}  

}//end of c_RestResponse::PrepareHttpData


static char g_Hex[17] = "0123456789abcdef";


//utf8 = response->GetJsonpCallbackName() + "( \"" + utf8 + "\" )";
void c_RestResponse::CreateJsonpCallbackString(const std::string& CallbackFuncName,const std::string& JsonValue,std::string& JsonP)
{
  //utf8 = response->GetJsonpCallbackName() + "( \"" + utf8 + "\" )"; 

  std::string::const_iterator iter;

  JsonP = CallbackFuncName + "( \'";
  for(iter=JsonValue.begin();iter!=JsonValue.end();iter++)
  {
    // replace ' with \' so it will again be evaluated to ' when apperas in html as parameter to fucntion call
    if( *iter == '\'' )
    {
      /*
      char ch = *iter;
      JsonP += '%';
      JsonP += g_Hex[(int)(ch>>4)];
      JsonP += g_Hex[(int)(ch&0x0F)];
      */

      JsonP += '\\'; 
      JsonP += '\'';  

    }
    else
    {
      JsonP += *iter;
    }
  }

  JsonP.append( "\' )" );
}



void c_RestResponse::SendError(MgException* e)
{
  MG_TRY()
    STRING shortError = e->GetMessage();
  STRING longError = e->GetDetails();
  STRING statusMessage = e->GetClassName();

  //TODO: Use a string resource for html error text format
  string sResponseHeader;
  char tempHeader[4096]; 

  /*
  sprintf(tempHeader, MapAgentStrings::StatusHeader, 559, MG_WCHAR_TO_CHAR(statusMessage));
  sResponseHeader.append(tempHeader);
  sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextHtml, MapAgentStrings::Utf8Text);
  sResponseHeader.append(tempHeader);
  sResponseHeader.append(MapAgentStrings::CrLf);
  */
  
  m_HttpData.SetStatusAndReason(559,MG_WCHAR_TO_CHAR(statusMessage));
  sprintf(tempHeader, "%s%s", MapAgentStrings::TextHtml, MapAgentStrings::Utf8Text);
  m_HttpData.AddHeader(MapAgentStrings::ContentTypeKey,tempHeader);

  WriteContent("\r\n"
    "<html>\n<head>\n"
    "<title>%s</title>\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "</head>\n"
    "<body>\n<h2>%s</h2>\n%s\n</body>\n</html>\n",
    MG_WCHAR_TO_CHAR(statusMessage),
    MG_WCHAR_TO_CHAR(shortError),
    MG_WCHAR_TO_CHAR(longError));
  

  MG_CATCH(L"IsapiResponseHandler.SendError")
}

void c_RestResponse::RequestAuth(c_RestRequest* Request)
{
  //TODO: Use string resources for realm and message
  
  DWORD dwSize;
  /*
  HSE_SEND_HEADER_EX_INFO HeaderExInfo;
  
  char szHeader[] = 
  //"Connection: Keep-Alive\r\n"
  "Content-Length: %lu\r\n"
  "Content-type: text/html\r\n\r\n";
  */

  string errorMsg = "You must enter a valid login ID and password to access this site\r\n";
  m_HttpData.SetContent(errorMsg);
  

  char tempHeader[4096];

/*
  std::string sResponseHeader = MapAgentStrings::UnauthorizedHeader;
  sprintf(tempHeader, MapAgentStrings::WWWAuth, MG_WCHAR_TO_CHAR(MapAgentStrings::ProductName));
  sResponseHeader.append(tempHeader);
  sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
  sResponseHeader.append(tempHeader);
  sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, errorMsg.length());
  sResponseHeader.append(tempHeader);
  sResponseHeader.append(MapAgentStrings::CrLf);
*/
  sprintf(tempHeader,"%s%s", MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
  m_HttpData.AddHeader(MapAgentStrings::ContentTypeKey,tempHeader);
  
  sprintf(tempHeader, "%d", errorMsg.length());
  m_HttpData.AddHeader(MapAgentStrings::ContentLengthKey,tempHeader);
  

  /*
  // Send outgoing header 
  HeaderLength = Header->length();
  const char* szHttpStatus = MapAgentStrings::Unauthorized;
  HeaderExInfo.pszStatus = szHttpStatus;
  HeaderExInfo.cchStatus = strlen( HeaderExInfo.pszStatus );
  HeaderExInfo.fKeepConn = TRUE;

  m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &HeaderExInfo, NULL, NULL);

  // Send content 

  dwSize = errorMsg.length();
  m_pECB->WriteClient(m_pECB->ConnID, (LPVOID)errorMsg.c_str(), &dwSize, 0);

  DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
  m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
  
  */

  //return HSE_STATUS_SUCCESS_AND_KEEP_CONN;
}


void c_RestResponse::WriteContent(const char *pszFormat, ...)
{
  char szBuffer[4096];
  va_list arg_ptr;
  va_start(arg_ptr, pszFormat); 
  vsprintf(szBuffer, pszFormat, arg_ptr);
  va_end(arg_ptr);

  m_HttpData.SetContent(szBuffer);
  
}


