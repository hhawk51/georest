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
#include "c_Odata_FilterParser.h"
#include "c_ExceptionGeoREST.h"
#include "Poco\UnicodeConverter.h"
#include "Poco\DateTimeParser.h"
#include "Poco\DateTimeFormatter.h"
#include "Poco\DateTimeFormat.h"
#include "Poco\Exception.h"


#define D_IS_TOKEN_END ((*Curr==0) || (*Curr==' ') || (*Curr==')') || (*Curr==','))




c_Odata_FilterParser::e_Token c_Odata_FilterParser::ParseToken( const wchar_t* Start, const wchar_t*& Curr )
{
  
  Curr = Start;
  
  while(*Curr==L' ') Curr++;  // skip spaces
  
  if( !*Curr ) return e_End;
  
  if( *Curr == L'(' )
  {
    Curr++;
    return e_OpenParenthesis;
  }
  if( *Curr == L')' )
  {
    Curr++;
    return e_CloseParenthesis;
  }
  if( *Curr == L',' )
  {
    Curr++;
    return e_Comma;
  }
  

// test for number
  
  if( (*Curr == L'-' ) || (*Curr == L'+' ) 
    || isdigit((int)(*Curr)) )
  {
    while( isdigit(*Curr))
    {
      Curr++;
    }
    if( D_IS_TOKEN_END )     
    {
      return e_Integer; 
    }
       
    if( *Curr == L'.')
    {
      Curr++;
      if( !isdigit((int)(*Curr))) return e_Invalid;
      while( isdigit(*Curr)) { Curr++; }
      
      if( *Curr == L'f')
      {
        if( D_IS_TOKEN_END ) return e_Single; // after number has to be ' ' or end 
        return e_Invalid;
      }
      
      if( D_IS_TOKEN_END ) return e_Double; // after number has to be ' ' or end 
      return e_Invalid;
    }
    if( *Curr == L'E')
    {
      Curr++;
      if( *Curr==L'+' || *Curr == L'-' ) 
      {
        Curr++;
      }
      if( !isdigit(*Curr)) return e_Invalid;
      while( isdigit(*Curr)) { Curr++;}
      
      if( D_IS_TOKEN_END ) return e_Double; // after number has to be ' ' or end 
      
      return e_Invalid;
    }
    
    if( *Curr == L'L')
    {
      Curr++;
      if( D_IS_TOKEN_END ) return e_Integer64 ; // after number has to be ' ' or end 
      return e_Invalid;
    }
    
    
    
    return e_Invalid; 
  }
  
  if( *Curr == L'\'')  
  {
    while( *Curr )
    {
      Curr++;
      if( *Curr == L'\'' )
      {
        // check for next one
        Curr++;
        if( *Curr != L'\'' )
        {
          return e_String;
        }
        else
        {
          Curr++;
        }
      }
    }
    
    // it is unclosed string
    return e_Invalid;
  }
  
  // now it can  be identifier or value which is similar to identifier datetime'...' or time'...' or guid'...' or X'...' or binary'...'
  // it can be function 
  if( isalpha(*Curr) || *Curr == '_'  )
  {
    Curr++;
    while( isalnum(*Curr) || *Curr == '_' ) Curr++;
    
    if( *Curr == L'/' ) 
    {
      Curr++;
      if( !( isalnum(*Curr) || *Curr == L'_' ) ) return e_Invalid;
      
      while( isalnum(*Curr) || *Curr == L'_' ) Curr++;
      
      if( D_IS_TOKEN_END ) return e_ComplexIdentifier; // after number has to be ' ' or end 
      
      return e_Invalid;
    }
    
    // now it can be '''
    if( *Curr == L'\'' ) 
    {
      return e_IdentifierOfLiteralValue; 
    }
    
    // now it can be function with '('
    const wchar_t* temp_curr = Curr;
    while(*temp_curr && *temp_curr==' ') temp_curr++; // check for spaces if it is function name
    if( *temp_curr == L'(' ) 
    {
      Curr = temp_curr;
      return e_FunctionName;
    }
    
   
    
    if( D_IS_TOKEN_END ) return e_Identifier; // after number has to be ' ' or end 
    
    return e_Invalid;
  }
  
  // it is invalid character
  // skip it; but calling function should error out
  Curr++;
  
  return e_Invalid;

}//end of c_Odata_FilterParser::ODataFilter_PraseToken

bool c_Odata_FilterParser::IsIdentifier(const wchar_t* Start,const wchar_t* End,const wchar_t* CompareString)
{
  // skip spaces
  while( *Start && *Start == L' ' && (Start != End)) Start++;
  
  while( Start != End && (*CompareString != 0) && (*Start==*CompareString) )
  {
    Start++;CompareString++;
  }
  
  if( (Start == End) && (*CompareString == 0) ) return true; // both come to end
  
  return false;
}


void c_Odata_FilterParser::ProcessDateTime(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter)
{
  start = curr;
  e_Token nexttoken = ParseToken(start,curr);
  if( nexttoken == e_String )  
  {
    std::wstring tmpstr;
    tmpstr.append(start+1,curr-1);
    std::string utf8;
    Poco::UnicodeConverter::toUTF8(tmpstr,utf8);            
    Poco::DateTime dt;
    int tz;
    try
    {            
      Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT,utf8,dt,tz);
    }
    catch(Poco::SyntaxException& )
    {
      std::wstring errstr;
      errstr.append(L"Invalid OData filter! Unable to parse datetime or datetimeoffset value '");
      errstr.append(start,curr);
      errstr.append(L"'.");
      std::string utf8;
      Poco::UnicodeConverter::toUTF8(errstr,utf8);
      throw c_ExceptionODataInvalidFilter(utf8);
    }

    std::string strdate = Poco::DateTimeFormatter::format(dt,Poco::DateTimeFormat::SORTABLE_FORMAT);
    std::wstring utf16;
    Poco::UnicodeConverter::toUTF16(strdate,utf16);

    FdoFilter.append(L" TIMESTAMP '");
    FdoFilter.append(utf16);
    FdoFilter.append(L"'");    
  }
  else
  {
    std::wstring errstr;
    errstr.append(L"Invalid OData filter! Unknown datetime or datetimeoffset value '");
    errstr.append(start,curr);
    errstr.append(L"'.");

    std::string utf8;
    Poco::UnicodeConverter::toUTF8(errstr,utf8);
    throw c_ExceptionODataInvalidFilter(utf8);
  }
}

void c_Odata_FilterParser::ProcessTime(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter)
{
  start = curr;
  e_Token nexttoken = ParseToken(start,curr);
  if( nexttoken == e_String )  
  {    
    FdoFilter.append(L" TIME ");
    FdoFilter.append(start,curr);
    FdoFilter.append(L"'");    
  }
  else
  {
    std::wstring errstr;
    errstr.append(L"Invalid OData filter! Unknown time value '");
    errstr.append(start,curr);
    errstr.append(L"'.");

    std::string utf8;
    Poco::UnicodeConverter::toUTF8(errstr,utf8);
    throw c_ExceptionODataInvalidFilter(utf8);
  }
}//end of  ProccessTime


bool c_Odata_FilterParser::TryProcessKeyword(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter)
{
  if( IsIdentifier(start,curr,L"true") )
  {
    FdoFilter.append(L" true");
    return true;
  }
  
  if( IsIdentifier(start,curr,L"false") )
  {
    FdoFilter.append(L" false");
    return true;
  }
  
  if( IsIdentifier(start,curr,L"eq") )
  {
    FdoFilter.append(L" =");
    return true;
  }
  if( IsIdentifier(start,curr,L"ne") )
  {
    FdoFilter.append(L" <>");
    return true;
  }
  if( IsIdentifier(start,curr,L"gt") )
  {
    FdoFilter.append(L" >");
    return true;
  }
  if( IsIdentifier(start,curr,L"ge") )
  {
    FdoFilter.append(L" >=");
    return true;
  }
  if( IsIdentifier(start,curr,L"lt") )
  {
    FdoFilter.append(L" <");
    return true;
  }
  if( IsIdentifier(start,curr,L"le") )
  {
    FdoFilter.append(L" <=");
    return true;
  }
  if( IsIdentifier(start,curr,L"and") )
  {
    FdoFilter.append(L" and");
    return true;
  }
  if( IsIdentifier(start,curr,L"or") )
  {
    FdoFilter.append(L" or");
    return true;
  }
  if( IsIdentifier(start,curr,L"not") )
  {
    FdoFilter.append(L" not");
    return true;
  }
  
  if( IsIdentifier(start,curr,L"add") )
  {
    FdoFilter.append(L" +");
    return true;
  }
  if( IsIdentifier(start,curr,L"sub") )
  {
    FdoFilter.append(L" -");
    return true;
  }
  if( IsIdentifier(start,curr,L"mul") )
  {
    FdoFilter.append(L" *");
    return true;
  }
  if( IsIdentifier(start,curr,L"div") )
  {
    FdoFilter.append(L" /");
    return true;
  }
  if( IsIdentifier(start,curr,L"mod") )
  {
    //FdoFilter.append(L"%");
    //return true;
    throw c_ExceptionODataInvalidFilter("Unsupported OData Filter operation 'mod'!");
  }
  
  
  return false;
}

void c_Odata_FilterParser::ProcessForLike(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter,e_LikeFunctionType LikeType)
{
  // next has to be e_OpenParenthesis
  start = curr;
  e_Token next_token = ParseToken(start,curr);
  if( next_token != e_OpenParenthesis )
    throw c_ExceptionODataInvalidFilter("Invalid OData Filter! Expected '(' after 'substring'.");

  start = curr;
  std::wstring param1_str;
  e_ExpressionType param1_exptype;
  next_token = ProcessExpression(start,curr,param1_str,param1_exptype);


  // now has to be comma
  if( next_token != e_Comma )
  {
    throw c_ExceptionODataInvalidFilter("Invalid OData Filter! Expected ',' while evaluating 'substring' function.");
  }

  // skip comma and parse next    
  start = curr;
  std::wstring param2_str;param2_str.reserve(64);
  e_ExpressionType param2_exptype;
  next_token = ProcessExpression(start,curr,param2_str,param2_exptype);


  // next has to be close parenthesis

  if( next_token != e_CloseParenthesis )
    throw c_ExceptionODataInvalidFilter("Invalid OData Filter! Expected ')' while evaluating 'substring' function.");

  // skip )
  start = curr;

  // in case of substringof second parameter is first parameter for LIKE
  if( LikeType==e_SubStringOf )
    FdoFilter.append(param2_str);
  else
    FdoFilter.append(param1_str);
    
  FdoFilter.append(L" LIKE ");
  
  bool doconcat=true;
  if( (param1_exptype==e_ExpString && LikeType==e_SubStringOf)
      || 
      (param2_exptype==e_ExpString && LikeType!=e_SubStringOf)
    )
  {
    // add % directly inside string
    // could do with concat but thought that this simple case should be 
    // converted as it would be written for LIKE operator  e.g. name LIKE '%city%'
    
    switch(LikeType)
    {
      case e_EndsWith:
      {
        int len = param2_str.length();
        if( (len>=2) && param2_str[0]=='\'' && param2_str[len-1]=='\'')
        {
          FdoFilter.append(L"'%");
          FdoFilter.append(param2_str,1,len-2); // do not include '
          FdoFilter.append(L"'");
          doconcat=false;
        }
      }
      break;
      case e_StartsWith:
      {
        int len = param2_str.length();
        if( (len>=2) && param2_str[0]=='\'' && param2_str[len-1]=='\'')
        {
          FdoFilter.append(L"'");
          FdoFilter.append(param2_str,1,len-2); // do not include '
          FdoFilter.append(L"%'");
          doconcat=false;
        }
      }
      break;
      case e_SubStringOf:
      {
        int len = param1_str.length();
        if( (len>=2) && param1_str[0]=='\'' && param1_str[len-1]=='\'')
        {
          FdoFilter.append(L"'%");
          FdoFilter.append(param1_str,1,len-2); // do not include '
          FdoFilter.append(L"%'");
          doconcat=false;
        }
      }
      break;
    }
      
    

  }
  if( doconcat )
  {
    // it is more complex string like function or concat e.g. Upper('city')
    // need to add % with concat
    switch(LikeType)
    {
      case e_EndsWith:
        FdoFilter.append(L" Concat('%',");
        FdoFilter.append(param2_str);
        FdoFilter.append(L")");
      break;
      case e_StartsWith:
        FdoFilter.append(L" Concat(");
        FdoFilter.append(param2_str);
        FdoFilter.append(L",'%')");
      break;
      case e_SubStringOf:
        FdoFilter.append(L" Concat(");
        FdoFilter.append(L" Concat('%',");
        FdoFilter.append(param1_str);
        FdoFilter.append(L" ),'%')");
      break;
    }
  }

  start = curr; // go after comma

}

void c_Odata_FilterParser::ProcessFuncGeneral(const std::wstring& FdoFuncName,const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter)
{
  
  // next has to be e_OpenParenthesis
  start = curr;
  e_Token next_token = ParseToken(start,curr);
  if( next_token != e_OpenParenthesis )
    throw c_ExceptionODataInvalidFilter("Invalid OData Filter! Expected '(' after function name.");

  start = curr; // skip (
  
  FdoFilter.append(FdoFuncName);
  FdoFilter.append(L"(");

  e_ExpressionType param1_exptype;
  bool repeat=true;
  while(repeat)
  {
    
    
    next_token = ProcessExpression(start,curr,FdoFilter,param1_exptype);
    
    switch(next_token)
    {
      case e_Comma:
        FdoFilter.append(L",");
        start = curr;
      break;
      case e_CloseParenthesis:
        FdoFilter.append(L")");
        start = curr;
        repeat = false;
      break;
      default:
        throw c_ExceptionODataInvalidFilter("Invalid OData Filter! Expected ')' or ',' while evaluating function.");
      break;
    }    
  }     
}//end of c_Odata_FilterParser::ProcessFuncGeneral

void c_Odata_FilterParser::ProcessFuncExtractFromDateTime(e_ExtractFromDateTime ExtractWhat,const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter)
{

  // next has to be e_OpenParenthesis
  start = curr;
  e_Token next_token = ParseToken(start,curr);
  if( next_token != e_OpenParenthesis )
    throw c_ExceptionODataInvalidFilter("Invalid OData Filter! Expected '(' after function name.");

  start = curr; // skip (

  FdoFilter.append(L"Extract");
  FdoFilter.append(L"(");
  switch(ExtractWhat)
  {
    case e_Year:
      FdoFilter.append(L"YEAR,");
    break;
    case e_Month:
      FdoFilter.append(L"MONTH,");
    break;
    case e_Day:
      FdoFilter.append(L"DAY,");
    break;
    case e_Hour:
      FdoFilter.append(L"HOUR,");
    break;
    case e_Minute:
      FdoFilter.append(L"MINUTE,");
    break;
    case e_Second:
      FdoFilter.append(L"DAY,");
    break;
  }

  e_ExpressionType param1_exptype;
  bool repeat=true;
  while(repeat)
  {
    next_token = ProcessExpression(start,curr,FdoFilter,param1_exptype);

    switch(next_token)
    {
    case e_Comma:
      FdoFilter.append(L",");
      start = curr;
      break;
    case e_CloseParenthesis:
      FdoFilter.append(L")");
      start = curr;
      repeat = false;
      break;
    default:
      throw c_ExceptionODataInvalidFilter("Invalid OData Filter! Expected ')' or ',' while evaluating function.");
      break;
    }    
  }     
}//end of c_Odata_FilterParser::ProcessFuncExtractFromDateTime

void c_Odata_FilterParser::ProcessFunction(const wchar_t*& start,const wchar_t*& curr,std::wstring& FdoFilter)
{
  if( IsIdentifier(start,curr,L"substringof") )
  {
    ProcessForLike(start,curr,FdoFilter,e_SubStringOf);
    return;      
  }
  if( IsIdentifier(start,curr,L"startswith") )
  {
    ProcessForLike(start,curr,FdoFilter,e_StartsWith);
    return;      
  }
  if( IsIdentifier(start,curr,L"endswith") )
  {
    ProcessForLike(start,curr,FdoFilter,e_EndsWith);
    return;      
  }
  if( IsIdentifier(start,curr,L"length") )
  {
    ProcessFuncGeneral(L"Length",start,curr,FdoFilter);    
    return;      
  }
  if( IsIdentifier(start,curr,L"indexOf") )
  {
    ProcessFuncGeneral(L"InStr",start,curr,FdoFilter);
    return;      
  }
  if( IsIdentifier(start,curr,L"replace") )
  {
    //ProcessFuncGeneral("Translate",start,curr,FdoFilter);
    //return;      
    
    // nothing here it will go into exception of unknown fucntion
  }
  if( IsIdentifier(start,curr,L"substring") )
  {
    ProcessFuncGeneral(L"Substr",start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"tolower") )
  {
    ProcessFuncGeneral(L"Lower",start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"toupper") )
  {
    ProcessFuncGeneral(L"Upper",start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"trim") )
  {
    ProcessFuncGeneral(L"Trim",start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"concat") )
  {
    ProcessFuncGeneral(L"Concat",start,curr,FdoFilter);
    return;  
  }
  
  
  // Date Functions
  if( IsIdentifier(start,curr,L"year") )
  {
    ProcessFuncExtractFromDateTime(e_Year,start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"month") )
  {
    ProcessFuncExtractFromDateTime(e_Month,start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"day") )
  {
    ProcessFuncExtractFromDateTime(e_Day,start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"hour") )
  {
    ProcessFuncExtractFromDateTime(e_Hour,start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"minute") )
  {
    ProcessFuncExtractFromDateTime(e_Minute,start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"second") )
  {
    ProcessFuncExtractFromDateTime(e_Second,start,curr,FdoFilter);
    return;  
  }
  
  // Math Functions
  if( IsIdentifier(start,curr,L"round") )
  {
    ProcessFuncGeneral(L"Round",start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"floor") )
  {
    ProcessFuncGeneral(L"Floor",start,curr,FdoFilter);
    return;  
  }
  if( IsIdentifier(start,curr,L"ceiling") )
  {
    ProcessFuncGeneral(L"Ceil",start,curr,FdoFilter);
    return;  
  }
  
  
  // So called type Of Functions - bad very bad  
  if( IsIdentifier(start,curr,L"isof") )
  {
    std::string errstr,u8;
    std::wstring ws;
    errstr = "OData 'isof' function is not supported! Bad Bad Function.";
    throw c_ExceptionODataInvalidFilter(errstr);
  }
  
  std::string errstr,u8;
  std::wstring ws;
  errstr = "Invalid OData Filter! Unknown function '";
  ws.append(start,curr);
  Poco::UnicodeConverter::toUTF8(ws,u8);
  errstr.append(u8);
  errstr.append("'.");
  throw c_ExceptionODataInvalidFilter(errstr);
}



c_Odata_FilterParser::e_Token c_Odata_FilterParser::ProcessExpression( const wchar_t*& start, const wchar_t*& curr
                        , std::wstring& FdoFilter, e_ExpressionType& ExpType )
{
  
  ExpType = e_ExpUnknown;
  e_Token token=e_End;
  bool repeat = true;
  while( repeat )  
  {
    token=ParseToken(start,curr);
    switch( token )
    {
      case e_End:
        return token;
      break;
      case e_Integer:
        ExpType = (ExpType == e_ExpUnknown) ? e_ExpInteger : e_ExpCombined;
        FdoFilter.append(start,curr);
      break;
      case e_Integer64: // it has L on the end
        ExpType = (ExpType == e_ExpUnknown) ? e_ExpInteger64 : e_ExpCombined;
        FdoFilter.append(start,curr-1);
      break;
      case e_OpenParenthesis:
      {
        FdoFilter.push_back(L'(');  
        start = curr; // step over (
        token = ProcessExpression(start,curr,FdoFilter,ExpType);
        start = curr;
        if( token != e_CloseParenthesis ) 
        {
          throw c_ExceptionODataInvalidFilter("Invalid OData filter! Expression is opened but not closed with ).");
        }
        FdoFilter.push_back(L')');
        ExpType = e_ExpCombined;        
      }
      break;
      case e_CloseParenthesis:
      {
        repeat = false; // leave start pointer on )
      }
      break;
      case e_Comma:
      {
        repeat = false;        
        continue; // leave start pointer on ,
      }
      break;
      case e_String:
        ExpType = (ExpType == e_ExpUnknown) ? e_ExpString : e_ExpCombined;
        FdoFilter.append(start,curr);
      break;
      case e_Double:
        ExpType = (ExpType == e_ExpUnknown) ? e_ExpDouble : e_ExpCombined;
        FdoFilter.append(start,curr);
      break;
      case e_Single:
        ExpType = (ExpType == e_ExpUnknown) ? e_ExpSingle : e_ExpCombined;
        FdoFilter.append(start,curr-1); // skip f on the end
      break;
      case e_IdentifierOfLiteralValue: // datetime'...'
      {
        // now check 
        if( IsIdentifier(start,curr,L"datetime") || IsIdentifier(start,curr,L"datetimeoffset") )
        {
          ProcessDateTime(start,curr,FdoFilter);
          ExpType = (ExpType == e_ExpUnknown) ? e_ExpDateTime : e_ExpCombined;
          continue;
        }
        if( IsIdentifier(start,curr,L"time") )
        {
          ProcessTime(start,curr,FdoFilter);
          ExpType = (ExpType == e_ExpUnknown) ? e_ExpDateTime : e_ExpCombined;
          continue;
        }
        if( IsIdentifier(start,curr,L"guid") )
        {
          ExpType = (ExpType == e_ExpUnknown) ? e_ExpGuid : e_ExpCombined;
          throw c_ExceptionODataInvalidFilter("Unable to parse Odata filter! guid'..' data type is not supported in filter.");
        }
        if( IsIdentifier(start,curr,L"binary") )
        {
          ExpType = (ExpType == e_ExpUnknown) ? e_ExpBinary : e_ExpCombined;
          throw c_ExceptionODataInvalidFilter("Unable to parse Odata filter! binary'..' data type is not supported in filter.");  
        }

        std::wstring errstr;
        errstr.append(L"Invalid OData filter! Unknown identifier '");
        errstr.append(start,curr);
        errstr.append(L"'.");

        std::string utf8;
        Poco::UnicodeConverter::toUTF8(errstr,utf8);
        throw c_ExceptionODataInvalidFilter(utf8);
      }
      break;

      case e_Identifier:
      {  
        // it can be some of keywords or 
        if( TryProcessKeyword(start,curr,FdoFilter) == false)
        {
          // if not keyword just add identifier
          FdoFilter.append(start,curr);
          ExpType = (ExpType == e_ExpUnknown) ? e_ExpIdentifier : e_ExpCombined;
        }
        else
        {
          ExpType = e_ExpCombined;
        }
        
      }
      break;
      case e_ComplexIdentifier: // people/name
      {  
        // change / to .
        while(start != curr )
        {
          if( *start == '/')
            FdoFilter.push_back('.')  ;
          else
            FdoFilter.push_back(*start);
          start++;
        }        
        
        ExpType = (ExpType == e_ExpUnknown) ? e_ExpIdentifier : e_ExpCombined;
      }
      break;

      case e_FunctionName:
      {
        ProcessFunction(start,curr,FdoFilter);
        ExpType = (ExpType == e_ExpUnknown) ? e_ExpFunction : e_ExpCombined;
      }  
      break;
     
      

      case e_Invalid: 
      {
        ExpType = e_ExpInvalid;
        token = e_Invalid;
        return e_Invalid;
      }
      break;

    }

    start = curr;
  }

  return token;
}


void c_Odata_FilterParser::ODataFilter2FdoFilter( const std::wstring& ODDataFilter,std::wstring& FdoFilter )
{
  const wchar_t* start = ODDataFilter.c_str();
  const wchar_t* curr=start;
  
  FdoFilter.clear();
  FdoFilter.reserve(ODDataFilter.length());
  
  e_ExpressionType exptype;
  e_Token token = ProcessExpression(start,curr,FdoFilter,exptype);
  if( (token != e_End) || (token == e_Invalid) )
  {
    int count=0;
    const wchar_t* cp = ODDataFilter.c_str();
    while( *cp && cp != start)  { cp++;count++; }

    std::wstring errstr = L"Invalid OData Filter string!";


    //errstr.append(L"Parsing of invalid token started at ");
    //wchar_t buff[15];
    //wsprintf(buff,L"%d",count);
    //errstr.append(buff);
    //errstr.append(L" character.");
    errstr.append(L" Filter string '");
    int cl = ODDataFilter.length();
    if( cl > 1024 ) cl = 1024;
    errstr.append(ODDataFilter.c_str(),cl);
    errstr.append(L"'.");

    std::string utf8;
    Poco::UnicodeConverter::toUTF8(errstr,utf8);
    throw c_ExceptionODataInvalidFilter(utf8);
  }
  
    
}

