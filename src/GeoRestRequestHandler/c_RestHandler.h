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

#ifndef _c_RestHandler_h
#define _c_RestHandler_h

#include "c_RestResponse.h"
#include "c_RestUriFactory.h"
class c_RestRequest;

class REST_REQUEST_HANDLER_API c_RestHandler : public MgDisposable
{
public:
    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructor.
    /// </summary>
    c_RestHandler();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructor.
    /// </summary>
    virtual ~c_RestHandler();


    /// <summary>
    /// Initializes the common parameters of the request.
    /// </summary>
    /// <param name="name">Input
    /// MgHttpRequest
    /// This contains all the parameters of the request.
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>
    void InitializeCommonParameters(c_RestRequest *hRequest);

    /// <summary>
    /// Validates the common parameters of the request.
    /// </summary>
    /// <returns>
    /// nothing
    /// </returns>
    virtual void ValidateCommonParameters();

    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    virtual void Execute(c_RestResponse& hResponse) = 0;

   

protected:
    /// <summary>
    /// This method is responsible for releasing all
    /// its member variables and then destruct itself.
    /// </summary>
    /// <returns>Returns nothing</returns>
    virtual void Dispose() { delete this; }

    Ptr<c_RestRequest> m_RestRequest;
    Ptr<c_RestUriFactory> m_RestUriFactory;
    
};

#endif // REQUEST_RESPONSE_HANDLER_H
