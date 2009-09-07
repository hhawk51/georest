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
#include "c_RestUriPathParam.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Initialize string property collection.
/// </summary>
c_RestUriPathParam::c_RestUriPathParam()
{
    m_CurrentParamInd=-1;
}
c_RestUriPathParam::~c_RestUriPathParam()
{
}

int c_RestUriPathParam::GetParametersCount()
{
  return m_reqParamCollection->GetCount();
}

STRING c_RestUriPathParam::GetCurrentParameterName() const
{
  return m_reqParamCollection->GetName(m_CurrentParamInd);
}

STRING c_RestUriPathParam::GetCurrentParameterValue() const
{
  return m_reqParamCollection->GetValue(m_CurrentParamInd);
}

bool c_RestUriPathParam::NextParameter()
{
  m_CurrentParamInd++;
  if( m_CurrentParamInd<GetParametersCount() )
  {
    return true;
  }
  
  return false;
}

bool c_RestUriPathParam::IsEndOfParameters()
{
  if( !GetParametersCount() ) return true;
  
  if( m_CurrentParamInd < GetParametersCount() ) return false;
  
  return true;  
}

bool c_RestUriPathParam::IsLastParameter()
{
  if( !GetParametersCount() ) return false;
  
  if( m_CurrentParamInd == GetParametersCount()-1 ) return true;

  return false;  
}

void c_RestUriPathParam::ResetParameterCurrentIndex()
{
  m_CurrentParamInd=-1;  
}
int c_RestUriPathParam::GetCurrentParameterIndex()
{
  return m_CurrentParamInd;  
}
void c_RestUriPathParam::SetCurrentParameterIndex(int Index)
{
  m_CurrentParamInd = Index;  
}

void c_RestUriPathParam::Dispose()
{
    delete this;
}

INT32 c_RestUriPathParam::GetClassId()
{
    return 0;
}



