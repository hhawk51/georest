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


#ifndef _c_FdoSelectFeatures_h
#define _c_FdoSelectFeatures_h

#include "MapGuideCommon.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "Fdo.h"
//#include "ServerGwsFeatureReader.h"

class MgFeatureQueryOptions;
class MgFeatureServiceCommand;
//template class Ptr<MgFeatureSourceCacheItem>;
class c_CfgDataSource_FDO;

class REST_FETCHURI_API c_FdoSelectFeatures
{
public:
    c_FdoSelectFeatures();
    ~c_FdoSelectFeatures();
    
    c_RestDataReader_FdoFeatureReader* SelectFeatures(c_CfgDataSource_FDO* FdoSource, MgFeatureQueryOptions* options);
private:
    void  ApplyQueryOptions(bool isSelectAggregate);
    void  ApplyClassProperties();
    void  ApplyComputedProperties();
    void  ApplyFilter();
    // void  ApplySpatialFilter();
    void  ApplyOrderingOptions();

    //bool HasCustomProperty() { return m_customPropertyFound; }
    //FdoFunction* GetCustomFunction() { return FDO_SAFE_ADDREF(m_customFunction); }
    //STRING GetCustomPropertyName() { return m_customPropertyName; }

    bool IsFdoSupportedFunction(FdoIConnection* connection, FdoFunction* fdoFunc);
    bool IsFdoSupportedFunction( FdoFunction* fdoFunc);
    bool ContainsUdf(FdoExpression* expression);
    bool IsCustomFunction(FdoFunction* fdoFunc);
    void AddFdoComputedProperty(CREFSTRING aliasName, FdoExpression* expression);
    void AddCustomComputedProperty(CREFSTRING aliasName, FdoExpression* expression);
    void ValidateConstraintsOnCustomFunctions();
    //void ApplyAggregateOptions(bool isSelectAggregate);
    void CreateCommand(c_CfgDataSource_FDO* FdoSource, bool isSelectAggregate);
    void ValidateParam(c_CfgDataSource_FDO* FdoSource, CREFSTRING className);
    //void ApplyFdoGroupingProperties(MgStringCollection* propertyNames);
    bool ContainsCustomFunction(MgFeatureQueryOptions* options);
    MgReader* GetCustomReader(MgReader* reader);

    // STRING                  m_providerName;
    //STRING                  className;
    Ptr<MgStringCollection> properties;
    Ptr<MgFeatureQueryOptions> m_options;
    STRING                  filterText;
    FdoPtr<FdoIConnection> m_FdoConn;
    FdoPtr<FdoISelect>    m_command;

    //Ptr<MgFeatureSourceCacheItem> m_featureSourceCacheItem;

    // Only one custom property is supported. No nesting of custom properties allowed.
    STRING                  m_customPropertyName;
    bool                    m_customPropertyFound;
    FdoPtr<FdoFunction>     m_customFunction;

    // Methods for Feature Join
    //bool FindFeatureJoinProperties(MgResourceIdentifier* resourceId, CREFSTRING extensionName);
    //bool FindFeatureCalculation(MgResourceIdentifier* resourceId, CREFSTRING extensionName);
    //  MgServerGwsFeatureReader* JoinFeatures(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName, FdoFilter* filter);
    //void UpdateCommandOnCalculation(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName);
    //void UpdateCommandOnJoinCalculation(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName);
    //void ParseQualifiedClassNameForCalculation(CREFSTRING qualifiedClassName, STRING& schemaName, STRING& className);
    //MgResourceIdentifier* GetSecondaryResourceIdentifier(MgResourceIdentifier* primResId, CREFSTRING extensionName, CREFSTRING relationName);

    // This setting limits the batch size used by the join query algorithm
    //INT32 m_nJoinQueryBatchSize;
};

#endif
