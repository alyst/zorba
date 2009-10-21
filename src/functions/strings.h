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
 
// ******************************************
// *                                        *
// * THIS IS A GENERATED FILE. DO NOT EDIT! *
// * SEE .xml FILE WITH SAME NAME           *
// *                                        *
// ******************************************


#ifndef ZORBA_FUNCTIONS_STRINGS_H
#define ZORBA_FUNCTIONS_STRINGS_H


#include "common/shared_types.h"
#include "functions/function_impl.h"


namespace zorba{


void populate_context_strings(static_context* sctx);


//fn:codepoints-to-string
class fn_codepoints_to_string : public function {
public:
  fn_codepoints_to_string ( const signature& sig) : function (sig, FunctionConsts::FN_CODEPOINTS_TO_STRING) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:compare
class fn_compare : public function {
public:
  fn_compare ( const signature& sig) : function (sig, FunctionConsts::FN_COMPARE) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:codepoint-equal
class fn_codepoint_equal : public function {
public:
  fn_codepoint_equal ( const signature& sig) : function (sig, FunctionConsts::FN_CODEPOINT_EQUAL) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:concat
class fn_concat : public function {
public:
  fn_concat ( const signature& sig) : function (sig, FunctionConsts::FN_CONCAT) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:string-join
class fn_string_join : public function {
public:
  fn_string_join ( const signature& sig) : function (sig, FunctionConsts::FN_STRING_JOIN) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:substring
class fn_substring : public function {
public:
  fn_substring ( const signature& sig) : function (sig, FunctionConsts::FN_SUBSTRING) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:string-length
class fn_string_length : public function {
public:
  fn_string_length ( const signature& sig) : function (sig, FunctionConsts::FN_STRING_LENGTH) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:normalize-space
class fn_normalize_space : public function {
public:
  fn_normalize_space ( const signature& sig) : function (sig, FunctionConsts::FN_NORMALIZE_SPACE) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:normalize-unicode
class fn_normalize_unicode : public function {
public:
  fn_normalize_unicode ( const signature& sig) : function (sig, FunctionConsts::FN_NORMALIZE_UNICODE) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:upper-case
class fn_upper_case : public function {
public:
  fn_upper_case ( const signature& sig) : function (sig, FunctionConsts::FN_UPPER_CASE) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:lower-case
class fn_lower_case : public function {
public:
  fn_lower_case ( const signature& sig) : function (sig, FunctionConsts::FN_LOWER_CASE) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:translate
class fn_translate : public function {
public:
  fn_translate ( const signature& sig) : function (sig, FunctionConsts::FN_TRANSLATE) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:encode-for-uri
class fn_encode_for_uri : public function {
public:
  fn_encode_for_uri ( const signature& sig) : function (sig, FunctionConsts::FN_ENCODE_FOR_URI) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:iri-to-uri
class fn_iri_to_uri : public function {
public:
  fn_iri_to_uri ( const signature& sig) : function (sig, FunctionConsts::FN_IRI_TO_URI) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:escape-html-uri
class fn_escape_html_uri : public function {
public:
  fn_escape_html_uri ( const signature& sig) : function (sig, FunctionConsts::FN_ESCAPE_HTML_URI) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:contains
class fn_contains : public function {
public:
  fn_contains ( const signature& sig) : function (sig, FunctionConsts::FN_CONTAINS) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:starts-with
class fn_starts_with : public function {
public:
  fn_starts_with ( const signature& sig) : function (sig, FunctionConsts::FN_STARTS_WITH) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:ends-with
class fn_ends_with : public function {
public:
  fn_ends_with ( const signature& sig) : function (sig, FunctionConsts::FN_ENDS_WITH) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:substring-before
class fn_substring_before : public function {
public:
  fn_substring_before ( const signature& sig) : function (sig, FunctionConsts::FN_SUBSTRING_BEFORE) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:substring-after
class fn_substring_after : public function {
public:
  fn_substring_after ( const signature& sig) : function (sig, FunctionConsts::FN_SUBSTRING_AFTER) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:matches
class fn_matches : public function {
public:
  fn_matches ( const signature& sig) : function (sig, FunctionConsts::FN_MATCHES) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};
//fn:replace
class fn_replace : public function {
public:
  fn_replace ( const signature& sig) : function (sig, FunctionConsts::FN_REPLACE) {}

  PlanIter_t codegen( CompilerCB*,
                      static_context* sctx,
                      const QueryLoc& loc,
                      std::vector<PlanIter_t>& argv,
                      AnnotationHolder& ann) const;
};


} //namespace zorba


#endif 
