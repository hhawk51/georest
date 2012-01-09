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
#include "c_CfgService.h"
#include "c_CfgEsriGS_Catalog.h"
#include "Poco\UnicodeConverter.h"
#include "Poco\String.h"
#include "value.h"
#include "writer.h"
#include "reader.h"


c_CfgEsriGS_Catalog::c_CfgEsriGS_Catalog( const wchar_t* UriTag ) : c_CfgService(UriTag,c_CfgService::e_EsriGS_Catalog)
{
  m_UriTag = UriTag;
}

c_CfgEsriGS_Folder* c_CfgEsriGS_Catalog::FindFolder( const wstring& Name )
{
  if( Name.empty()) return &m_RootFolder;
  
  return m_RootFolder.FindSubFolder(Name);
}

void c_CfgEsriGS_Catalog::AddServer( c_CfgEsriGS_Server* Server )
{
  m_RootFolder.m_ListOfServices.push_back(Server);
}
c_CfgEsriGS_Server* c_CfgEsriGS_Catalog::FindServer( const wstring& Name )
{
  return m_RootFolder.FindServer(Name);
}


void c_CfgEsriGS_Catalog::AddFolder( c_CfgEsriGS_Folder* Folder )
{
  m_RootFolder.AddSubFolder(Folder);
}

void c_CfgEsriGS_Catalog::WriteToJson( std::ostream& Out ,bool Pretty ) const
{
  m_RootFolder.WriteToJson(Out,Pretty);
}

void c_CfgEsriGS_Catalog::ReadFromJson( const std::string& InJson )
{
  m_RootFolder.ReadFromJson(InJson);
}
c_CfgEsriGS_Folder::c_CfgEsriGS_Folder()
{

}

c_CfgEsriGS_Folder::c_CfgEsriGS_Folder(std::wstring& Name)
{
  m_Name = Name;
}
c_CfgEsriGS_Folder::~c_CfgEsriGS_Folder( void )
{
  Clear();
}

void c_CfgEsriGS_Folder::Clear()
{
  {  
    t_c_CfgEsriGS_ServerVector::iterator iter;
    for ( iter = m_ListOfServices.begin( ) ; iter != m_ListOfServices.end( ) ; iter++ )
    {
      c_CfgEsriGS_Server * datalayer = *iter;
      delete datalayer;
    }
    m_ListOfServices.clear();
  }
  {  
    t_c_CfgEsriGS_FolderVector::iterator iter;
    for ( iter = m_ListOfSubFolders.begin( ) ; iter != m_ListOfSubFolders.end( ) ; iter++ )
    {
      c_CfgEsriGS_Folder * datalayer = *iter;
      delete datalayer;
    }
    m_ListOfSubFolders.clear();  
  }
}

c_CfgEsriGS_Folder* c_CfgEsriGS_Folder::FindSubFolder( const wstring& Name )
{
  t_c_CfgEsriGS_FolderVector::iterator iter;
  for ( iter = m_ListOfSubFolders.begin( ) ; iter != m_ListOfSubFolders.end( ) ; iter++ )
  {
    if( Poco::icompare(Name,(*iter)->m_Name) == 0 ) return *iter;
  }
  
  return NULL;
}

c_CfgEsriGS_Folder* c_CfgEsriGS_Folder::AddSubFolder( c_CfgEsriGS_Folder* SubFolder )
{
  m_ListOfSubFolders.push_back(SubFolder);
  
  return SubFolder;
}

void c_CfgEsriGS_Folder::AddServer( c_CfgEsriGS_Server* Server )
{
  m_ListOfServices.push_back(Server);

  
}

c_CfgEsriGS_Server* c_CfgEsriGS_Folder::FindServer( const wstring& Name )
{
  t_c_CfgEsriGS_ServerVector::iterator iter;
  for ( iter = m_ListOfServices.begin( ) ; iter != m_ListOfServices.end( ) ; iter++ )
  {
    if( Poco::icompare(Name,(*iter)->m_Name) == 0 ) return *iter;
  }

  return NULL;
}

void c_CfgEsriGS_Folder::ReadFromJson( const std::string& InJson )
{ 
  Json::Value root_val;
  Json::Reader reader;
  if( !reader.parse(InJson,root_val,false)  )
  {
    std::string errmsg = reader.getFormatedErrorMessages();
    throw c_ExceptionRestConfig(errmsg.c_str());
  }
  
  {
  const Json::Value& folder_array = root_val["folders"];
  if( !folder_array.isNull() )
  {
    if( folder_array.type() != Json::arrayValue )
    {
      throw c_ExceptionRestConfig("Error parsing Json! 'folders' should be of type array.");
    }
    int size = folder_array.size();
    for(int ind=0;ind<size;ind++)
    {
      const Json::Value& folder = folder_array[ind];
      if( folder.type() != Json::stringValue )
      {
        throw c_ExceptionRestConfig("Error parsing Json! Values of 'folders' array should be of type string.");
      }
      std::wstring wname;
      Poco::UnicodeConverter::toUTF16(folder.asString(),wname);
      c_CfgEsriGS_Folder* esrigs_folder = new c_CfgEsriGS_Folder(wname);
      AddSubFolder(esrigs_folder);
    }    
  }
  }
  
  {
  const Json::Value& server_array = root_val["services"];
  if( !server_array.isNull() )
  {
    if( server_array.type() != Json::arrayValue )
    {
      throw c_ExceptionRestConfig("Error parsing Json! 'services' should be of type array.");
    }
    int size = server_array.size();
    for(int ind=0;ind<size;ind++)
    {
      const Json::Value& server = server_array[ind];
      if( server.type() != Json::objectValue )
      {
        throw c_ExceptionRestConfig("Error parsing Json! Values of 'services' array should be of type object.");
      }
      const Json::Value& server_name = server["name"];
      if( server_name.type() != Json::stringValue )
      {
        throw c_ExceptionRestConfig("Error parsing Json! 'name' of server should be of type string.");
      }
      const Json::Value& server_type = server["type"];
      if( server_type.type() != Json::stringValue )
      {
        throw c_ExceptionRestConfig("Error parsing Json! 'type' of server should be of type string.");
      }
      c_CfgEsriGS_Server* esrigs_server=NULL;
      if( server_type.asString().compare("FeatureServer")==0)
      {
        std::wstring wname;
        Poco::UnicodeConverter::toUTF16(server_name.asString(),wname);
        esrigs_server = new c_CfgEsriGS_FeatureServer(wname);
      }
      
      if( !esrigs_server )
      {
        std::string eerstr;
        eerstr.append("Error parsing Json! Server type '");
        eerstr.append(server_type.asString());
        eerstr.append("' is not recognized.");
        throw c_ExceptionRestConfig("Error parsing Json! 'type'");
      }
      AddServer(esrigs_server);
    }
  }
  }
}

void c_CfgEsriGS_Folder::WriteToJson( std::ostream& Out ,bool Pretty ) const
{
  Json::Value root_val(Json::objectValue);
  
  
  //root_val["specVersion"] = Json::Value(1.0); 
  root_val["currentVersion"] = Json::Value(10); 
  
{ 
  root_val["folders"] = Json::Value(Json::arrayValue); 
  t_c_CfgEsriGS_FolderVector::const_iterator iter;
  for ( iter = m_ListOfSubFolders.begin( ) ; iter != m_ListOfSubFolders.end( ) ; iter++ )
  {
    c_CfgEsriGS_Folder * datalayer = *iter;
    std::string name;
    Poco::UnicodeConverter::toUTF8(datalayer->m_Name,name);
    root_val["folders"].append(name);
  }
  
}
  
  root_val["services"] = Json::Value(Json::arrayValue);
  t_c_CfgEsriGS_ServerVector::const_iterator iter;
  for ( iter = m_ListOfServices.begin( ) ; iter != m_ListOfServices.end( ) ; iter++ )
  {
    c_CfgEsriGS_Server * server = *iter;
    
    Json::Value server_val(Json::objectValue);
    
    std::string name;
    Poco::UnicodeConverter::toUTF8(server->m_Name,name);
    server_val["name"] = name;
    switch(server->GetType())
    {
      case c_CfgEsriGS_Server::e_FeatureServer:
        server_val["type"] = Json::StaticString("FeatureServer");
      break;
      case c_CfgEsriGS_Server::e_ImageServer:
        server_val["type"] = Json::StaticString("ImageServer");
      break;
      case c_CfgEsriGS_Server::e_MapServer:
        server_val["type"] = Json::StaticString("MapServer");
      break;
    }
    root_val["services"].append(server_val);
  }
  
if( Pretty )    
{
  Json::StyledStreamWriter sswrite;
  
  sswrite.write(Out,root_val);
}
else
{
  Json::FastWriter fwrite;

  std::string outstr = fwrite.write(root_val);
  Out << outstr;
}
  
}
/*
const c_CfgService* c_CfgEsriGS_Folder::FindUriTag( const std::wstring& UriTag ) 
{
  t_c_CfgEsriGS_ServiceVector::iterator iter;
  for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
  {
    if( wcsicmp(UriTag.c_str(),(*iter)->m_UriTag.c_str()) == 0 ) return *iter;
  }
  
  return NULL;
}

const c_CfgService* c_CfgServiceVector::FindClassName( const std::wstring& ClassName ) 
{
  
  std::wstring::size_type iColon = ClassName.find(':');
  if(iColon != std::wstring::npos) 
  {
    // full name
    t_CfgDataResourceVector::iterator iter;
    for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ClassName.c_str()) == 0 ) return *iter;
    }  
  }
  else
  {
    // short name
    t_CfgDataResourceVector::iterator iter;
    for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ShortClassName.c_str()) == 0 ) return *iter;
    }  
  }
  
  
  return NULL;
}
*/



c_CfgEsriGS_Server::c_CfgEsriGS_Server( std::wstring Name,e_EsriGS_ServerType Type )
  : m_Name(Name), m_Type(Type)
{

}

bool c_CfgEsriGS_Server::MatchServerType( const std::wstring& Type ) const
{
  switch(m_Type)
  {
    case c_CfgEsriGS_Server::e_FeatureServer:
    {
      if( Type.compare(L"FeatureServer") == 0 ) return true;
    }
    break;
    case c_CfgEsriGS_Server::e_ImageServer:
    {
      if( Type.compare(L"ImageServer") == 0 ) return true;
    }
    break;
    case c_CfgEsriGS_Server::e_MapServer:
    {
      if( Type.compare(L"MapServer") == 0 ) return true;
    }
    break;
  }
  
  return false;
}