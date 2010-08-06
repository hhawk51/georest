//
//  Copyright (C) 2010 by Haris Kurtagic
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
#include "c_RestResult.h"
#include "Poco\UnicodeConverter.h"

#ifdef _WIN32
    #undef GetMessage
#endif

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new c_RestResult object.
/// </summary>
c_RestResult::c_RestResult()
{
  m_StatusCode = Poco::Net::HTTPResponse::HTTP_OK; // HTTP_STATUS_OK;
    m_pResultObject = NULL;
    
    m_FeatureReader_StartIndex = -1;
    m_FeatureReader_Count = 64;
    
    
    m_IsSitemapXML=false;
}


c_RestResult::c_RestResult(MgDisposable* resultObject)
{
    m_StatusCode = Poco::Net::HTTPResponse::HTTP_OK;
    m_pResultObject = resultObject;
    
    m_FeatureReader_StartIndex = -1;
    m_FeatureReader_Count = 64;
    
    m_IsSitemapXML=false;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor. This will clean up the result.
/// </summary>
c_RestResult::~c_RestResult()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the status code of the result.
/// <returns>
/// Success/Error code.
/// </returns>
/// </summary>
Poco::Net::HTTPResponse::HTTPStatus c_RestResult::GetStatusCode()
{
    return m_StatusCode;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the status code of the result.
/// <returns>
/// Nothing.
/// </returns>
/// </summary>
void c_RestResult::SetStatusCode(Poco::Net::HTTPResponse::HTTPStatus status)
{
    m_StatusCode = status;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the error message in the result.
/// <returns>
/// Error message or empty string if error message not set.
/// </returns>
/// </summary>
STRING c_RestResult::GetErrorMessage()
{
    return m_ErrorMessage;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the detailed error message in the result.
/// </summary>
STRING c_RestResult::GetDetailedErrorMessage()
{
    return m_DetailedMessage;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the HTTP error message in the result.
/// <returns>
/// Error message or empty string if error message not set.
/// </returns>
/// </summary>
STRING c_RestResult::GetHttpStatusMessage()
{
    return m_HttpStatusMessage;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the error message in the result.
/// <returns>
/// Nothing.
/// </returns>
/// </summary>
void c_RestResult::SetErrorMessage(CREFSTRING errorMsg)
{
    m_ErrorMessage = errorMsg;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the detailed error message in the result.
/// </summary>
void c_RestResult::SetDetailedErrorMessage(CREFSTRING errorMsg)
{
    m_DetailedMessage = errorMsg;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the HTTP error message in the result.
/// <returns>
/// Nothing.
/// </returns>
/// </summary>
void c_RestResult::SetHttpStatusMessage(CREFSTRING errorMsg)
{
    m_HttpStatusMessage = errorMsg;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the result object.
/// <returns>
/// Pointer to an object
/// </returns>
/// </summary>
MgDisposable* c_RestResult::GetResultObject()
{
    return SAFE_ADDREF((MgDisposable*)m_pResultObject);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the result object.
/// </summary>
/// <param name="resultObject">
/// object returned by the API
/// </param>
/// <returns>
/// Nothing.
/// </returns>
void c_RestResult::SetResultObject(MgDisposable* resultObject, CREFSTRING contentType)
{
    m_pResultObject = SAFE_ADDREF(resultObject);
    m_contentType = contentType;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the result content type.
/// <returns>
    /// Content type (e.g. "text/xml")
/// </returns>
/// </summary>
STRING c_RestResult::GetResultContentType()
{
    return m_contentType;
}

void c_RestResult::Dispose()
{
    delete this;
}

INT32 c_RestResult::GetClassId()
{
    return 0;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the error information based on a Mg exception.
/// </summary>
///----------------------------------------------------------------------------

void c_RestResult::SetErrorInfo(c_RestRequest* ,  MgException* mgException)
{
#ifdef _DEBUG
    assert(0 != mgException);
#endif

    Poco::Net::HTTPResponse::HTTPStatus httpStatusCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR; // HTTP_STATUS_INTERNAL_SERVER_ERROR;
    STRING httpStatusMessage;
    STRING errorMessage;
    STRING detailedMessage;

    try
    {
        // Map Mg exception to HTTP result.

        httpStatusMessage = mgException->GetClassName();
        #ifdef _MG_ENT_2011
          errorMessage = mgException->GetExceptionMessage();
        #else
          errorMessage = mgException->GetMessage();
        #endif
        detailedMessage = mgException->GetDetails();

#ifdef _DEBUG
        detailedMessage += L"\n";
        detailedMessage += mgException->GetStackTrace();
#endif

        //httpStatusCode = HTTP_STATUS_MG_ERROR;
    }
    catch (MgException* e)
    {
        SAFE_RELEASE(e);
    }
    catch (...)
    {
    }

    // Set the error information.

    try
    {
       

        SetStatusCode(httpStatusCode);
        //SetHttpStatusMessage(httpStatusMessage);
        SetErrorMessage(errorMessage);
        SetDetailedErrorMessage(detailedMessage);
    }
    catch (MgException* e)
    {
        SAFE_RELEASE(e);
    }
    catch (...)
    {
    }
}

void c_RestResult::SetErrorInfo( c_RestRequest* awRequest, c_ExceptionHTTPStatus& Exception )
{
  SetStatusCode(Exception.httpStatus());
  std::wstring tmpstr;
  Poco::UnicodeConverter::toUTF16(Exception.displayText(),tmpstr);
  SetErrorMessage(tmpstr);
}



void c_RestResult::SetErrorInfo( c_RestRequest* awRequest, Poco::Exception& Exception )
{
  SetStatusCode(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
  std::wstring tmpstr;
  Poco::UnicodeConverter::toUTF16(Exception.displayText(),tmpstr);
  SetErrorMessage(tmpstr);
}
void c_RestResult::SetErrorInfo( c_RestRequest* awRequest, FdoException*  Exception )
{
  SetStatusCode(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
  std::wstring tmpstr;
  if( Exception->GetExceptionMessage() )
    tmpstr = Exception->GetExceptionMessage();
  SetErrorMessage(tmpstr);
}
