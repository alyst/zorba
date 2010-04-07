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
#include <fstream>

#include "zorbaerrors/Assert.h"
#include "zorbaerrors/error_messages.h"

#include "system/globalenv.h"

#include "context/static_context.h"
#include "context/static_context_consts.h"
#include "context/dynamic_context.h"
#include "context/static_context_consts.h"

#include "compiler/xqddf/collection_decl.h"
#include "compiler/xqddf/value_ic.h"
#include "compiler/expression/var_expr.h"

#include "runtime/introspection/sctx.h"

#include "functions/function.h"

#include "store/api/item_factory.h"
#include "store/api/iterator.h"
#include "store/api/collection.h"
#include "store/api/store.h"

#include "types/schema/xercesIncludes.h"
#include "types/schema/schema.h"
#include "types/schema/PrintSchema.h"
#include "types/schema/StrX.h"


namespace zorba {

/*******************************************************************************

********************************************************************************/
bool IsDeclaredCollectionIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  store::Item_t      lName;

  PlanIteratorState* lState;
  DEFAULT_STACK_INIT(PlanIteratorState, lState, aPlanState);

  consumeNext(lName, theChildren[0].getp(), aPlanState);

  if (theSctx->lookup_collection(lName.getp()) == 0)
  {
    STACK_PUSH(GENV_ITEMFACTORY->createBoolean(aResult, false), lState);
  }
  else
  {
    STACK_PUSH(GENV_ITEMFACTORY->createBoolean(aResult, true), lState);
  }
  STACK_END (lState);
}


/*******************************************************************************

********************************************************************************/
DeclaredCollectionsIteratorState::~DeclaredCollectionsIteratorState()
{
  if ( nameItState != NULL )
  {
    nameItState->close();
    nameItState = NULL;
  }
}


void DeclaredCollectionsIteratorState::init(PlanState& planState)
{
  PlanIteratorState::init(planState);
  nameItState = NULL;
}


void DeclaredCollectionsIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  if ( nameItState != NULL )
  {
    nameItState->close();
    nameItState = NULL;
  }
}


bool DeclaredCollectionsIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  store::Item_t lName;

  DeclaredCollectionsIteratorState* lState;
  DEFAULT_STACK_INIT(DeclaredCollectionsIteratorState, lState, aPlanState);

  for ((lState->nameItState = theSctx->collection_names())->open();
       lState->nameItState->next(lName); )
  {
    aResult = lName;
    STACK_PUSH(true, lState);
  }

  lState->nameItState->close();

  STACK_END(lState);
}


/*******************************************************************************

********************************************************************************/
bool IsDeclaredIndexIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  store::Item_t      lName;

  PlanIteratorState* lState;
  DEFAULT_STACK_INIT(PlanIteratorState, lState, aPlanState);

  consumeNext(lName, theChildren[0].getp(), aPlanState);

  if (theSctx->lookup_index(lName.getp()) == 0)
  {
    STACK_PUSH(GENV_ITEMFACTORY->createBoolean(aResult, false), lState);
  }
  else
  {
    STACK_PUSH(GENV_ITEMFACTORY->createBoolean(aResult, true), lState);
  }

  STACK_END(lState);
}


/*******************************************************************************

********************************************************************************/
DeclaredIndexesIteratorState::~DeclaredIndexesIteratorState()
{
  if (nameItState != NULL)
  {
    nameItState->close();
    nameItState = NULL;
  }
}


void DeclaredIndexesIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  if (nameItState != NULL)
  {
    nameItState->close();
    nameItState = NULL;
  }
}


bool DeclaredIndexesIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  store::Item_t lName;

  DeclaredIndexesIteratorState* lState;
  DEFAULT_STACK_INIT(DeclaredIndexesIteratorState, lState, aPlanState);

  for ((lState->nameItState = theSctx->index_names())->open();
       lState->nameItState->next(lName); )
  {
    aResult = lName;
    STACK_PUSH(true, lState);
  }

  lState->nameItState->close();

  STACK_END(lState);
}


/*******************************************************************************

********************************************************************************/
StaticNamespacesIteratorState::~StaticNamespacesIteratorState()
{
}


void StaticNamespacesIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  thePosition = 0;
  theBindings.clear();
}


bool StaticNamespacesIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  StaticNamespacesIteratorState* lState;
  xqpStringStore_t prefix;

  DEFAULT_STACK_INIT(StaticNamespacesIteratorState, lState, aPlanState);

  theSctx->get_namespace_bindings(lState->theBindings);
  lState->thePosition = 0;

  while (lState->thePosition < lState->theBindings.size())
  {
    prefix = lState->theBindings.at(lState->thePosition).first;
    STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, prefix), lState);
    ++lState->thePosition;
  }

  STACK_END (lState);
}


/*******************************************************************************

********************************************************************************/
bool StaticNamespaceBindingIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  store::Item_t lName;
  xqpStringStore_t ns;

  PlanIteratorState* state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  consumeNext(lName, theChildren[0].getp(), aPlanState);

  if (theSctx->lookup_ns(ns, lName->getStringValue(), loc))
    STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, ns), state);

  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
InscopeVariablesIteratorState::~InscopeVariablesIteratorState()
{
}


void InscopeVariablesIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  theVariables.clear();
  thePosition = 0;
}


bool InscopeVariablesIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  InscopeVariablesIteratorState* state;
  store::Item_t temp;
  DEFAULT_STACK_INIT(InscopeVariablesIteratorState, state, aPlanState);

  theSctx->getVariables(state->theVariables);
  state->thePosition = 0;

  while (state->thePosition < state->theVariables.size())
  {
    // STACK_PUSH(state->theVariables[state->thePosition]->get_name(), state); // TODO: passing a QName directly will result in a segmentation fault
    temp = state->theVariables[state->thePosition]->get_name();
    STACK_PUSH(GENV_ITEMFACTORY->createQName(aResult, temp->getNamespace()->c_str(), temp->getPrefix()->c_str(), temp->getLocalName()->c_str()), state);
    ++state->thePosition;
  }

  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
bool DefaultCollectionTypeIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  store::Item_t lName;
  xqpStringStore_t typeString;

  PlanIteratorState* state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  typeString = new xqpStringStore(theSctx->get_default_w3c_collection_type()->toSchemaString());

  STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, typeString), state);
  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
bool XPath10CompatModeIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  PlanIteratorState* state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  STACK_PUSH(GENV_ITEMFACTORY->createBoolean(aResult,
                                             theSctx->xpath_compatibility() == StaticContextConsts::xpath1_0_only), state);

  STACK_END (state);
}


/*******************************************************************************

********************************************************************************/
bool StaticallyKnownDocumentsIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  PlanIteratorState* state;
  store::Item_t lName;
  xqpString temp_str;
  xqtref_t type;

  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  consumeNext(lName, theChildren[0].getp(), aPlanState);
  type = theSctx->lookup_document(lName->getStringValue());
  if (type.getp() == NULL)
  {
    temp_str = "document-node()?"; // as per http://www.w3.org/TR/xquery/#dt-known-docs
    STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, temp_str.theStrStore), state);
  }
  else
  {
    // STACK_PUSH(type->get_qname(), state); // TODO: passing a QName directly will result in a segmentation fault
    STACK_PUSH(GENV_ITEMFACTORY->createQName(aResult, type->get_qname()->getNamespace()->c_str(),
               type->get_qname()->getPrefix()->c_str(), type->get_qname()->getLocalName()->c_str()), state);

    /*
    ns = qname->getNamespace();
    prefix = qname->getPrefix();

    // find the prefix of the given namespace
    if (prefix.length() == 0)
    {
      string_keys.reset(theSctx->get_all_str_keys());
      for (unsigned int i=0; i<string_keys->size(); i++)
      {
        theSctx->lookup_ns(string_keys->at(i).substr(3), temp_ns);
        if (temp_ns == ns)
        {
          prefix = string_keys->at(i).substr(3);
          break;
        }
      }
    }
    */

    // STACK_PUSH( GENV_ITEMFACTORY->createQName(aResult, ns.theStrStore, prefix.theStrStore, qname->getLocalName()), state);
  }

  STACK_END (state);
}


/*******************************************************************************

********************************************************************************/
StaticallyKnownCollationsIteratorState::~StaticallyKnownCollationsIteratorState()
{
}

void StaticallyKnownCollationsIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  thePosition = 0;
  theCollations.clear();
}


bool StaticallyKnownCollationsIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  xqpStringStore_t temp;

  StaticallyKnownCollationsIteratorState* state;
  DEFAULT_STACK_INIT(StaticallyKnownCollationsIteratorState, state, aPlanState);

  theSctx->get_collations(state->theCollations);
  state->thePosition = 0;

  while (state->thePosition < state->theCollations.size())
  {
    temp = new xqpStringStore(state->theCollations.at(state->thePosition));
    STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, temp), state);
    ++state->thePosition;
  }

  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
bool ConstructionModeIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  xqpString temp;

  PlanIteratorState* state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  if (theSctx->construction_mode() == StaticContextConsts::cons_preserve)
    temp = "preserve";
  else
    temp = "strip";

  STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, temp.theStrStore), state);
  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
bool OrderingModeIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  PlanIteratorState* state;
  xqpString temp;
  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  if (theSctx->ordering_mode() == StaticContextConsts::ordered)
    temp = "ordered";
  else
    temp = "unordered";

  STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, temp.theStrStore), state);
  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
bool DefaultOrderIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  PlanIteratorState* state;
  xqpString temp;
  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  if (theSctx->empty_order_mode() == StaticContextConsts::empty_greatest)
    temp = "greatest";
  else
    temp = "least";

  STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, temp.theStrStore), state);
  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
bool BoundarySpacePolicyIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  xqpString temp;

  PlanIteratorState* state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  if (theSctx->boundary_space_mode() == StaticContextConsts::preserve_space)
    temp = "preserve";
  else
    temp = "strip";

  STACK_PUSH( GENV_ITEMFACTORY->createString(aResult, temp.theStrStore), state);
  STACK_END (state);
}


/*******************************************************************************

********************************************************************************/
bool CopyNamespacesModeIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  PlanIteratorState* state;
  xqpString inherit, preserve;

  if (theSctx->inherit_mode() == StaticContextConsts::inherit_ns)
    inherit = "inherit";
  else
    inherit = "no-inherit";

  if (theSctx->preserve_mode() == StaticContextConsts::preserve_ns)
    preserve = "preserve";
  else
    preserve = "no-preserve";

  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);
  STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, inherit.theStrStore), state);
  STACK_PUSH(GENV_ITEMFACTORY->createString(aResult, preserve.theStrStore), state);
  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
FunctionNamesIteratorState::~FunctionNamesIteratorState()
{
}


void FunctionNamesIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  theFunctions.clear();
  thePosition = 0;
}


bool FunctionNamesIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  FunctionNamesIteratorState* state;
  store::Item_t temp;

  DEFAULT_STACK_INIT(FunctionNamesIteratorState, state, aPlanState);

  theSctx->get_functions(state->theFunctions);
  state->thePosition = 0;
  while (state->thePosition < state->theFunctions.size())
  {
    temp = state->theFunctions[state->thePosition]->getName();
    // STACK_PUSH(state->theFunctions[state->thePosition]->getName(), state); // TODO: if this call is used, a segmentation fault will result later
    STACK_PUSH(GENV_ITEMFACTORY->createQName(aResult, temp->getNamespace()->c_str(), temp->getPrefix()->c_str(), temp->getLocalName()->c_str()), state);
    ++state->thePosition;
  }

  STACK_END(state);
}


/*******************************************************************************

********************************************************************************/
bool FunctionArgumentsCountIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  FunctionArgumentsCountIteratorState* lState;
  store::Item_t lName;
  std::vector<function*> funcs;

  DEFAULT_STACK_INIT(FunctionArgumentsCountIteratorState, lState, aPlanState);

  consumeNext(lName, theChildren[0].getp(), aPlanState);
  theSctx->find_functions(lName, funcs);
  for (unsigned int i=0; i<funcs.size(); i++)
    lState->arities.push_back(funcs[i]->get_arity());

  lState->position = 0;
  while (lState->position < lState->arities.size())
  {
    STACK_PUSH( GENV_ITEMFACTORY->createInt(aResult, lState->arities[lState->position]), lState);
    lState->position++;
  }

  STACK_END (lState);
}

FunctionArgumentsCountIteratorState::~FunctionArgumentsCountIteratorState()
{
}

void FunctionArgumentsCountIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  position = 0;
}


/*******************************************************************************

********************************************************************************/
InScopeSchemaTypesIteratorState::~InScopeSchemaTypesIteratorState()
{
}

void InScopeSchemaTypesIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  position = 0;
}


bool InScopeSchemaTypesIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  InScopeSchemaTypesIteratorState* state;
  store::Item_t temp;
  // const simplestore::QNameItem* temp_qname;

  DEFAULT_STACK_INIT(InScopeSchemaTypesIteratorState, state, aPlanState);

  /*
  for (state->position = 0;
       state->position < ((simplestore::SimpleStore*)&GENV_STORE)->theSchemaTypeNames.size();
       state->position++)
  {
    temp = ((simplestore::SimpleStore*)&GENV_STORE)->theSchemaTypeNames[state->position];
    temp_qname = dynamic_cast<simplestore::QNameItem*>(temp.getp());
    if (temp_qname != NULL)
      STACK_PUSH( GENV_ITEMFACTORY->createQName(aResult, temp_qname->getNamespace(), temp_qname->getPrefix(), temp_qname->getLocalName()), state);
  }
  */

  STACK_END (state);
}

/*******************************************************************************

********************************************************************************/
InScopeElementDeclarationsIteratorState::~InScopeElementDeclarationsIteratorState()
{
}

void InScopeElementDeclarationsIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  ns_pos = 0;
  elem_pos = 0;
}


bool InScopeElementDeclarationsIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  XERCES_CPP_NAMESPACE_USE;
  InScopeElementDeclarationsIteratorState* state;
  xqpString qname_ns;
  bool modelHasChanged;
  Schema* schema;
  XMLGrammarPool* grammarPool;
  XSModel* xsModel;
  StringList* namespaces;
  const XMLCh* nameSpace;
  XSNamedMap<XSObject>* xsElements;
  XSElementDeclaration* xsElement;
  const XMLCh* elemNameSpace;

  // MemoryManager* memoryManager = XMLPlatformUtils::fgMemoryManager;
  // GrammarResolver* theGrammarResolver = new (memoryManager)GrammarResolver(grammarPool, memoryManager);
  // theGrammarResolver->useCachedGrammarInParse(true);
  schema = theSctx->get_typemanager()->getSchema();
  grammarPool = schema->getGrammarPool();
  xsModel = grammarPool->getXSModel(modelHasChanged);
  namespaces = xsModel->getNamespaces();

  // std::cout << "--> PrintSchema::printInfo(): " << std::endl;
  // PrintSchema::printInfo(false, grammarPool);

  DEFAULT_STACK_INIT(InScopeElementDeclarationsIteratorState, state, aPlanState);
  state->ns_pos = 0;
  state->elem_pos = 0;

  while (state->ns_pos < namespaces->size())
  {
    nameSpace = namespaces->elementAt(state->ns_pos);
    if (nameSpace == NULL || XMLString::stringLen(nameSpace) <= 0)
    {
      state->ns_pos++;
      state->elem_pos = 0;
      continue;
    }

    xsElements = xsModel->getComponentsByNamespace(XSConstants::ELEMENT_DECLARATION, nameSpace);
    if (xsElements == NULL || xsElements->getLength() <= 0 || state->elem_pos >= xsElements->getLength())
    {
      state->ns_pos++;
      state->elem_pos = 0;
      continue;
    }

    xsElement = (XSElementDeclaration*)xsElements->item(state->elem_pos);
    elemNameSpace = xsElement->getNamespace();
    if (elemNameSpace && (XMLString::stringLen(elemNameSpace )>0))
      qname_ns = StrX(elemNameSpace).localForm();

    state->elem_pos++;
    STACK_PUSH(GENV_ITEMFACTORY->createQName(aResult, qname_ns.c_str(), "", StrX(xsElement->getName()).localForm()), state);
  }

  STACK_END (state);
}

/*******************************************************************************

********************************************************************************/
InScopeAttributeDeclarationsIteratorState::~InScopeAttributeDeclarationsIteratorState()
{
}

void InScopeAttributeDeclarationsIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  ns_pos = 0;
  elem_pos = 0;
}


bool InScopeAttributeDeclarationsIterator::nextImpl(
    store::Item_t& aResult,
    PlanState& aPlanState) const
{
  XERCES_CPP_NAMESPACE_USE;
  InScopeAttributeDeclarationsIteratorState* state;
  xqpString qname_ns;
  bool modelHasChanged;
  Schema* schema;
  XMLGrammarPool* grammarPool;
  XSModel* xsModel;
  StringList* namespaces;
  const XMLCh* nameSpace;
  XSNamedMap<XSObject>* xsElements;
  XSElementDeclaration* xsElement;
  const XMLCh* elemNameSpace;

  schema = theSctx->get_typemanager()->getSchema();
  grammarPool = schema->getGrammarPool();
  xsModel = grammarPool->getXSModel(modelHasChanged);
  namespaces = xsModel->getNamespaces();

  DEFAULT_STACK_INIT(InScopeAttributeDeclarationsIteratorState, state, aPlanState);
  state->ns_pos = 0;
  state->elem_pos = 0;

  while (state->ns_pos < namespaces->size())
  {
    nameSpace = namespaces->elementAt(state->ns_pos);
    if (nameSpace == NULL || XMLString::stringLen(nameSpace) <= 0)
    {
      state->ns_pos++;
      state->elem_pos = 0;
      continue;
    }

    xsElements = xsModel->getComponentsByNamespace(XSConstants::ATTRIBUTE_DECLARATION, nameSpace);
    if (xsElements == NULL || xsElements->getLength() <= 0 || state->elem_pos >= xsElements->getLength())
    {
      state->ns_pos++;
      state->elem_pos = 0;
      continue;
    }

    xsElement = (XSElementDeclaration*)xsElements->item(state->elem_pos);
    elemNameSpace = xsElement->getNamespace();
    if (elemNameSpace && (XMLString::stringLen(elemNameSpace )>0))
      qname_ns = StrX(elemNameSpace).localForm();

    state->elem_pos++;
    STACK_PUSH(GENV_ITEMFACTORY->createQName(aResult, qname_ns.c_str(), "", StrX(xsElement->getName()).localForm()), state);
  }

  STACK_END (state);
}



/*******************************************************************************
*******************************************************************************/
bool
IsDeclaredICIterator::nextImpl(store::Item_t& aResult, PlanState& aPlanState)
  const
{
  PlanIteratorState *lState;
  store::Item_t      lName;

  DEFAULT_STACK_INIT(PlanIteratorState, lState, aPlanState);
  consumeNext(lName, theChildren[0].getp(), aPlanState);
  if (theSctx->lookup_ic(lName.getp()) == 0) {
    STACK_PUSH (GENV_ITEMFACTORY->createBoolean ( aResult, false ), lState);
  }
  else {
    STACK_PUSH (GENV_ITEMFACTORY->createBoolean ( aResult, true ), lState);
  }
  STACK_END (lState);
}

/*******************************************************************************
********************************************************************************/
bool
DeclaredICsIterator::nextImpl(store::Item_t& aResult, PlanState& aPlanState)
  const
{
  DeclaredICsIteratorState * lState;
  store::Item_t              lName;

  DEFAULT_STACK_INIT(DeclaredICsIteratorState, lState, aPlanState);

  for ((lState->nameItState = theSctx->ic_names())->open ();
       lState->nameItState->next(lName); )
  {
    aResult = lName;
    STACK_PUSH( true, lState);
  }

  lState->nameItState->close();

  STACK_END (lState);
}

DeclaredICsIteratorState::~DeclaredICsIteratorState()
{
  if ( nameItState != NULL ) {
    nameItState->close();
    nameItState = NULL;
  }
}


void DeclaredICsIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  if ( nameItState != NULL ) {
    nameItState->close();
    nameItState = NULL;
  }
}

} /* namespace zorba */
