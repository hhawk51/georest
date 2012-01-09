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

#include "c_RestUri.h"
#include "c_RestUriPathParam.h"
#include "c_RestUriRequestParam.h"

#define D_REST_HTTP_METHOD_GET "GET"
#define D_REST_HTTP_METHOD_POST "POST"
#define D_REST_HTTP_METHOD_PUT "PUT"
#define D_REST_HTTP_METHOD_DELETE "DELETE"
#define D_REST_HTTP_METHOD_OPTION "OPTION"
#define D_REST_HTTP_METHOD_HEAD "HEAD"


//#include "RestDefs.h"

#include <algorithm>
#include "Poco\UnicodeConverter.h"
using namespace std;




/// <summary>
/// Constructor. Initialize member variables.
/// </summary>
c_RestUri::c_RestUri(const std::string& FullUri,const std::string& BaseUri,const std::string& RestUri,const std::string& HttpMethod,const std::string& XmlPostData)
  
{
  m_UriPathParameters = new c_RestUriPathSegment();
  m_RestRequestParam = new c_RestUriRequestParam();
  
  m_OriginalFullUri = FullUri;
  
  basic_string <char>::size_type rpos = m_OriginalFullUri.find('?');
  if( rpos != string::npos )
  {
    m_OriginalFullUri_WithoutQuery = m_OriginalFullUri.substr(0,rpos);
    m_OriginalFullUri_Query = m_OriginalFullUri.substr(rpos+1,m_OriginalFullUri.length()-rpos);
    
  }
  else
  {
    m_OriginalFullUri_WithoutQuery = m_OriginalFullUri;
    m_OriginalFullUri_Query = "";
  }
  
  
  
  if( m_OriginalFullUri_Query.length() )
    ParseQuery(m_OriginalFullUri_Query.c_str(),m_RestRequestParam);
  
  m_BaseUri = BaseUri;
  m_RestUri = RestUri;
  
  m_RestRequestParam->SetXmlPostData(XmlPostData.c_str());
  m_HttpMethod = HttpMethod;
  
    
  ParsePath(RestUri,m_UriPathParameters);
   
}

/// <summary>
/// Default constructor to shut Ptr<> up.
/// </summary>
c_RestUri::c_RestUri()
{
  m_UriPathParameters = new c_RestUriPathSegment();
  m_RestRequestParam = new c_RestUriRequestParam();
}



void c_RestUri::GetFullUri( std::string& Uri,MgStringCollection* RemoveParams,MgStringPropertyCollection* AddParams ) const
{
  Uri.reserve(m_OriginalFullUri.length()+128);
  
  STRING newquery;newquery.reserve(128);
  m_RestRequestParam->GetAsUriQuery(newquery,RemoveParams,AddParams);
  
  std::string query_utf8;
  Poco::UnicodeConverter::toUTF8(newquery,query_utf8);
  
  Uri = m_OriginalFullUri_WithoutQuery;
  if( newquery.length() > 0 )
  {
    Uri.append("?");
    Uri.append(query_utf8);
  }
}


c_RestUriPathSegment* c_RestUri::GetUriPathParameters()
{
  return SAFE_ADDREF((c_RestUriPathSegment*)m_UriPathParameters);
}


c_RestUriRequestParam* c_RestUri::GetRequestParam()
{
  return SAFE_ADDREF(m_RestRequestParam.p);
}


void c_RestUri::Dispose()
{
    delete this;
}

INT32 c_RestUri::GetClassId()
{
    return m_cls_id;
}

c_RestUri::~c_RestUri()
{

}


c_RestUri::e_HttpMethod c_RestUri::GetHttpMethod()
{
  
  if( stricmp(m_HttpMethod.c_str(),D_REST_HTTP_METHOD_GET) == 0  ) return e_HttpMethod_Get;
  
  if( stricmp(m_HttpMethod.c_str(),D_REST_HTTP_METHOD_POST) == 0  ) return e_HttpMethod_Post;
  
  if( stricmp(m_HttpMethod.c_str(),D_REST_HTTP_METHOD_PUT) == 0  ) return e_HttpMethod_Put;
  
  if( stricmp(m_HttpMethod.c_str(),D_REST_HTTP_METHOD_DELETE) == 0  ) return e_HttpMethod_Delete;
  
  if( stricmp(m_HttpMethod.c_str(),D_REST_HTTP_METHOD_OPTION) == 0  ) return e_HttpMethod_Option;
  
  if( stricmp(m_HttpMethod.c_str(),D_REST_HTTP_METHOD_HEAD) == 0  ) return e_HttpMethod_Head;
  
  return e_HttpMethod_Undefined;
}


void c_RestUri::ParseQuery(const char* pszQuery, c_RestUriRequestParam* params)
{
  MG_TRY()

    // but we need the support of a string class, so we wrap it.
    string sQuery(pszQuery);

  string::size_type startPos = 0;
  string::size_type sepPos = 0;
  while (sepPos != sQuery.npos)
  {
    sepPos = sQuery.find('&',startPos);
    if(sepPos != sQuery.npos)
    {
      // Extract one parameter.
      string sParameter = sQuery.substr(startPos,sepPos - startPos);

      // Update the next start to be past the ampersand.
      startPos = sepPos + 1;

      // Now, digest the parameter.
      ParseOneParameter(sParameter,params);
    }
  }

  ParseOneParameter(sQuery.substr(startPos),params);

  MG_CATCH_AND_THROW(L"MapAgentGetParser.Parse");
}


void c_RestUri::ParseOneParameter(string sParameter, c_RestUriRequestParam* params)
{
  if(sParameter.length() == 0)
    return;

  string sName;
  string sValue;
  string::size_type sepPosEqual = sParameter.find('=');
  if(sepPosEqual != sParameter.npos) { // A name/value pair.
    // Unescape any decorations encoded within the name or value.
    MgUtil::UnEscapeUrl((char*)sParameter.substr(0,sepPosEqual).c_str(),sName);
    MgUtil::UnEscapeUrl((char*)sParameter.substr(sepPosEqual+1).c_str(),sValue);
  }
  else { // just a name, no value.
    MgUtil::UnEscapeUrl((char*)sParameter.c_str(),sName);
    sValue = "";
  }
  STRING wsName  = MgUtil::MultiByteToWideChar(sName);
  STRING wsValue = MgUtil::MultiByteToWideChar(sValue);
  params->AddParameter(wsName, wsValue);
}

inline char AsHex(char ch)
{
  switch(ch)
  {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return ch - '0';

  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
    return ch - 'a' + 10;

  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
    return ch - 'A' + 10;
  }
  return 0;
}

void c_RestUri::ParsePath(const std::string& Uri, c_RestUriPathSegment* Params)
{
  //std::vector < std::string > segments;
  MG_TRY()

    char ch;
  int inside_brackets=0;
  std::string::const_iterator it  = Uri.begin();
  std::string::const_iterator end = Uri.end();
  std::string seg_name,seg_val;
  while (it != end)
  { 
    if( *it == '?' ) break; // stop on query part

    if( !inside_brackets && (*it == '/' || *it=='\\' ) )
    {
      if (!seg_name.empty())
      {                
        STRING wname  = MgUtil::MultiByteToWideChar(seg_name);
        STRING wval = MgUtil::MultiByteToWideChar(seg_val);
        Params->AddParameter(wname, wval, true); // add even val is empty
        seg_name.clear();
        seg_val.clear();
      }
    }
    else 
    {
      ch = *it;
      switch(ch)
      {
      case '%':               // collapse %xx notation
        it++;
        if( it == end ) break;
        ch = AsHex(*it) << 4;
        it++;
        if( it == end ) break;
        ch |= AsHex(*it);
        break;

      case '+':               // convert + to space
        ch = ' ';
        break;

      default:                // or just copy the character
        break;
      }

      switch(ch)
      {
      case '[':
      case '(':
        if( inside_brackets == 0 )
        {
          if( !seg_name.empty() ) 
          {
            inside_brackets++;
            ch=0;
          }
          else // start of bracket but there was no any characters for name of segment ( ignore '[' )
          {            
          }  
        }
        else
        {
          inside_brackets++; // just count it but add character to segment value
        }
        break;
      case ']':
      case ')':
        if( inside_brackets>0 ) 
        {            
          inside_brackets--;

          if( inside_brackets == 0 ) ch=0; // closing bracket will not be added to value
        }
        break;
      default:
        {

        }
        break;
      }

      if( ch != 0 )
      {      
        if( inside_brackets )
          seg_val.append(&ch,1); 
        else
          seg_name.append(&ch,1); 
      }

    }
    ++it;
  }
  if (!seg_name.empty())
  {
    STRING wname  = MgUtil::MultiByteToWideChar(seg_name);
    STRING wval = MgUtil::MultiByteToWideChar(seg_val);
    Params->AddParameter(wname, wval, true); // add even val is empty
    seg_name.clear();
    seg_val.clear();
  }


  MG_CATCH_AND_THROW(L"RestUriParser.Parse");
}

