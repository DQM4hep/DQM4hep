/// \file test-qtest-exact-ref-comp.cc
/*
 *
 * test-qtest-exact-ref-comp.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2014
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElementManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/PluginManager.h>

#include <TH1.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

#define assert_test(Command)                                                                                           \
  if (!(Command)) {                                                                                                    \
    dqm_error("Assertion failed : {0}, line {1}", #Command, __LINE__);                                                 \
    exit(1);                                                                                                           \
  }
  
void fillExact(TH1 *histogram) {
  histogram->Fill(1);
  histogram->Fill(50);
  histogram->Fill(50);
  histogram->Fill(53);
  histogram->Fill(42);
  histogram->Fill(-2); // underflow
  histogram->Fill(-6); // underflow
  histogram->Fill(102);// overflow
  histogram->Fill(105);// overflow
  histogram->Fill(106);// overflow
}

void fillNotExact(TH1 *histogram) {
  histogram->Fill(2);
  histogram->Fill(40);
  histogram->Fill(50);
  histogram->Fill(23);
  histogram->Fill(22);
  histogram->Fill(-1); // underflow
  histogram->Fill(-3); // underflow
  histogram->Fill(106);// overflow
}

int main(int argc, char *argv[]) {
  Logger::createLogger("test-qtest-exact-ref-comp", {Logger::coloredConsole()});
  Logger::setMainLogger("test-qtest-exact-ref-comp");
  Logger::setLogLevel(spdlog::level::debug);

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  MonitorElementPtr testElement;
  meMgr->bookObject("/", "TestHisto", testElement, TH1FAllocator(), "", "A test histogram", 100, 0.f, 99.f);
  
  TH1F *histogram = testElement->objectTo<TH1F>();
  assert_test(nullptr != histogram);
  
  fillExact(histogram);
  
  PtrHandler<TObject> reference(new TH1F("", "The good reference", 100, 0.f, 99.f), true);
  PtrHandler<TObject> badReferenceBining(new TH1F("", "The bad reference - Wrong binning", 110, 0.f, 99.f), true);
  PtrHandler<TObject> badReferenceContent(new TH1F("", "The bad reference - Wrong content", 100, 0.f, 99.f), true);
  PtrHandler<TObject> badReferenceType(new TH1D("", "The bad reference - Wrong type", 100, 0.f, 99.f), true);
  
  fillExact((TH1F*)reference.ptr());
  fillExact((TH1F*)badReferenceBining.ptr());
  fillExact((TH1F*)badReferenceType.ptr());
  fillNotExact((TH1F*)reference.ptr());

  return 0;
}
