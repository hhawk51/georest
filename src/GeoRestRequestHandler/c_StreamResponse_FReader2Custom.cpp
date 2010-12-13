//
//  Copyright (C) 2010 by  Haris Kurtagic  www.sl-king.com
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
#include "c_StreamResponse_FReader2Custom.h"
#include "c_CfgRepresentation.h"
#include "c_CustomRenderer.h"

c_StreamResponse_FReader2Custom::c_StreamResponse_FReader2Custom( c_RestDataReader* Reader,c_CfgRepresentation_Custom* CustomeRep )
{
  m_Reader = SAFE_ADDREF(Reader);
  m_CustomRep = CustomeRep;
}


void c_StreamResponse_FReader2Custom::StreamOut( std::ostream* OutStream )
{
  (*(m_CustomRep->m_CustomRenderer->m_FuncContent2Stream))(m_Reader.p,OutStream);
  
  
}

c_StreamResponse_FReader2Custom::~c_StreamResponse_FReader2Custom()
{

}

