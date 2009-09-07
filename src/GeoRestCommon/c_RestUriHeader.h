//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _c_RestUriHeader_h
#define _c_RestUriHeader_h

#define D_MISSING_HEADER -1
#define D_CANNOT_CONVERT -2
#define D_API_NOT_FOUND  -3

class c_RestUriHeader;
template class REST_COMMON_API Ptr<c_RestUriHeader>;

/// <summary>
/// Purpose of this class is to package header variables into a collection.
/// Users can add/remove/modify headers received from the actual request.
/// </summary>
class REST_COMMON_API c_RestUriHeader : public MgDisposable
{
    EXTERNAL_API:
        c_RestUriHeader();

        /// <summary>
        /// Adds the header name and value to the collection.
        /// header name  -    case-insensitive
        /// header value -    case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the header
        /// Two header can have same name.
        /// </param>
        /// <param name="value">Input
        /// Value corresponding to the header.
        /// NULL value and empty string is NOT allowed.
        /// </param>
        /// <returns>
        /// TRUE  -  Header name is successfully added.
        /// FALSE -  Header could not be added.
        /// Possible cause - header value is NULL.
        /// </returns>
        bool AddHeader(CREFSTRING name, CREFSTRING value);

        /// <summary>
        /// Removes the header name and value from the collection.
        /// header name - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the header to be removed.
        /// </param>
        /// <returns>
        /// TRUE  -  header name is successfully removed.
        /// FALSE -  header name could not be removed.
        /// Possible cause is header name does not exist.
        /// </returns>
        bool RemoveHeader(CREFSTRING name);

        /// <summary>
        /// Retrieve the value for the specified header.
        /// header name  - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the header for which value to be retrieved.
        /// </param>
        /// <returns>
        /// Value for the specified header or empty string if header does not exist.
        /// </returns>
        STRING GetHeaderValue(CREFSTRING name);

        /// <summary>
        /// Update the value for the specified header.
        /// header name  - case-insensitive
        /// header value - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the header to be updated.
        /// </param>
        /// <param name="value">Input
        /// Value corresponding to the header.
        /// NULL value and empty string is NOT allowed.
        /// </param>
        /// <returns>
        /// TRUE  -  header is successfully updated.
        /// FALSE -  header could not be updated.
        /// Possible cause is header does not exist or value is NULL.
        /// </returns>
        bool SetHeaderValue(CREFSTRING name, CREFSTRING value);

        /// <summary>
        /// Returns the value of the specified header as a long value that represents a Date object.
        /// Use this method with headers that contain dates, such as If-Modified-Since.
        /// The date is returned as the number of milliseconds since January 1, 1970 GMT.
        /// </summary>
        /// <param name="name">Input
        /// Name of the header (case insensitive).
        /// </param>
        /// <returns>
        /// long - The date is returned as the number of milliseconds since January 1, 1970 GMT
        /// S_MISSING_HEADER -   request did not have a header of the specified name
        /// S_CANNOT_CONVERT -   header can't be converted to a date
        /// </returns>
        long GetDateHeader(CREFSTRING name);

        /// <summary>
        /// Returns the value of the specified request header as an int.
        /// </summary>
        /// <param name="name">Input
        /// Name of the header (case-insensitive).
        /// </param>
        /// <returns>
        /// INT32 - An integer value
        /// S_MISSING_HEADER -   request did not have a header of the specified name
        /// S_CANNOT_CONVERT -   header can't be converted to an integer.
        /// </returns>
        INT32 GetIntHeader(CREFSTRING name);

        /// <summary>
        /// Retrieve the list of all headers.
        /// </summary>
        /// <returns>
        /// A string collection containing names of all headers.
        /// </returns>
        MgStringCollection* GetHeaderNames();

        /// <summary>
        /// Retrieve the list of all possible values for a header.
        /// </summary>
        /// <returns>
        /// A string collection containing possible values of a header.
        /// </returns>
        MgStringCollection* GetHeaders(CREFSTRING name);

    INTERNAL_API:
        /// <summary>
        /// Destructor to clean up internal objects
        /// </summary>
        virtual ~c_RestUriHeader();

    protected:
        /// <summary>
        /// This method is responsible for releasing all
        /// its member variables and then destruct itself.
        /// </summary>
        /// <returns>Returns nothing</returns>
        virtual void Dispose();

        /// <summary>
        /// Get the class Id
        /// </summary>
        /// <returns>
        /// The integer value
        /// </returns>
        INT32 GetClassId();

    private:
        MgStringPropertyCollection m_headerCollection;

};

#endif
