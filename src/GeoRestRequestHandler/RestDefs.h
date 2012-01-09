//
//  Copyright (C) Haris Kurtagic
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

#ifndef _RESTDEFS_H
#define _RESTDEFS_H

#define D_REST_URI_SEGMENT_REST L"REST"
#define D_REST_URI_SEGMENT_REST_UTF8 "REST"

#define D_REST_URI_SEGMENT_HELLO L"HELLO"
#define D_REST_URI_SEGMENT_HELLO_UTF8 "HELLO"

#define D_REST_URI_SEGMENT_TASK L"TASK"

#define D_REST_URI_SEGMENT_DATA L"DATA"      // /REST/DATA/CityLimits
#define D_REST_URI_SEGMENT_DATA_UTF8 "DATA"      // /REST/DATA/CityLimits

#define D_REST_URI_SEGMENT_ODATA L"OData.svc"
#define D_REST_URI_SEGMENT_ODATA_UTF8 "OData.svc"

#define D_REST_URI_SEGMENT_ESRIGS L"Esri.GS"
#define D_REST_URI_SEGMENT_ESRIGS_UTF8 "Esri.GS"


//#define D_GEOMETRY "the_geom"

#define D_REST_URI_SEGMENT_FDO L"FDO"
#define D_REST_URI_SEGMENT_FDO_DATASOURCE L"DATASOURCE"
#define D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS L"CLASS"
#define D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS_FEATURE L"FEATURE"
#define D_REST_URI_SEGMENT_SESSSION L"SESSION"
#define D_REST_URI_SEGMENT_WEBLAYOUT L"WEBLAYOUT"
#define D_REST_URI_SEGMENT_MAP L"MAP"
#define D_REST_URI_SEGMENT_MAP_IMAGE_PNG L"IMAGE.PNG"
#define D_REST_URI_SEGMENT_MAP_IMAGE_JPG L"IMAGE.JPG"

#define D_REST_URI_SEGMENT_MAP_QUERYMAPFEATURES L"QUERYMAPFEATURES"

#define D_REST_URI_SEGMENT_LAYERDEFINITION L"LAYERDEFINITION"
#define D_REST_URI_SEGMENT_LAYERDEFINITION_LEGENDICON L"LEGENDICON"


#define D_REST_HTTP_METHOD_GET "GET"
#define D_REST_HTTP_METHOD_POST "POST"
#define D_REST_HTTP_METHOD_PUT "PUT"
#define D_REST_HTTP_METHOD_DELETE "DELETE"
#define D_REST_HTTP_METHOD_OPTION "OPTION"

#define D_REST_JSON_DATA L"_json" // used as a query parameter when we cannot send data in header (when using jsonp)

#define D_REST_URI_QUERY_PARAM_WEBLAYOUT L"WEBLAYOUT"

#define D_REST_JSONP_CALLBACK_STR L"_callback"
#define D_REST_METHOD_STR L"_method"

#endif
