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

#ifndef _c_FeatureReaderToXML_H
#define _c_FeatureReaderToXML_H

class c_RestRequest;
// This class parsers REST URI requests in form/url-encoded format
class REST_REQUEST_HANDLER_API c_FeatureReaderToXML
{
public:
    c_FeatureReaderToXML(void);
    ~c_FeatureReaderToXML(void);
    
    static void ToXml(c_RestDataReader* FeatureReader, string& XmlStr,int StartIndex,int MaxCount);
    static void ToSitemapXml(c_RestDataReader* FeatureReader, c_RestRequest*Request,const string& SiteUri ,string& XmlStr,int StartIndex,int MaxCount);
protected:    
    static MgPropertyCollection* GetPropertyCollection(c_RestDataReader* FeatureReader);
    static void GetPropertyAsString(c_RestDataReader* FeatureReader,CREFSTRING PropName,REFSTRING StringValue);
    static void GetPropertyAsString(c_RestDataReader* FeatureReader,CREFSTRING PropName,std::string& StringValue);
    static void ToFeatureXML(c_RestDataReader* FeatureReader,std::string& XmlStr);
protected:    
    

};

#endif
