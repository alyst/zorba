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
#pragma once
#ifndef ZORBA_XQUERY_DRIVER_H
#define ZORBA_XQUERY_DRIVER_H

#include <ostream>
#include <string>
#include <zorba/config.h>
#include "compiler/parser/symbol_table.h"
#include "compiler/api/compilercb.h"

// needed because we have to delete the main module node
#include "compiler/parsetree/parsenode_base.h"

namespace zorba {

class location;
class parsenode;
class CompilerCB;
class ZorbaParserError;


// exported for unit testing only
class ZORBA_DLL_PUBLIC xquery_driver
{
protected:
  class xquery_scanner* xquery_lexer;
  class jsoniq_scanner* jsoniq_lexer;
  
public:
  enum GRAMMAR_TYPE { 
    XQUERY_GRAMMAR,
    JSONIQ_GRAMMAR
  };
   
public:
  std::stringstream theDocComment;
  std::string theMainModuleDocComment;
  zstring theFilename;
  std::string theFilename2;
  symbol_table symtab;
  rchandle<parsenode> expr_p;
  CompilerCB* theCompilerCB;
  ZorbaParserError* parserError;
  GRAMMAR_TYPE grammar_type;
      
public:  
  xquery_driver(CompilerCB* aCompilerCB, GRAMMAR_TYPE a_grammar_type, uint32_t initial_heapsize = 1024);

  virtual ~xquery_driver();

  bool parse_stream(std::istream& in, const zstring& aFilename = "");

  bool parse_string(const zstring& input);

  bool parse_file(const zstring& aFilename);

  void set_expr(parsenode* e_p);

  parsenode* get_expr() { return expr_p; }
  
  class xquery_scanner* getXqueryLexer() { return xquery_lexer; }
  class jsoniq_scanner* getJsoniqLexer() { return jsoniq_lexer; }

  QueryLoc createQueryLoc(const location& aLoc) const;
  
  void enableCommonLanguage() { theCompilerCB->theCommonLanguageEnabled = true; }
  bool commonLanguageEnabled() { return theCompilerCB->theCommonLanguageEnabled; }
  
  // This function will add a warning for the given language feature, but only if the common-language option is enabled  
  void addCommonLanguageWarning(const location& loc, const char* warning);

  // Error generators
  ZorbaParserError* unrecognizedCharErr(const char* _error_token, const location& loc);
  ZorbaParserError* unterminatedCommentErr(const location& loc);
  ZorbaParserError* unterminatedElementConstructor(const location& loc);
  ZorbaParserError* noClosingTagForElementConstructor(const location& loc);
  ZorbaParserError* unrecognizedToken(const char* _error_token, const location& loc);
  ZorbaParserError* invalidCharRef(const char* _error_token, const location& loc);
  ZorbaParserError* parserErr(const std::string& _message, const location& loc, Error const &code = err::XPST0003);
  ZorbaParserError* parserErr(const std::string& _message, const QueryLoc& loc, Error const &code = err::XPST0003);
};

}	/* namespace zorba */
#endif /* ZORBA_XQUERY_DRIVER_H */

/*
 * Local variables:
 * mode: c++
 * End:
 */
/* vim:set et sw=2 ts=2: */
