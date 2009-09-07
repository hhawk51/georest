//
//  Copyright (C) 2007 by Haris Kurtagic
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
//#include "WebApp.h"
#include "RestSessionMap.h"
#include "RestDefs.h"

#include "VectorLayerDefinition.h"
#include "DrawingLayerDefinition.h"
#include "GridLayerDefinition.h"


class c_Scale_StyleRuleData
{
public:
  c_Scale_StyleRuleData()
  {
    m_ThemeCategory=0;
    m_GeomType=-1;
  }
  
  std::wstring m_RuleLegendLabel;
  std::wstring m_RuleFilterText;

  int m_ThemeCategory; // Index of this type style in a list of style's; This one is need for creatinf Legend Image I believe
  int m_GeomType;

  std::string m_LegendIconUri;
};
class c_LayerDef_ScaleData
{
public:
  c_LayerDef_ScaleData()
  {
    m_ScaleMin = 0;
    m_ScaleMax = MdfModel::VectorScaleRange::MAX_MAP_SCALE;
    
  }
  
public:
  void Add(const c_Scale_StyleRuleData& Data)
  {
    m_Scale_StyleRuleData.push_back(Data);
  }
public:  
  double m_ScaleMin;
  double m_ScaleMax;
  
  std::vector<c_Scale_StyleRuleData> m_Scale_StyleRuleData;
      
};

class c_TreeNode_MgLayerGroup
{
public:
  enum e_NodeType {
    e_Unknown,
    e_Layer,
    e_LayerGroup
  };   
  
  public:
    c_TreeNode_MgLayerGroup(MgLayerGroup* Group)
    {
      m_NodeType = e_LayerGroup;
      m_MgLayerGroup = SAFE_ADDREF(Group);
    }
    c_TreeNode_MgLayerGroup(MgLayer* Layer)
    {
      m_NodeType = e_Layer;
      m_MgLayer = SAFE_ADDREF(Layer);
    }
    
  public:
    void Add(c_LayerDef_ScaleData& Data)
    {
      m_LayerDef_ScaleData.push_back(Data);
    }
        
  public:
    std::vector<c_TreeNode_MgLayerGroup> m_Childs;
    e_NodeType m_NodeType;
    
    Ptr<MgLayerGroup> m_MgLayerGroup;
    Ptr<MgLayer> m_MgLayer;
    // For Layer I need additional data about their scale ranges and geometry styles
    std::vector<c_LayerDef_ScaleData> m_LayerDef_ScaleData;
};

class c_Tree_MgLayerGroup
{
  public:
    bool Add( MgLayerGroup* ParentGroup,c_TreeNode_MgLayerGroup& Node )
    {
      if( !ParentGroup )
      {
        m_Roots.push_back(Node);
        return true;
      }
      std::vector<c_TreeNode_MgLayerGroup>::iterator iter = m_Roots.begin();
      while( iter != m_Roots.end() )  
      {
        if( iter->m_NodeType == c_TreeNode_MgLayerGroup::e_LayerGroup )  
        {
          if( iter->m_MgLayerGroup == ParentGroup )
          {          
            iter->m_Childs.push_back(Node);
            return true;
          }
          if( AddToChild(*iter,ParentGroup,Node) )
          {
            return true;
          }
        }
        iter++;        
      }
      
      return false;
    }
    
    void ToJson(MgJsonDoc& JsonDoc,const char* Name,MgRestUriFactory* RestUriFactory)
    {
      std::vector<c_TreeNode_MgLayerGroup>::iterator iter = m_Roots.begin();
      
      JsonDoc.BeginArray(Name);
      
      while( iter != m_Roots.end() )  
      {      
        ToJson_Node(JsonDoc,*iter,RestUriFactory);
        iter++;
      }
      
      JsonDoc.EndArray();
    }
    
  public:
    std::vector<c_TreeNode_MgLayerGroup> m_Roots;
    
  protected:
    bool AddToChild(c_TreeNode_MgLayerGroup& ParentNode, MgLayerGroup* ParentGroup,c_TreeNode_MgLayerGroup& Node)    
    {
      std::vector<c_TreeNode_MgLayerGroup>::iterator iter = ParentNode.m_Childs.begin();
      while( iter != ParentNode.m_Childs.end() )  
      {        
        if( iter->m_NodeType == c_TreeNode_MgLayerGroup::e_LayerGroup )  
        {
          if( iter->m_MgLayerGroup == ParentGroup )
          {
            iter->m_Childs.push_back(Node);
            return true;
          }
          
          if( AddToChild(*iter,ParentGroup,Node) )
          {
            return true;
          }
        }   
        iter++;                
      }
      
      return false;
    }
    
    void ToJson_Node(MgJsonDoc& JsonDoc,c_TreeNode_MgLayerGroup& Node,MgRestUriFactory* RestUriFactory)
    {
      JsonDoc.BeginAppendArrayObject();  
      
      switch( Node.m_NodeType)
      {
        case c_TreeNode_MgLayerGroup::e_LayerGroup:
          if( Node.m_MgLayerGroup.p )
          {
            JsonDoc.Add("m_Type","Group");
            JsonDoc.Add("m_Name",Node.m_MgLayerGroup->GetName());          
            JsonDoc.Add("m_ObjectId",Node.m_MgLayerGroup->GetObjectId());
            JsonDoc.Add("m_Visible",Node.m_MgLayerGroup->GetVisible());
            JsonDoc.Add("m_IsVisible",Node.m_MgLayerGroup->IsVisible());
            JsonDoc.Add("m_DisplayInLegend",Node.m_MgLayerGroup->GetDisplayInLegend());
            JsonDoc.Add("m_ExpandInLegend",Node.m_MgLayerGroup->GetExpandInLegend());
            JsonDoc.Add("m_LegendLabel",Node.m_MgLayerGroup->GetLegendLabel());
          }
        break;
        case c_TreeNode_MgLayerGroup::e_Layer:
          if( Node.m_MgLayer.p )
          {                    
            JsonDoc.Add("m_Type","Layer");
            JsonDoc.Add("m_Name",Node.m_MgLayer->GetName());          
            JsonDoc.Add("m_ObjectId",Node.m_MgLayer->GetObjectId());
            
            // Feature Data Source Resource Id
            JsonDoc.Add("m_DataSource",Node.m_MgLayer->GetFeatureSourceId());
            JsonDoc.Add("m_ClassName",Node.m_MgLayer->GetFeatureClassName());
            std::string tempstr;
            RestUriFactory->Create_FDO_DataSource_Class_Feature(Node.m_MgLayer->GetFeatureSourceId(),Node.m_MgLayer->GetFeatureClassName(),tempstr);
            JsonDoc.Add("m_ClassFeatureUri",tempstr);
            JsonDoc.Add("m_FeatureGeomName",Node.m_MgLayer->GetFeatureGeometryName());
            
            
            JsonDoc.Add("m_Visible",Node.m_MgLayer->GetVisible());          
            JsonDoc.Add("m_IsVisible",Node.m_MgLayer->IsVisible());
            JsonDoc.Add("m_DisplayInLegend",Node.m_MgLayer->GetDisplayInLegend());
            JsonDoc.Add("m_ExpandInLegend",Node.m_MgLayer->GetExpandInLegend());
            JsonDoc.Add("m_LegendLabel",Node.m_MgLayer->GetLegendLabel());
            JsonDoc.Add("m_Selectable",Node.m_MgLayer->GetSelectable());
            
            if( Node.m_LayerDef_ScaleData.size() > 0 )
            {
              //c_LayerDef_ScaleStyleRuleData
              JsonDoc.BeginArray("m_ScaleData");
              for( std::vector<c_LayerDef_ScaleData>::iterator scaleiter = Node.m_LayerDef_ScaleData.begin();scaleiter!=Node.m_LayerDef_ScaleData.end();scaleiter++)
              {
                JsonDoc.BeginAppendArrayObject();  
                JsonDoc.Add("m_Type","ScaleData");
                JsonDoc.Add("m_ScaleMin",scaleiter->m_ScaleMin);
                JsonDoc.Add("m_ScaleMax",scaleiter->m_ScaleMax);
                
                JsonDoc.BeginArray("m_RuleData");
                
                for( std::vector<c_Scale_StyleRuleData>::iterator ruleiter = scaleiter->m_Scale_StyleRuleData.begin();ruleiter!=scaleiter->m_Scale_StyleRuleData.end();ruleiter++)
                {
                  JsonDoc.BeginAppendArrayObject(); 
                  
                  JsonDoc.Add("m_Type","RuleData");
                  JsonDoc.Add("m_ScaleMin",scaleiter->m_ScaleMin);
                  JsonDoc.Add("m_ScaleMax",scaleiter->m_ScaleMax); 
                  JsonDoc.Add("m_RuleLegendLabel",ruleiter->m_RuleLegendLabel);
                  //JsonDoc.Add("m_RuleFilterText",ruleiter->m_RuleFilterText);
                  JsonDoc.Add("m_ThemeCategory",ruleiter->m_ThemeCategory);
                  JsonDoc.Add("m_GeomType",ruleiter->m_GeomType);
                  JsonDoc.Add("m_LegendIconUri",ruleiter->m_LegendIconUri);
                  JsonDoc.EndAppendArrayObject();
                }
                JsonDoc.EndArray();
                
                //JsonDoc.Add("m_SSR_RuleLegendLabel",iter->m_RuleLegendLabel);
                //JsonDoc.Add("m_SSR_RuleFilterText",iter->m_RuleFilterText);
                //JsonDoc.Add("m_SSR_LegendIconUri",iter->m_LegendIconUri);
                
                JsonDoc.EndAppendArrayObject();
                
              }
              JsonDoc.EndArray();
            }
            else
            {
              JsonDoc.AddEmptyObject("m_ScaleData");
            }
          }
        break;
      }
      
      if( Node.m_Childs.size() > 0 )
      {
        JsonDoc.BeginArray("m_Children");
        for( std::vector<c_TreeNode_MgLayerGroup>::iterator iter = Node.m_Childs.begin();iter!=Node.m_Childs.end();iter++)
        {
          ToJson_Node(JsonDoc,*iter,RestUriFactory);
        }
        JsonDoc.EndArray();
      }
      else
      {
        JsonDoc.AddEmptyObject("m_Children");
      }
      
      
      JsonDoc.EndAppendArrayObject();
    }
};



MgRestRequestResponseHandler* MgRestTask::CreateObject(MgRestRequest* request) 
{ 
  return new MgRestTask(request); 
}



/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="name">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
/// <returns>
/// nothing
/// </returns>
MgRestTask::MgRestTask(MgRestRequest *Request)
{
    InitializeCommonParameters(Request);
    
    Ptr<MgHttpRequestParam> query_params = Request->GetRequestParam();
    
    // Get any map name
    m_WebLayout = query_params->GetParameterValue(D_REST_URI_QUERY_PARAM_WEBLAYOUT);

    

}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgRestTask::Execute(MgHttpResponse& HttpResponse)
{
  Ptr<MgHttpResult> hResult = HttpResponse.GetResult();
  Ptr<MgHttpRequestParam> query_params = m_hRequest->GetRequestParam();

  MG_HTTP_HANDLER_TRY()

    //
    // 1. URI for getting data about MAP
    // HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]
    //
    // 2. URI creating new MAP in Session
    // HttpMethod: POST URI: REST/SESSION[Id]/MAP ? MapDefName={Map Definition Name}
    // 
    // 3. Get Image Extent
    // HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]/Extent
    //
    // 4. Get Image ( return png )
    // HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]/Image.Png[...ImageCreationParameters...] {.jpg }
    //
    // 5. Set Image Extent
    // HttpMethod: PUT URI: REST/SESSION[Id]/MAP[Id]/Extent ? {SETCETNTERX=..}&{SETCENTERY=..}&..   
    //

  // Current Index should be on Map path parameter
  Ptr<MgRestUriPathParam> path_params = m_RestRequest-> GetUriPathParameters();
  
  switch( m_RestRequest->GetHttpMethod() )
  {
    case MgRestRequest::e_HttpMethod_Get:
    {
   
    }
    break;
       
    case MgRestRequest::e_HttpMethod_Post:
    {
      //Execute_Post_StartWebLayout(HttpResponse);
    }
    break;
    
    case MgRestRequest::e_HttpMethod_Put:
    break;
    
    case MgRestRequest::e_HttpMethod_Delete:
    break;
    
    default:
    break;
  }

  
  Execute_Post_StartWebLayout(HttpResponse);


  MG_CATCH(L"REST_Request_Hello.Execute")                                                      

    if (mgException != NULL)                                                  
    {                                                                         
      if (hResult != NULL)                                                  
      {                                                                      
        hResult->SetErrorInfo(m_hRequest, mgException);                   
      }                                                                     
      (*mgException).AddRef();                                              
      mgException->Raise();                                                 
    } 


}


void MgRestTask::Execute_Post_StartWebLayout(MgHttpResponse& HttpResponse)
{
  Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(CreateService(MgServiceType::ResourceService));

  bool openedMap = false;
  
// Open MgWebLayout
  Ptr<MgResourceIdentifier> resid_weblayout = new MgResourceIdentifier(m_WebLayout);
  Ptr<MgWebLayout> weblayout = new MgWebLayout(resourceService,resid_weblayout);

  MgJsonDoc jsondoc;
  
  STRING ws_mapdefname = weblayout->GetMapDefinition();
  
  
  std::string baseuri = m_RestRequest->GetBaseUri();
  
  jsondoc.Add("m_BaseUri",baseuri);   
  jsondoc.Add("m_MapDefName",ws_mapdefname);   // like Library://XE/Map_Nanaimo.MapDefinition
  
  std::string tempstr;
  m_RestUriFactory->Create_MapDefinition(ws_mapdefname,tempstr);
  jsondoc.Add("m_MapDefNameUri",tempstr);   // REST/MapDefinition[Library://XE/Map_Nanaimo.MapDefinition]
  
  
  
//-------------------------------------------------------
// Check if Session is defined if not create a new one
//-------------------------------------------------------
  Ptr<MgUserInformation> userinfo = resourceService->GetUserInfo();
  
  STRING ws_sessionid = userinfo->GetMgSessionId();
  
  if( ws_sessionid.length() == 0 )
  {
    Ptr<MgSite> mgSite = m_siteConn->GetSite();
    ws_sessionid = mgSite->CreateSession();
    userinfo->SetMgSessionId(ws_sessionid);
  }

  jsondoc.Add("m_Session",ws_sessionid);   
  m_RestUriFactory->Create_Session(ws_sessionid,tempstr);
  jsondoc.Add("m_SessionUri",tempstr);   // REST/MapDefinition[Library://XE/Map_Nanaimo.MapDefinition]
  
//-------------------------------------------------------
// Create a new Map from MapDefinition in a Session
//-------------------------------------------------------  
  Ptr<MgMap> map = new MgMap();
  Ptr<MgResourceIdentifier> resid_mapdef = new MgResourceIdentifier(ws_mapdefname);
  
  STRING ws_mapname = resid_mapdef->GetName(); // name of the Map; needs to be unique inside session
  STRING uuid;
  MgUtil::GenerateUuid(uuid);
  ws_mapname.append(L"-");
  ws_mapname.append(uuid);
  
  map->Create(resourceService, resid_mapdef, ws_mapname );  
  // Save the MgMap state
  Ptr<MgResourceIdentifier> resid_map = new MgResourceIdentifier(L"Session:" + ws_sessionid + L"//" + ws_mapname + L"." + MgResourceType::Map);
  map->Save(resourceService,resid_map);
  
  jsondoc.Add("m_MapDisplayName",resid_mapdef->GetName());   // like Library://XE/Map_Nanaimo.MapDefinition
  
//-------------------------------------------------------
// Read Layer defintions
//-------------------------------------------------------    
  
//-------------------------------------------------------
// Create Tree of Map Layers/Groups
//-------------------------------------------------------  
  c_Tree_MgLayerGroup tree_mglayergroup;
  Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
  for(int ind=0;ind<groups->GetCount();ind++)
  {
    Ptr<MgLayerGroup> group = groups->GetItem(ind);
    Ptr<MgLayerGroup> parentgroup = group->GetGroup();
    
    c_TreeNode_MgLayerGroup node(group);
    
    tree_mglayergroup.Add(parentgroup,node);
    
    STRING name = group->GetName();
    
  
    
  }
  
  Ptr<MgLayerCollection> layers = map->GetLayers();
  for(int ind=0;ind<layers->GetCount();ind++)
  {
    Ptr<MgLayer> layer = (MgLayer*)layers->GetItem(ind);
    STRING name = layer->GetName();
    Ptr<MgLayerGroup> parentgroup = layer->GetGroup();
    
    c_TreeNode_MgLayerGroup node(layer);
    
  
    AddStyleRanges(node,resourceService,baseuri);
    
    tree_mglayergroup.Add(parentgroup,node);
 
  }
  
 // Now create JSON  of it
  tree_mglayergroup.ToJson(jsondoc,"m_LayersGroups",m_RestUriFactory);
  
//-------------------------------------------------------
// Write Map data like Extent, Meters per Unit, Scale, CenterX, Center Y
//------------------------------------------------------- 
  Ptr<MgEnvelope> envelope = map->GetMapExtent();
  Ptr<MgCoordinate> lowerleft = envelope->GetLowerLeftCoordinate();
  
 
  jsondoc.Add("m_MapExtentMinX",lowerleft->GetX());   
  
  jsondoc.Add("m_MapExtentMinY",lowerleft->GetY());   
  Ptr<MgCoordinate> upperright = envelope->GetUpperRightCoordinate();
  
  jsondoc.Add("m_MapExtentMaxX",upperright->GetX());   
  
  jsondoc.Add("m_MapExtentMaxY",upperright->GetY());   

  // meters per unit  
  
  jsondoc.Add("m_MapMetersPerUnit",map->GetMetersPerUnit());   
  // DPI  
  
  jsondoc.Add("m_MapDisplayDpi",map->GetDisplayDpi());     
  
  // Center X,Y
  Ptr<MgPoint> center = map->GetViewCenter();
  Ptr<MgCoordinate> centercoordinate = center->GetCoordinate();
  
  jsondoc.Add("m_MapViewCenterX",centercoordinate->GetX());     
  
  jsondoc.Add("m_MapViewCenterY",centercoordinate->GetY());     
  
  // View Scale    
  jsondoc.Add("m_MapViewScale",map->GetViewScale());     

//-------------------------------------------------------
// Create a Selection Set for Map 
//-------------------------------------------------------    
  Ptr<MgSelection> selection;
  selection = new MgSelection(map);
  selection->Save(resourceService, ws_mapname);
    
  jsondoc.Add("m_Map",ws_mapname);   
  m_RestUriFactory->Create_Session_Map(ws_sessionid,ws_mapname,tempstr);
  jsondoc.Add("m_MapUri",tempstr);   // REST/MapDefinition[Library://XE/Map_Nanaimo.MapDefinition]

  m_RestUriFactory->Create_Session_Map_ImagePng(ws_sessionid,ws_mapname,tempstr);
  jsondoc.Add("m_MapImageUri",tempstr);   
  
  m_RestUriFactory->Create_Session_Map_QueryMapFeatures(ws_sessionid,ws_mapname,tempstr);
  jsondoc.Add("m_QueryMapFeaturesUri",tempstr);   
  

//-------------------------------------------------------
// Set JSon string as result
//-------------------------------------------------------  

  string json_string;
  
  jsondoc.PrintFast(json_string);
  //jsondoc.Print(json_string);
     
  STRING json_widestring;
  MgUtil::MultiByteToWideChar(json_string, json_widestring);
  Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(json_widestring);
  
  Ptr<MgHttpResult> result;
  result = HttpResponse.GetResult();
  result->SetResultObject(value, MgMimeType::JsonP);
    
}//end of MgRestTask::Execute_Post_StartWebLayout

// Build style range for Layers
void MgRestTask::AddStyleRanges(c_TreeNode_MgLayerGroup& Node,MgResourceService* resourceService,const std::string& BaseUri)
{
  MgLayer* layer = Node.m_MgLayer;
  
  Ptr<MgResourceIdentifier> layerid = layer->GetLayerDefinition();
  auto_ptr<MdfModel::LayerDefinition> layerdef( MgLayerBase::GetLayerDefinition(resourceService, layerid));  
  if(layerdef.get() != NULL)
  {
    MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(layerdef.get());
    MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(layerdef.get());
    MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(layerdef.get());

    // Vector Layer
    if(vl != NULL)
    {
      //does the layer have tooltips?
      //we include layers with hyperlinks, since the presence of a hyperlink
      //results in a tooltip
      //m_hasTooltips = (!vl->GetToolTip().empty()) || (!vl->GetUrl().empty());

      //store the scale ranges
      MdfModel::VectorScaleRangeCollection* scalerange_coll = vl->GetScaleRanges();
      for (int indscale=0; indscale < scalerange_coll->GetCount(); indscale++)
      {
        MdfModel::VectorScaleRange* scalerange = scalerange_coll->GetAt(indscale);
        //m_scaleRanges.push_back(scaleRange->GetMinScale());
        //m_scaleRanges.push_back(scaleRange->GetMaxScale());
        
        c_LayerDef_ScaleData scale_data;
        scale_data.m_ScaleMin = scalerange->GetMinScale();
        scale_data.m_ScaleMax = scalerange->GetMaxScale();
        
        scalerange->GetFeatureTypeStyles();
        MdfModel::FeatureTypeStyleCollection* style_coll = scalerange->GetFeatureTypeStyles();
        
        for (int indstyle=0; indstyle<style_coll->GetCount(); indstyle++)
        {
          MdfModel::FeatureTypeStyle* style = style_coll->GetAt(indstyle);
          
          MdfModel::RuleCollection* rule_coll = style->GetRules();
          for (int indrule=0; indrule<rule_coll->GetCount(); indrule++)
          {
            MdfModel::Rule* rule = rule_coll->GetAt(indrule);
            
            c_Scale_StyleRuleData stylerule_data;
            
            stylerule_data.m_RuleLegendLabel = rule->GetLegendLabel();
            stylerule_data.m_RuleFilterText = rule->GetFilter();
            stylerule_data.m_ThemeCategory = indrule;
            
            // TODO: Need to set geometry type for layer to display appropriate legend icon
            // Unable to determine geometry type from dfModel::FeatureTypeStyle
            // Could go to pick up Class Definition and check Geometry there ( but that requires to go to server again fro Class Definition)
            // Viewers (PHP,JAVA,..) are taking Rule Style Type (point,line,polygon,composite) from XML
            // and than converting that style to Point,Line,...
            // But MgServerMappingService::GenerateLegendImage requires that real geometry type of feature is sent to it.
            // Now it is working but it would be problem for composite style (or not beacuse always takes first one )
            stylerule_data.m_GeomType = -1;
            
            
            
            m_RestUriFactory->Create_LayerDef_LegendIcon(layerid->ToString(),scale_data.m_ScaleMin,stylerule_data.m_ThemeCategory,stylerule_data.m_GeomType,stylerule_data.m_LegendIconUri);      
            
            stylerule_data.m_LegendIconUri = BaseUri + stylerule_data.m_LegendIconUri;
            scale_data.Add(stylerule_data);
          }
        }
        
         Node.Add(scale_data);
      }

      //get the feature name
      //m_featureName = vl->GetFeatureName();

      //get the geometry property
      //m_geometry = vl->GetGeometry();
    }
    // Drawing Layer
    else if(dl != NULL)
    {
      c_LayerDef_ScaleData scale_data;
      scale_data.m_ScaleMin = dl->GetMinScale();
      scale_data.m_ScaleMax = dl->GetMaxScale();
      
      c_Scale_StyleRuleData rule;
      scale_data.Add(rule);
      m_RestUriFactory->Create_LayerDef_LegendIcon(layerid->ToString(),scale_data.m_ScaleMin,-1,-1,rule.m_LegendIconUri);      
      rule.m_LegendIconUri = BaseUri + rule.m_LegendIconUri;
      
      Node.Add(scale_data);
    }
    // Grid Layer
    else if(gl != NULL)
    {
      c_LayerDef_ScaleData scale_data;
      
      c_Scale_StyleRuleData rule;
      scale_data.Add(rule);
      m_RestUriFactory->Create_LayerDef_LegendIcon(layerid->ToString(),scale_data.m_ScaleMin,-1,-1,rule.m_LegendIconUri);      
      rule.m_LegendIconUri = BaseUri + rule.m_LegendIconUri;
      
      Node.Add(scale_data);
      
      /*
      //store the scale ranges
      MdfModel::GridScaleRangeCollection* scaleRanges = gl->GetScaleRanges();
      for (int i=0; i < scaleRanges->GetCount(); i++)
      {
        MdfModel::GridScaleRange* scaleRange = scaleRanges->GetAt(i);
        m_scaleRanges.push_back(scaleRange->GetMinScale());
        m_scaleRanges.push_back(scaleRange->GetMaxScale());
      }
      */
      //get the feature name
      //m_featureName = gl->GetFeatureName();

      //get the geometry property
      //m_geometry = gl->GetGeometry();        
    }
  } 
}//end of MgRestTask::AddStyleRanges










/*
stcCallback1001( '   
{
  "m_BaseUri":"/MapGuideREST",
    "m_LayersGroups":
  [{"m_Children":
        [{"m_Children":null,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"Islands","m_Name":"Islands"
          ,"m_ObjectId":"8508186a-ffff-ffff-8000-005056c00008"
          ,"m_ScaleData":[{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}]
        ,"m_ScaleMax":-2147483648,"m_ScaleMin":-2147483648}]
        ,"m_Selectable":false,"m_Type":"Layer","m_Visible":"true"}

        ,{"m_Children":null,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"Hydrography","m_Name":"Hydrography","m_ObjectId":"850a7ac4-ffff-ffff-8000-005056c00008","m_ScaleData":[{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":-2147483648}],"m_Selectable":false,"m_Type":"Layer","m_Visible":"true"}
        ,{"m_Children":null,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"CityLimits","m_Name":"CityLimits","m_ObjectId":"850f3f78-ffff-ffff-8000-005056c00008","m_ScaleData":[{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":10000},{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":-2147483648}],"m_Selectable":false,"m_Type":"Layer","m_Visible":"true"
        }]
    ,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"Base Map","m_Name":"Base Map","m_ObjectId":"84f9ca4e-ffff-ffff-8000-005056c00008"
    ,"m_Type":"Group"
   }
    ,{"m_Children":[{"m_Children":null,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"false","m_LegendLabel":"Districts","m_Name":"Districts","m_ObjectId":"8500f15c-ffff-ffff-8000-005056c00008","m_ScaleData":[{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":-2147483648}],"m_Selectable":false,"m_Type":"Layer","m_Visible":"true"},{"m_Children":null,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"Buildings","m_Name":"Buildings","m_ObjectId":"850353b6-ffff-ffff-8000-005056c00008","m_ScaleData":[{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":1500}],"m_Selectable":false,"m_Type":"Layer","m_Visible":"true"},{"m_Children":null,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"Parcels","m_Name":"Parcels","m_ObjectId":"8505b610-ffff-ffff-8000-005056c00008","m_ScaleData":[{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"RTYPE = 'AGR'","m_RuleLegendLabel":"Zone: AGR","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":10000},{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"\"RTYPE\" = 'AGR'","m_RuleLegendLabel":" 'AGR'","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":-2147483648}],"m_Selectable":true,"m_Type":"Layer","m_Visible":"true"}],"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"Municipal","m_Name":"Municipal","m_ObjectId":"84f9ca4e-ffff-ffff-8001-005056c00008","m_Type":"Group"},{"m_Children":[{"m_Children":null,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"Roads","m_Name":"Roads","m_ObjectId":"84f9ca4e-ffff-ffff-8003-005056c00008","m_ScaleData":[{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":10000},{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":24000}],"m_Selectable":false,"m_Type":"Layer","m_Visible":"true"},{"m_Children":null,"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"false","m_LegendLabel":"Rail Lines","m_Name":"Rail Lines","m_ObjectId":"84fc2ca8-ffff-ffff-8000-005056c00008","m_ScaleData":[{"m_RuleData":[{"m_GeomType":-1,"m_LegendIconUri":"","m_RuleFilterText":"","m_RuleLegendLabel":"","m_ThemeCategory":0}],"m_ScaleMax":-2147483648,"m_ScaleMin":24000}],"m_Selectable":false,"m_Type":"Layer","m_Visible":"false"}],"m_DisplayInLegend":"true","m_ExpandInLegend":"true","m_IsVisible":"true","m_LegendLabel":"Transportation","m_Name":"Transportation","m_ObjectId":"84f9ca4e-ffff-ffff-8002-005056c00008","m_Type":"Group"
  }]  
  ,"m_Map":"Sheboygan-84f767f4-ffff-ffff-8002-005056c00008","m_MapDefName":"Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition","m_MapDefNameUri":"/REST/MapDefinition[Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition]","m_MapDisplayDpi":96,"m_MapDisplayName":"Sheboygan","m_MapExtentMaxX":-87.69552151089972,"m_MapExtentMaxY":43.79752000048035,"m_MapExtentMinX":-87.76498699096284,"m_MapExtentMinY":43.69139812878778,"m_MapImageUri":"/REST/Session[84f767f4-ffff-ffff-8002-005056c00008_en_C0A800100AFC0AFB0AFA]/Map[Sheboygan-84f767f4-ffff-ffff-8002-005056c00008]/Image.Png","m_MapMetersPerUnit":111319.4908,"m_MapUri":"/REST/Session[84f767f4-ffff-ffff-8002-005056c00008_en_C0A800100AFC0AFB0AFA]/Map[Sheboygan-84f767f4-ffff-ffff-8002-005056c00008]","m_MapViewCenterX":-87.73025425093128,"m_MapViewCenterY":43.74445906463406,"m_MapViewScale":1,"m_Session":"84f767f4-ffff-ffff-8002-005056c00008_en_C0A800100AFC0AFB0AFA"
  ,"m_SessionUri":"/REST/Session[84f767f4-ffff-ffff-8002-005056c00008_en_C0A800100AFC0AFB0AFA]"
}

' )
*/