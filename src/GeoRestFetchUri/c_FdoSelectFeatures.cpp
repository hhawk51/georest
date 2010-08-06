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
#include "MapGuideCommon.h"
#include "RestConfigInclude.h"
//#include "ServerFeatureServiceDefs.h"
//#include "Services/FeatureService.h"
#include "c_FdoSelectFeatures.h"
//#include "ServerFeatureConnection.h"
//#include "ServerFeatureReader.h"
//#include "ServerFeatureUtil.h"
//#include "FeatureServiceCommand.h"
//#include "SelectCommand.h"
//#include "SelectAggregateCommand.h"
//#include "FeatureDistribution.h"
//#include "ServerGwsFeatureReader.h"
//#include "GwsConnectionPool.h"
//#include "GwsCommonImp.h"
//#include "GwsMutableFeature.h"
//#include "GwsFdoCommand.h"
//#include "GwsQuery.h"
#include "FdoExpressionEngineCopyFilter.h"
//#include "CacheManager.h"
#include "Poco\UnicodeConverter.h"
#include "c_ExceptionGeoREST.h"

#define EQUAL_CATEGORY      0
#define STDEV_CATEGORY      1
#define QUANTILE_CATEGORY   2
#define JENK_CATEGORY       3
#define MINIMUM             4
#define MAXIMUM             5
#define MEAN                6
#define STANDARD_DEV        7
#define UNIQUE              8
#define EXTENT              9

static bool Initialize();

static std::map<FdoPropertyType, INT32>                 s_FdoPropertyType;
static std::map<FdoDataType, INT32>                     s_FeatureUtilFdoDataType;
static std::map<INT32, FdoOrderingOption>               s_FdoOrderingOption;
static std::map<INT32, STRING>                          s_CustomFunctions;
static std::map<INT32, FdoSpatialOperations> s_FdoSpatialOperation;

bool m_isInitialized = Initialize();

bool Initialize()
{
  s_FeatureUtilFdoDataType[FdoDataType_Boolean]  = MgPropertyType::Boolean;
  s_FeatureUtilFdoDataType[FdoDataType_Byte]     = MgPropertyType::Byte;
  s_FeatureUtilFdoDataType[FdoDataType_DateTime] = MgPropertyType::DateTime;
  s_FeatureUtilFdoDataType[FdoDataType_Double]   = MgPropertyType::Double;
  s_FeatureUtilFdoDataType[FdoDataType_Int16]    = MgPropertyType::Int16;
  s_FeatureUtilFdoDataType[FdoDataType_Int32]    = MgPropertyType::Int32;
  s_FeatureUtilFdoDataType[FdoDataType_Int64]    = MgPropertyType::Int64;
  s_FeatureUtilFdoDataType[FdoDataType_Single]   = MgPropertyType::Single;
  s_FeatureUtilFdoDataType[FdoDataType_String]   = MgPropertyType::String;
  s_FeatureUtilFdoDataType[FdoDataType_BLOB]     = MgPropertyType::Blob;
  s_FeatureUtilFdoDataType[FdoDataType_CLOB]     = MgPropertyType::Clob;

  // Implementation Note:  FdoDataType_Decimal is currently mapped to MgPropertyType::Double.
  // An MgDecimalProperty class should be implemented in a future release.
  s_FeatureUtilFdoDataType[FdoDataType_Decimal]  = MgPropertyType::Double;

  s_FdoOrderingOption[MgOrderingOption::Ascending]  = FdoOrderingOption_Ascending;
  s_FdoOrderingOption[MgOrderingOption::Descending] = FdoOrderingOption_Descending;

  s_CustomFunctions[EQUAL_CATEGORY]       = L"EQUAL_DIST";
  s_CustomFunctions[STDEV_CATEGORY]       = L"STDEV_DIST";
  s_CustomFunctions[QUANTILE_CATEGORY]    = L"QUANT_DIST";
  s_CustomFunctions[JENK_CATEGORY]        = L"JENK_DIST";
  s_CustomFunctions[MINIMUM]              = L"MINIMUM";
  s_CustomFunctions[MAXIMUM]              = L"MAXIMUM";
  s_CustomFunctions[MEAN]                 = L"MEAN";
  s_CustomFunctions[STANDARD_DEV]         = L"STD_DEV";
  s_CustomFunctions[UNIQUE]               = L"UNIQUE";
  s_CustomFunctions[EXTENT]               = L"EXTENT";


  s_FdoSpatialOperation[MgFeatureSpatialOperations::Contains]            =   FdoSpatialOperations_Contains;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::Crosses]             =   FdoSpatialOperations_Crosses;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::Disjoint]            =   FdoSpatialOperations_Disjoint;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::Equals]              =   FdoSpatialOperations_Equals;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::Intersects]          =   FdoSpatialOperations_Intersects;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::Overlaps]            =   FdoSpatialOperations_Overlaps;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::Touches]             =   FdoSpatialOperations_Touches;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::Within]              =   FdoSpatialOperations_Within;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::CoveredBy]           =   FdoSpatialOperations_CoveredBy;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::Inside]              =   FdoSpatialOperations_Inside;
  s_FdoSpatialOperation[MgFeatureSpatialOperations::EnvelopeIntersects]  =   FdoSpatialOperations_EnvelopeIntersects;

  s_FdoPropertyType[FdoPropertyType_RasterProperty]       = MgFeaturePropertyType::RasterProperty;
  s_FdoPropertyType[FdoPropertyType_DataProperty]         = MgFeaturePropertyType::DataProperty;
  s_FdoPropertyType[FdoPropertyType_GeometricProperty]    = MgFeaturePropertyType::GeometricProperty;
  s_FdoPropertyType[FdoPropertyType_ObjectProperty]       = MgFeaturePropertyType::ObjectProperty;
  s_FdoPropertyType[FdoPropertyType_AssociationProperty]  = MgFeaturePropertyType::AssociationProperty;

  return true;
}

FdoOrderingOption GetFdoOrderingOption(INT32 orderingOption)
{
  return s_FdoOrderingOption[orderingOption];
}
FdoSpatialOperations GetFdoSpatialOperation(INT32 spatialOp)
{
  
  return s_FdoSpatialOperation[spatialOp];
}
bool FindCustomFunction(CREFSTRING funcName, INT32 &index)
{
  int cnt = (int)s_CustomFunctions.size();
  bool supports = false;
  index = -1;

  // No function name is no custom function
  if (!funcName.empty())
  {
    // look for custom function name
    for (int i=0; i < cnt; i++)
    {
      index++;
      int cmp = _wcsicmp(s_CustomFunctions[i].c_str(), funcName.c_str());
      if (cmp == 0)
      {
        supports = true;
        break;
      }
    }
  }
  return supports;
}
void ValidateCustomConstraints(FdoFunction* customFunction)
{
  if (customFunction == NULL) { return; } // Nothing to do here

  // Custom (Distribution) methods should have atleast two properties. Name of property and Number of categories
  FdoPtr<FdoExpressionCollection> exprCol = customFunction->GetArguments();
  FdoInt32 cnt = exprCol->GetCount();

  INT32 funcCode;
  bool supported = FindCustomFunction(STRING(customFunction->GetName()), funcCode);

  if (supported)
  {
    switch(funcCode)
    {
    case EQUAL_CATEGORY: // Equal Category
    case STDEV_CATEGORY: // StdDev Category
    case QUANTILE_CATEGORY: // Quantile Category
    case JENK_CATEGORY: // Jenk Category
      {
        if (cnt < 2)
        {
          STRING message = L"Invalid Computed Property"; // GetMessage(L"MgInvalidComputedProperty");

          MgStringCollection arguments;
          arguments.Add(message);
          throw new MgFeatureServiceException(L"MgServerFeatureUtil.ValidateCustomConstraints",
            __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        break;
      }
    case MINIMUM:
    case MAXIMUM:
    case MEAN:
    case STANDARD_DEV:
    case UNIQUE:
    case EXTENT:
      {
        if (cnt > 1)
        {
          STRING message = L"Invalid Computed Property"; // GetMessage(L"MgInvalidComputedProperty");

          MgStringCollection arguments;
          arguments.Add(message);
          throw new MgFeatureServiceException(L"MgServerFeatureUtil.ValidateCustomConstraints",
            __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        break;
      }
    }
  }
}


c_FdoSelectFeatures::c_FdoSelectFeatures()
{
    m_cmd_select = NULL;
    m_is_cmd_ext_select = false;
    m_options = NULL;
    m_customPropertyFound = false;
    m_customFunction = NULL;
    m_customPropertyName = L"";
    

    
}

c_FdoSelectFeatures::~c_FdoSelectFeatures()
{
}

// Executes the select features command and serializes the reader
c_RestDataReader_FdoFeatureReader* c_FdoSelectFeatures::SelectFeatures(const c_CfgDataSource_FDO* FdoSource,
                                                 MgFeatureQueryOptions* options)
{
    
    //bool isSelectAggregate = executeSelectAggregate;
    bool isSelectAggregate = false;


try
{
    // Validate parameters
    ValidateParam(FdoSource,FdoSource->GetFeatureClass());

    

    // Check if a feature join is to be performed by inspecting the resource for join properties
    bool bFeatureJoinProperties = false; //FindFeatureJoinProperties(resource, className);
    // Check if a feature join is only a calculation
    //bool bFeatureCalculation = FindFeatureCalculation(resource, className);
    
    {
        // Custom function specified from Select command is not allowed
        if (!isSelectAggregate && ContainsCustomFunction(options))
        {
            STRING message = L"Custom function specified from Select command is not allowed";

            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgFeatureServiceException(L"c_FdoSelectFeatures.SelectFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // Custom function specified using SelectAggregate,
        // we execute as standard Select command on single property
        // then apply function on top of it.
        bool useClassName = true;
        if (isSelectAggregate && ContainsCustomFunction(options))
        {
            isSelectAggregate = false;
            useClassName = false;
        }

        bool isordering=false;
        Ptr<MgStringCollection> ordr_coll = options->GetOrderingProperties();
        if( ordr_coll.p && ordr_coll->GetCount() > 0) isordering=true;

        // Create Command
        CreateCommand(FdoSource, isSelectAggregate,isordering);

        
        m_cmd_select->SetFeatureClassName(FdoSource->GetFeatureClass().c_str());
        
        // Set options (NULL is a valid value)
        m_options = SAFE_ADDREF(options);

        // Apply options to FDO command
        ApplyQueryOptions(isSelectAggregate);

        
        m_cmd_select->SetFeatureClassName(FdoSource->GetFeatureClass().c_str());

        // If custom function is found, then validate that no more than one function/property is specified
        ValidateConstraintsOnCustomFunctions();

        // Execute the command
        FdoPtr<FdoIFeatureReader> fdoreader;
        if( m_is_cmd_ext_select )
        {
          
          fdoreader = (FdoIFeatureReader*)(((FdoIExtendedSelect*)(m_cmd_select.p))->ExecuteScrollable());
        }
        else
          fdoreader = m_cmd_select->Execute();
         
        //reader = m_command->Execute();
        
        //FdoPtr<MgFdoFeatureReader> featureReader = new MgFdoFeatureReader(frc);
        
        return new c_RestDataReader_FdoFeatureReader(m_FdoConn, fdoreader);
        
    }

}                                                                         \
catch (FdoException* e)                                                   \
{
  std::wstring wstr = e->GetExceptionMessage();
  FDO_SAFE_RELEASE(e);
  throw new MgRuntimeException(L"c_FdoSelectFeatures::SelectFeatures",__LINE__, __WFILE__, NULL, wstr, NULL);
  
}

    return NULL;
}//end of c_FdoSelectFeatures::SelectFeatures

void c_FdoSelectFeatures::ApplyQueryOptions(bool isSelectAggregate)
{
    CHECKNULL(m_cmd_select, L"c_FdoSelectFeatures.ApplyQueryOptions");

    if (m_options != NULL)
    {
        ApplyClassProperties();
        ApplyComputedProperties();
        ApplyFilter();
        // ApplySpatialFilter();
        ApplyOrderingOptions();
        //ApplyAggregateOptions(isSelectAggregate);
    }
}

// ClassProperties
void c_FdoSelectFeatures::ApplyClassProperties()
{
    CHECKNULL(m_options, L"c_FdoSelectFeatures.ApplyClassProperties");
    CHECKNULL(m_cmd_select, L"c_FdoSelectFeatures.ApplyClassProperties");

    Ptr<MgStringCollection> properties = m_options->GetClassProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    FdoPtr<FdoIdentifierCollection> fic = m_cmd_select->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"c_FdoSelectFeatures.ApplyClassProperties");

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
        CHECKNULL((FdoIdentifier*)fdoIden, L"c_FdoSelectFeatures.ApplyClassProperties");

        fic->Add(fdoIden);
    }
}


// Computed properties
void c_FdoSelectFeatures::ApplyComputedProperties()
{
    CHECKNULL(m_options, L"c_FdoSelectFeatures.ApplyComputedProperties");
    CHECKNULL(m_cmd_select, L"c_FdoSelectFeatures.ApplyComputedProperties");

    Ptr<MgStringPropertyCollection> properties = m_options->GetComputedProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    // TODO: Add support for custom functions

    for (INT32 i=0; i < cnt; i++)
    {
        STRING aliasName = properties->GetName(i);
        STRING expression = properties->GetValue(i);

        FdoString* str = (FdoString*)expression.c_str();
        if (str != NULL)
        {
            FdoPtr<FdoExpression> expression = FdoExpression::Parse(str);
            CHECKNULL((FdoExpression*)expression, L"c_FdoSelectFeatures.ApplyComputedProperties");

            bool udf = ContainsUdf(expression);
            if (!udf)
            {
                AddFdoComputedProperty(aliasName, expression);
            }
            else
            {
                AddCustomComputedProperty(aliasName, expression);
            }
        }
    }
}

//// Filter text
//void c_FdoSelectFeatures::ApplyFilter()
//{
//    CHECKNULL(m_options, L"c_FdoSelectFeatures.ApplyFilter");
//    CHECKNULL(m_command, L"c_FdoSelectFeatures.ApplyFilter");
//
//    STRING filterText = m_options->GetFilter();
//    if (filterText.empty()) { return; } // Nothing to do
//
//    m_command->SetFilter(filterText.c_str());
//}

// Spatial Filter
void c_FdoSelectFeatures::ApplyFilter()
{
    CHECKNULL(m_options, L"c_FdoSelectFeatures.ApplyFilter");
    CHECKNULL(m_cmd_select, L"c_FdoSelectFeatures.ApplyFilter");

    FdoPtr<FdoFilter> regularFilter;
    FdoPtr<FdoSpatialCondition> spatialFilter;
    FdoPtr<FdoFilter> combineFilter;
    FdoBinaryLogicalOperations bOper = FdoBinaryLogicalOperations_And;

    // Build regular filter
    STRING filterText = m_options->GetFilter();
    if (!filterText.empty())
    {
        regularFilter = FdoFilter::Parse(filterText.c_str());
        #ifdef _DEBUG
        ACE_DEBUG((LM_ERROR, ACE_TEXT("FILTER(size=%d):\n%W\n\n"), filterText.length(), filterText.c_str()));
        #endif
    }

    // Build spatial filter
    bool isAnd = m_options->GetBinaryOperator();
    Ptr<MgGeometry> geom = m_options->GetGeometry();
    INT32 spatialOp = m_options->GetSpatialOperation();

    STRING geomProp = m_options->GetGeometryProperty();

    if (!isAnd) // Apply OR operator
    {
        bOper = FdoBinaryLogicalOperations_Or;
    }

    // Check whether we have valid geometric filter
    if (!geomProp.empty() && (geom != NULL))
    {
        MgAgfReaderWriter agfWriter;

        Ptr<MgByteReader> reader = agfWriter.Write(geom);
        Ptr<MgByteSink> sink = new MgByteSink(reader);
        Ptr<MgByte> bytes = sink->ToBuffer();

        FdoByte* gisBytes = bytes->Bytes();
        INT32 len = bytes->GetLength();

        FdoPtr<FdoByteArray> byteArray = FdoByteArray::Create(gisBytes, (FdoInt32)len);

        #ifdef _DEBUG
        // Get the spatial filter geometry text
        FdoPtr<FdoFgfGeometryFactory> geometryFactory = FdoFgfGeometryFactory::GetInstance();
        if(geometryFactory)
        {
            FdoPtr<FdoIGeometry> geometry = geometryFactory->CreateGeometryFromFgf(byteArray);
            STRING geomText = geometry->GetText();
            ACE_DEBUG((LM_ERROR, ACE_TEXT("SPATIAL FILTER:\n%W\n\n"), geomText.c_str()));
        }
        #endif

        FdoPtr<FdoGeometryValue> geomValue = FdoGeometryValue::Create(byteArray);
        if (geomValue != NULL)
        {
            FdoSpatialOperations fdoSpatialOp = GetFdoSpatialOperation(spatialOp);
            spatialFilter = FdoSpatialCondition::Create((FdoString*)geomProp.c_str(), fdoSpatialOp, (FdoExpression*)geomValue);
        }
    }

    // Determine which one to apply
    if ((regularFilter != NULL) && (spatialFilter != NULL))
    {
        // Both filter exists, combine them
        combineFilter = FdoFilter::Combine(regularFilter, bOper, spatialFilter);
    }
    else
    {
        if (regularFilter != NULL)
        {
            // Only regular filter exists
            combineFilter = FDO_SAFE_ADDREF((FdoFilter*)regularFilter);
        }
        else if (spatialFilter != NULL)
        {
            // Only spatial filter exists
            combineFilter = FDO_SAFE_ADDREF((FdoFilter*)spatialFilter);
        }
    }

    // Apply the filter
    if (combineFilter != NULL)
    {
        m_cmd_select->SetFilter(combineFilter);
    }
}

// Ordering options
void c_FdoSelectFeatures::ApplyOrderingOptions()
{
    CHECKNULL(m_options, L"c_FdoSelectFeatures.ApplyOrderingOptions");
    CHECKNULL(m_cmd_select, L"c_FdoSelectFeatures.ApplyOrderingOptions");

    Ptr<MgStringCollection> properties = m_options->GetOrderingProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    // Ordering options are supplied but provider does not support it
    
    FdoPtr<FdoICommandCapabilities> fcc = m_FdoConn->GetCommandCapabilities();
      
    if ( !fcc || (!fcc->SupportsSelectOrdering() && !m_is_cmd_ext_select) )
    {
      std::string errstr = "Ordering options are supplied but provider does not support it or doesn't support extended select!";
      throw c_ExceptionGeoREST(errstr);
    }

    if( !m_is_cmd_ext_select )
    {
    
      FdoPtr<FdoIdentifierCollection> fic = m_cmd_select->GetOrdering();
      CHECKNULL((FdoIdentifierCollection*)fic, L"c_FdoSelectFeatures.ApplyOrderingOptions");

      // Order option Asc or Desc (default is Asc)
      FdoOrderingOption option = GetFdoOrderingOption(m_options->GetOrderOption());
      m_cmd_select->SetOrderingOption(option);

      for (INT32 i=0; i < cnt; i++)
      {
          STRING propertyName = properties->GetItem(i);

          if (!propertyName.empty())
          {
              FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
              
              fic->Add(fdoIden);
          }
      }
    }
    else
    {
      FdoIExtendedSelect* extselect = (FdoIExtendedSelect*)m_cmd_select.p;
      
      FdoOrderingOption option = GetFdoOrderingOption(m_options->GetOrderOption());
      Ptr<MgStringCollection> properties = m_options->GetOrderingProperties();
      
      FdoPtr<FdoIdentifierCollection> fic = m_cmd_select->GetOrdering();
      
      INT32 cnt = properties->GetCount();
      for (INT32 i=0; i < cnt; i++)
      {
        STRING propertyName = properties->GetItem(i);
        FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
        fic->Add(fdoIden);
        
        extselect->SetOrderingOption(propertyName.c_str(),option);
          
      }
    }
}


bool c_FdoSelectFeatures::IsFdoSupportedFunction( FdoFunction* fdoFunc)
{
  
  FdoPtr<FdoIExpressionCapabilities> fec = m_FdoConn->GetExpressionCapabilities();
  CHECKNULL((FdoIExpressionCapabilities*)fec, L"MgServerSelectFeatures.SupportsFunction");

  bool supports = false;

  FdoPtr<FdoFunctionDefinitionCollection> ffdc = fec->GetFunctions();
  if (NULL != (FdoFunctionDefinitionCollection*)ffdc)
  {
    FdoInt32 funcCnt = ffdc->GetCount();
    for (FdoInt32 i=0; i < funcCnt; i++)
    {
      FdoPtr<FdoFunctionDefinition> ffd = ffdc->GetItem(i);
      CHECKNULL((FdoFunctionDefinition*)ffd, L"MgServerSelectFeatures.SupportsFunction");

      // TODO: Just comparing name is enough?
      // TODO: I think, NOT, because there can be overloaded functions like one function
      // with multiple arguments, differnet datatypes etc.
      //
      // Comparing argument count is not sufficient because, there can be optional arguments
      // as well. Therefore, we should just restrict to name comparision only
      FdoString* funcNameAllowed = ffd->GetName();
      FdoString* funcNameSupplied = fdoFunc->GetName();
      size_t cmp = _wcsicmp(funcNameAllowed, funcNameSupplied);
      if (cmp == 0)
      {
        supports = true;
        break;
      }
    }
  }
  return supports;
}

bool c_FdoSelectFeatures::ContainsUdf(FdoExpression* expression)
{
    bool isUdf = false;
    bool fdoSupported = false;

    // Downcast to FdoFunction
    FdoFunction* function = dynamic_cast<FdoFunction*>(expression);

    // If we are unable to downcast, it means it is not a function, it is just
    // an expression. We do not do anything with this. We just pass it to FDO
    if (function != NULL)
    {
        if (m_cmd_select != NULL)
        {
            // Check if FDO supports this function, if so, let FDO handle it
            fdoSupported = IsFdoSupportedFunction(function);
        }

        if (!fdoSupported)
        {
            // If function is not supported, then check if it is a custom function.
            isUdf = IsCustomFunction(function);
        }
    }

    return isUdf;
}


bool c_FdoSelectFeatures::IsCustomFunction(FdoFunction* fdoFunc)
{
    bool customFunc = false;

    FdoString* funcNameAllowed = fdoFunc->GetName();
    if (funcNameAllowed != NULL)
    {
        INT32 funcIndex = -1;
        customFunc = FindCustomFunction(STRING(funcNameAllowed),funcIndex);
    }

    return customFunc;
}

void c_FdoSelectFeatures::AddFdoComputedProperty(CREFSTRING aliasName, FdoExpression* expression)
{
    FdoPtr<FdoIdentifierCollection> fic = m_cmd_select->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"c_FdoSelectFeatures.AddFdoComputedProperty");

    FdoString* expName = aliasName.c_str();
    if (expName != NULL)
    {
        FdoPtr<FdoComputedIdentifier> fdoIden = FdoComputedIdentifier::Create(expName, expression);
        CHECKNULL((FdoComputedIdentifier*)fdoIden, L"c_FdoSelectFeatures.AddFdoComputedProperty");

        fic->Add(fdoIden);
    }
}

void c_FdoSelectFeatures::AddCustomComputedProperty(CREFSTRING aliasName, FdoExpression* expression)
{
    CHECKNULL((FdoExpression*)expression, L"c_FdoSelectFeatures.AddCustomComputedProperty");

    FdoPtr<FdoIdentifierCollection> fic = m_cmd_select->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"c_FdoSelectFeatures.AddCustomComputedProperty");

    // If property is already found, two custom properties are not supported and therefore throw exception
    if (m_customPropertyFound)
    {
        STRING message = L"If property is already found, two custom properties are not supported"; // MgServerFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"c_FdoSelectFeatures.AddCustomComputedProperty", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Downcast to FdoFunction
    FdoFunction* function = dynamic_cast<FdoFunction*>(expression);

    if (function != NULL)
    {
        FdoString* expName = aliasName.c_str();
        if (expName != NULL)
        {
            FdoPtr<FdoExpressionCollection> exprCol = function->GetArguments();
            FdoInt32 cnt = exprCol->GetCount();
            FdoPtr<FdoExpression> expr;
            if (cnt > 0)
            {
                expr = exprCol->GetItem(0);   // Property Name
            }

            // Just pass in the property name
            // FdoPtr<FdoComputedIdentifier> fdoIden = FdoComputedIdentifier::Create(expName, expr);

            // NOTE: Each provider has its own rule for supporting computed properties, select and select aggregate
            // functionality. Therefore we just work with simple select command, fetch the property and do the needed
            // calculations. Therefore, we are not adding them as computed properties.

            FdoIdentifier* propName = dynamic_cast<FdoIdentifier*>(expr.p);

            if (propName != NULL)
                fic->Add(propName);

            m_customPropertyName = aliasName;
            m_customPropertyFound = true;
            m_customFunction = FDO_SAFE_ADDREF(function);
        }
    }
}

void c_FdoSelectFeatures::ValidateConstraintsOnCustomFunctions()
{
    // Custom function should only be allowed stand alone. This means, no other property along withit
    // will be supported. Therefore
    // 1. ClassProperties should be zero.
    // 2. Computed properties (custom) should not be more than one
    // 3. No GroupBy clause allowed
    if (m_options != NULL)
    {
        Ptr<MgStringCollection> strCol = m_options->GetClassProperties();
        INT32 classPropCnt = strCol->GetCount();

        if (m_customPropertyFound)
        {
            if (classPropCnt > 0)
            {
                STRING message = L"Only One Property Allowed"; // MgServerFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

                MgStringCollection arguments;
                arguments.Add(message);
                throw new MgFeatureServiceException(L"c_FdoSelectFeatures.ValidateConstraintsOnCustomFunctions",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
            ValidateCustomConstraints(m_customFunction);
        }
    }
}


void c_FdoSelectFeatures::CreateCommand(const c_CfgDataSource_FDO* FdoSource, bool isSelectAggregate, bool isordering)
{
  FdoPtr<IConnectionManager> manager = FdoFeatureAccessManager::GetConnectionManager ();

  m_FdoConn = manager->CreateConnection (FdoSource->GetProvider().c_str());

  if( !m_FdoConn.p )
  {
    std::string errmsg,u8;
    errmsg = "Unable to Create FDO Connection! Provider='";
    Poco::UnicodeConverter::toUTF8(FdoSource->GetProvider(),u8);
    errmsg = errmsg + u8 + "'.";
    throw c_ExceptionGeoREST(errmsg);   
  }

  m_FdoConn->SetConnectionString(FdoSource->GetConnString().c_str());
  if( m_FdoConn->Open() != FdoConnectionState_Open )
  {
    std::string errmsg,u8;
    errmsg = "Unable to Open FDO Connection! Provider='";
    Poco::UnicodeConverter::toUTF8(FdoSource->GetProvider(),u8);
    errmsg.append(u8);
    errmsg.append("' ConnectionString='");
    Poco::UnicodeConverter::toUTF8(FdoSource->GetConnString(),u8);
    errmsg.append(u8);
    errmsg.append("'.");
     
    throw c_ExceptionGeoREST(errmsg);   
  }

  FdoPtr<FdoICommandCapabilities> comm_cpb = m_FdoConn->GetCommandCapabilities();
  bool is_fdosqlite=false;;
  if( FdoSource->GetProvider().substr(0,12).compare(L"OSGeo.SQLite") == 0 )
  {
    is_fdosqlite = true;
  }
  if( !isordering || (comm_cpb->SupportsSelectOrdering() && !is_fdosqlite) )
  {
    m_cmd_select = (FdoISelect*)m_FdoConn->CreateCommand(FdoCommandType_Select);
  }
  else
  {
    try
    {
      m_cmd_select = (FdoISelect*)m_FdoConn->CreateCommand(FdoCommandType_ExtendedSelect);
      if( m_cmd_select ) m_is_cmd_ext_select = true;
    }
    catch (...)
    {
    	
    }
    
  }

  

  if( !m_cmd_select ) 
  { 
    std::string errmsg,u8;
    errmsg = "Unable to Create FDO Select command! Provider='";
    Poco::UnicodeConverter::toUTF8(FdoSource->GetProvider(),u8);
    errmsg = errmsg + u8 + "'.";
    throw c_ExceptionGeoREST(errmsg);
  }
      
}

void c_FdoSelectFeatures::ValidateParam(const c_CfgDataSource_FDO* FdoSource, CREFSTRING className)
{
    // className and resource identifier can not be NULL
    if (FdoSource == NULL)
    {
        throw new MgNullArgumentException(L"c_FdoSelectFeatures::ValidateParam()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (className.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"c_FdoSelectFeatures::ValidateParam()",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
}



// Check whether user is requesting custom operations
bool c_FdoSelectFeatures::ContainsCustomFunction(MgFeatureQueryOptions* options)
{
    bool hasCustomFunction = false;

    if (options == NULL)
        return false;

    Ptr<MgStringPropertyCollection> properties = options->GetComputedProperties();

    if (properties == NULL)
        return false;   // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return false;   // Nothing to do

    for (INT32 i=0; i < cnt; i++)
    {
        STRING aliasName = properties->GetName(i);
        STRING expression = properties->GetValue(i);

        FdoString* str = (FdoString*)expression.c_str();
        if (str != NULL)
        {
            FdoPtr<FdoExpression> expression = FdoExpression::Parse(str);
            CHECKNULL((FdoExpression*)expression, L"c_FdoSelectFeatures.ContainsCustomFunction");

            hasCustomFunction = ContainsUdf(expression);
        }
    }

    if (hasCustomFunction && (cnt != 1))
    {
        // Only one custom function with no property is allowed
        STRING message = L"Only one custom function with no property is allowed"; // MgServerFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"c_FdoSelectFeatures.ContainsCustomFunction",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return hasCustomFunction;
}



