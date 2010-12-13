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

#ifndef _c_RestDataStreamOut_h
#define _c_RestDataStreamOut_h


//
// c_RestDataStreamOut 
//
//
class REST_FETCHURI_API c_RestDataStreamOut : public MgDisposable
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

  c_RestDataStreamOut(c_RestDataReader* DataReader,);

private:
    static const INT32 m_cls_id = 0; // PlatformBase_FeatureService_DataReader;

  
public:
  void HeaderOut();
  void ContentOut(std::ostream* OutStream);
  
protected:
  
  
 
  
};


#endif
