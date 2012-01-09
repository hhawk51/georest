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

#ifndef _c_FeatureReaderToHtml_H
#define _c_FeatureReaderToHtml_H

#include "minizip/unzip.h"
#include "minizip/zip.h"
#include "minizip/ioapi.h"

class c_RestRequest;

// c_FeatureReaderToHtml... Converts Feature Reader to HTML file
// HTML file is generated from template. Template is defined in c_CfgDataLayer.
//
// ToHtml function will take Feature Class from FeatureReader, find corresponding c_CfgDataLayer and read template files.
// ToHtml will also fetch additional data while creating template if so defined in c_CfgDataLayer.
class c_FeatureReaderToHtml
{
public:
    c_FeatureReaderToHtml(void);
    ~c_FeatureReaderToHtml(void);
    //static void ToHtml(c_RestDataReader* FeatureReader, const string& UriBase, string& XmlStr,int StartIndex,int MaxCount);
    static void ToHtml(c_RestDataReader* FeatureReader, c_RestRequest* RestRequest, const string& AgentUri,const string& UriBase,string& HtmlStr,int StartIndex,int MaxCount);
    static void ToKml(c_RestDataReader* FeatureReader, c_RestRequest* RestRequest, const string& AgentUri,const string& UriBase,string& HtmlStr,int StartIndex,int MaxCount);
    static MgByteReader * ToKmz(c_RestDataReader* FeatureReader, c_RestRequest* RestRequest, const string& AgentUri,const string& UriBase,int StartIndex,int MaxCount);
    static void ToTemplate(bool IsKml,c_RestDataReader* FeatureReader, c_RestRequest* RestRequest, const string& AgentUri,const string& UriBase ,string& HtmlStr,int StartIndex,int MaxCount);
    
    
    
protected:    
    //static MgPropertyCollection* FillDictionary(ctemplate::TemplateDictionary* Dict,const std::string& NamePrefix, c_RestDataReader* FeatureReader);
    //static void c_FeatureReaderToHtml::ToTemplate(bool IsKml,c_RestDataReader* FeatureReader, const string& AgentUri,const string& UriBase
    //                        ,string& HtmlStr,int StartIndex,int MaxCount);
    
protected:    
    

};


 voidpf ZCALLBACK fopen_mem_func (voidpf opaque, const char*filename, int mode);
 uLong ZCALLBACK fread_mem_func (voidpf opaque, voidpf stream, void* buf,uLong size);
 uLong ZCALLBACK fwrite_mem_func (voidpf opaque, voidpf stream, const void*buf, uLong size);  
 long ZCALLBACK ftell_mem_func (voidpf opaque, voidpf stream);
 long ZCALLBACK fseek_mem_func (voidpf opaque, voidpf stream, uLong  offset, int origin);
 int ZCALLBACK fclose_mem_func (voidpf opaque, voidpf stream);
 int ZCALLBACK ferror_mem_func (voidpf opaque, voidpf stream);
 void fill_memory_filefunc (zlib_filefunc_def* pzlib_filefunc_def);

#endif

