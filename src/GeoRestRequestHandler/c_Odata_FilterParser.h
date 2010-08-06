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

#ifndef _c_Odata_FilterParser_h
#define _c_Odata_FilterParser_h



class REST_REQUEST_HANDLER_API c_Odata_FilterParser
{
  enum e_Token
  {
    e_Invalid,
    e_End, // end of string
    e_Integer,
    e_Integer64,  // 25L
    e_Single, // with '.' 2.3f
    e_Double, // with '.' 2.3
    e_DateTime, // with '.' 2.3
    e_String,
    e_Identifier,
    e_ComplexIdentifier, // includes / in name like: People/Name
    e_FunctionName, // identifier with ( after it
    e_IdentifierOfLiteralValue, // special literal value ( identifier with string after it) datetime'...' X'...' guid'...' time'...'
    e_Comma,
    e_OpenParenthesis,
    e_CloseParenthesis,
  };
  
  enum e_ExpressionType
  {
    e_ExpUnknown, // not set
    e_ExpInvalid, // not an expression
    e_ExpInteger,
    e_ExpInteger64,  // 25L
    e_ExpSingle, // with '.' 2.3f
    e_ExpDouble, // with '.' 2.3
    e_ExpDateTime, // with '.' 2.3
    e_ExpGuid,
    e_ExpBinary,
    e_ExpString,
    e_ExpIdentifier,
    e_ExpFunction,
    e_ExpCombined, // when inside ( ... )
  };
public:
  static void ODataFilter2FdoFilter( const std::wstring& ODDataFilter,std::wstring& FdoFilter );
  
protected:
  enum e_LikeFunctionType
  {
    e_SubStringOf,
    e_StartsWith,
    e_EndsWith,
  };
  enum e_ExtractFromDateTime
  { 
    e_Year,
    e_Month,
    e_Day,
    e_Hour,
    e_Minute,
    e_Second,    
  };
  //e_Token ODataFilter_PraseToken( const std::wstring::const_iterator& Start,const std::wstring::const_iterator& End,std::wstring::const_iterator& Curr );
  static e_Token ParseToken( const wchar_t* Start, const wchar_t*& Curr );
  static bool IsIdentifier(const wchar_t* Start,const wchar_t* End,const wchar_t* CompareString);
  static void ProcessDateTime(const wchar_t*& Start,const wchar_t*& End,std::wstring& FdoFilter);
  static void ProcessTime(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter);
  static void ProcessFunction(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter);
  static bool TryProcessKeyword(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter);
  static c_Odata_FilterParser::e_Token ProcessExpression( const wchar_t*& start, const wchar_t*& curr, std::wstring& FdoFilter,e_ExpressionType& ExpType );
  static void CopyIdentifier(const wchar_t* Start,const wchar_t* End,std::wstring& ToString);
  static void ProcessForLike(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter,e_LikeFunctionType LikeType);
  static void ProcessFuncExtractFromDateTime(e_ExtractFromDateTime ExtractWhat,const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter);
  static void ProcessFuncGeneral(const std::wstring& FdoFuncName,const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter);
};

#endif