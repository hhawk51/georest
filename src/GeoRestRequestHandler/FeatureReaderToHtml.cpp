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
#include "MapGuideCommon.h"
//#include "HttpHandler.h"
//#include "MapAgentGetParser.h"
#include "RestConfigInclude.h"
#include "c_RestRequest.h"
#include "c_RestMapNameToResourceId.h"

#include "FeatureReaderToHtml.h"

#include <ctemplate/template.h>
//#include "..\HttpHandler\Rest_MapNameToResourceId.h"
//#include "..\HttpHandler\RestFetchData.h"
#include "Poco/Uri.h"
#include "c_KmlUtil.h"
//#include "zlib.h"
//#include "deflate.h"
#include "minizip/unzip.h"
#include "minizip/zip.h"
#include "minizip/ioapi.h"
#include "c_GeoRssUtil.h"
#include "c_GmlUtil.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"

extern string g_HtmlTemplatePath;




typedef struct ourmemory_s {
  char *base; /* Base of the region of memory we're using */
  uLong size; /* Size of the region of memory we're using */
  uLong limit; /* Furthest we've written */
  uLong cur_offset; /* Current offset in the area */
} ourmemory_t;

voidpf ZCALLBACK fopen_mem_func (voidpf opaque, const char*filename, int mode)
   
{
/*
    ourmemory_t *mem = (ourmemory_t *)malloc(sizeof(*mem));
    if (mem==NULL)
      return NULL; /// Can't allocate space, so failed 

    // Filenames are specified in the form :
    //     <hex base of zip file>+<hex size of zip file>
    //  This may not work where memory addresses are longer than the
    //  size of an int and therefore may need addressing for 64bit
    //  architectures
    // 
    if (sscanf(filename,"%x+%x",&mem->base,&mem->size)!=2)
      return NULL;

    if (mode & ZLIB_FILEFUNC_MODE_CREATE)
      mem->limit=0; // When writing we start with 0 bytes written 
    else
      mem->limit=mem->size;

    mem->cur_offset = 0;

    return mem;
*/
  ourmemory_t *mem;
  if (sscanf(filename,"%p",&mem)!=1)
      return NULL;
    
    
  if (mode & ZLIB_FILEFUNC_MODE_CREATE)
      mem->limit=0; // When writing we start with 0 bytes written 
    else
      mem->limit=mem->size;

    mem->cur_offset = 0;

    return mem;    
}


uLong ZCALLBACK fread_mem_func (voidpf opaque, voidpf stream, void* buf,uLong size)
{
    ourmemory_t *mem = (ourmemory_t *)stream;

    if (size > mem->size - mem->cur_offset)
      size = mem->size - mem->cur_offset;

    memcpy(buf, mem->base + mem->cur_offset, size);
    mem->cur_offset+=size;

    return size;
}


uLong ZCALLBACK fwrite_mem_func (voidpf opaque, voidpf stream, const void*buf, uLong size)
   
{
    ourmemory_t *mem = (ourmemory_t *)stream;

    if (size > mem->size - mem->cur_offset)
      size = mem->size - mem->cur_offset;

    memcpy(mem->base + mem->cur_offset, buf, size);
    mem->cur_offset+=size;
    if (mem->cur_offset > mem->limit)
      mem->limit = mem->cur_offset;

    return size;
}

long ZCALLBACK ftell_mem_func (voidpf opaque, voidpf stream)

{
    ourmemory_t *mem = (ourmemory_t *)stream;

    return mem->cur_offset;
}

long ZCALLBACK fseek_mem_func (voidpf opaque, voidpf stream, uLong  offset, int origin)

   
{
    ourmemory_t *mem = (ourmemory_t *)stream;
    uLong new_pos;
    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR :
        new_pos = mem->cur_offset + offset;
        break;
    case ZLIB_FILEFUNC_SEEK_END :
        new_pos = mem->limit + offset;
        break;
    case ZLIB_FILEFUNC_SEEK_SET :
        new_pos = offset;
        break;
    default: return -1;
    }

    if (new_pos > mem->size)
      return 1; /* Failed to seek that far */

    if (new_pos > mem->limit)
      memset(mem->base + mem->limit, 0, new_pos - mem->limit);

    mem->cur_offset = new_pos;
    return 0;
}

int ZCALLBACK fclose_mem_func (voidpf opaque, voidpf stream)
   
{
    ourmemory_t *mem = (ourmemory_t *)stream;

    /* Note that once we've written to the buffer we don't tell anyone
       about it here. Probably the opaque handle could be used to inform
       some other component of how much data was written.

       This, and other aspects of writing through this interface, has
       not been tested.
     */

    //free (mem);
    return 0;
}

int ZCALLBACK ferror_mem_func (voidpf opaque, voidpf stream)
 
{
    ourmemory_t *mem = (ourmemory_t *)stream;
    /* We never return errors */
    return 0;
}

void fill_memory_filefunc (zlib_filefunc_def* pzlib_filefunc_def)   
{
    pzlib_filefunc_def->zopen_file = fopen_mem_func;
    pzlib_filefunc_def->zread_file = fread_mem_func;
    pzlib_filefunc_def->zwrite_file = fwrite_mem_func;
    pzlib_filefunc_def->ztell_file = ftell_mem_func;
    pzlib_filefunc_def->zseek_file = fseek_mem_func;
    pzlib_filefunc_def->zclose_file = fclose_mem_func;
    pzlib_filefunc_def->zerror_file = ferror_mem_func;
    pzlib_filefunc_def->opaque = NULL;
}

   

// Static.
MgByteReader * WriteKmz(const std::string& kml) 
{
  zlib_filefunc_def fdef;
  fill_memory_filefunc(&fdef);
  
  char buff[256];
  
  int outsize = kml.length() + 256;;
  char* outbuff = (char*)malloc(outsize);
   
  ourmemory_t *mem = (ourmemory_t *)malloc(sizeof(*mem)); 
  
  mem->base = outbuff;
  mem->size = outsize;
  mem->limit = 0;
  mem->cur_offset = 0;
   
  sprintf(buff,"%p",mem);
  zipFile zipfile_ = zipOpen2(buff, 0,0,&fdef);
  if (!zipfile_) {
    return false;
  }
  zipOpenNewFileInZip(zipfile_, "doc.kml", 0, 0, 0, 0, 0, 0, Z_DEFLATED,
                      Z_DEFAULT_COMPRESSION);
  //zipWriteInFileInZip(zipfile_, (void* const)kml.data(),
  zipWriteInFileInZip(zipfile_, static_cast<const void*>(kml.data()),
                      static_cast<unsigned int>(kml.size()));
  zipCloseFileInZip(zipfile_);
  zipClose(zipfile_, 0);
  
  
  MgByteReader *breader = new MgByteReader((unsigned char*)outbuff,mem->limit,MgMimeType::Kmz);
  
  free(outbuff);
  free(mem);
  
  return breader;
 
  
 
}


c_FeatureReaderToHtml::c_FeatureReaderToHtml(void)
{
}

c_FeatureReaderToHtml::~c_FeatureReaderToHtml(void)
{
  
}

void ReplaceSpaces(string& StrVal)
{
  bool repeat=true;
  string::size_type strind = 0;
  while( strind != string::npos )
  {
    strind = StrVal.find(' ',strind);
    if( strind != string::npos )
      StrVal[strind] = '_';
  }
}

void GetPropertyAsString(c_RestDataReader* FeatureReader,CREFSTRING PropName,REFSTRING StringValue)
{
  
  
  int ptype = FeatureReader->GetPropertyType(PropName);
  switch( ptype )
  {
    case MgPropertyType::Geometry:
    {
      //Ptr<MgByteReader> bytes = FeatureReader->GetGeometry(propname);
      //Ptr<MgGeometryProperty> prop = new MgGeometryProperty(propname,bytes);
      //prop_coll->Add(prop);
    }
    break;
    case MgPropertyType::Int32:
    {
      if( FeatureReader->IsNull(PropName) )
      {
        StringValue = L"";
      }
      else
      {
        long val = FeatureReader->GetInt32(PropName);
        MgUtil::Int32ToString(val,StringValue);      
      }
    }
    break;
    case MgPropertyType::Int16:
    {
      if( FeatureReader->IsNull(PropName) )
      {
        StringValue = L"";
      }
      else
      {
        long val = FeatureReader->GetInt16(PropName);
        MgUtil::Int32ToString(val,StringValue);
      }
      
    }
    break;
    case MgPropertyType::Single:
    {
      if( FeatureReader->IsNull(PropName) )
      {
        StringValue = L"";
      }
      else
      {
        float val = FeatureReader->GetSingle(PropName);
        MgUtil::SingleToString(val,StringValue);
      }
      
    }
    break;
    case MgPropertyType::Double:
    {
      if( FeatureReader->IsNull(PropName) )
      {
        StringValue = L"";
      }
      else
      {
        double val = FeatureReader->GetDouble(PropName);
        MgUtil::DoubleToString(val,StringValue);
      }
      
    }
    break;
    case MgPropertyType::DateTime:
    {
      if( FeatureReader->IsNull(PropName) )
      {
        StringValue = L"";
      }
      else
      {
        Ptr<MgDateTime> val = FeatureReader->GetDateTime(PropName);
        StringValue = val->ToString();
        
      }
      
    }
    break;
    case MgPropertyType::String:
    {
      if( FeatureReader->IsNull(PropName) )
      {
        StringValue = L"";
      }
      else
      {
        StringValue = FeatureReader->GetString(PropName);      
      }
    }
    break;
  }
     

}


#define MG_MAX_DATE_TIME_BUFFER_LENGTH  64
void MgDateTimeToString(MgDateTime* DateTime,string& fdoDateTime)
{
    // Ensure date and/or time values are valid.

    DateTime->Validate();

    // Format the FDO date time string.

    
    char buf[MG_MAX_DATE_TIME_BUFFER_LENGTH];

    ::memset(buf, 0, sizeof(buf));

    if (DateTime->IsDateTime())
    {
        if (0 == DateTime->GetMicrosecond() )
        {
            ::sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
                DateTime->GetYear(), DateTime->GetMonth(), DateTime->GetDay(), DateTime->GetHour(), DateTime->GetMinute(), DateTime->GetSecond());

            fdoDateTime = buf;
        }
        else
        {
            if (DateTime->GetMicrosecond() % 1000 == 0) // only report milliseconds for FDO
                ::sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                    DateTime->GetYear(), DateTime->GetMonth(), DateTime->GetDay(), DateTime->GetHour(), DateTime->GetMinute(), DateTime->GetSecond()
                    , DateTime->GetMicrosecond() / 1000);
            else
                ::sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%06d",
                    DateTime->GetYear(), DateTime->GetMonth(), DateTime->GetDay(), DateTime->GetHour(), DateTime->GetMinute(), DateTime->GetSecond()
                    , DateTime->GetMicrosecond());

            fdoDateTime = buf;
            MgUtil::TrimEndingZeros(fdoDateTime);            
        }
    }
    else if (DateTime->IsDate())
    {
        ::sprintf(buf, "%04d-%02d-%02d",
            DateTime->GetYear(), DateTime->GetMonth(), DateTime->GetDay());

        fdoDateTime = buf;
    }
    else if (DateTime->IsTime())
    {
        if (0 == DateTime->GetMicrosecond())
        {
            ::sprintf(buf, "%02d:%02d:%02d",
                DateTime->GetHour(), DateTime->GetMinute(), DateTime->GetSecond());

            fdoDateTime = buf;
        }
        else
        {
            if (DateTime->GetMicrosecond() % 1000 == 0) // only report milliseconds for FDO
                ::sprintf(buf, "%02d:%02d:%02d.%03d",
                    DateTime->GetHour(), DateTime->GetMinute(), DateTime->GetSecond(), DateTime->GetMicrosecond() / 1000);
            else
                ::sprintf(buf, "%02d:%02d:%02d.%06d",
                    DateTime->GetHour(), DateTime->GetMinute(), DateTime->GetSecond(), DateTime->GetMicrosecond());

            fdoDateTime = buf;
            MgUtil::TrimEndingZeros(fdoDateTime);
            
        }
    }
    else
    {
        throw new MgDateTimeException(L"MgDateTime.ToString",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    
}

void FillDictionary(ctemplate::TemplateDictionary* Dict,const std::string& NamePrefix, c_RestDataReader* FeatureReader)
{
  
  int count = FeatureReader->GetPropertyCount();
  for(int ind=0;ind<count;ind++)
  {
    STRING propname = FeatureReader->GetPropertyName(ind);
    
    if( !FeatureReader->IsNull(propname) )
    {
      string dictkey = NamePrefix + MgUtil::WideCharToMultiByte(propname);
      
      ReplaceSpaces(dictkey); // google ctemplate doesn't allow spaces
      
      int ptype = FeatureReader->GetPropertyType(propname);
      switch( ptype )
      {
        case MgPropertyType::Geometry:
        {
          /*
          int geomlength;
          BYTE_ARRAY_OUT geombytes = FeatureReader->GetGeometry(propname,geomlength);
          Ptr<MgByteReader> bytes = new MgByteReader( geombytes,geomlength,L"" );
          */
          try
          {
          
          Ptr<MgByteReader> bytes = FeatureReader->GetGeometry(propname);
          
          MgAgfReaderWriter agfReader;                
          Ptr<MgGeometry> fgfgeom = agfReader.Read(bytes);
          
          MgWktReaderWriter wktReader;        
          STRING wktstring = wktReader.Write(fgfgeom);
          string mb_wktstring = MgUtil::WideCharToMultiByte(wktstring);
          Dict->SetValue(dictkey,mb_wktstring);
          
          string kmlgeometry,kmlcentroid,kml_multi_centroid_polygon;
          double cent_x,cent_y;
          c_KmlUtil::ToKml(fgfgeom,kmlgeometry,kmlcentroid,cent_x,cent_y,kml_multi_centroid_polygon);
          Dict->SetValue(dictkey+"_KML",kmlgeometry);
          
          Dict->SetValue(dictkey+"_KML_CENTROID",kmlcentroid);
          Dict->SetValue(dictkey+"_KML_MULTI",kml_multi_centroid_polygon);
          
          string georsssimple;
          c_GeoRssUtil::ToGeoRssSimple(fgfgeom,georsssimple);
          Dict->SetValue(dictkey+"_GEORSS",georsssimple);
          
          string gml;
          c_GmlUtil::ToGML(fgfgeom,gml);
          Dict->SetValue(dictkey+"_GML",gml);
          
          
          char buff[4*64+6];
          
          ::sprintf(buff, "%.7g", cent_x);
          Dict->SetValue(dictkey+"_CENTROID_X",buff);
          ::sprintf(buff, "%.7g", cent_y);
          Dict->SetValue(dictkey+"_CENTROID_Y",buff);
          
          Ptr<MgEnvelope> e2 = fgfgeom->Envelope();
          Ptr<MgCoordinate> ll = e2->GetLowerLeftCoordinate();
          Ptr<MgCoordinate> ur = e2->GetUpperRightCoordinate();
          
          
          ::sprintf(buff, "%.7g,%.7g,%.7g,%.7g", ll->GetX(),ll->GetY(),ur->GetX(),ur->GetY());
          Dict->SetValue(NamePrefix + "REST_ENVELOPE",buff);
          
          // set same value for BBOX
          Dict->SetValue(dictkey+"_BBOX",buff);
          
          ::sprintf(buff, "%.7g", ll->GetX());
          Dict->SetValue(dictkey+"_BBOX_MIN_X",buff);
          
          ::sprintf(buff, "%.7g", ur->GetX());
          Dict->SetValue(dictkey+"_BBOX_MAX_X",buff);
          
          ::sprintf(buff, "%.7g", (ll->GetX()+ur->GetX())/2.0);
          Dict->SetValue(dictkey+"_BBOX_MID_X",buff);
          
          ::sprintf(buff, "%.7g", ll->GetY());
          Dict->SetValue(dictkey+"_BBOX_MIN_Y",buff);
          
          ::sprintf(buff, "%.7g", ur->GetY());
          Dict->SetValue(dictkey+"_BBOX_MAX_Y",buff);
          
          ::sprintf(buff, "%.7g", (ll->GetY()+ur->GetY())/2.0);
          Dict->SetValue(dictkey+"_BBOX_MID_Y",buff);
          }
          catch (MgException *ex) // just ignore in case of invalid geometries
          {
            ex->Release();
          }
          
        }
        break;
        case MgPropertyType::Int64:
        {
          INT64 val = FeatureReader->GetInt64(propname);
          std::string tmp = "";
          MgUtil::Int64ToString(val, tmp);
          Dict->SetValue(dictkey,tmp);        
        }
        break;
        case MgPropertyType::Int32:
        {
          long val = FeatureReader->GetInt32(propname);
          Dict->SetFormattedValue(dictkey,"%ld",val);        
        }
        break;
        case MgPropertyType::Int16:
        {
          int val = FeatureReader->GetInt16(propname);
          Dict->SetIntValue(dictkey,val);
        }
        break;
        case MgPropertyType::Double:
        {
          double val = FeatureReader->GetDouble(propname);
          Dict->SetFormattedValue(dictkey,"%.5g",val);
        }
        break;
        case MgPropertyType::Single:
        {
          float val = FeatureReader->GetSingle(propname);
          Dict->SetFormattedValue(dictkey,"%.5f",val);
        }
        break;
        case MgPropertyType::Boolean:
        {
          bool val = FeatureReader->GetBoolean(propname);
          Dict->SetValue(dictkey,val ? "TRUE" : "FALSE");
        }
        break;
        case MgPropertyType::Byte:
        {
          BYTE val = FeatureReader->GetByte(propname);
          Dict->SetIntValue(dictkey,val );
        }
        break;
        case MgPropertyType::DateTime:
        {
          Ptr<MgDateTime> date = FeatureReader->GetDateTime(propname);
          string val;
          MgDateTimeToString(date,val);
          Dict->SetValue(dictkey,val );
          
          
          // set date as Atom string 
          Poco::DateTime dt(date->GetYear(), date->GetMonth(), date->GetDay(), date->GetHour(), date->GetMinute(), date->GetSecond());

          std::string str = Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::ISO8601_FORMAT);
          Dict->SetValue(dictkey+"_ATOM_DATE",str);
        }
        break;
        case MgPropertyType::String:
        {
          string val = MgUtil::WideCharToMultiByte(FeatureReader->GetString(propname));
          
          Dict->SetValue(dictkey,val);
          
        }
        break;
      }
    }
  }

}

void c_FeatureReaderToHtml::ToHtml(c_RestDataReader* FeatureReader, c_RestRequest* RestRequest
                            ,const string& AgentUri,const string& UriBase
                            ,string& HtmlStr,int StartIndex,int MaxCount)
{
  ToTemplate(false,FeatureReader, RestRequest, AgentUri,UriBase,HtmlStr,StartIndex,MaxCount);
}
void c_FeatureReaderToHtml::ToKml(c_RestDataReader* FeatureReader, c_RestRequest* RestRequest
                            , const string& AgentUri,const string& UriBase
                            ,string& HtmlStr,int StartIndex,int MaxCount)
{
  ToTemplate(true,FeatureReader, RestRequest, AgentUri,UriBase,HtmlStr,StartIndex,MaxCount);
}
MgByteReader * c_FeatureReaderToHtml::ToKmz(c_RestDataReader* FeatureReader, c_RestRequest* RestRequest
                            , const string& AgentUri,const string& UriBase
                            ,int StartIndex,int MaxCount)
{
  string kml;
  ToTemplate(true,FeatureReader, RestRequest, AgentUri,UriBase,kml,StartIndex,MaxCount);
  
  // Now need to zip it do 

 
  MgByteReader * breader = WriteKmz(kml); 
  return breader;
}

void GetPropertyAsBase64(c_RestDataReader* FeatureReader,CREFSTRING PropName,std::string& StringValue)
{
  try
  {
    StringValue = "";
    MgMemoryStreamHelper stream;

    int ptype = FeatureReader->GetPropertyType(PropName);
    switch( ptype )
    {
    case MgPropertyType::Geometry:
      {
        //Ptr<MgByteReader> bytes = FeatureReader->GetGeometry(propname);
        //Ptr<MgGeometryProperty> prop = new MgGeometryProperty(propname,bytes);
        //prop_coll->Add(prop);
      }
      break;
    case MgPropertyType::Int32:
      {
        if( FeatureReader->IsNull(PropName) )
        {
          StringValue = "";
        }
        else
        {
          stream.WriteUINT32(FeatureReader->GetInt32(PropName));

        }
      }
      break;
    case MgPropertyType::Int16:
      {
        if( FeatureReader->IsNull(PropName) )
        {
          StringValue = "";
        }
        else
        {
          stream.WriteUINT16(FeatureReader->GetInt16(PropName));
        }

      }
      break;
    case MgPropertyType::Single:
      {
        if( FeatureReader->IsNull(PropName) )
        {
          StringValue = "";
        }
        else
        {
          //float val = FeatureReader->GetSingle(PropName);
          stream.WriteSingle(FeatureReader->GetSingle(PropName));

        }

      }
      break;
    case MgPropertyType::Double:
      {
        if( FeatureReader->IsNull(PropName) )
        {
          StringValue = "";
        }
        else
        {
          //double val = FeatureReader->GetDouble(PropName);
          //MgUtil::DoubleToString(val,StringValue);
          stream.WriteDouble(FeatureReader->GetDouble(PropName));
        }

      }
      break;
    case MgPropertyType::DateTime:
      {
        if( FeatureReader->IsNull(PropName) )
        {
          StringValue = "";
        }
        else
        {
          Ptr<MgDateTime> dateTime = FeatureReader->GetDateTime(PropName);
          Ptr<MgStream> tempStream = new MgStream(&stream);
          dateTime->Serialize(tempStream);

        }

      }
      break;
    case MgPropertyType::String:
      {
        if( FeatureReader->IsNull(PropName) )
        {
          StringValue = "";
        }
        else
        {
          //StringValue = FeatureReader->GetString(PropName);    
          stream.WriteNullTermString(FeatureReader->GetString(PropName));  

        }
      }
      break;
    }

    StringValue = stream.ToBase64();
    //UnicodeString::MultiByteToWideChar(stream.ToBase64().c_str(), StringValue);
  }
  catch(...)
  {
  }  
}

void c_FeatureReaderToHtml::ToTemplate(bool IsKml,c_RestDataReader* Reader, c_RestRequest* RestRequest
                            , const string& FullUri,const string& UriBase
                            ,string& HtmlStr,int StartIndex,int MaxCount)
{

  Ptr<MgClassDefinition> classdef;
  // classdef =  if( FeatureReader ) FeatureReader->GetClassDefinition() ;
  classdef = RestRequest->m_DataClassDef;
  if( !classdef ) return;
  
 
  
  //const c_CfgDataLayer* cfgdata = MgRest_MapNameToResourceId::GetCfgDataForClass(classfullname);
  const c_CfgDataResource* cfgresource = RestRequest->m_CfgDataResource;
  
  if( !cfgresource ) return; // error ; class has to be in configuration file
  
  const c_CfgRepTemplate* templatedata;
  if( RestRequest->m_CfgRepresentation->GetType() != c_CfgRepresentation::e_Template )
  {
    return;  
  }
  
  templatedata = (c_CfgRepTemplate*)RestRequest->m_CfgRepresentation;
    
  
  string rest_uri_part;
  if( cfgresource->m_RestUriPart.length() > 0 )
  {
    MgUtil::WideCharToMultiByte(cfgresource->m_RestUriPart,rest_uri_part);
    
  }
  else
  {
    rest_uri_part = "/rest/data/";
  }
  
  //ctemplate::Template::SetTemplateRootDirectory(g_HtmlTemplatePath);
  ctemplate::Template::SetTemplateRootDirectory(RestRequest->m_CfgDataResource->m_TemplateFolder);
  
  
  
   // get identity property name;
  STRING identname;      
  if( classdef )
  {
    Ptr<MgPropertyDefinitionCollection> idents = classdef->GetIdentityProperties();
    if( idents->GetCount() == 1) 
    {
      int ind=0;  
      Ptr<MgPropertyDefinition> identprop = idents->GetItem(ind);
      identname = identprop->GetName();
    }
    else
    {
      identname = L"";
    }
  }
  
  ctemplate::TemplateDictionary dict_main("KING.MGREST");
  
  // now fill in request parameters
  if( RestRequest )
  {
    Ptr<c_RestUriRequestParam> req_param = RestRequest->GetRequestParam();
    MgStringPropertyCollection* params = req_param->GetParameters();
    
    if( params && (params->GetCount()>0) )
    {
      string dictkey;
      string param_name,param_val;        
      string keyprefix = "REST_PARAM_";
      for(int ind=0;ind<params->GetCount();ind++)
      {
        Ptr<MgStringProperty> param = params->GetItem(ind);
        MgUtil::WideCharToMultiByte(param->GetName(),param_name);
        ReplaceSpaces(param_name); // google ctemplate doesn't allow spaces
        MgUtil::WideCharToMultiByte(param->GetValue(),param_val);
      
        
        dict_main.SetValue(keyprefix+param_name,param_val);
      }
    }
  }
  
  string resturiparam;
  bool ismorerecords=false;
  int featurecount=0;
  if( (classdef != NULL) && (Reader!=NULL) )
  {
    
    STRING wresturiparam = cfgresource->m_UriTag;
    
    MgUtil::WideCharToMultiByte(wresturiparam,resturiparam);
    
    
      bool isnext=true;
      int skipstart = StartIndex;
      if( skipstart > 0 )
      {
        
        while ( (skipstart>0) && (isnext=Reader->ReadNext())==true  )
        {
          skipstart--;
        }
        
      }
      if( isnext )
      {
        
        string dictname; // = mb_classname;
        MgUtil::WideCharToMultiByte(templatedata->m_HtmlTemp_Section,dictname);
        string nameprefix; // = mb_classname + "_";
        MgUtil::WideCharToMultiByte(templatedata->m_HtmlTemp_Prefix,nameprefix);
        
        while ( Reader->ReadNext() )
        {
          
          if( MaxCount >= 0 )
          {
            
            if( featurecount >= MaxCount ) 
            {
              ismorerecords=true;
              break; // go out
            }
          }
          ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary(dictname);
          FillDictionary(dict_section,nameprefix,Reader);
          
          if( identname.length() > 0 )
          {
            STRING strval;
            string mb_strval;
            GetPropertyAsString(Reader,identname,strval);
            MgUtil::WideCharToMultiByte(strval,mb_strval);
            
            
            //
            string dictkey = nameprefix + "REST_PNG";  
            string val = UriBase + rest_uri_part + resturiparam + "/" + mb_strval + ".png";                
            dict_section->SetValue(dictkey,val);
              
            dictkey = nameprefix + "REST_HTML";  
            val = UriBase + rest_uri_part + resturiparam + "/" + mb_strval + ".html";  
            dict_section->SetValue(dictkey,val);
            
            dictkey = nameprefix + "REST_KML";  
            val = UriBase + rest_uri_part + resturiparam + "/" + mb_strval + ".kml";  
            dict_section->SetValue(dictkey,val);
            
            dictkey = nameprefix + "REST_KMZ";  
            val = UriBase + rest_uri_part + resturiparam + "/" + mb_strval + ".kmz";  
            dict_section->SetValue(dictkey,val);
            
            dictkey = nameprefix + "REST_XML";  
            val = UriBase + rest_uri_part + resturiparam + "/" + mb_strval + ".xml";  
            dict_section->SetValue(dictkey,val);
            
            dictkey = nameprefix + "REST_JSON";  
            val = UriBase + rest_uri_part + resturiparam + "/" + mb_strval + ".json";  
            dict_section->SetValue(dictkey,val);
            
            // create feature identifier value as base64 coded
            string ident_base64;
            GetPropertyAsBase64(Reader,identname,ident_base64);
            dictkey = nameprefix + "REST_IDENT_BASE64";  
            dict_section->SetValue(dictkey,ident_base64);
            
          }
          
          featurecount++;
        }
        
      }
    
  
    
    // Now check additional data to be fetched for html template
    // it applies only to single feature templates
    if( featurecount==1 && templatedata->GetCountHtmlExtraData() > 0 )
    {
      int count = templatedata->GetCountHtmlExtraData();
      for(int ind=0;ind<count;ind++)
      {
        const c_CfgRepTemplateExtraData* extradata = templatedata->GetHtmlExtraData(ind);
        
        if( extradata->m_FetchUri.length() > 0 )
        {
        
          // generate template unique name
          std::string filename,mb_str;
          MgUtil::WideCharToMultiByte(cfgresource->m_UriTag,filename);        
          MgUtil::WideCharToMultiByte(extradata->m_HtmlTemp_Prefix,mb_str);        
          filename += mb_str;
          
          
          MgUtil::WideCharToMultiByte(extradata->m_FetchUri,mb_str);
          filename="";
          ctemplate::Template * temp = ctemplate::Template::StringToTemplate(mb_str,ctemplate::STRIP_WHITESPACE);
          /*
          ctemplate::Template * temp = ctemplate::Template::RegisterStringAsTemplate(filename,        
                                               ctemplate::STRIP_WHITESPACE,ctemplate::TC_MANUAL,mb_str);
          */          
                    
          std::string uristr;                  
          temp->Expand(&uristr,&dict_main);     
          
          // Now use uristr to fetch data
          std::string mb_str2,mb_str3,mb_str4;
          
          MgUtil::WideCharToMultiByte(extradata->m_HtmlTemp_Section,mb_str2);
          MgUtil::WideCharToMultiByte(extradata->m_HtmlTemp_DataSection,mb_str3);
          MgUtil::WideCharToMultiByte(extradata->m_HtmlTemp_Prefix,mb_str4);
          c_RestFetchSource::FetchFeaturesToDictionary( uristr,&dict_main,mb_str2,mb_str3,mb_str4 );                                             
          
          delete temp;
        }
        
      }
    }
    
    // now add in dictionary values for next and previous
    if( ismorerecords || StartIndex>0 )
    {    
      Poco::URI uri_parser(FullUri);    
      
      std::string query = uri_parser.getQuery();
          
      c_RestUriRequestParam params;
      c_RestUri::ParseQuery(query.c_str(),&params);
      
      
      
      if( StartIndex>0 )
      {
        int newstart = StartIndex - MaxCount;
        if( newstart < 0 ) newstart=0;
        
        newstart++;        
        wchar_t strstart[20];
        ::swprintf(&strstart[0],L"%d",newstart);
        
        if( params.ContainsParameter(L"Start") )
        {
          params.SetParameterValue(L"Start",strstart);
        }
        else
        {
          params.AddParameter(L"Start",strstart);
        }
        
        wstring wuriquery;
        params.GetAsUriQuery(wuriquery);
        
        string uriquery;
        MgUtil::WideCharToMultiByte(wuriquery,uriquery);
        
        //string prev_uri = UriBase + rest_uri_part + resturiparam + "/" + ".html" + "?" + uriquery;
        string prev_uri = "?" + uriquery; // use only query part so link will be correct in case of url rewrites
        
        ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("PREVIOUS_PAGE_SECTION");
        string dictkey = "PREVIOUS_PAGE";  
        dict_section->SetValue(dictkey,prev_uri);
      }
      else
      {
        // no previous link
        //string dictkey = nameprefix + "PREVIOUS_PAGE";  
        //dict_main.SetEscapedValue(,);
      }
      
      
      if( ismorerecords>0 )
      {
        int newstart = StartIndex + MaxCount;
        if( newstart < 0 ) newstart=0;
        
        newstart++;        
        wchar_t strstart[20];
        ::swprintf(&strstart[0],L"%d",newstart);
        
        if( params.ContainsParameter(L"Start") )
        {
          params.SetParameterValue(L"Start",strstart);
        }
        else
        {
          params.AddParameter(L"Start",strstart);
        }
        
        wstring wuriquery;
        params.GetAsUriQuery(wuriquery);
        
        string uriquery;
        MgUtil::WideCharToMultiByte(wuriquery,uriquery);
        
        
        
        
        
        //string next_uri = UriBase + rest_uri_part + resturiparam + "/" + ".html" + "?" + uriquery;
        string next_uri = "?" + uriquery; // use only query part so link will be correct in case of url rewrites
        
        ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("NEXT_PAGE_SECTION");
        string dictkey = "NEXT_PAGE";          
        dict_section->SetValue(dictkey,next_uri);
                
      }
      else
      {
        // no previous link
        //string dictkey = nameprefix + "NEXT_PAGE";  
        //dict_main.SetEscapedValue(,);
      }

    }
  }
  
  
    string tmpl; // name of template to use    
  
  if(Reader)
  {
    
    if( featurecount == 1 )
      MgUtil::WideCharToMultiByte(templatedata->m_HtmlTemp_Single,tmpl);
    else
    {  
      if( featurecount == 0 )  
        MgUtil::WideCharToMultiByte(templatedata->m_HtmlTemp_Zero,tmpl);
      else
        MgUtil::WideCharToMultiByte(templatedata->m_HtmlTemp_Many,tmpl);
    }
  }
  else
  {
    // if there was no feature reader
    MgUtil::WideCharToMultiByte(templatedata->m_HtmlTemp_Error,tmpl);    
    if(RestRequest->m_RestResultObjectStatus  == c_RestRequest::e_BBox_OutOfRange )
      dict_main.ShowSection("EXCEPTION_BBOX_LIMIT");
    if(RestRequest->m_RestResultObjectStatus  == c_RestRequest::e_Count_OutOfRange )
      dict_main.ShowSection("EXCEPTION_COUNT_LIMIT");
  }    
    /*      
    string tmpl = mb_classname;
    // if there is only one feature then use different template
    // name of that template for class "parcel" is parcel.tpl , added "1" to end of class name
    if( count <= 1 )
    {
      tmpl.append("1");
    }
    
    tmpl.append(".tpl");
    */
    ctemplate::Template::ReloadAllIfChanged();
    
    ctemplate::TemplateContext tc;
    if( IsKml )
      tc = ctemplate::TC_XML;
    else
      tc = ctemplate::TC_HTML;
    
    //ctemplate::Template* tpl = ctemplate::Template::GetTemplateWithAutoEscaping(tmpl,ctemplate::DO_NOT_STRIP,tc);
    ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tmpl,ctemplate::DO_NOT_STRIP);
    if( !tpl )
    {
      //Ptr<MgStringCollection> scol = new MgStringCollection();
      std::wstring wsarg;
      MgUtil::MultiByteToWideChar(tmpl,wsarg);
      //scol->Add(wsarg);
      
      std::wstring errmsg = L"Unable to load Template file '";
      errmsg = errmsg.append(wsarg);
      errmsg = errmsg.append(L"'. Check config file and template file location!");
      
      throw new MgRuntimeException(L"c_FeatureReaderToHtml::ToTemplate",__LINE__, __WFILE__, NULL, errmsg, NULL);
    }
    
    tpl->Expand(&HtmlStr,&dict_main);



}//end of c_FeatureReaderToHtml::ToTemplate



