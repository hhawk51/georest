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

#ifndef _c_ExceptionGeoREST_h
#define _c_ExceptionGeoREST_h

#include "Poco\Exception.h"
#include "Poco\Net\HTTPResponse.h"

// Base Class for all exceptions inside GeoREST
POCO_DECLARE_EXCEPTION(REST_COMMON_API,c_ExceptionGeoREST,Poco::Exception)

POCO_DECLARE_EXCEPTION(REST_COMMON_API,c_ExceptionInvalidKeyValueString,c_ExceptionGeoREST)

POCO_DECLARE_EXCEPTION(REST_COMMON_API,c_ExceptionODataInvalidAtomEntry,c_ExceptionGeoREST)

POCO_DECLARE_EXCEPTION(REST_COMMON_API,c_ExceptionODataReader,c_ExceptionGeoREST)

POCO_DECLARE_EXCEPTION(REST_COMMON_API,c_ExceptionODataInvalidFilter,c_ExceptionGeoREST)

using Poco::Net::HTTPResponse;

// Base class for all exceptions which will result in corresponding HTTP status code and reason
class REST_COMMON_API c_ExceptionHTTPStatus :  public c_ExceptionGeoREST
{
public:
  c_ExceptionHTTPStatus(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,int code = 0);
  c_ExceptionHTTPStatus(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg, int code = 0);
  c_ExceptionHTTPStatus(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg,const Poco::Exception& exc, int code = 0);
  ~c_ExceptionHTTPStatus(void) throw();
  
  virtual c_ExceptionHTTPStatus& operator = (const c_ExceptionHTTPStatus& exc);
  
  virtual const char* name() const throw() { return "GeoREST base exception with HTTP Status Code"; }
  /// Returns a static string describing the exception.

  virtual const char* className() const throw() { return typeid(*this).name(); }
  /// Returns the name of the exception class.
  
  HTTPResponse::HTTPStatus httpStatus() const { return m_HttpStatus; }
  const std::string& httpStatusReason() const { return  m_HttpStatusReason; }
  
 

  virtual Exception* clone() const { return new c_ExceptionHTTPStatus(*this); }
  /// Creates an exact copy of the exception.
  ///
  /// The copy can later be thrown again by
  /// invoking rethrow() on it.

  virtual void rethrow() const { throw *this; }
  /// (Re)Throws the exception.
  ///
  /// This is useful for temporarily storing a
  /// copy of an exception (see clone()), then
  /// throwing it again.
  
protected:
  c_ExceptionHTTPStatus(HTTPResponse::HTTPStatus Status=HTTPResponse::HTTP_INTERNAL_SERVER_ERROR, int code=0);
  
protected:
  HTTPResponse::HTTPStatus m_HttpStatus;
  std::string m_HttpStatusReason;
};

/*
#define GEOREST_DECLARE_EXCEPTION_HTTP(API, CLS, BASE) \
  class API CLS: public BASE														\
  {																				\
  public:																			\
    CLS(int code = 0);															\
    CLS(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg, int code = 0);									\
    CLS(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg, const std::string& arg, int code = 0);			\
    CLS(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg, const Poco::Exception& exc, int code = 0);		\
    CLS(const CLS& exc);														\
    ~CLS() throw();																\
    CLS& operator = (const CLS& exc);											\
    const char* name() const throw();											\
    const char* className() const throw();										\
    Poco::Exception* clone() const;												\
    void rethrow() const;														\
  };


#define GEOREST_IMPLEMENT_EXCEPTION_HTTP(CLS, BASE, NAME)													\
  CLS::CLS(int code): BASE(code)																	\
{																								\
}																								\
  CLS::CLS(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg, int code): BASE(Status,StatusReason,msg, code)										\
{																								\
}																								\
  CLS::CLS(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg, const std::string& arg, int code): BASE(Status,StatusReason,msg, arg, code)		\
{																								\
}																								\
  CLS::CLS(HTTPResponse::HTTPStatus Status,const std::string& StatusReason,const std::string& msg, const Poco::Exception& exc, int code): BASE(Status,StatusReason,msg, exc, code)	\
{																								\
}																								\
  CLS::CLS(const CLS& exc): BASE(exc)																\
{																								\
}																								\
  CLS::~CLS() throw()																				\
{																								\
}																								\
  CLS& CLS::operator = (const CLS& exc)															\
{																								\
  BASE::operator = (exc);																		\
  return *this;																				\
}																								\
  const char* CLS::name() const throw()															\
{																								\
  return NAME;																				\
}																								\
  const char* CLS::className() const throw()														\
{																								\
  return typeid(*this).name();																\
}																								\
  Poco::Exception* CLS::clone() const																\
{																								\
  return new CLS(*this);																		\
}																								\
  void CLS::rethrow() const																		\
{																								\
  throw *this;																				\
}
*/

#define GEOREST_DECLARE_EXCEPTION_HTTP(API, CLS, BASE) \
class API CLS: public BASE														\
  {																				\
  public:																			\
  CLS(int code = 0);															\
  CLS(const std::string& msg, int code = 0);									\
  CLS(const std::string& msg, const std::string& arg, int code = 0);			\
  CLS(const std::string& msg, const Poco::Exception& exc, int code = 0);		\
  CLS(const CLS& exc);														\
  ~CLS() throw();																\
  CLS& operator = (const CLS& exc);											\
  const char* name() const throw();											\
  const char* className() const throw();										\
  Poco::Exception* clone() const;												\
  void rethrow() const;														\
};


#define GEOREST_IMPLEMENT_EXCEPTION_HTTP(CLS, BASE, NAME, HTTPSTATUS)													\
  CLS::CLS(int code): BASE(HTTPSTATUS,code)																	\
{																								\
}																								\
  CLS::CLS(const std::string& msg, int code): BASE(HTTPSTATUS,"",msg, code)										\
{																								\
}																								\
  CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(HTTPSTATUS,"",msg, arg, code)		\
{																								\
}																								\
  CLS::CLS(const std::string& msg, const Poco::Exception& exc, int code): BASE(HTTPSTATUS,"",msg, exc, code)	\
{																								\
}																								\
  CLS::CLS(const CLS& exc): BASE(exc)																\
{																								\
}																								\
  CLS::~CLS() throw()																				\
{																								\
}																								\
  CLS& CLS::operator = (const CLS& exc)															\
{																								\
  BASE::operator = (exc);																		\
  return *this;																				\
}																								\
  const char* CLS::name() const throw()															\
{																								\
  return NAME;																				\
}																								\
  const char* CLS::className() const throw()														\
{																								\
  return typeid(*this).name();																\
}																								\
  Poco::Exception* CLS::clone() const																\
{																								\
  return new CLS(*this);																		\
}																								\
  void CLS::rethrow() const																		\
{																								\
  throw *this;																				\
}

GEOREST_DECLARE_EXCEPTION_HTTP(REST_COMMON_API,c_ExceptionHttpBadRequest,c_ExceptionHTTPStatus)
GEOREST_DECLARE_EXCEPTION_HTTP(REST_COMMON_API,c_ExceptionHttpNotFound,c_ExceptionHTTPStatus)
GEOREST_DECLARE_EXCEPTION_HTTP(REST_COMMON_API,c_ExceptionHttpForbidden,c_ExceptionHTTPStatus)

GEOREST_DECLARE_EXCEPTION_HTTP(REST_COMMON_API,c_ExceptionHttpInternalError,c_ExceptionHTTPStatus)
GEOREST_DECLARE_EXCEPTION_HTTP(REST_COMMON_API,c_ExceptionHttpNotImplemented,c_ExceptionHTTPStatus)


#endif