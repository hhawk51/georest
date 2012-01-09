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


#include "StdAfx.h"
#include "c_CfgDataSource.h"

c_CfgDataSource::c_CfgDataSource(c_CfgDataSource::e_SourceType SourceType)
{
  m_SourceType = SourceType;
}

c_CfgDataSource::~c_CfgDataSource(void)
{
}

c_CfgDataSource_FDO::c_CfgDataSource_FDO(  )
: c_CfgDataSource(c_CfgDataSource::e_FDO)
{
  
}

c_CfgDataSource_MapGuide::c_CfgDataSource_MapGuide( e_SourceType SourceType )
  : c_CfgDataSource(SourceType)
{
  m_ServerIP = L"127.0.0.1";
  m_ServerPort = 2812;
  
  m_Username = L"Anonymous";
  m_Password = L"";
}

c_CfgDataSource_MapGuide::~c_CfgDataSource_MapGuide()
{

}


c_CfgDataSource_MgLayerDefinition::c_CfgDataSource_MgLayerDefinition( const std::wstring& MgLayerDefinition )
  : c_CfgDataSource_MapGuide(c_CfgDataSource::e_MgLayerDefinition)
{
  m_MgLayerDefinition = MgLayerDefinition;
}



c_CfgDataSource_MgLayerDefinition::~c_CfgDataSource_MgLayerDefinition()
{

}


c_CfgDataSource_MgFeatureSource::c_CfgDataSource_MgFeatureSource( )
  : c_CfgDataSource_MapGuide(c_CfgDataSource::e_MgFeatureSource)
{
  
}

c_CfgDataSource_MgFeatureSource::~c_CfgDataSource_MgFeatureSource()
{

}

