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

#ifndef _c_RestFetchImage_h
#define _c_RestFetchImage_h

#include "c_RestUri.h"

class c_RestMgSiteConnection;

class REST_FETCHURI_API c_RestFetchImage
{
public:
  c_RestFetchImage(void);
public:
  ~c_RestFetchImage(void);
  
  
public:
  static MgByteReader* GetDynamicMapOverlayImage_ViewCommands(c_RestMgSiteConnection* MgSiteConn,CREFSTRING mapName, CREFSTRING format, bool bKeepSelection,MgPropertyCollection* mapViewCommands);
  static void ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands);
  
protected:    
  static MgByteReader* FetchImage(c_RestUri* HttpRequest,c_CfgDataSource_MapGuide* MapGuideSource , const std::wstring& MapDefId,MgEnvelope* ZoomTo, const std::wstring& ZoomFactor , const std::wstring& MapSelectionLayer, const std::wstring& FeatureSource, const std::wstring& FdoClassName , MgFeatureQueryOptions *QueryOptions );
  static void ShowLayers(MgMap* map, CREFSTRING strLayers, bool show);
  static void ShowGroups(MgMap* map, CREFSTRING strGroups, bool show);
  

protected:
	static const STRING     m_mapCmdSetViewCenterX;
	static const STRING     m_mapCmdSetViewCenterY;
	static const STRING     m_mapCmdSetViewScale;
	static const STRING     m_mapCmdSetDataExtent;
	static const STRING     m_mapCmdSetDisplayDpi;
	static const STRING     m_mapCmdSetDisplayWidth;
	static const STRING     m_mapCmdSetDisplayHeight;
	static const STRING     m_mapCmdShowLayers;
	static const STRING     m_mapCmdHideLayers;
	static const STRING     m_mapCmdShowGroups;
	static const STRING     m_mapCmdHideGroups;
	static const STRING     m_mapCmdRefreshLayers;
};

#endif