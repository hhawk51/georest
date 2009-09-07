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
#include "c_RestMapNameToResourceId.h"
#include "RestConfigInclude.h"

c_RestMapNameToResourceId::c_RestMapNameToResourceId(void)
{
}

c_RestMapNameToResourceId::~c_RestMapNameToResourceId(void)
{
}



const c_CfgDataResource * c_RestMapNameToResourceId::GetDataResource( const STRING& UriTag)
{
  c_RestConfig* cfg_rest = c_RestConfig::GetInstance(); 
  const c_CfgDataResource * datalayer = cfg_rest->FindUriTag(UriTag.c_str());
  if( !datalayer ) return NULL;
  
  
  //DataSource_ResourceId = datalayer->m_FeatureSource; // L"Library://Samples/Sheboygan/Data/CityLimits.FeatureSource";
  //DataSource_ClassName = datalayer->m_ClassName; // L"SHP_Schema:CityLimits";
  //MapDef = datalayer->m_MapDef; // L"Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition";
  
  
  return datalayer;
  

}


/*
bool c_RestMapNameToResourceId::GetRestUriParamForClass( const STRING& ClassName,STRING& RestUriParam )
{
  c_RestConfig* cfg_rest = c_RestConfig::GetInstance(); 
  const c_CfgDataResource * datalayer = cfg_rest->FindClassName(ClassName.c_str());
  if( !datalayer ) return false;
  
  RestUriParam = datalayer->m_UriTag;
  return true;    
}


const c_CfgDataResource* c_RestMapNameToResourceId::GetCfgDataForClass( const STRING& ClassName)
{
  c_RestConfig* cfg_rest = c_RestConfig::GetInstance(); 
  return cfg_rest->FindClassName(ClassName.c_str());
  
}
*/
