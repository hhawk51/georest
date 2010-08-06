//
//  Copyright (C) 20010 by  Haris Kurtagic  www.sl-king.com
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
#include "RestCommonInclude.h"
#include "c_ExceptionGeoREST.h"

using Poco::Net::HTTPResponse;

POCO_IMPLEMENT_EXCEPTION(c_ExceptionGeoREST,Poco::Exception,"GeoREST Base Exception")

POCO_IMPLEMENT_EXCEPTION(c_ExceptionInvalidKeyValueString,c_ExceptionGeoREST,"Invalid Key Value String")

POCO_IMPLEMENT_EXCEPTION(c_ExceptionODataInvalidAtomEntry,c_ExceptionGeoREST,"Invalid OData Atom Entry!")

POCO_IMPLEMENT_EXCEPTION(c_ExceptionODataReader,c_ExceptionGeoREST,"Invalid OData Atom Entry!")

POCO_IMPLEMENT_EXCEPTION(c_ExceptionODataInvalidFilter,c_ExceptionGeoREST,"Invalid OData Filter!")



c_ExceptionHTTPStatus::c_ExceptionHTTPStatus( HTTPResponse::HTTPStatus Status,const std::string& StatusReason,int code /*= 0*/ )
  : c_ExceptionGeoREST(code), m_HttpStatus(Status), m_HttpStatusReason(StatusReason)
{

}

c_ExceptionHTTPStatus::c_ExceptionHTTPStatus( HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg, int code /*= 0*/ )
  : c_ExceptionGeoREST(msg,code), m_HttpStatus(Status), m_HttpStatusReason(StatusReason)
{

}

c_ExceptionHTTPStatus::c_ExceptionHTTPStatus( HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg,const Poco::Exception& exc, int code /*= 0*/ )
  : c_ExceptionGeoREST(msg,exc,code), m_HttpStatus(Status), m_HttpStatusReason(StatusReason)
{

}

c_ExceptionHTTPStatus::c_ExceptionHTTPStatus( HTTPResponse::HTTPStatus Status/*=HTTPResponse::HTTP_INTERNAL_SERVER_ERROR*/, int code/*=0*/ )
  : c_ExceptionGeoREST(code), m_HttpStatus(Status)
{

}
c_ExceptionHTTPStatus::~c_ExceptionHTTPStatus( void ) throw()
{

}

c_ExceptionHTTPStatus& c_ExceptionHTTPStatus::operator=( const c_ExceptionHTTPStatus& exc )
{
  c_ExceptionGeoREST::operator = (exc);
  m_HttpStatus = exc.httpStatus();
  m_HttpStatusReason = exc.httpStatus();
  return *this;
}

GEOREST_IMPLEMENT_EXCEPTION_HTTP(c_ExceptionHttpBadRequest,c_ExceptionHTTPStatus,"GeoREST HTTP Bad Request",HTTPResponse::HTTP_BAD_REQUEST)
GEOREST_IMPLEMENT_EXCEPTION_HTTP(c_ExceptionHttpNotFound,c_ExceptionHTTPStatus,"GeoREST HTTP Not Found",HTTPResponse::HTTP_NOT_FOUND)
GEOREST_IMPLEMENT_EXCEPTION_HTTP(c_ExceptionHttpForbidden,c_ExceptionHTTPStatus,"GeoREST HTTP Forbidden",HTTPResponse::HTTP_FORBIDDEN)

GEOREST_IMPLEMENT_EXCEPTION_HTTP(c_ExceptionHttpInternalError,c_ExceptionHTTPStatus,"GeoREST HTTP Internal Server Error",HTTPResponse::HTTP_INTERNAL_SERVER_ERROR)
GEOREST_IMPLEMENT_EXCEPTION_HTTP(c_ExceptionHttpNotImplemented,c_ExceptionHTTPStatus,"GeoREST HTTP Internal Server Error",HTTPResponse::HTTP_NOT_IMPLEMENTED)
