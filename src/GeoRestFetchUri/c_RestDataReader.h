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

#ifndef _c_RestDataReader_h
#define _c_RestDataReader_h

//
// c_RestDataReader is returned after completion of RestFetchUri execution.
//
// c_RestDataReader has same functionality as standard MapGuide FeatureReader only that allows also
// reading data not only from FeatureReader but also from XML ( future: json, kml, ... )
//
class REST_FETCHURI_API c_RestDataReader : public MgDisposable
{

public:
  void Dispose()
  {
    delete this;
  }

  INT32 GetClassId()
  {
    return 0; // m_cls_id;
  }      

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Advances the reader to the next item and returns true if there is
    /// another object to read or false if reading is complete. The default
    /// position of the reader is prior to the first item. Thus you must
    /// call ReadNext to begin accessing any data.
    ///
    /// \return
    /// Returns true if there is a next item.
    ///
    virtual bool ReadNext() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the definition of the object currently being read. If the user
    /// has requested only a subset of the class properties, the class
    /// definition reflects what the user has asked, rather than the full class
    /// definition.
    ///
    /// \return
    /// A MgClassDefinition representing the current object
    ///
    virtual MgClassDefinition* GetClassDefinition() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    ///  Returns true if the value of the specified property is null.
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns true if the value is null.
    ///
    virtual bool IsNull(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Boolean value of the specified property. No conversion is
    /// performed, thus the property must be a of boolean type the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the Boolean value.
    ///
    /// \exception MgInvalidArgumentException if the property type is not boolean
    ///
    virtual bool GetBoolean(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Byte value of the specified property. No conversion is
    /// performed, thus the property must be a of byte type or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the Byte value.
    ///
    /// \exception MgInvalidArgumentException if the property type is not byte
    ///
    virtual BYTE GetByte(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the DTime value of the specified property. No conversion is
    /// performed, thus the property must be a of date type or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the DTime value.
    ///
    /// \exception MgInvalidArgumentException if the property type is not date/time
    ///
    virtual MgDateTime* GetDateTime(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Single value of the specified property. No conversion is
    /// performed, thus the property must be a of type single or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the single value.
    ///
    /// \exception MgInvalidArgumentException if the property type is not float
    ///
    virtual float GetSingle(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Double value of the specified property. No conversion is
    /// performed, thus the property must be a of type double or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the double value.
    ///
    /// \exception MgInvalidArgumentException if the property type is not double
    ///
    virtual double GetDouble(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 16 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 16 bits or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the integer 16 bits value.
    ///
    /// \exception MgInvalidArgumentException if the property type is not int16
    ///
    virtual INT16 GetInt16(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 32 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 32 bits or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the integer 32 bits value.
    ///
    /// \exception MgInvalidArgumentException if the property type is not int32
    ///
    virtual INT32 GetInt32(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 64 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 64 bits or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the integer 64 bits value.
    /// Note: INT64 is actually a pointer to an Integer64 object
    ///
    /// \exception MgInvalidArgumentException if the property type is not int64
    ///
    virtual INT64 GetInt64(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the string value.
    ///
    /// \exception MgInvalidArgumentException if the property type is not a string
    ///
    virtual STRING GetString(CREFSTRING propertyName) = 0;

    

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geometry value of the specified property as a GeometryReader.
    /// Because no conversion is performed, the property must be
    /// of Geometric type; otherwise, the result is NULL.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the Geometry object.
    ///
    /// \exception MgInvalidArgumentException if the property type is not geometry
    ///
    virtual MgByteReader* GetGeometry(CREFSTRING propertyName) = 0;

  
    
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of properties in the result set.
    ///
    /// \return
    /// Returns the number of properties.
    ///
    virtual INT32 GetPropertyCount() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the property at the given ordinal position.
    ///
    /// \param index
    /// Input the position of the property.
    ///
    /// \return
    /// Returns the property name
    ///
    virtual STRING GetPropertyName(INT32 index) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the data type of the property with the specified name.
    /// Please refer to MgPropertyType for list of values
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the type of the property.
    ///
    virtual INT32 GetPropertyType(CREFSTRING propertyName) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes all features into an XML.
    /// XML is serialized from the current position of feature reader in the order
    /// data are retrieved.
    ///
    /// \return
    /// MgByteReader holding XML.
    ///
    //virtual MgByteReader* ToXml();
    //virtual MgByteReader* ToXml(int StartIndex,int MaxCount);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Releases all the resources of feature reader.
    /// This must be called when user is done with Feature Reader
    ///
    /// \return
    /// Nothing
    ///
    virtual void Close() = 0;

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the definition of the object currently being read. If the user
    /// has requested only a subset of the class properties, the class
    /// definition reflects what the user has asked, rather than the full class
    /// definition.
    ///
    /// \return
    /// A MgClassDefinition representing the current object
    ///
    //Note: Same as GetClassDefinition, however does not serialize
    //the class definition to XML in the case where our feature
    //reader and its caller are local
    //virtual MgClassDefinition* GetClassDefinitionNoXml();

    
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Geometry for the specified property. No conversion is
    /// performed, thus the property must be a of type Geometry or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns a ByteReader object
    ///
    //virtual BYTE_ARRAY_OUT GetGeometry(CREFSTRING propertyName, INT32& length);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the string value.
    ///
    //virtual const wchar_t* GetString(CREFSTRING propertyName, INT32& length);

    
    
    //virtual void ToXml(string &str,int StartIndex=-1,int MaxCount=-1);
    

    //void SetService(MgFeatureService* service);
    
    //c_RestDataReader(MgFeatureReader* FeatureReader);
    //c_RestDataReader(FdoIConnection* FdoConnection,FdoIFeatureReader* FdoReader);
    
    //~c_RestDataReader();
    
protected:
  //virtual void Dispose();
  //virtual INT32 GetClassId();    

private:

    //MgProperty* GetProperty(CREFSTRING propertyName, INT16 expectedType);
    /// Get the property for the specified name
    //MgProperty* GetProperty(CREFSTRING propertyName);
    //void UpdateCurrentSet(MgBatchPropertyCollection* bpCol);

    //Ptr<MgBatchPropertyCollection> m_set;
    //Ptr<MgPropertyDefinitionCollection> m_propDefCol;

    //STRING m_providerName;
    //INT32 m_currRecord;
    //INT32 m_serverDataReaderPtr;
    //MgFeatureService* m_service; // Do not want to implement a default constructor

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_DataReader;

  
public:
  enum e_Status
  {
    e_Unknown,
    e_OK,
    e_CountOverflow,
    e_BboxOverflow,
  };
  
protected:
  
  
 
  
};

class REST_FETCHURI_API c_RestDataReader_MgFeatureReader : public c_RestDataReader
{
public:
  c_RestDataReader_MgFeatureReader(MgProxyFeatureReader* MgReader);
  
public:
  virtual bool ReadNext();  
  virtual MgClassDefinition* GetClassDefinition();
  virtual bool IsNull(CREFSTRING propertyName);

  virtual bool GetBoolean(CREFSTRING propertyName);

  virtual BYTE GetByte(CREFSTRING propertyName);

  virtual MgDateTime* GetDateTime(CREFSTRING propertyName);

  virtual float GetSingle(CREFSTRING propertyName);

  virtual double GetDouble(CREFSTRING propertyName);

  virtual INT16 GetInt16(CREFSTRING propertyName);

  virtual INT32 GetInt32(CREFSTRING propertyName);

  virtual INT64 GetInt64(CREFSTRING propertyName);

  virtual STRING GetString(CREFSTRING propertyName);

  virtual MgByteReader* GetBLOB(CREFSTRING propertyName);

  virtual MgByteReader* GetCLOB(CREFSTRING propertyName);
  virtual MgByteReader* GetGeometry(CREFSTRING propertyName);

  virtual MgRaster* GetRaster(CREFSTRING propertyName);

  virtual INT32 GetPropertyCount();

  virtual STRING GetPropertyName(INT32 index);

  virtual INT32 GetPropertyType(CREFSTRING propertyName);

  virtual void Close();
  
protected:
  Ptr<MgProxyFeatureReader> m_FeatureReader;
};

class REST_FETCHURI_API c_RestDataReader_FdoFeatureReader : public c_RestDataReader
{
public:
  c_RestDataReader_FdoFeatureReader( FdoIConnection* FdoConnection,FdoIFeatureReader* FdoReader );
public:
  virtual bool ReadNext();  
  virtual MgClassDefinition* GetClassDefinition();
  virtual bool IsNull(CREFSTRING propertyName);

  virtual bool GetBoolean(CREFSTRING propertyName);

  virtual BYTE GetByte(CREFSTRING propertyName);

  virtual MgDateTime* GetDateTime(CREFSTRING propertyName);

  virtual float GetSingle(CREFSTRING propertyName);

  virtual double GetDouble(CREFSTRING propertyName);

  virtual INT16 GetInt16(CREFSTRING propertyName);

  virtual INT32 GetInt32(CREFSTRING propertyName);

  virtual INT64 GetInt64(CREFSTRING propertyName);

  virtual STRING GetString(CREFSTRING propertyName);

 
  virtual MgByteReader* GetGeometry(CREFSTRING propertyName);



  virtual INT32 GetPropertyCount();

  virtual STRING GetPropertyName(INT32 index);

  virtual INT32 GetPropertyType(CREFSTRING propertyName);

  virtual void Close();
  
protected:
  FdoPtr<FdoIConnection> m_FdoConnection;
  FdoPtr<FdoIFeatureReader> m_FdoReader;
};


#endif
