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


#ifndef _c_CfgEsriGS_Catalog_h
#define _c_CfgEsriGS_Catalog_h

#include <vector>






class REST_CONFIG_API c_CfgEsriGS_Server 
{
  
public:
  enum e_EsriGS_ServerType{
    e_MapServer,
    e_FeatureServer,
    e_ImageServer,

  } ;
  
public:
  c_CfgEsriGS_Server(std::wstring Name,e_EsriGS_ServerType Type);
   
  e_EsriGS_ServerType GetType() const { return m_Type;};
  bool MatchServerType(const std::wstring& Type) const; // return true is string matches server type
  
  virtual void WriteToJson(std::ostream& Out ,bool Pretty) const {} ;
  virtual void ReadFromJson(const std::string& InJson) {};
public:
  std::wstring m_Name;
  e_EsriGS_ServerType m_Type;
};
typedef std::vector<c_CfgEsriGS_Server*> t_c_CfgEsriGS_ServerVector;




class c_CfgEsriGS_Folder;
typedef std::vector<c_CfgEsriGS_Folder*> t_c_CfgEsriGS_FolderVector;

class REST_CONFIG_API c_CfgEsriGS_Folder
{
public:
  c_CfgEsriGS_Folder(); // root folder
  c_CfgEsriGS_Folder(std::wstring& Name);
  ~c_CfgEsriGS_Folder();
  void Clear();
  
  void WriteToJson(std::ostream& Out ,bool Pretty) const;
  void ReadFromJson(const std::string& InJson);
  
public: 
  std::wstring m_Name;
  //std::wstring m_ParentFolder;
  t_c_CfgEsriGS_FolderVector m_ListOfSubFolders;
  t_c_CfgEsriGS_ServerVector m_ListOfServices;
  
public:
  c_CfgEsriGS_Folder* FindSubFolder(const wstring& Name);  
  c_CfgEsriGS_Folder* AddSubFolder(c_CfgEsriGS_Folder* SubFolder);  
  void AddServer(c_CfgEsriGS_Server* Server);
  c_CfgEsriGS_Server* FindServer( const wstring& Name );
};




class REST_CONFIG_API c_CfgEsriGS_Catalog : public c_CfgService
{
  
public:
    c_CfgEsriGS_Catalog(const wchar_t* UriTag);

    c_CfgEsriGS_Folder* GetRootFolder() { return &m_RootFolder; }
    
    c_CfgEsriGS_Folder* FindFolder(const wstring& Name);
    void AddFolder(c_CfgEsriGS_Folder* Folder);
    void AddServer(c_CfgEsriGS_Server* Server);
    
    void WriteToJson(std::ostream& Out ,bool Pretty) const;
    void ReadFromJson(const std::string& InJson);
    c_CfgEsriGS_Server* FindServer( const wstring& Name );

public:
  c_CfgEsriGS_Folder m_RootFolder;  
};

#endif
