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

#ifndef _c_RestUriPathParam_H
#define _c_RestUriPathParam_H

#include "c_RestUriRequestParam.h"
class c_RestUriPathParam;
template class REST_COMMON_API Ptr<c_RestUriPathParam>;

/// <summary>
/// Purpose of this class is to package request parameters into a collection.
/// Users can add/remove/modify parameters received from the actual request
/// for execution.
/// </summary>
class REST_COMMON_API c_RestUriPathParam : public c_RestUriRequestParam
{
    public:
      c_RestUriPathParam();
      ~c_RestUriPathParam();
      
public: 
    int GetParametersCount();
    STRING GetCurrentParameterName() const;
    STRING GetCurrentParameterValue() const;

    void SetCurrentParameterIndex(int Index);
    int GetCurrentParameterIndex();
    void ResetParameterCurrentIndex();
    bool NextParameter();
    bool IsEndOfParameters();
    //bool IsLastParameter();

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
      int m_CurrentParamInd;
       


};

#endif
