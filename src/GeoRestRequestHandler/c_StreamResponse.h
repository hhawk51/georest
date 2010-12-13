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

#ifndef _c_StreamResponse_h
#define _c_StreamResponse_h

#include "Poco/Net/MessageHeader.h"
#include "c_RestResult.h"




class REST_REQUEST_HANDLER_API c_StreamResponse : public MgDisposable
{
public:
  c_StreamResponse()
  {
    
  }
  
public:
  virtual ~c_StreamResponse() {};

protected:
  /// <summary>
  /// This method is responsible for releasing all
  /// its member variables and then destruct itself.
  /// </summary>
  /// <returns>Returns nothing</returns>
  virtual void Dispose() { delete this; } ;
  
public:
  
  virtual void StreamOut(std::ostream* OutStream) = 0;
 
};


#endif
