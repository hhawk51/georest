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

#ifndef _MGRESTSESSIONMAP_H
#define _MGRESTSESSIONMAP_H


//
// Implements REST interface for MAP object. Map object is runtime Map created from MapDefinition
// and kept in session repository.
//
// 1. URI for get data about MAP
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
//
class MgRestSessionMap : public MgRestRequestResponseHandler
{
public:
  static MgRestRequestResponseHandler* CreateObject(MgRestRequest* Request);

public:
    /// <summary>
    /// Initializes the common parameters of the request.
    /// </summary>
    /// <param name="name">Input
    /// MgHttpRestRequest
    /// This contains all the parameters of the request.
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>
    MgRestSessionMap(MgRestRequest *Request);

    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    void Execute(MgHttpResponse& hResponse);

    /// <summary>
    /// Returns the classification of this request/response handler
    /// </summary>
    /// <returns>
    /// Classification of handler
    /// </returns>
    MgRequestClassification GetRequestClassification() { return MgHttpRequestResponseHandler::mrcViewer; }
    
    void Execute_Get_SessionMapImage_Png(MgHttpResponse& HttpResponse);
    void Execute_Get_SessionMapImage_Jpg(MgHttpResponse& HttpResponse);
    
    void Execute_Get_SessionMap_QueryMapFeatures(MgHttpResponse& HttpResponse);
    
protected:
  STRING m_WebLayout;
  STRING m_MapName;
  

};

#endif  //_MGHTTPCREATESESSION_H_
