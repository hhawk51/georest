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
c_RestUriPathSegment::c_RestUriPathSegment()
{
    m_CurrentSegmentInd=-1;
}
c_RestUriPathSegment::~c_RestUriPathSegment()
{
}

int c_RestUriPathSegment::GetSegmentsCount()
{
  return m_reqParamCollection->GetCount();
}

STRING c_RestUriPathSegment::GetCurrentSegmentName() const
{
  return m_reqParamCollection->GetName(m_CurrentSegmentInd);
}

STRING c_RestUriPathSegment::GetCurrentSegmentValue() const
{
  return m_reqParamCollection->GetValue(m_CurrentSegmentInd);
}

bool c_RestUriPathSegment::NextSegment()
{
  m_CurrentSegmentInd++;
  if( m_CurrentSegmentInd<GetSegmentsCount() )
  {
    return true;
  }
  
  return false;
}

bool c_RestUriPathSegment::IsNextSegment()
{
  if( !GetSegmentsCount() ) return false;

  if( (m_CurrentSegmentInd+1) < GetSegmentsCount() ) return true;

  return false;  
}

bool c_RestUriPathSegment::IsEndOfSegments()
{
  if( !GetSegmentsCount() ) return true;
  
  if( m_CurrentSegmentInd < GetSegmentsCount() ) return false;
  
  return true;  
}

/*
bool c_RestUriPathParam::IsLastParameter()
{
  if( !GetParametersCount() ) return false;
  
  if( m_CurrentParamInd == GetParametersCount()-1 ) return true;

  return false;  
}
*/

void c_RestUriPathSegment::ResetSegmentCurrentIndex()
{
  m_CurrentSegmentInd=-1;  
}
int c_RestUriPathSegment::GetCurrentSegmentIndex()
{
  return m_CurrentSegmentInd;  
}
void c_RestUriPathSegment::SetCurrentSegmentIndex(int Index)
{
  m_CurrentSegmentInd = Index;  
}

void c_RestUriPathSegment::Dispose()
{
    delete this;
}

INT32 c_RestUriPathSegment::GetClassId()
{
    return 0;
}



