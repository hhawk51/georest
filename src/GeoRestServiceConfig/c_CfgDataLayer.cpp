#include "StdAfx.h"
#include "c_CfgDataLayer.h"

c_CfgDataLayer::c_CfgDataLayer(void)
{
}

c_CfgDataLayer::~c_CfgDataLayer(void)
{
  t_CfgHtmlTemplateExtraDataVector::iterator iter;
  for ( iter = m_VectorExtraData.begin( ) ; iter != m_VectorExtraData.end( ) ; iter++ )
  {
    c_CfgHtmlTemplateExtraData * adata = *iter;
    delete adata;
  }
}



c_CfgDataLayerVector::~c_CfgDataLayerVector( void )
{
  t_CfgDataLayerVector::iterator iter;
  for ( iter = m_CfgDataLayerVector.begin( ) ; iter != m_CfgDataLayerVector.end( ) ; iter++ )
  {
    c_CfgDataLayer * datalayer = *iter;
    delete datalayer;
  }
}

const c_CfgDataLayer* c_CfgDataLayerVector::FindUriTag( const std::wstring& UriTag ) 
{
  t_CfgDataLayerVector::iterator iter;
  for ( iter = m_CfgDataLayerVector.begin( ) ; iter != m_CfgDataLayerVector.end( ) ; iter++ )
  {
    if( wcsicmp(UriTag.c_str(),(*iter)->m_UriTag.c_str()) == 0 ) return *iter;
  }
  
  return NULL;
}

const c_CfgDataLayer* c_CfgDataLayerVector::FindClassName( const std::wstring& ClassName ) 
{
  
  std::wstring::size_type iColon = ClassName.find(':');
  if(iColon != std::wstring::npos) 
  {
    // full name
    t_CfgDataLayerVector::iterator iter;
    for ( iter = m_CfgDataLayerVector.begin( ) ; iter != m_CfgDataLayerVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ClassName.c_str()) == 0 ) return *iter;
    }  
  }
  else
  {
    // short name
    t_CfgDataLayerVector::iterator iter;
    for ( iter = m_CfgDataLayerVector.begin( ) ; iter != m_CfgDataLayerVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ShortClassName.c_str()) == 0 ) return *iter;
    }  
  }
  
  
  return NULL;
}

bool c_CfgDataLayerVector::Add( c_CfgDataLayer* DataLayer )
{
  if( FindUriTag(DataLayer->m_UriTag.c_str()) )
  {
    return false;
  }
  
  m_CfgDataLayerVector.push_back(DataLayer);
  return true;
}

