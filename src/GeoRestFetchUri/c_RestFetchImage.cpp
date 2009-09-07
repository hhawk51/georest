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
#include "c_RestFetchImage.h"
#include "c_RestMgSiteConnection.h"


// Static member data
//
const STRING c_RestFetchImage::m_mapCmdSetViewCenterX     = L"SETVIEWCENTERX";
const STRING c_RestFetchImage::m_mapCmdSetViewCenterY     = L"SETVIEWCENTERY";
const STRING c_RestFetchImage::m_mapCmdSetViewScale       = L"SETVIEWSCALE";
const STRING c_RestFetchImage::m_mapCmdSetDataExtent      = L"SETDATAEXTENT";
const STRING c_RestFetchImage::m_mapCmdSetDisplayDpi      = L"SETDISPLAYDPI";
const STRING c_RestFetchImage::m_mapCmdSetDisplayWidth    = L"SETDISPLAYWIDTH";
const STRING c_RestFetchImage::m_mapCmdSetDisplayHeight   = L"SETDISPLAYHEIGHT";
const STRING c_RestFetchImage::m_mapCmdShowLayers         = L"SHOWLAYERS";
const STRING c_RestFetchImage::m_mapCmdHideLayers         = L"HIDELAYERS";
const STRING c_RestFetchImage::m_mapCmdShowGroups         = L"SHOWGROUPS";
const STRING c_RestFetchImage::m_mapCmdHideGroups         = L"HIDEGROUPS";
const STRING c_RestFetchImage::m_mapCmdRefreshLayers      = L"REFRESHLAYERS";


c_RestFetchImage::c_RestFetchImage(void)
{
}

c_RestFetchImage::~c_RestFetchImage(void)
{
}


MgByteReader* c_RestFetchImage::FetchImage(c_RestUri* HttpRequest,c_CfgDataSource_MapGuide* MapGuideSource
                                  , const std::wstring& MapDefId,MgEnvelope* ZoomTo, const std::wstring& ZoomFactor
                                  , const std::wstring& MapSelectionLayer, const std::wstring& FeatureSource, const std::wstring& FdoClassName
                                  , MgFeatureQueryOptions *QueryOptions
                                  )
{
  
  
  Ptr<c_RestUriRequestParam> query_params = HttpRequest-> GetRequestParam();
  
  //Ptr<c_RestUriPathParam> path_params = HttpRequest-> GetUriPathParameters();
  //STRING filter = path_params->GetParameterValue(D_REST_URI_SEGMENT_MAP_IMAGE_PNG);  
  //Ptr<c_RestRequestParam> filterparams = new c_RestRequestParam();
  //RestUriFilterParser::Parse(filter, filterparams);
  
  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(MapGuideSource->GetUsername(),MapGuideSource->GetPassword(),MapGuideSource->GetServerIP(),MapGuideSource->GetServerPort());
  
  
  //MgHtmlController controller(mgsiteconn);
  
  // Get the map view commands
  //Ptr<c_RestRequestParam> params = m_hRequest->GetRequestParam();
  //Ptr<MgPropertyCollection> mapViewCommands = params->GetParameters()->GetPropertyCollection();
  //Ptr<MgPropertyCollection> mapViewCommands = filterparams->GetParameters()->GetPropertyCollection();
  
  
  STRING map_resid;
  STRING mapname;
  Ptr<MgMap> map;
  Ptr<MgSelection> selection;
  
  Ptr<MgPropertyCollection> mapviewcommands = new MgPropertyCollection();
  {
    STRING sessionid;
    Ptr<MgSite> mgSite = mgsiteconn->GetSite();
    sessionid = mgSite->CreateSession();
    
    
    //-------------------------------------------------------
    // Create a new Map from MapDefinition in a Session
    //-------------------------------------------------------  
      map = new MgMap();
      Ptr<MgResourceIdentifier> resid_mapdef = new MgResourceIdentifier(MapDefId);
      
     
      mapname = L"KINGREST";
   
      Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(mgsiteconn->CreateService(MgServiceType::ResourceService));
         
      map->Create(resourceService, resid_mapdef, mapname );  
      // Save the MgMap state
      map_resid = L"Session:" + sessionid + L"//" + mapname + L"." + MgResourceType::Map;
      Ptr<MgResourceIdentifier> resid_map = new MgResourceIdentifier(map_resid);
      map->Save(resourceService,resid_map);
      
      
      selection = new MgSelection(map);
      if( MapSelectionLayer.length()>0 )
      {
        MgResourceIdentifier resId(FeatureSource);
        Ptr<MgFeatureService> service = (MgFeatureService*)(mgsiteconn->CreateService(MgServiceType::FeatureService));
        Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, FdoClassName, QueryOptions);
        
        //Ptr<MgResourceIdentifier> layerresidid = new MgResourceIdentifier(HttpRequest->m_DataLayerConfig->m_MapSelectionLayer);
        //Ptr<MgLayer> layerdef = new  MgLayer(layerid,resourceService);
        Ptr<MgLayerBase> layer;
        Ptr<MgLayerCollection> layers = map->GetLayers();
        
        
        int layerid =layers->IndexOf(MapSelectionLayer);
        if( layerid >= 0 )
        {
          layer = layers->GetItem(layerid);
        }
        //auto_ptr<MdfModel::LayerDefinition> layerdef( MgLayerBase::GetLayerDefinition(resourceService, layerid));         
        selection->AddFeatures(layer,featureReader,0);
        
        featureReader->Close();
      }
      selection->Save(resourceService, mapname);
      
      Ptr<MgReadOnlyLayerCollection> selLayers = selection->GetLayers();
      int sel_layercount = selLayers ? selLayers->GetCount() : 0;
      
      MgStringPropertyCollection* coll_params = query_params->GetParameters();
      
      int dwidth=260;
      int dheight=260;
      int dpi = 96;
      
      STRING strval;
      Ptr<MgStringProperty> prop;
      
      if( coll_params->Contains(L"SETDISPLAYWIDTH" ) )
      {
        
        
        STRING val = coll_params->GetValue(L"SETDISPLAYWIDTH");   
        dwidth = MgUtil::StringToDouble(val);     
        if( dwidth <= 0 ) dwidth=260;
      }
      
      
      
      {
        MgUtil::Int32ToString(dwidth,strval);
        prop = new MgStringProperty(L"SETDISPLAYWIDTH",strval);
        mapviewcommands->Add(prop);
      }
      
      if( coll_params->Contains(L"SETDISPLAYHEIGHT" ) )
      {
        STRING val = coll_params->GetValue(L"SETDISPLAYHEIGHT");   
        dheight = MgUtil::StringToDouble(val);     
        if( dheight <= 0 ) dheight=260;
      }
      
      
      
      {
        MgUtil::Int32ToString(dheight,strval);
        prop = new MgStringProperty(L"SETDISPLAYHEIGHT",strval);
        mapviewcommands->Add(prop);
      }
      
      if( coll_params->Contains(L"SETDISPLAYDPI" ) )
      {
        STRING val = coll_params->GetValue(L"SETDISPLAYDPI");   
        dpi = MgUtil::StringToDouble(val);     
        if( dpi <= 0 ) dpi=96;
      }
      
      
      
      {
        MgUtil::Int32ToString(dpi,strval);
        prop = new MgStringProperty(L"SETDISPLAYDPI",strval);
        mapviewcommands->Add(prop);
      }
      
      Ptr<MgCoordinate> xy1 = ZoomTo->GetLowerLeftCoordinate();
      Ptr<MgCoordinate> xy2 = ZoomTo->GetUpperRightCoordinate();
      
      double cx = ((double)xy1->GetX() + (double)xy2->GetX()) / 2.0;
      double cy = ((double)xy1->GetY() + (double)xy2->GetY()) / 2.0;
      
    
      
      
      MgUtil::DoubleToString(cx,strval);
      prop = new MgStringProperty(L"SETVIEWCENTERX",strval);
      mapviewcommands->Add(prop);
      
      MgUtil::DoubleToString(cy,strval);
      prop = new MgStringProperty(L"SETVIEWCENTERY",strval);
      mapviewcommands->Add(prop);
      
      double zoomfactor=1.3;
      if( coll_params->Contains(L"ZOOMFACTOR" ) )
      {
        STRING val = coll_params->GetValue(L"ZOOMFACTOR");   
        zoomfactor = MgUtil::StringToDouble(val);     
        if( zoomfactor <= 0.0 ) zoomfactor = 1.3;
      }
      else
      {
        if( ZoomFactor.length() > 0 )
        {
          zoomfactor = MgUtil::StringToDouble(ZoomFactor);     
          if( zoomfactor <= 0.0 ) zoomfactor = 1.3;
        }
      }
      
      double scale;
      double mcsH = xy2->GetY() - xy1->GetY();
      double mcsW = xy2->GetX() - xy1->GetX();
      
      mcsH = mcsH * zoomfactor;
      mcsW = mcsW * zoomfactor;
           
      double metersperpixel  = 0.0254 / dpi;
                        
      if (dheight * mcsW > dwidth * mcsH)
          scale = mcsW * map->GetMetersPerUnit() / (dwidth * metersperpixel); // width-limited
      else
          scale = mcsH * map->GetMetersPerUnit() / (dheight * metersperpixel); // height-limited
          
          
      MgUtil::DoubleToString(scale,strval);
      prop = new MgStringProperty(L"SETVIEWSCALE",strval);
      mapviewcommands->Add(prop);
    
  }
  
  Ptr<MgByteReader> mapimage = GetDynamicMapOverlayImage_ViewCommands(mgsiteconn,mapname, MgImageFormats::Png, true,mapviewcommands);
  //Ptr<MgByteReader> mapimage = controller.GetMapImage(map,selection, MgImageFormats::Png, mapviewcommands,true,true);

  // Set the result
  //Ptr<c_RestResult> result;
  //result = HttpResponse.GetResult();
  //result->SetResultObject(mapimage, mapimage->GetMimeType());
  return mapimage;
	
}//end of c_RestFetchImage::FetchImage

//////////////////////////////////////////////////////////////////
// Processes a GetDynamicMapOverlayImage request from the Viewer and returns an image of the specified map.
//
MgByteReader* c_RestFetchImage::GetDynamicMapOverlayImage_ViewCommands(c_RestMgSiteConnection* MgSiteConn,CREFSTRING mapName, CREFSTRING format, bool bKeepSelection,MgPropertyCollection* mapViewCommands)
{
	// Create a Resource Service instance
	Ptr<MgResourceService> resourceService = (MgResourceService*)MgSiteConn->CreateService(MgServiceType::ResourceService);

	// Create MgMap
	Ptr<MgMap> map = new MgMap();
	map->Open(resourceService, mapName);

	// Apply map view commands
	ApplyMapViewCommands(map, mapViewCommands);

	// Make sure we clear any track changes - these are not applicable for AJAX.
	Ptr<MgNamedCollection> changeLists = map->GetChangeLists();
	if (changeLists->GetCount() > 0)
	{
		map->ClearChanges();
		map->Save(resourceService);
	}

	// Get the selection
	Ptr<MgSelection> selection = new MgSelection(map);
	selection->Open(resourceService, mapName);

	// Create Proxy Rendering Service instance
	Ptr<MgRenderingService> service = (MgRenderingService*)(MgSiteConn->CreateService(MgServiceType::RenderingService));

	// Call the C++ API
	return service->RenderDynamicOverlay(map, selection, format, bKeepSelection);
}//end of c_RestFetchImage::GetDynamicMapOverlayImage_ViewCommands


//////////////////////////////////////////////////////////////////
// Apply the specified set of commands to the map.
//
void c_RestFetchImage::ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands)
{
	if(mapViewCommands == NULL)
		return;

	Ptr<MgProperty> val;

	//Set view center X
	val = mapViewCommands->FindItem(m_mapCmdSetViewCenterX);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetViewCenterX);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		Ptr<MgPoint> center = map->GetViewCenter();
		Ptr<MgCoordinate> coord = center? center->GetCoordinate(): NULL;
		double y = coord? coord->GetY(): 0.;
		MgGeometryFactory gf;
		Ptr<MgCoordinate> coordNewCenter = gf.CreateCoordinateXY(MgUtil::StringToDouble(((MgStringProperty*)((MgProperty*)val))->GetValue()), y);
		Ptr<MgPoint> ptNewCenter = gf.CreatePoint(coordNewCenter);
		map->SetViewCenter(ptNewCenter);
	}

	//Set view center Y
	val = mapViewCommands->FindItem(m_mapCmdSetViewCenterY);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetViewCenterY);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		Ptr<MgPoint> center = map->GetViewCenter();
		Ptr<MgCoordinate> coord = center? center->GetCoordinate(): NULL;
		double x = coord? coord->GetX(): 0.;
		MgGeometryFactory gf;
		Ptr<MgCoordinate> coordNewCenter = gf.CreateCoordinateXY(x, MgUtil::StringToDouble(((MgStringProperty*)((MgProperty*)val))->GetValue()));
		Ptr<MgPoint> ptNewCenter = gf.CreatePoint(coordNewCenter);
		map->SetViewCenter(ptNewCenter);
	}

	//Set view scale
	val = mapViewCommands->FindItem(m_mapCmdSetViewScale);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetViewScale);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		STRING sScale = ((MgStringProperty*)((MgProperty*)val))->GetValue();
		double scale = MgUtil::StringToDouble(sScale);
		if(scale <= 0.0)
		{
			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetViewScale);
			arguments.Add(sScale);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanOrEqualToZero", NULL);
		}

		map->SetViewScale(scale);

		//refresh all layers that are visible
		map->SetLayerRefreshMode(MgMap::refreshAll);
	}

	//Set display dpi
	val = mapViewCommands->FindItem(m_mapCmdSetDisplayDpi);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetDisplayDpi);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		STRING sDpi = ((MgStringProperty*)((MgProperty*)val))->GetValue();
		INT32 dpi = MgUtil::StringToInt32(sDpi);
		if(dpi <= 0)
		{
			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetDisplayDpi);
			arguments.Add(sDpi);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanOrEqualToZero", NULL);
		}

		map->SetDisplayDpi(dpi);
	}

	//Set display width
	val = mapViewCommands->FindItem(m_mapCmdSetDisplayWidth);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetDisplayWidth);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		STRING sWidth = ((MgStringProperty*)((MgProperty*)val))->GetValue();
		INT32 width = MgUtil::StringToInt32(sWidth);
		if(width <= 0)
		{
			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetDisplayWidth);
			arguments.Add(sWidth);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanOrEqualToZero", NULL);
		}

		map->SetDisplayWidth(width);
	}

	//Set display height
	val = mapViewCommands->FindItem(m_mapCmdSetDisplayHeight);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetDisplayHeight);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		STRING sHeight = ((MgStringProperty*)((MgProperty*)val))->GetValue();
		INT32 height = MgUtil::StringToInt32(sHeight);
		if(height <= 0)
		{
			MgStringCollection arguments;
			arguments.Add(m_mapCmdSetDisplayHeight);
			arguments.Add(sHeight);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanOrEqualToZero", NULL);
		}

		map->SetDisplayHeight(height);
	}

	//Show layers
	val = mapViewCommands->FindItem(m_mapCmdShowLayers);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdShowLayers);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		ShowLayers(map, ((MgStringProperty*)((MgProperty*)val))->GetValue(), true);
	}

	//Hide layers
	val = mapViewCommands->FindItem(m_mapCmdHideLayers);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdHideLayers);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		ShowLayers(map, ((MgStringProperty*)((MgProperty*)val))->GetValue(), false);
	}

	//Show groups
	val = mapViewCommands->FindItem(m_mapCmdShowGroups);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdShowGroups);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		ShowGroups(map, ((MgStringProperty*)((MgProperty*)val))->GetValue(), true);
	}

	//Hide groups
	val = mapViewCommands->FindItem(m_mapCmdHideGroups);
	if(val != NULL)
	{
		if(val->GetPropertyType() != MgPropertyType::String)
		{
			STRING buffer;
			MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

			MgStringCollection arguments;
			arguments.Add(m_mapCmdHideGroups);
			arguments.Add(buffer);

			throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
				__LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
		}

		ShowGroups(map, ((MgStringProperty*)((MgProperty*)val))->GetValue(), false);
	}

	//Refresh layers - applies only to DwfController
}//end of c_RestFetchImage::ApplyMapViewCommands

//////////////////////////////////////////////////////////////////
// Show or Hide a set of layers in the specified map.
//
void c_RestFetchImage::ShowLayers(MgMap* map, CREFSTRING strLayers, bool show)
{
	Ptr<MgStringCollection> layerIds = MgStringCollection::ParseCollection(strLayers, L",");
	if(layerIds != NULL && layerIds->GetCount() > 0)
	{
		Ptr<MgLayerCollection> layers = map->GetLayers();
		for(INT32 index = 0; index < layerIds->GetCount(); index++)
		{
			Ptr<MgLayer> layer;
			STRING id = layerIds->GetItem(index);

			for(INT32 li = 0; li < layers->GetCount(); li++)
			{
				layer = (MgLayer*)layers->GetItem(li);
				if(!layer->GetObjectId().compare(id))
				{
					if (layer->GetLayerType() != MgLayerType::BaseMap)
						layer->SetVisible(show);

					break;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////
// Show or Hide a set of groups in the specified map.
//
void c_RestFetchImage::ShowGroups(MgMap* map, CREFSTRING strGroups, bool show)
{
	Ptr<MgStringCollection> groupIds = MgStringCollection::ParseCollection(strGroups, L",");
	if(groupIds != NULL && groupIds->GetCount() > 0)
	{
		Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
		for(INT32 index = 0; index < groupIds->GetCount(); index++)
		{
			Ptr<MgLayerGroup> group;
			STRING id = groupIds->GetItem(index);

			for(INT32 gi = 0; gi < groups->GetCount(); gi++)
			{
				group = (MgLayerGroup*)groups->GetItem(gi);
				if(!group->GetObjectId().compare(id))
				{
					group->SetVisible(show);
					break;
				}
			}
		}
	}
}
