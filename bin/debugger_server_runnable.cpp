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

#include "debugger_server_runnable.h"

#include <iostream>
#include "zorba/xquery.h"
#include <zorba/xquery_exception.h>
#include "error_printer.h"

namespace zorba {

DebuggerServerRunnable::DebuggerServerRunnable(
  const XQuery_t& aQuery,
  std::ostream&   aOutStream,
  const std::string& aHost,
  unsigned short  aPort,
  const Zorba_SerializerOptions& 
  aSerOpts)
  : theQuery(aQuery),
    theOutStream(aOutStream),
    theHost(aHost),
    thePort(aPort),
    theSerializerOptions(aSerOpts)
{
}


DebuggerServerRunnable::~DebuggerServerRunnable()
{
}

void
DebuggerServerRunnable::run()
{
  try {
    theQuery->debug(theOutStream,
      theSerializerOptions,      
      thePort);
  } catch (zorba::XQueryException const& qe) {
    ErrorPrinter::print(qe, std::cerr, false, true);
  } catch (zorba::ZorbaException const& ze) {
    std::cerr << ze << std::endl;
  }
}

void
DebuggerServerRunnable::finish()
{
}

} /* namespace zorba */