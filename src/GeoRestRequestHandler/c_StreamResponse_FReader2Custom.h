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

#ifndef _c_StreamResponse_FReader2Custom_h
#define _c_StreamResponse_FReader2Custom_h


#include "c_StreamResponse.h"



class REST_REQUEST_HANDLER_API c_StreamResponse_FReader2Custom : public c_StreamResponse
{
public:
  c_StreamResponse_FReader2Custom(c_RestDataReader* Reader,c_CfgRepresentation_Custom* CustomeRep);
  
public:
  virtual ~c_StreamResponse_FReader2Custom();

protected:
 
  
public:
  
  virtual void StreamOut(std::ostream* OutStream);
  
protected:
   Ptr<c_RestDataReader> m_Reader;
   c_CfgRepresentation_Custom * m_CustomRep;
 
};


#endif
