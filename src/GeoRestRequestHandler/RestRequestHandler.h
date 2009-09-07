#ifndef _RestConfigInclude_h
#define _RestConfigInclude_h



enum e_REST_AccessCodes
{
  e_Unknown, // 
  e_AccessOK,
  e_NoAccess_Format,   // format (html,xml,json,..) is not allowed
  e_NoAccess_FormatMethod, // format is allowed but not the http method on that format
  e_NoAccess_FormatMethodUser, // format and method are allowed but not for that user
};


#include "c_RestConfig.h"

#endif