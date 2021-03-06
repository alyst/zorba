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

#include <iostream>

#include <zorba/zorba.h>
#include <zorba/serializer.h>
#include <zorba/singleton_item_sequence.h>
#include <zorba/store_manager.h>
#include <zorba/zorba_exception.h>

using namespace zorba;

/**
 * Example to show a quick way to serialize results of query execution using
 * the << operator on ostreams
 */
bool
serialization_example_1(Zorba* aZorba)
{
  try {
    XQuery_t lQuery = aZorba->compileQuery("for $i in (1 to 3) return <a> { $i } </a>"); 

    std::cout << lQuery << std::endl;
  } catch (ZorbaException& e) {
    std::cerr << e << std::endl;
    return false;
  }

	return true;
}

/**
 * Example to invoke the serializer using the serialize() call on the query object.
 */
bool
serialization_example_2(Zorba* aZorba)
{
  try {
    XQuery_t lQuery = aZorba->compileQuery("for $i in (1 to 3) return <a> { $i } </a>"); 

    lQuery->execute(std::cout);
  } catch (ZorbaException& e) {
    std::cerr << e << std::endl;
    return false;
  }

	return true;
}

/**
 * Example that shows HTML serialization of the results.
 */
bool
serialization_example_3(Zorba* aZorba)
{
  try {
    XQuery_t lQuery = aZorba->compileQuery("for $i in (1 to 3) return <a> { $i } </a>"); 

    Zorba_SerializerOptions lSerOptions;
    lSerOptions.ser_method = ZORBA_SERIALIZATION_METHOD_HTML;

    lQuery->execute(std::cout, &lSerOptions);
  } catch (ZorbaException& e) {
    std::cerr << e << std::endl;
    return false;
  }

	return true;
}

/**
 * Example to serialize the result of a query with indentation.
 */
bool
serialization_example_4(Zorba* aZorba)
{
  try {
    XQuery_t lQuery = aZorba->compileQuery("for $i in (1 to 3) return <a> { $i } </a>"); 

    Zorba_SerializerOptions lSerOptions;
    lSerOptions.indent = ZORBA_INDENT_YES;

    lQuery->execute(std::cout, &lSerOptions);
  } catch (ZorbaException& e) {
    std::cerr << e << std::endl;
    return false;
  }

	return true;
}

bool
serialization_example_5(Zorba* aZorba)
{
  try {
    XQuery_t lQuery = aZorba->compileQuery("for $i in (1 to 3) return <a> { $i } </a>"); 

    Zorba_SerializerOptions lSerOptions;
    lSerOptions.omit_xml_declaration = ZORBA_OMIT_XML_DECLARATION_YES;

    lQuery->execute(std::cout, &lSerOptions);
  } catch (ZorbaException& e) {
    std::cerr << e << std::endl;
    return false;
  }

	return true;
}

bool
serialization_example_6(Zorba* aZorba)
{
  try {
    XQuery_t lQuery = aZorba->compileQuery("for $i in (1 to 3) return <a> { $i } </a>"); 

    Zorba_SerializerOptions lSerOptions;
    lSerOptions.byte_order_mark = ZORBA_BYTE_ORDER_MARK_YES;

    lQuery->execute(std::cout, &lSerOptions);
  } catch (ZorbaException& e) {
    std::cerr << e << std::endl;
    return false;
  }

	return true;
}

bool
serialization_example_7(Zorba* aZorba)
{
  XQuery_t lQuery = aZorba->compileQuery("for $i in (1 to 3) return <a> { $i } </a>"); 

  Zorba_SerializerOptions_t lOptions;
  Serializer_t lSerializer = Serializer::createSerializer(lOptions);

  Iterator_t lIterator = lQuery->iterator();

  try {
    lIterator->open();

    Item lItem;
    while ( lIterator->next(lItem) ) {
      // we have to wrap the item in a Serializable object
      SingletonItemSequence lSequence(lItem);
      lSerializer->serialize(&lSequence, std::cout);
    }

    lIterator->close();

  } catch (ZorbaException& e) {
    std::cerr << e << std::endl;
    lIterator->close();
    return false;
  }
	return true;
}

int 
serialization(int argc, char* argv[])
{
  void* lStore = zorba::StoreManager::getStore();
  Zorba* lZorba = Zorba::getInstance(lStore);
  bool res = false;

  std::cout << "executing serialization example 1" << std::endl;
  res = serialization_example_1(lZorba);
  if (!res) return 1; 
  std::cout << std::endl;

  std::cout << "executing serialization example 2" << std::endl;
  res = serialization_example_2(lZorba);
  if (!res) return 1; 
  std::cout << std::endl;

  std::cout << "executing serialization example 3" << std::endl;
  res = serialization_example_3(lZorba);
  if (!res) return 1; 
  std::cout << std::endl;

  std::cout << "executing serialization example 4" << std::endl;
  res = serialization_example_4(lZorba);
  if (!res) return 1; 
  std::cout << std::endl;

  std::cout << "executing serialization example 5" << std::endl;
  res = serialization_example_5(lZorba);
  if (!res) return 1; 
  std::cout << std::endl;

  std::cout << "executing serialization example 6" << std::endl;
  res = serialization_example_6(lZorba);
  if (!res) return 1; 
  std::cout << std::endl;

  std::cout << "executing serialization example 7" << std::endl;
  res = serialization_example_7(lZorba);
  if (!res) return 1; 
  std::cout << std::endl;

  lZorba->shutdown();
  zorba::StoreManager::shutdownStore(lStore);
  return 0;
}
