/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ZORBA_STORE_XS_TYPES_ENUM
#define ZORBA_STORE_XS_TYPES_ENUM


namespace zorba 
{ 

namespace store
{

/*******************************************************************************
  !!! ATTENTION: The order of thr enum values within SchemaTypeCode is important.
  !!! DO NOT change this order!!!!
********************************************************************************/
enum SchemaTypeCode
{
  XS_ANY_ATOMIC            = 0,

  XS_STRING                = 1,
  XS_NORMALIZED_STRING     = 2,
  XS_TOKEN                 = 3,
  XS_LANGUAGE              = 4,
  XS_NMTOKEN               = 5,
  XS_NAME                  = 6,
  XS_NCNAME                = 7,
  XS_ID                    = 8,
  XS_IDREF                 = 9,
  XS_ENTITY                = 10,

  XS_UNTYPED_ATOMIC        = 11,

  XS_DATETIME              = 12,
  XS_DATE                  = 13,
  XS_TIME                  = 14,
  XS_DURATION              = 15,
  XS_DT_DURATION           = 16,
  XS_YM_DURATION           = 17,

  XS_FLOAT                 = 18,
  XS_DOUBLE                = 19,

  XS_DECIMAL               = 20,
  XS_INTEGER               = 21,
  XS_NON_POSITIVE_INTEGER  = 22,
  XS_NEGATIVE_INTEGER      = 23,
  XS_LONG                  = 24,
  XS_INT                   = 25,
  XS_SHORT                 = 26,
  XS_BYTE                  = 27,
  XS_NON_NEGATIVE_INTEGER  = 28,
  XS_UNSIGNED_LONG         = 29,
  XS_UNSIGNED_INT          = 30,
  XS_UNSIGNED_SHORT        = 31,
  XS_UNSIGNED_BYTE         = 32,
  XS_POSITIVE_INTEGER      = 33,

  XS_GYEAR_MONTH           = 34,
  XS_GYEAR                 = 35,
  XS_GMONTH_DAY            = 36,
  XS_GDAY                  = 37,
  XS_GMONTH                = 38,

  XS_BOOLEAN               = 39,

  XS_BASE64BINARY          = 40,
  XS_HEXBINARY             = 41,

  XS_ANY_URI               = 42,

  XS_QNAME                 = 43,
  XS_NOTATION              = 44,

  XS_LAST
};

}
}
#endif
/* vim:set et sw=2 ts=2: */