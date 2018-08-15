/// \file test-root-event-streamer.cc
/*
 *
 * test-root-event-streamer.cc main source file template automatically generated
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
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Event.h>
#include <dqm4hep/EventStreamer.h>

// -- root headers
#include <TObject.h>
#include <TH1.h>

// -- xdrstream headers
#include <xdrstream/xdrstream.h>

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

EventPtr createEvent() {
  auto event = std::shared_ptr<Event>(new EventBase<TObject>());
  event->setStreamerName("RootEventStreamer");
  return event;
}

int main(int /*argc*/, char ** /*argv*/) {
  Logger::createLogger("test-root-event-streamer", {Logger::coloredConsole()});
  Logger::setMainLogger("test-root-event-streamer");
  Logger::setLogLevel(spdlog::level::debug);

  // Input event
  EventPtr outEvent = createEvent();
  TH1I *histogram = new TH1I("HistoEvent", "A fake histo representing an event", 10, 0, 9);
  outEvent->setEvent<TObject>(histogram, true);
  histogram->SetBinContent(1, 3);
  histogram->SetBinContent(5, 17);
  
  xdrstream::BufferDevice outDevice;
  EventStreamer streamer;
  
  // Test writing out
  assert_test(STATUS_CODE_SUCCESS == streamer.writeEvent(outEvent, &outDevice));
  
  // dqm_debug("outDevice.getPosition() is {0}", outDevice.getPosition());
  // dqm_debug("outDevice.size() is {0}", outDevice.size());
  
  // prepare for reading in
  EventPtr inEvent = createEvent();
  xdrstream::BufferDevice inDevice(outDevice.getBuffer(), outDevice.getPosition(), false);
  inDevice.setOwner(false);
  // dqm_debug("inDevice.getPosition() is {0}", inDevice.getPosition());
  // dqm_debug("inDevice.size() is {0}", inDevice.size());
  
  // Test reading in
  assert_test(STATUS_CODE_SUCCESS == streamer.readEvent(inEvent, &inDevice));
  TObject *inTObject = inEvent->getEvent<TObject>();
  
  // Check TObject pointer
  assert_test(nullptr != inTObject);
  
  TH1I *inHistogram = (TH1I*)inTObject;
  
  // Check histogram pointer
  assert_test(nullptr != inHistogram);
  
  // Check histogram content
  assert_test(inHistogram->GetBinContent(1) == 3);
  assert_test(inHistogram->GetBinContent(5) == 17);

  return 0;
}
