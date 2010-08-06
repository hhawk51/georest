//
//  Copyright (C) 2009 by Haris Kurtagic
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

#ifndef _c_RestPrimitiveValue_h
#define _c_RestPrimitiveValue_h



//////////////////////////////////////////////////////////////////
/// <summary>
/// Represents a primitive value returned by a call to a Mg Service.
/// </summary>
class REST_REQUEST_HANDLER_API c_RestPrimitiveValue : public MgDisposable
{
EXTERNAL_API:  
    enum e_PrimitiveType 
    {
      e_PrimitiveTypeBool,
      e_PrimitiveTypeInt,
      e_PrimitiveTypeString,
      e_PrimitiveTypeStringUTF8,
    };
EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructor. This will clean up the value.
    /// </summary>
    virtual ~c_RestPrimitiveValue();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructor. Create a new c_RestPrimitiveValue object initialized
    /// with a boolean value
    /// </summary>
    c_RestPrimitiveValue(bool value);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructor. Create a new c_RestPrimitiveValue object initialized
    /// with an integer value
    /// </summary>
    c_RestPrimitiveValue(INT32 value);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructor. Create a new c_RestPrimitiveValue object initialized
    /// with a string value
    /// </summary>
    c_RestPrimitiveValue(CREFSTRING value);
    
    c_RestPrimitiveValue(const std::string& value);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the type of the data held by this object
    /// </summary>
    e_PrimitiveType GetType();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the boolean primitive value
    /// </summary>
    bool GetBoolValue();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the integer primitive value
    /// </summary>
    INT32 GetIntegerValue();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the string primitive value
    /// </summary>
    STRING GetStringValue();
    
    const std::string& GetStringUTF8Value() const;

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get a string representation of the primitive value
    /// </summary>
    STRING ToString();
    
    std::string ToStringUTF8();

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

    union Val
    {
        bool    b;
        INT32   i;
        STRING* pstr;
        std::string* pstr_utf8;
    } m_value;

    e_PrimitiveType m_PrimitiveType;

CLASS_ID:
    static const INT32 m_cls_id = 0; // HttpHandler_MapAgent_HttpPrimitiveValue;
};

#endif
