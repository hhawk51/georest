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
#include "c_RestHandler.h"
#include "c_RestRequest.h"


//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
c_RestHandler::c_RestHandler() 
{ 
  m_RestUriFactory = c_RestUriFactory::CreateObject();
}

/// <summary>
/// Initializes the common parameters of the request.
/// </summary>
/// <param name="name">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
void c_RestHandler::InitializeCommonParameters(c_RestRequest *RestRequest)
{
    MG_TRY()

    //MgHttpRequestResponseHandler::InitializeCommonParameters(RestRequest);

    m_RestRequest = SAFE_ADDREF(RestRequest);
    
    MG_CATCH_AND_THROW(L"c_RestHandler.InitializeCommonParameters")
}

/// <summary>
/// Validates the common parameters of the request.
/// </summary>
void c_RestHandler::ValidateCommonParameters()
{
    MG_TRY()

        

    MG_CATCH_AND_THROW(L"c_RestHandler.ValidateCommonParameters")
}



c_RestHandler::~c_RestHandler()
{
}


    