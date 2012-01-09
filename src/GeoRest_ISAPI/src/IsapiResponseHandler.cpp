//
//  Copyright (C) 2010 by Haris Kurtagic
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

#include "MapGuideCommon.h"
//#include "HttpHandler.h"
//#include "HttpPrimitiveValue.h"
#include "IsapiResponseHandler.h"
#include "MapAgentStrings.h"
#include "RestCommonInclude.h"
#include "RestRequestHandlerInclude.h"
#include "c_RestRequest.h"

#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"

#include <stdlib.h>
#include <sstream>
#include "../c_IsapiOstream.h"
#include "Poco/StreamCopier.h"

// AgentUri is full URI, like http://localhost/mgrest/rest/data/parcel/.html?filter_1_and_likeright_NAME=u
// UriBase is just part of URI without REST partand query
IsapiResponseHandler::IsapiResponseHandler(c_RestRequest* RestRequest,const string& AgentUri,const string& UriBase,EXTENSION_CONTROL_BLOCK *pECB) :
    m_pECB(pECB)
{
  m_RestRequest =   SAFE_ADDREF( RestRequest );
  m_AgentUri = AgentUri;
  m_UriBase = UriBase;
}

IsapiResponseHandler::~IsapiResponseHandler(void)
{
}

void IsapiResponseHandler::SendResponse(c_RestResponse* response,c_RestRequest* RestRequest)
{
  c_RestResponse_HttpData* http_data = response->GetHttpData(RestRequest);
  
  Poco::Net::MessageHeader& headers = http_data->GetMsgHeaders();
  
  std::ostringstream ostr2;
  headers.write(ostr2);
  std::string s2 = ostr2.str();
  
  //Poco::Net::HTTPResponse httpresponse(Poco::Net::HTTPMessage::HTTP_1_1, Poco::Net::HTTPResponse::HTTP_OK);
  //httpresponse.set("Location", "http://www.appinf.com/index.html");
  //response.set("Server", "Poco/1.0");
  char statusline[512];
  sprintf(statusline,"%d %s",http_data->GetStatus(),http_data->GetStatusReason());
  
  
  /*
  char temphead[512];
  sprintf(temphead,"%d",http_data->GetStatus());
  httpresponse.setReason(http_data->GetStatusReason());
  httpresponse.setStatus(temphead);
  httpresponse.swap(headers);
  std::ostringstream ostr;
  httpresponse.write(ostr);
  std::string sheader = ostr.str();
  */
  
  //std::string sheader = "Status: 200 OK\r\n";
  //sheader += "Connection: keep-alive\r\n";
  //sheader += s2;
  
  /*
  HSE_SEND_HEADER_EX_INFO HeaderExInfo;
  HeaderExInfo.pszHeader = s2.c_str();
  HeaderExInfo.cchHeader = s2.length();

  const char* szHttpStatus = MapAgentStrings::Unauthorized;
  HeaderExInfo.pszStatus = statusline;
  HeaderExInfo.cchStatus = strlen( HeaderExInfo.pszStatus );
  HeaderExInfo.fKeepConn = TRUE;
  m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &HeaderExInfo, NULL, NULL);
  */
 

  

  //std::istringstream istr(http_data->GetHeader());
  //response.read(istr);

  Ptr<c_StreamResponse> streamrep = http_data->GetStreamResponse();
  if( streamrep.p )
  {
    //std::stringstream strstream;
    //streamrep->StreamOut(&strstream);    
    //std::streamsize ssize = strstream.rdbuf()->in_avail();
    
    char temphead[512];
    sprintf(temphead,"HTTP/1.1 %d %s",http_data->GetStatus(),http_data->GetStatusReason());
    //std::string temphead = "Status: 200 OK\r\n";
    string sheader = temphead;
    sheader = sheader.append("\r\n");
    sheader = sheader.append(s2);
    //sprintf(temphead,"Content-Length: %ld\r\n",(long)ssize);
    sheader.append("Connection: Keep-Alive\r\n");
    sprintf(temphead,"Transfer-Encoding: chunked\r\n");
    sheader = sheader.append(temphead);
    sheader = sheader.append("\r\n");
    
    //WriteHeader(sheader.c_str());
    DWORD dwsize = (DWORD)sheader.length();
    //m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, (LPVOID)NULL, &dwSize, (LPDWORD)sheader.c_str()); 
    m_pECB->WriteClient(m_pECB->ConnID, (void*)sheader.data(),&dwsize,0); 
    
    c_IsapiOstream ostream(m_pECB,true);
    streamrep->StreamOut(&ostream);    
    
    //Poco::StreamCopier::copyStream(strstream,ostream);
    
    //ostream << strstream;
      
    ostream.Close(); // will end chunked transfer if used
    

  }
  else {   
  
  
    char temphead[512];
    sprintf(temphead,"Status: %d %s",http_data->GetStatus(),http_data->GetStatusReason());
    //std::string temphead = "Status: 200 OK\r\n";
    string sheader = temphead;
    sheader = sheader + "\r\n";
    sheader = sheader + s2;
    sheader = sheader + "\r\n";
    //WriteHeader(sheader.c_str());
    DWORD dwSize = (DWORD)sheader.length();
    m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, (LPVOID)NULL, &dwSize, (LPDWORD)sheader.c_str()); 
    
  Ptr<MgByteReader> bytereader = http_data->GetContentByteReader();

  if( bytereader.p )
  {
    //MgByteSource* bytes = bytereader->GetByteSource();
    //ByteSourceImpl* bsrcimpl = bytes->GetSourceImpl();
    //std::ostream& outstr = response.send(); 

    unsigned char buf[4096];
    DWORD dwSize;
    int nBytes = bytereader->Read(buf,4096);
    while (nBytes > 0)
    {
      dwSize = nBytes;
      //response.sendBuffer(buf,nBytes);  
      
      //outstr.write((char*)&buf[0],nBytes);
      bool ret = m_pECB->WriteClient(m_pECB->ConnID, buf, &dwSize, 0);
      
      nBytes = bytereader->Read(&buf[0],4096);                                
    }

 
  }
  else
  {    
    //std::ostream& outstr = response.send(); 
    //outstr << http_data->GetContentString();
    
    DWORD dwBufLen = (DWORD)http_data->GetContentString().length();
    m_pECB->WriteClient(m_pECB->ConnID, (LPVOID)http_data->GetContentString().c_str(), &dwBufLen, 0);
  }
  
  }

  // Tell IIS to keep the connection open 
  DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
  m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
  //m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_STATUS_SUCCESS_AND_KEEP_CONN , &dwState, NULL, 0);
  
} 
/*

void IsapiResponseHandler::SendResponse(c_RestResponse* response)
{
    MG_TRY()

    string sResponseHeader;
    char tempHeader[4096];
    Ptr<MgByteReader> outputReader;

    Ptr<MgHttpResult> result = response->GetResult();
    STATUS status = result->GetStatusCode();
    if (status != 200)
    {
        STRING statusMessage = result->GetHttpStatusMessage();
        if (statusMessage == MapAgentStrings::FailedAuth1 ||
            statusMessage == MapAgentStrings::FailedAuth2
            )
        {
            RequestAuth();
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
            
            
            sprintf(tempHeader, MapAgentStrings::StatusHeader, status, MG_WCHAR_TO_CHAR(statusMessage));
            sResponseHeader.append(tempHeader);
            
            sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, (int)content_length);
            sResponseHeader.append(tempHeader);
            
            sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextHtml, "");
            sResponseHeader.append(tempHeader);
            sResponseHeader.append(MapAgentStrings::CrLf);
            WriteHeader(sResponseHeader.c_str());
            
            

            m_pECB->WriteClient(m_pECB->ConnID, content, &content_length, 0);
            
            // Tell IIS to keep the connection open 

            DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;

            m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);

        }
    }
    else
    {

        // Status was ok.  Send the real result back.
        STRING result_contentType = result->GetResultContentType();
        STRING response_contentType = response->GetContentType();
        std::string stringval_utf8;

        sprintf(tempHeader, MapAgentStrings::StatusOkHeader);
        sResponseHeader.append(tempHeader);
        
        if (response_contentType.length() > 0)
        {
            // If we are returning text, state that it is utf-8.
            string charSet = "";
            if (response_contentType.find(L"text") != response_contentType.npos)  //NOXLATE
            {
                charSet = MapAgentStrings::Utf8Text;
            }
            sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MG_WCHAR_TO_CHAR(response_contentType), charSet.c_str());
            sResponseHeader.append(tempHeader);
        }
        else
        {
            sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
            sResponseHeader.append(tempHeader);
        }

        

        
        Ptr<MgDisposable> resultObj = result->GetResultObject();
        MgDisposable* pResultObj = (MgDisposable*)resultObj;
        
        // Test type of response needed
        
        if( response_contentType == MgMimeType::Html  )
        {
          MgFeatureReader* freader = dynamic_cast<MgFeatureReader*>(pResultObj);
          if( (pResultObj==NULL) || freader )
          {
            c_FeatureReaderToHtml::ToHtml((MgFeatureReader*)pResultObj,m_RestRequest,m_AgentUri,m_UriBase,stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);            
          }
        }
        else
        {
          if( response_contentType == MgMimeType::Kml  )
          {
            MgFeatureReader* freader = dynamic_cast<MgFeatureReader*>(pResultObj);
            if( (pResultObj==NULL) || freader )
            {
              c_FeatureReaderToHtml::ToKml((MgFeatureReader*)pResultObj,m_RestRequest,m_AgentUri,m_UriBase,stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);            
            }
          }
          else
          {
            if( response_contentType == MgMimeType::Kmz  )
            {
              MgFeatureReader* freader = dynamic_cast<MgFeatureReader*>(pResultObj);
              if( (pResultObj==NULL) || freader )
              {
                outputReader = c_FeatureReaderToHtml::ToKmz((MgFeatureReader*)pResultObj,m_RestRequest,m_AgentUri,m_UriBase,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);
              }
            }
            else
            {
              if( response_contentType == MgMimeType::Xml  )
              {
                MgFeatureReader* freader = dynamic_cast<MgFeatureReader*>(pResultObj);
                if( freader )
                {
                  if( !result->m_IsSitemapXML )
                  {
                    c_FeatureReaderToXML::ToXml(freader,stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);
                  }
                  else
                  {
                    
                    c_FeatureReaderToXML::ToSitemapXml(freader,m_RestRequest,m_UriBase,stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);
                  }                  
                }
              }
              else
              {
                if( response_contentType == MgMimeType::Json || response_contentType == MgMimeType::JsonP )
                {
                  MgFeatureReader* freader = dynamic_cast<MgFeatureReader*>(pResultObj);
                  if( freader )
                  {
                    c_FeatureReaderToGeoJson::ToGeoJson((MgFeatureReader*)pResultObj,stringval_utf8,result->m_FeatureReader_StartIndex,result->m_FeatureReader_Count);
                  }
                }
              }
            }
          }
        }
        

        if (NULL != dynamic_cast<MgByteReader*>(pResultObj))
        {
          outputReader = (MgByteReader*) SAFE_ADDREF(pResultObj);
         
        }
        else if (NULL != dynamic_cast<MgStringCollection*>(pResultObj))
        {
          outputReader = ((MgStringCollection*)pResultObj)->ToXml();
        
        }
        else if (NULL != dynamic_cast<MgSqlDataReader*>(pResultObj))
        {
          outputReader = ((MgSqlDataReader*)pResultObj)->ToXml();
         
        
        }
        else if (NULL != dynamic_cast<MgDataReader*>(pResultObj))
        {
          outputReader = ((MgDataReader*)pResultObj)->ToXml();
        
       
        }
        else if (NULL != dynamic_cast<MgSpatialContextReader*>(pResultObj))
        {
          outputReader = ((MgSpatialContextReader*)pResultObj)->ToXml();
        
       
        }
        else if (NULL != dynamic_cast<MgLongTransactionReader*>(pResultObj))
        {
          outputReader = ((MgSpatialContextReader*)pResultObj)->ToXml();
      
       
        }
        else if (NULL != dynamic_cast<MgHttpPrimitiveValue*>(pResultObj))
        { 
          STRING temp = ((MgHttpPrimitiveValue*)pResultObj)->ToString();
          stringval_utf8 = MG_WCHAR_TO_CHAR(temp);
        }

        if (stringval_utf8.length() > 0)
        {
            
            if( response_contentType == MgMimeType::JsonP || 
                ( response_contentType == MgMimeType::Json && (response->GetJsonpCallbackName().length()>0))
              )
            {
              //utf8 = response->GetJsonpCallbackName() + "( \"" + utf8 + "\" )"; ...
              std::string jsonp;
              CreateJsonpCallbackString(response->GetJsonpCallbackName(),stringval_utf8,jsonp);
              stringval_utf8 = jsonp;
            }
            
            sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, stringval_utf8.length());
            sResponseHeader.append(tempHeader);
            sResponseHeader.append(MapAgentStrings::CrLf);
            DWORD dwHeaderSize = (DWORD)sResponseHeader.length();
            m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, NULL, &dwHeaderSize, (LPDWORD)sResponseHeader.c_str());

            DWORD dwBufLen = (DWORD)stringval_utf8.length();
            m_pECB->WriteClient(m_pECB->ConnID, (LPVOID)stringval_utf8.c_str(), &dwBufLen, 0);
            
            // Tell IIS to keep the connection open 
            DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
            m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
        }
        else if (outputReader != NULL)
        {
            if( response_contentType == MgMimeType::Json || response_contentType == MgMimeType::JsonP )
            {
              MgXmlJsonConvert convert;
              convert.ToJson(outputReader);
              
              if( response_contentType == MgMimeType::JsonP )
              {
                string json;
                // set read back to text from Jason so it can be converted to string with function 'ToStringUtf8'
                outputReader->GetByteSource()->SetMimeType(MgMimeType::Text);
                outputReader->ToStringUtf8(json);

                string jsonp;
                CreateJsonpCallbackString(response->GetJsonpCallbackName(),json,jsonp);
                
                Ptr<MgByteSource> byteSource = new MgByteSource(
                  (unsigned char*)jsonp.c_str(), (INT32)jsonp.length());
                byteSource->SetMimeType(MgMimeType::JsonP);
                outputReader.Attach(byteSource->GetReader());   
              }
            }
            
            INT64 outLen = outputReader->GetLength();
            sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, (INT32)outLen);
            sResponseHeader.append(tempHeader);
            sResponseHeader.append(MapAgentStrings::CrLf);
            WriteHeader(sResponseHeader.c_str());

            unsigned char buf[4096];
            DWORD dwSize;
            int nBytes = outputReader->Read(buf,4096);
            while (nBytes > 0)
            {
                dwSize = nBytes;
                m_pECB->WriteClient(m_pECB->ConnID, buf, &dwSize, 0);
                nBytes = outputReader->Read(buf,4096);                                
            }
            
            // Tell IIS to keep the connection open 
            DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
            m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
        }
        else
        {
            sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, 0);
            sResponseHeader.append(tempHeader);
            sResponseHeader.append(MapAgentStrings::CrLf);
            WriteHeader(sResponseHeader.c_str());
            
            // Tell IIS to keep the connection open 
            DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
            m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
        }
    }

    MG_CATCH_AND_THROW(L"IsapiResponseHandler.SendResponse");
}
*/

static char g_Hex[17] = "0123456789abcdef";

    
//utf8 = response->GetJsonpCallbackName() + "( \"" + utf8 + "\" )";
void IsapiResponseHandler::CreateJsonpCallbackString(const std::string& CallbackFuncName,const std::string& JsonValue,std::string& JsonP)
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



void IsapiResponseHandler::SendError(MgException* e)
{
    MG_TRY()
    #ifdef _MG_ENT_2011
        STRING shortError = mgException->GetExceptionMessage();
    #else
        STRING shortError = mgException->GetMessage();
    #endif
    STRING longError = e->GetDetails();
    STRING statusMessage = e->GetClassName();

    //TODO: Use a string resource for html error text format
    string sResponseHeader;
    char tempHeader[4096]; 

    sprintf(tempHeader, MapAgentStrings::StatusHeader, 559, MG_WCHAR_TO_CHAR(statusMessage));
    sResponseHeader.append(tempHeader);
    sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextHtml, MapAgentStrings::Utf8Text);
    sResponseHeader.append(tempHeader);
    sResponseHeader.append(MapAgentStrings::CrLf);
    WriteHeader(sResponseHeader.c_str());
    
    WriteContext("\r\n"
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

void IsapiResponseHandler::RequestAuth()
{
    //TODO: Use string resources for realm and message
/*
    char tempHeader[4096];
    string errorMsg = "You must enter a valid login ID and password to access this site\r\n";

    string sResponseHeader = MapAgentStrings::UnauthorizedHeader;
    sprintf(tempHeader, MapAgentStrings::WWWAuth, MG_WCHAR_TO_CHAR(MapAgentStrings::ProductName));
    sResponseHeader.append(tempHeader);
    sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
    sResponseHeader.append(tempHeader);
    sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, errorMsg.length());
    sResponseHeader.append(MapAgentStrings::CrLf);

    DWORD dwHeaderSize = (DWORD)sResponseHeader.length();

    DWORD dwContentSize;
    dwContentSize = (DWORD)errorMsg.length();

    const char* szHttpStatus = MapAgentStrings::Unauthorized;
    
    m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, (LPVOID) szHttpStatus, &dwHeaderSize, (LPDWORD)sResponseHeader.c_str()); 
    
    m_pECB->WriteClient(m_pECB->ConnID, (LPVOID) errorMsg.c_str(), &dwContentSize, 0);
    
    // Tell IIS to keep the connection open 
    DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
    m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
   */ 
    
    DWORD dwSize;
    HSE_SEND_HEADER_EX_INFO HeaderExInfo;
/*
    char szHeader[] = 
      //"Connection: Keep-Alive\r\n"
      "Content-Length: %lu\r\n"
      "Content-type: text/html\r\n\r\n";
      */
    
    string errorMsg = "You must enter a valid login ID and password to access this site\r\n";
    
    
    char tempHeader[4096];
    
    string sResponseHeader = MapAgentStrings::UnauthorizedHeader;
    sprintf(tempHeader, MapAgentStrings::WWWAuth, MG_WCHAR_TO_CHAR(MapAgentStrings::ProductName));
    sResponseHeader.append(tempHeader);
    sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
    sResponseHeader.append(tempHeader);
    sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, errorMsg.length());
    sResponseHeader.append(tempHeader);
    sResponseHeader.append(MapAgentStrings::CrLf);
    
      /* Send outgoing header */

    

      HeaderExInfo.pszHeader = sResponseHeader.c_str();
      HeaderExInfo.cchHeader = sResponseHeader.length();
      
      const char* szHttpStatus = MapAgentStrings::Unauthorized;
      HeaderExInfo.pszStatus = szHttpStatus;
      HeaderExInfo.cchStatus = strlen( HeaderExInfo.pszStatus );
      HeaderExInfo.fKeepConn = TRUE;

      m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &HeaderExInfo, NULL, NULL);
      
      /* Send content */

      dwSize = errorMsg.length();
      m_pECB->WriteClient(m_pECB->ConnID, (LPVOID)errorMsg.c_str(), &dwSize, 0);

      DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
      m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
      
      //return HSE_STATUS_SUCCESS_AND_KEEP_CONN;
}

void IsapiResponseHandler::WriteHeader(const char* szBuffer, const char* szStatusBuffer)
{
    DWORD dwSize = (DWORD)strlen(szBuffer);
    m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, (LPVOID)szStatusBuffer, &dwSize, (LPDWORD)szBuffer); 
}

void IsapiResponseHandler::WriteContext(const char *pszFormat, ...)
{
	char szBuffer[4096];
	va_list arg_ptr;
	va_start(arg_ptr, pszFormat); 
	vsprintf(szBuffer, pszFormat, arg_ptr);
	va_end(arg_ptr);
	
	DWORD dwSize = (DWORD)strlen(szBuffer);
	m_pECB->WriteClient(m_pECB->ConnID, szBuffer, &dwSize, 0);
  // Tell IIS to keep the connection open 
  DWORD dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
  m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
}



