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

#include "runtime/function_item/function_item.h"

#include "functions/signature.h"

#include "store/api/iterator.h"

namespace zorba {

namespace simplestore {

#ifdef ZORBA_FOR_ONE_THREAD_ONLY
  FunctionItem::FunctionItem(
          const std::vector<store::Iterator_t>& aVariableValues,
          const signature& aSignature,
          const store::Iterator_t& aImplementation)
  : store::Item(),
    theName(0),
    theVariableValues(aVariableValues),
    theSignature(aSignature),
    theImplementation(aImplementation){}
    
    Function::FunctionItem(const store::Item_t& aName,
          const signature& aSignature,
          const store::Iterator_t& aImplementation)
  : store::Item(),
    theName(aName),
    theSignature(aSignature),
    theImplementation(aImplementation){}

#else
    FunctionItem::FunctionItem(
          const std::vector<store::Iterator_t>& aVariableValues,
          const signature& aSignature,
          const store::Iterator_t& aImplementation)
    : store::Item(),
      theName(0),
      theVariableValues(aVariableValues),
      theSignature(aSignature),
      theImplementation(aImplementation)
    { SYNC_CODE(theRCLockPtr = &theRCLock;) }
    
    FunctionItem::FunctionItem(const store::Item_t& aName,
          const signature& aSignature,
          const store::Iterator_t& aImplementation)
  : store::Item(),
    theName(aName),
    theVariableNames(),
    theVariableValues(),
    theSignature(aSignature),
    theImplementation(aImplementation){ SYNC_CODE(theRCLockPtr = &theRCLock;) }

#endif

void FunctionItem::getFunctionVariables(std::vector<store::Iterator_t>& aResult) const
{
    std::vector<store::Iterator_t>::const_iterator lIt;
    for(lIt = theVariableValues.begin(); lIt != theVariableValues.end(); lIt++)
    {
        aResult.push_back(*lIt);
    }
}

const store::Iterator_t FunctionItem::getFunctionImplementation() const
{ return theImplementation; }

const signature FunctionItem::getFunctionSignature() const
{ return theSignature; }

} //namespace simplestore
} //namespace zorba
