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

#ifndef ZORBA_UNIT_TEST_LIST_H
#define ZORBA_UNIT_TEST_LIST_H


namespace zorba {

  namespace UnitTests {
    int runUriTest(int argc, char* argv[]);
    int runDebuggerProtocolTest(int argc, char* argv[]);
    /**
     * ADD NEW UNIT TESTS HERE
     */


    void initializeTestList();
  };
    

} /* namespace zorba */

#endif /* ZORBA_UNIT_TEST_LIST_H */
