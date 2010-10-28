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

#include "c_EsriGS_Interface.h"


#include "c_EsriGS_Impl.h"


c_EsriGS_Impl* g_EsriGSImpl = NULL;


c_EsriGS_Interface* c_EsriGS_Interface::Impl()
{
  if( g_EsriGSImpl ) return g_EsriGSImpl;

  g_EsriGSImpl = new c_EsriGS_Impl();
  return g_EsriGSImpl;
}