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
#ifndef ZORBA_SIMPLE_STORE_INDEX_HASH_VALUE
#define ZORBA_SIMPLE_STORE_INDEX_HASH_VALUE

#include "store/naive/simple_index.h"
#include <map>

namespace zorba
{

namespace simplestore
{

/**************************************************************************//**
  Class ValueIndexValue represents a value set as a vector of item handles.
*******************************************************************************/
class ValueIndexValue : public store::ItemVector
{
public:
  ValueIndexValue(ulong size = 0) : store::ItemVector(size) {}
};


/******************************************************************************

*******************************************************************************/
class ValueIndex : public IndexImpl
{
  friend class SimpleStore;

protected:
  ValueIndex(
        const store::Item_t& qname,
        const store::IndexSpecification& spec);
};


/******************************************************************************

*******************************************************************************/
class ValueHashIndex : public ValueIndex
{
  friend class SimpleStore;
  friend class ProbeHashValueIndexIterator;

  typedef HashMap<const store::IndexKey*,
                  ValueIndexValue*,
                  IndexCompareFunction> IndexMap;

private:
  IndexCompareFunction   theCompFunction;
  IndexMap               theMap;

protected:
  ValueHashIndex(
        const store::Item_t& qname,
        const store::IndexSpecification& spec);

  ~ValueHashIndex();

  void clear();

  bool insert(store::IndexKey*& key, store::Item_t& item, bool multikey = false);

  bool remove(const store::IndexKey* key, store::Item_t& item);
};


/******************************************************************************
  Iterator to probe a hash-based value index
********************************************************************************/
class ProbeHashValueIndexIterator : public store::IndexProbeIterator
{
protected:
  rchandle<ValueHashIndex>               theIndex;

  rchandle<IndexPointValueCondition>     theCondition;

  ValueIndexValue                      * theResultSet;
  ValueIndexValue::const_iterator        theIte;
  ValueIndexValue::const_iterator        theEnd;

public:
  ProbeHashValueIndexIterator(const store::Index_t& index) : theResultSet(NULL)
  {
    theIndex = static_cast<ValueHashIndex*>(index.getp());
  }

  void init(const store::IndexCondition_t& cond);

  void open();

  bool next(store::Item_t& result);

  void reset();

  void close();
};


/******************************************************************************

********************************************************************************/
class STLMapIndex : public ValueIndex
{
  friend class SimpleStore;
  friend class ProbeTreeValueIndexIterator;

  typedef std::pair<const store::IndexKey*, ValueIndexValue*> STLMapPair;

  typedef std::map<const store::IndexKey*,
                   ValueIndexValue*,
                   IndexCompareFunction> IndexMap;

private:
  IndexCompareFunction   theCompFunction;
  IndexMap               theMap;
  SYNC_CODE(Mutex        theMapMutex;)

public:
  void clear();

  bool insert(store::IndexKey*& key, store::Item_t& item, bool multikey = false);

  bool remove(const store::IndexKey* key, store::Item_t& item);

protected:
  STLMapIndex(
        const store::Item_t& qname,
        const store::IndexSpecification& spec);

  ~STLMapIndex();
};


/*******************************************************************************
  Iterator to probe a tree-based value index
********************************************************************************/
class ProbeTreeValueIndexIterator : public store::IndexProbeIterator
{
protected:
  rchandle<STLMapIndex>                    theIndex;

  rchandle<IndexPointValueCondition>       thePointCond;
  rchandle<IndexBoxValueCondition>         theBoxCond;

  bool                                     theDoExtraFiltering;

  STLMapIndex::IndexMap::const_iterator    theMapBegin;
  STLMapIndex::IndexMap::const_iterator    theMapEnd;
  STLMapIndex::IndexMap::const_iterator    theMapIte;

  ValueIndexValue                        * theResultSet;
  ValueIndexValue::const_iterator          theIte;
  ValueIndexValue::const_iterator          theEnd;

public:
  ProbeTreeValueIndexIterator(const store::Index_t& index)
    :
    theDoExtraFiltering(true),
    theResultSet(NULL)
  {
    theIndex = reinterpret_cast<STLMapIndex*>(index.getp());
  }

  void init(const store::IndexCondition_t& cond);

  void open();

  bool next(store::Item_t& result);

  void reset();

  void close();

protected:
  void initExact();

  void initBox();
};


}
}

#endif

/*
 * Local variables:
 * mode: c++
 * End:
 */
