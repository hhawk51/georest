//
//  Copyright (C) 2009 by Haris Kurtagic
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

#ifndef RestStatusCodes_h
#define RestStatusCodes_h

typedef INT32 STATUS;

#define D_MISSING_HEADER -1
#define D_CANNOT_CONVERT -2
#define D_API_NOT_FOUND  -3

const int HTTP_STATUS_OK                       = 200;

const int HTTP_STATUS_INTERNAL_SERVER_ERROR    = 500;

const int HTTP_STATUS_MG_ERROR                 = 559;

#endif
