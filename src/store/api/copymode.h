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
#ifndef ZORBA_STORE_API_COPYMODE
#define ZORBA_STORE_API_COPYMODE

#include <zorba/config.h>
#include <string>

namespace zorba { namespace store {

/**
 * \brief Instances of this class are used to pass information about the
 * construction-mode and copy-namespace-mode components of a query's static
 * context to the Item::copy() method.
 *
 */
class CopyMode
{
public:
  bool  theDoCopy;
  bool  theTypePreserve;
  bool  theNsPreserve;
  bool  theNsInherit;
  bool	theFreezeReference;

  CopyMode()
    :
    theDoCopy(true),
    theTypePreserve(true),
    theNsPreserve(true),
    theNsInherit(true),
    theFreezeReference(false)
  {
  }

  CopyMode(const CopyMode& aCopyMode)
    :
    theDoCopy(aCopyMode.theDoCopy),
    theTypePreserve(aCopyMode.theTypePreserve),
    theNsPreserve(aCopyMode.theNsPreserve),
    theNsInherit(aCopyMode.theNsInherit),
    theFreezeReference(aCopyMode.theFreezeReference)
  {
  }

  void set(
        bool copy,
        bool typePreserve,
        bool nsPreserve,
        bool nsInherit,
        bool freezeReference=false)
  {
    theDoCopy = copy;
    theTypePreserve = typePreserve;
    theNsPreserve = nsPreserve;
    theNsInherit = nsInherit;
    theFreezeReference=freezeReference;
  }

  std::string toString() const
  {
    std::string s;
    s = "[";
    s += (theDoCopy ? "T" : "F");
    s += " ";
    s += (theTypePreserve ? "T" : "F");
    s += " ";
    s += (theNsPreserve ? "T" : "F");
    s += " ";
    s += (theNsInherit ? "T" : "F");
    s += " ";
    s += (theFreezeReference ? "T" : "F");
    s += "]";
    return s;
  }
};

} // namespace store
} // namespace zorba
#endif
/* vim:set et sw=2 ts=2: */
