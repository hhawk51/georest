//
//  Copyright (C) 2009 by SL-King
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

#ifndef _c_RestUriRequestParam_H
#define _c_RestUriRequestParam_H

class c_RestUriRequestParam;
template class REST_COMMON_API Ptr<c_RestUriRequestParam>;

/// <summary>
/// Purpose of this class is to package request parameters into a collection.
/// Users can add/remove/modify parameters received from the actual request
/// for execution.
/// </summary>
class REST_COMMON_API c_RestUriRequestParam : public MgDisposable
{
    EXTERNAL_API:
        c_RestUriRequestParam();

        /// <summary>
        /// Adds the parameter and value to the collection.
        /// Parameter name  - case-insensitive
        /// Parameter value - case-sensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the parameter
        /// No two parameters can have same name.
        /// </param>
        /// <param name="value">Input
        /// Value corresponding to the parameter.
        /// NULL value is NOT allowed but empty string is allowed.
        /// </param>
        /// <returns>
        /// TRUE  -  Parameter is successfully added.
        /// FALSE -  Parameter could not be added.
        /// Possible cause is parameter already exist or its value is NULL.
        /// </returns>
        bool AddParameter(CREFSTRING name, CREFSTRING value, bool AddEmptyVal=false);

        /// <summary>
        /// Removes the parameter and value from the collection.
        /// Parameter name  - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the parameter to be removed.
        /// </param>
        /// <returns>
        /// TRUE  -  Parameter is successfully removed.
        /// FALSE -  Parameter could not be removed.
        /// Possible cause is parameter does not exist.
        /// </returns>
        bool RemoveParameter(CREFSTRING name);

        /// <summary>
        /// Retrieve the value for the specified parameter.
        /// Parameter name  - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the parameter for which value to be retrieved.
        /// </param>
        /// <returns>
        /// Value for the specified parameter or empty string if parameter does not
        /// exist.
        /// </returns>
        STRING GetParameterValue(CREFSTRING parameterName);

        /// <summary>
        /// Retrieve the type for the specified parameter.  This is mime
        /// type for file parameters
        /// Parameter name  - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the parameter for which type to be retrieved.
        /// </param>
        /// <returns>
        /// Value for the specified parameter or empty string if parameter does not
        /// exist.
        /// </returns>
        STRING GetParameterType(CREFSTRING parameterName);

        /// <summary>
        /// Update the value for the specified parameter.
        /// Parameter name  - case-insensitive
        /// Parameter value - case-sensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the parameter to be updated.
        /// </param>
        /// <param name="value">Input
        /// Value corresponding to the parameter.
        /// NULL value is NOT allowed but empty string is allowed.
        /// </param>
        /// <returns>
        /// TRUE  -  Parameter is successfully updated.
        /// FALSE -  Parameter could not be updated.
        /// Possible cause is parameter does not exist.
        /// </returns>
        bool SetParameterValue(CREFSTRING name, CREFSTRING value);

        /// <summary>
        /// Update the type for the specified parameter.  This method
        /// sets file mime types.
        /// Parameter name  - case-insensitive
        /// Parameter value - case-sensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the parameter to be updated.
        /// </param>
        /// <param name="value">Input
        /// Type of the parameter.
        /// NULL value is NOT allowed but empty string is allowed.
        /// </param>
        /// <returns>
        /// TRUE  -  Parameter is successfully updated.
        /// FALSE -  Parameter could not be updated.
        /// Possible cause is parameter does not exist.
        /// </returns>
        bool SetParameterType(CREFSTRING name, CREFSTRING value);

        /// <summary>
        /// Retrieve the list of all parameters.
        /// </summary>
        /// <returns>
        /// A string collection containing names of all parameters.
        /// </returns>
        MgStringCollection* GetParameterNames();

        /// <summary>
        /// Tells if the specified parameter exists.
        /// </summary>
        /// <param name="name">Input
        /// Name of the parameter.
        /// </param>
        /// <returns>
        /// True is the specified parameters exists
        /// </returns>
        bool ContainsParameter(CREFSTRING name);
        
        
        // RemoveParams- parameters to be removed from original parameters
        // AddParams- parameters to be be added to original parameters
        // If parameter allready exists and needs to be replaced then it has to be added to remove and also to add list
        // if only in add list then it will be added if allready not defined
        void GetAsUriQuery(REFSTRING Query,MgStringCollection* RemoveParams=NULL,MgStringPropertyCollection* AddParams=NULL);

        //void GetCount(REFSTRING Query);

    INTERNAL_API:
        /// <summary>
        /// Destructor to clean up internal objects
        /// </summary>
        virtual ~c_RestUriRequestParam();

        /// <summary>
        /// Retrieve the list of all parameters.
        /// </summary>
        /// <returns>
        /// A string property collection containing all parameters.
        /// </returns>
        MgStringPropertyCollection* GetParameters();

        /// <summary>
        /// Returns POSTed XML data as a string
        /// </summary>
        /// <returns>
        /// A string containing the XML data
        /// </returns>
        string& GetXmlPostData();

        string& GetPostData();
        /// <summary>
        /// Stores POSTed XML data as a string
        /// </summary>
        /// <param name="name">Input
        /// The XML data
        /// </param>
        void SetXmlPostData(const char* xmlData);
        void SetPostData(std::string xmlData);
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
        

    protected:
        Ptr<MgStringPropertyCollection> m_reqParamCollection;
        MgStringPropertyCollection m_paramTypeCollection;
        string m_xmlPostData;
        string m_PostData;


};

#endif
