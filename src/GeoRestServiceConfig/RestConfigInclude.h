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

#ifndef _RestConfigInclude_h
#define _RestConfigInclude_h

#ifdef _WIN32
#ifdef RESTSERVICECONFIG_EXPORTS
#    define REST_CONFIG_API __declspec(dllexport)
#else
#    define REST_CONFIG_API __declspec(dllimport)
#endif
#else
#define REST_CONFIG_API
#endif


#define D_XMLTAG_URITAG "UriTag"
#define D_XMLTAG_RESTURIPART "RestUriPart"

#define D_XMLTAG_SOURCE "Source"

#define D_XMLTAG_MG_FEATURESOURCE "MapGuideFeatureSource"  // those two are in older cfg version
#define D_XMLTAG_MG_CLASSNAME "ClassName"                 // those two are in older cfg version

#define D_XMLTAG_FEATURESOURCE "FeatureSource"
#define D_XMLTAG_FEATURECLASS "FeatureClass"

#define D_XMLTAG_MG_SERVERIP "ServerIp"
#define D_XMLTAG_MG_SERVERPORT "ServerPort"  

#define D_XMLTAG_MG_USERNAME "UserName"
#define D_XMLTAG_MG_PASSWORD "Password"  

#define D_XMLTAG_FDO_PROVIDER "Provider"
#define D_XMLTAG_FDO_CONNECTIONSTRING "ConnectionString"


#define D_XMLTAG_MG_MAPDEF "MapGuideMapDefinition"
#define D_XMLTAG_MAPDEF "MapDefinition"
#define D_XMLTAG_MG_MAP_ZOOM_FACTOR "ZoomFactor"
#define D_XMLTAG_DEFAULT_ZOOM_FACTOR "DefaultZoomFactor"
#define D_XMLTAG_MG_MAP_SELECTION_LAYER "LayerForSelectionInMap"
#define D_XMLTAG_SELECTION_LAYER "SelectionLayer"
#define D_XMLTAG_MG_MAP_FEATURE_SOURCE "MapFeatureSource"
#define D_XMLTAG_MG_MAP_FEATURE_SOURCE_FILTER "MapFeatureSourceFilter"

// CFG V2 Tags for Templates
#define D_XMLTAG_TEMPLATE "Template"
#define D_XMLTAG_SINGLE "Single"
#define D_XMLTAG_MANY "Many"
#define D_XMLTAG_ZERO "None"
#define D_XMLTAG_ERROR "Error"
#define D_XMLTAG_SECTION "section"
#define D_XMLTAG_PREFIX "prefix"


// CFG V1 Tags for Templates
#define D_XMLTAG_HTMLTEMPLATE "HtmlTemplate"
#define D_XMLTAG_HTMLTEMPLATE_SINGLE "SingleFeature"
#define D_XMLTAG_HTMLTEMPLATE_MANY "ManyFeatures"
#define D_XMLTAG_HTMLTEMPLATE_ZERO "ZeroFeatures"
#define D_XMLTAG_HTMLTEMPLATE_ERROR "Error"
#define D_XMLTAG_HTMLTEMPLATE_SECTION "TemplateSection"
#define D_XMLTAG_HTMLTEMPLATE_PREFIX "TemplatePrefix"


// CFG V1 tags for describing data limitations
#define D_XMLTAG_HTMLTEMPLATE_MAX_BBOX "MaxBBOX"
#define D_XMLTAG_HTMLTEMPLATE_MAX_BBOX_HEIGHT "Height"
#define D_XMLTAG_HTMLTEMPLATE_MAX_BBOX_WIDTH "Width"
#define D_XMLTAG_HTMLTEMPLATE_MAX_COUNT "MaxCount"


// CFG V2 Tags for additional data
#define D_XMLTAG_ADATA_RESOURCE "resource"
#define D_XMLTAG_ADATA_SECTION "section"
#define D_XMLTAG_ADATA_PREFIX "prefix"
#define D_XMLTAG_ADATA_ROWSECTION "rowsection"

// CFG V1 tags for describing additional data to be fetched for html templates
#define D_XMLTAG_HTMLTEMPLATE_ADATA "AdditionalData"
#define D_XMLTAG_HTMLTEMPLATE_ADATA_URI  "FetchUri"
#define D_XMLTAG_HTMLTEMPLATE_ADATA_SECTION "TemplateSection"
#define D_XMLTAG_HTMLTEMPLATE_ADATA_DATA_SECTION "TemplateDataSection"
#define D_XMLTAG_HTMLTEMPLATE_ADATA_PREFIX  "TemplatePrefix"

#define D_XMLTAG_KMLTEMPLATE "KmlTemplate"
#define D_XMLTAG_KMLTEMPLATE_SINGLE "SingleFeature"
#define D_XMLTAG_KMLTEMPLATE_MANY "ManyFeatures"
#define D_XMLTAG_KMLTEMPLATE_ZERO "ZeroFeatures"
#define D_XMLTAG_KMLTEMPLATE_ERROR "Error"
#define D_XMLTAG_KMLTEMPLATE_SECTION "TemplateSection"
#define D_XMLTAG_KMLTEMPLATE_PREFIX "TemplatePrefix"
// tags for describing additional data to be fetched for html templates
#define D_XMLTAG_KMLTEMPLATE_ADATA "AdditionalData"
#define D_XMLTAG_KMLTEMPLATE_ADATA_URI  "FetchUri"
#define D_XMLTAG_KMLTEMPLATE_ADATA_SECTION "TemplateSection"
#define D_XMLTAG_KMLTEMPLATE_ADATA_DATA_SECTION "TemplateDataSection"
#define D_XMLTAG_KMLTEMPLATE_ADATA_PREFIX  "TemplatePrefix"

// describing 
#define D_XMLTAG_ACCESS "Access"

#define D_XMLTAG_ACCESS_FORMAT "Format"
#define D_XMLTAG_ACCESS_FORMAT_ATTR_TYPE "Type"

#define D_XMLTAG_ACCESS_FORMAT_METHOD "Method"
#define D_XMLTAG_ACCESS_FORMAT_METHOD_ATTR_TYPE "Type"

#define D_XMLTAG_ACCESS_FORMAT_METHOD_USER "User"
#define D_XMLTAG_ACCESS_FORMAT_METHOD_USER_ATTR_NAME "Name"
#define D_XMLTAG_ACCESS_FORMAT_METHOD_USER_ATTR_PASSWORD "Password"


enum e_REST_AccessCodes
{
  e_Unknown, // 
  e_AccessOK,
  e_NoAccess_Format,   // format (html,xml,json,..) is not allowed
  e_NoAccess_FormatMethod, // format is allowed but not the http method on that format
  e_NoAccess_FormatMethodUser, // format and method are allowed but not for that user
};


#include "c_RestConfig.h"

#endif