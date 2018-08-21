//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Event.h>
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/UnitTesting.h>

// -- root headers
#include <TObject.h>
#include <TH1.h>
#include <TBufferFile.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;

EventPtr createEvent() {
  auto event = std::shared_ptr<Event>(new EventBase<TObject>());
  event->setStreamerName("RootEventStreamer");
  return event;
}

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-root-event-streamer");

  // Input event
  EventPtr outEvent = createEvent();
  TH1I *histogram = new TH1I("HistoEvent", "A fake histo representing an event", 10, 0, 9);
  outEvent->setEvent<TObject>(histogram, true);
  histogram->SetBinContent(1, 3);
  histogram->SetBinContent(5, 17);
  
  TBufferFile outBuffer(TBuffer::kWrite);
  EventStreamer streamer;
  
  // Test writing out
  unitTest.test("WRITE_EVENT", STATUS_CODE_SUCCESS == streamer.writeEvent(outEvent, outBuffer));
  
  // prepare for reading in
  EventPtr inEvent = createEvent();
  TBufferFile inBuffer(TBuffer::kRead);
  inBuffer.SetBuffer(outBuffer.Buffer(), outBuffer.Length(), false);
  
  // Test reading in
  unitTest.test("READ_EVENT", STATUS_CODE_SUCCESS == streamer.readEvent(inEvent, inBuffer));
  TObject *inTObject = inEvent->getEvent<TObject>();
  
  // Check TObject pointer
  unitTest.test("VALID_OBJECT", nullptr != inTObject);
  
  TH1I *inHistogram = (TH1I*)inTObject;
  
  // Check histogram pointer
  unitTest.test("VALID_HISTO", nullptr != inHistogram);
  
  // Check histogram content
  unitTest.test("VALID_BIN_CONTENT1", inHistogram->GetBinContent(1) == 3);
  unitTest.test("VALID_BIN_CONTENT2", inHistogram->GetBinContent(5) == 17);

  return 0;
}
