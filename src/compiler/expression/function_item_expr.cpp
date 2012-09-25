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
#include "stdafx.h"

#include "compiler/expression/function_item_expr.h"

#include "compiler/expression/var_expr.h"

#include "compiler/expression/expr_visitor.h"

#include "functions/function.h"
#include "functions/udf.h"
#include "functions/signature.h"

namespace zorba {


DEF_EXPR_ACCEPT (dynamic_function_invocation_expr)


dynamic_function_invocation_expr::dynamic_function_invocation_expr(
    static_context* sctx,
    const QueryLoc& loc,
    const expr_t& anExpr,
    const std::vector<expr_t>& args)
  :
  expr(sctx, loc, dynamic_function_invocation_expr_kind),
  theExpr(anExpr),
  theArgs(args)
{
  assert(anExpr != 0);
  compute_scripting_kind();
}


void dynamic_function_invocation_expr::compute_scripting_kind()
{
  // TODO ????
  theScriptingKind = SIMPLE_EXPR;
#if 0
  theScriptingKind = (APPLYING_EXPR | SEQUENTIAL_FUNC_EXPR)
#endif
}


expr_t dynamic_function_invocation_expr::clone(substitution_t& s) const
{
  checked_vector<expr_t> lNewArgs;
  for (checked_vector<expr_t>::const_iterator lIter = theArgs.begin();
       lIter != theArgs.end();
       ++lIter)
  {
    lNewArgs.push_back((*lIter)->clone(s));
  }

  return new dynamic_function_invocation_expr(theSctx,
                                              get_loc(),
                                              theExpr->clone(s),
                                              lNewArgs);
}


/*******************************************************************************

********************************************************************************/

DEF_EXPR_ACCEPT (function_item_expr)


function_item_expr::function_item_expr(
    static_context* sctx,
    const QueryLoc& loc,
    const store::Item* aQName,
    function* f,
    uint32_t aArity)
	:
  expr(sctx, loc, function_item_expr_kind),
  theQName(const_cast<store::Item*>(aQName)),
  theFunction(f),
  theArity(aArity)
{
  assert(f != NULL);
  compute_scripting_kind();
}


function_item_expr::function_item_expr(
    static_context* sctx,
    const QueryLoc& loc)
	:
  expr(sctx, loc, function_item_expr_kind),
  theQName(0),
  theFunction(NULL),
  theArity(0)
{
  theScriptingKind = SIMPLE_EXPR;
}


function_item_expr::~function_item_expr()
{
}


void function_item_expr::add_variable(expr* var, const store::Item_t& name)
{
  theScopedVarsValues.push_back(var);
  theScopedVarsNames.push_back(name);
}


const std::vector<expr_t>& function_item_expr::get_scoped_vars_values() const
{
  return theScopedVarsValues;
}


const std::vector<store::Item_t>& function_item_expr::get_scoped_vars_names() const
{
  return theScopedVarsNames;
}


/*
bool function_item_expr::replace_variable(var_expr_t replacement)
{
  bool res = false;

  for (csize i = 0; i<theScopedVariables.size(); i++)
  {
    var_expr* scopedVar = dynamic_cast<var_expr*>(theScopedVariables[i].getp());
    if (scopedVar != NULL && replacement->get_name()->equals(scopedVar->get_name()))
    {
      theScopedVariables[i] = replacement;
      res = true;
    }
  }
  return res;
}
*/


void function_item_expr::set_function(user_function_t& udf)
{
  theFunction = udf;
  theArity = udf->getArity();
  // compute_scripting_kind();
}


void function_item_expr::compute_scripting_kind()
{
  // ???? TODO
  theScriptingKind = SIMPLE_EXPR;
}


expr_t function_item_expr::clone(substitution_t& s) const
{
  std::auto_ptr<function_item_expr> lNewExpr(
      new function_item_expr(theSctx,
                             get_loc(),
                             theFunction->getName(),
                             theFunction.getp(),
                             theArity)
  );

  std::vector<expr_t> lNewVariables;
  std::vector<expr_t>::const_iterator valIter = theScopedVarsValues.begin();
  std::vector<store::Item_t>::const_iterator nameIter = theScopedVarsNames.begin();
  for ( ; valIter != theScopedVarsValues.end(); ++valIter, ++nameIter)
  {
    lNewExpr->add_variable((*valIter)->clone(s), (*nameIter));
  }

  return lNewExpr.release();
}


}//end of namespace
/* vim:set et sw=2 ts=2: */
