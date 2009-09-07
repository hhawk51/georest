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

#ifndef _c_RestUriFactory_h
#define _c_RestUriFactory_h



class c_RestUriFactory : public MgDisposable
{
public:
  static c_RestUriFactory * CreateObject();

protected:
  c_RestUriFactory();
  ~c_RestUriFactory();
public:
    /// <summary>
    /// Creates REST URI for MapDefinition
    /// </summary>
    /// <param name="MapDefName">Input
    /// MapDefName
    /// This contains all the parameters of the request.
    /// </param>
    /// <param name="Uri">Output
    /// Uri
    /// This contains all the parameters of the request.
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>
    

    void Create_MapDefinition(const std::string& MapDefName,std::string& Uri);
    void Create_MapDefinition(const std::wstring& MapDefName,std::string& Uri);
  
    void Create_Session(const std::string& Session,std::string& Uri);
    void Create_Session(const std::wstring& Session,std::string& Uri);
    void Create_Session_Map(const std::string& Session,const std::string& Map,std::string& Uri);
    void Create_Session_Map(const std::wstring& Session,const std::wstring& Map,std::string& Uri);
    
    void Create_Session_Map_ImagePng(const std::string& Session,const std::string& Map,std::string& Uri);
    void Create_Session_Map_ImagePng(const std::wstring& Session,const std::wstring& Map,std::string& Uri);
    
    void Create_Session_Map_QueryMapFeatures(const std::string& Session,const std::string& Map,std::string& Uri);
    void Create_Session_Map_QueryMapFeatures(const std::wstring& Session,const std::wstring& Map,std::string& Uri);
    
    void Create_LayerDef_LegendIcon( const std::wstring& LayerDefResId,double Scale,int ThemeCategory,int GeometryType,std::string& Uri );
    void Create_LayerDef_LegendIcon( const std::string& LayerDefResId,double Scale,int ThemeCategory,int GeometryType,std::string& Uri );
    
    void Create_FDO_DataSource_Class_Feature(const std::string& DataSource,const std::string& ClassName,std::string& Uri);
    void Create_FDO_DataSource_Class_Feature(const std::wstring& DataSource,const std::wstring& ClassName,std::string& Uri);
        
protected:
  /// <summary>
  /// This method is responsible for releasing all
  /// its member variables and then destruct itself.
  /// </summary>
  /// <returns>Returns nothing</returns>
  virtual void Dispose();

  /// <summary>
  /// Get the class Id
  /// </summary>
  /// <returns>
  /// The integer value
  /// </returns>
  INT32 GetClassId();    
  
  
  
  
  

    
protected:
  static const INT32 m_cls_id = 0; // HttpHandler_MapAgent_RestUriFactory;
  

};

#endif  //_MGHTTPCREATESESSION_H_
