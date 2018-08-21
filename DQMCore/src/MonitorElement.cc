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
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/QualityTest.h>

// -- root headers
#include <TAxis.h>
#include <TH1.h>
#include <TPad.h>
#include <TClass.h>
#include <TBuffer.h>
#include <TBufferJSON.h>
#include <TBufferFile.h>

templateClassImp(dqm4hep::core::TScalarObject) 
ClassImp(dqm4hep::core::TDynamicGraph)

namespace dqm4hep {

  namespace core {

    MonitorElementPtr MonitorElement::make_shared() {
      return std::shared_ptr<MonitorElement>(new MonitorElement());
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElementPtr MonitorElement::make_shared(TObject *pMonitorObject) {
      return std::shared_ptr<MonitorElement>(new MonitorElement(pMonitorObject));
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElementPtr MonitorElement::make_shared(TObject *pMonitorObject, TObject *pReferenceObject) {
      return std::shared_ptr<MonitorElement>(new MonitorElement(pMonitorObject, pReferenceObject));
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElementPtr MonitorElement::make_shared(const PtrHandler<TObject> &monitorObject) {
      return std::shared_ptr<MonitorElement>(new MonitorElement(monitorObject));
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElementPtr MonitorElement::make_shared(const PtrHandler<TObject> &monitorObject,
                                                  const PtrHandler<TObject> &referenceObject) {
      return std::shared_ptr<MonitorElement>(new MonitorElement(monitorObject, referenceObject));
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement() : m_path(""), m_monitorObject(), m_referenceObject() {
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement(TObject *pMonitorObject) : m_monitorObject(pMonitorObject), m_referenceObject() {
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement(TObject *pMonitorObject, TObject *pReferenceObject)
        : m_monitorObject(pMonitorObject), m_referenceObject(pReferenceObject) {
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement(const PtrHandler<TObject> &monitorObject)
        : m_monitorObject(monitorObject.ptr(), false), m_referenceObject() {
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement(const PtrHandler<TObject> &monitorObject, const PtrHandler<TObject> &referenceObject)
        : m_monitorObject(monitorObject.ptr(), false), m_referenceObject(referenceObject.ptr(), false) {
    }

    //-------------------------------------------------------------------------------------------------

    std::string MonitorElement::type() const {
      return (m_monitorObject ? m_monitorObject->ClassName() : "");
    }

    //-------------------------------------------------------------------------------------------------

    std::string MonitorElement::name() const {
      return (m_monitorObject ? m_monitorObject->GetName() : "");
    }

    //-------------------------------------------------------------------------------------------------

    std::string MonitorElement::title() const {
      return (m_monitorObject ? m_monitorObject->GetTitle() : "");
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void MonitorElement::setTitle(const std::string &t) {
      if(nullptr == m_monitorObject) {
        dqm_error("Trying to set monitor element title: not initialized!");
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
      }
      if(not m_monitorObject.ptr()->InheritsFrom("TNamed")) {
        dqm_error("Trying to set monitor element title: does not inherits from TNamed!");
        throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);
      }
      TNamed *named = objectTo<TNamed>();
      named->SetTitle(t.c_str());
    }

    //-------------------------------------------------------------------------------------------------

    std::string MonitorElement::path() const {
      return m_path;
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setPath(const std::string &p) {
      m_path = p;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void MonitorElement::setName(const std::string &n) {
      if(nullptr == m_monitorObject) {
        dqm_error("Trying to set monitor element name: not initialized!");
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
      }
      if(not m_monitorObject.ptr()->InheritsFrom("TNamed")) {
        dqm_error("Trying to set monitor element name: does not inherits from TNamed!");
        throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);
      }
      TNamed *named = objectTo<TNamed>();
      named->SetName(n.c_str());
    }

    //-------------------------------------------------------------------------------------------------

    bool MonitorElement::hasObject() const {
      return (m_monitorObject != nullptr);
    }

    //-------------------------------------------------------------------------------------------------

    bool MonitorElement::hasReference() const {
      return (m_referenceObject != nullptr);
    }

    //-------------------------------------------------------------------------------------------------

    TObject *MonitorElement::object() {
      return m_monitorObject ? m_monitorObject.ptr() : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    const TObject *MonitorElement::object() const {
      return m_monitorObject ? m_monitorObject.ptr() : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    TObject *MonitorElement::reference() {
      return m_referenceObject ? m_referenceObject.ptr() : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    const TObject *MonitorElement::reference() const {
      return m_referenceObject ? m_referenceObject.ptr() : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setMonitorObject(TObject *pMonitorObject) {
      m_monitorObject.clear();
      m_monitorObject.set(pMonitorObject);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setMonitorObject(const PtrHandler<TObject> &monitorObject) {
      m_monitorObject.clear();
      m_monitorObject.set(monitorObject.ptr(), false);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setReferenceObject(TObject *pReferenceObject) {
      m_referenceObject.clear();
      m_referenceObject.set(pReferenceObject);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setReferenceObject(const PtrHandler<TObject> &referenceObject) {
      m_referenceObject.clear();
      m_referenceObject.set(referenceObject.ptr(), false);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::set(TObject *pMonitorObject, TObject *pReferenceObject) {
      m_monitorObject.clear();
      m_monitorObject.set(pMonitorObject);
      m_referenceObject.clear();
      m_referenceObject.set(pReferenceObject);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::set(const PtrHandler<TObject> &monitorObject, const PtrHandler<TObject> &referenceObject) {
      m_monitorObject.clear();
      m_monitorObject.set(monitorObject.ptr(), false);
      m_referenceObject.clear();
      m_referenceObject.set(referenceObject.ptr(), false);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void MonitorElement::reset(bool resetQtests) {
      m_monitorObject.clear();
      m_referenceObject.clear();
      m_path.clear();
      if(resetQtests) {
        m_qualityTests.clear();
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void MonitorElement::toJson(json &jobject) const {
      
      json jsonObject = nullptr, jsonReference = nullptr;
      
      if(nullptr != m_monitorObject) {
        jsonObject = json::parse(TBufferJSON::ConvertToJSON(m_monitorObject.ptr(), 23).Data());
      }
      
      if(nullptr != m_referenceObject) {
        jsonReference = json::parse(TBufferJSON::ConvertToJSON(m_referenceObject.ptr(), 23).Data());
      }

      jobject = {
        {"object", jsonObject},
        {"reference", jsonReference},
        {"path", m_path}
      };
    }
  
    //-------------------------------------------------------------------------------------------------
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 14, 0)
    void MonitorElement::fromJson(const json &object) {
      // read object
      m_monitorObject.clear();
      auto jsonObject = object.value("object", json(nullptr));
      if(nullptr != jsonObject) {
        TObject *pTObject = TBufferJSON::ConvertFromJSON(jsonObject.dump().c_str());  
        if(nullptr != pTObject) {
          m_monitorObject.set(pTObject, true);
        }
      }
      // read reference
      m_referenceObject.clear();
      auto jsonReference = object.value("reference", json(nullptr));
      if(nullptr != jsonReference) {
        TObject *pTObject = TBufferJSON::ConvertFromJSON(jsonReference.dump().c_str());
        
        if(nullptr != pTObject) {
          m_referenceObject.set(pTObject, true);
        }
      }
      // read path
      m_path = object.value<std::string>("path", "");
    }
#endif
    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::write(TBuffer &buffer) const {
      if(not buffer.IsWriting()) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      // write path
      buffer.WriteStdString(&m_path);
      // write object
      const bool hasObjectWrite = hasObject();
      buffer.WriteBool(hasObjectWrite);
      if(hasObjectWrite) {
        TClass *objClass = TClass::GetClass(object()->ClassName());
        if(not buffer.WriteObjectAny((void*)object(), objClass)) {
          return STATUS_CODE_FAILURE;
        }
      }
      // write reference
      const bool hasReferenceWrite = hasReference();
      buffer.WriteBool(hasReferenceWrite);
      if(hasReferenceWrite) {
        TClass *refClass = TClass::GetClass(reference()->ClassName());
        if(not buffer.WriteObjectAny((void*)reference(), refClass)) {
          return STATUS_CODE_FAILURE;
        }
      }
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::read(TBuffer &buffer) {
      if(not buffer.IsReading()) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      reset(false);
      // write path
      buffer.ReadStdString(&m_path);
      bool hasObjectRead(false);
      buffer.ReadBool(hasObjectRead);
      if(hasObjectRead) {
        TObject *obj = buffer.ReadObject(nullptr);
        if(nullptr == obj) {
          return STATUS_CODE_FAILURE;
        }
        m_monitorObject.set(obj, true);
      }
      bool hasReferenceRead(false);
      buffer.ReadBool(hasReferenceRead);
      if(hasReferenceRead) {
        TObject *ref = buffer.ReadObject(nullptr);
        if(nullptr == ref) {
          return STATUS_CODE_FAILURE;
        }
        m_referenceObject.set(ref, true);
      }
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::addQualityTest(QTestPtr qualityTest) {
      const std::string &qname(qualityTest->name());
      auto iter = m_qualityTests.find(qname);

      if (m_qualityTests.end() != iter)
        return STATUS_CODE_ALREADY_PRESENT;

      m_qualityTests.insert(QTestMap::value_type(qname, qualityTest));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::removeQualityTest(const std::string &qname) {
      auto iter = m_qualityTests.find(qname);

      if (m_qualityTests.end() == iter)
        return STATUS_CODE_NOT_FOUND;

      m_qualityTests.erase(iter);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::runQualityTests(QReportMap &reports) {
      try {
        for (auto iter : m_qualityTests) {
          QReport report;
          iter.second->run(this, report);
          reports.insert(QReportMap::value_type(iter.first, report));
        }
      } catch (StatusCodeException &exception) {
        return exception.getStatusCode();
      } catch (...) {
        return STATUS_CODE_FAILURE;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::runQualityTest(const std::string &qname, QReport &report) {
      auto iter = m_qualityTests.find(qname);

      if (m_qualityTests.end() == iter)
        return STATUS_CODE_NOT_FOUND;

      iter->second->run(this, report);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    TDynamicGraph::TDynamicGraph() : TGraph(), m_rangeLength(-1.) {
      SetBit(kDynamicRange);
      SetBit(kShrinkToRange);
    }

    //-------------------------------------------------------------------------------------------------

    TDynamicGraph::~TDynamicGraph() = default;

    //-------------------------------------------------------------------------------------------------

    void TDynamicGraph::SetRangeLength(Double_t rangeLength) {
      m_rangeLength = rangeLength;
    }

    //-------------------------------------------------------------------------------------------------

    void TDynamicGraph::AddPoint(Double_t x, Double_t y) {
      Int_t nPoints = this->GetN();
      this->SetPoint(nPoints, x, y);
      this->ShrinkToRange();
    }

    //-------------------------------------------------------------------------------------------------

    void TDynamicGraph::Draw(Option_t *option) {
      TGraph::Draw(option);

      if (TestBit(kDynamicRange)) {
        Int_t nPoints = this->GetN();

        if (m_rangeLength <= 0 || nPoints == 0)
          return;

        Double_t lastX, lastY;
        this->GetPoint(nPoints - 1, lastX, lastY);

        this->GetXaxis()->SetRangeUser(lastX - m_rangeLength, lastX);

        if (gPad)
          gPad->Update();
      }
    }

    //-------------------------------------------------------------------------------------------------

    void TDynamicGraph::ShrinkToRange() {
      if (!TestBit(kDynamicRange) || !TestBit(kShrinkToRange))
        return;

      Int_t nPoints = this->GetN();

      if (m_rangeLength <= 0 || nPoints == 0)
        return;

      Double_t lastX, lastY;
      Int_t res = this->GetPoint(nPoints - 1, lastX, lastY);

      if (res < 0)
        return;

      Double_t xLimit = lastX - m_rangeLength;

      while (true) {
        Double_t x, y;
        res = this->GetPoint(0, x, y);

        if (res < 0)
          break;

        if (x < xLimit)
          this->RemovePoint(0);
        else
          break;
      }
    }
  }
}
