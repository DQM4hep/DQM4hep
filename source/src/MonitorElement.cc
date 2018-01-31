/// \file MonitorElement.cc
/*
 *
 * MonitorElement.cc source template automatically generated by a class generator
 * Creation date : jeu. sept. 4 2014
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
 * @author Ete Remi
 * @copyright CNRS , IPNL
 */

// -- dqm4hep headers
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/QualityTest.h>

// -- root headers
#include <TH1.h>
#include <TAxis.h>
#include <TPad.h>

templateClassImp(dqm4hep::core::TScalarObject)
ClassImp(dqm4hep::core::TDynamicGraph)

namespace dqm4hep {

  namespace core {
    
    MonitorElementPtr MonitorElement::make_shared()
    {
      return std::shared_ptr<MonitorElement>(new MonitorElement());
    }
    
    //-------------------------------------------------------------------------------------------------

    MonitorElementPtr MonitorElement::make_shared(TObject *pMonitorObject)
    {
      return std::shared_ptr<MonitorElement>(new MonitorElement(pMonitorObject));
    }
    
    //-------------------------------------------------------------------------------------------------

    MonitorElementPtr MonitorElement::make_shared(TObject *pMonitorObject, TObject *pReferenceObject)
    {
      return std::shared_ptr<MonitorElement>(new MonitorElement(pMonitorObject, pReferenceObject));
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElementPtr MonitorElement::make_shared(const PtrHandler<TObject> &monitorObject)
    {
      return std::shared_ptr<MonitorElement>(new MonitorElement(monitorObject));
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElementPtr MonitorElement::make_shared(const PtrHandler<TObject> &monitorObject, const PtrHandler<TObject> &referenceObject)
    {
      return std::shared_ptr<MonitorElement>(new MonitorElement(monitorObject, referenceObject));
    }
    
    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement() :
      m_path(""),
      m_monitorObject(),
      m_referenceObject()
    {
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement(TObject *pMonitorObject) :
      m_monitorObject(pMonitorObject),
      m_referenceObject()
    {
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement(TObject *pMonitorObject, TObject *pReferenceObject) :
      m_monitorObject(pMonitorObject),
      m_referenceObject(pReferenceObject)
    {
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement(const PtrHandler<TObject> &monitorObject) :
      m_monitorObject(monitorObject.ptr(), false),
      m_referenceObject()
    {
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElement::MonitorElement(const PtrHandler<TObject> &monitorObject, const PtrHandler<TObject> &referenceObject) :
      m_monitorObject(monitorObject.ptr(), false),
      m_referenceObject(referenceObject.ptr(), false)
    {
    }

    //-------------------------------------------------------------------------------------------------

    std::string MonitorElement::type() const
    {
      return (m_monitorObject ? m_monitorObject->ClassName() : "");
    }

    //-------------------------------------------------------------------------------------------------

    std::string MonitorElement::name() const
    {
      return (m_monitorObject ? m_monitorObject->GetName() : "");
    }

    //-------------------------------------------------------------------------------------------------

    std::string MonitorElement::title() const
    {
      return (m_monitorObject ? m_monitorObject->GetTitle() : "");
    }

    //-------------------------------------------------------------------------------------------------

    std::string MonitorElement::path() const
    {
      return m_path;
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setPath(const std::string &path)
    {
      m_path = path;
    }

    //-------------------------------------------------------------------------------------------------

    bool MonitorElement::hasObject() const
    {
      return (m_monitorObject != nullptr);
    }

    //-------------------------------------------------------------------------------------------------

    bool MonitorElement::hasReference() const
    {
      return (m_referenceObject != nullptr);
    }

    //-------------------------------------------------------------------------------------------------

    TObject *MonitorElement::object()
    {
      return m_monitorObject ? m_monitorObject.ptr() : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    const TObject *MonitorElement::object() const
    {
      return m_monitorObject ? m_monitorObject.ptr() : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    TObject *MonitorElement::reference()
    {
      return m_referenceObject ? m_referenceObject.ptr() : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    const TObject *MonitorElement::reference() const
    {
      return m_referenceObject ? m_referenceObject.ptr() : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setMonitorObject(TObject *pMonitorObject)
    {
      m_monitorObject.clear();
      m_monitorObject.set(pMonitorObject);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setMonitorObject(const PtrHandler<TObject> &monitorObject)
    {
      m_monitorObject.clear();
      m_monitorObject.set(monitorObject.ptr(), false);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setReferenceObject(TObject *pReferenceObject)
    {
      m_referenceObject.clear();
      m_referenceObject.set(pReferenceObject);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::setReferenceObject(const PtrHandler<TObject> &referenceObject)
    {
      m_referenceObject.clear();
      m_referenceObject.set(referenceObject.ptr(), false);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::set(TObject *pMonitorObject, TObject *pReferenceObject)
    {
      m_monitorObject.clear();
      m_monitorObject.set(pMonitorObject);
      m_referenceObject.clear();
      m_referenceObject.set(pReferenceObject);
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElement::set(const PtrHandler<TObject> &monitorObject, const PtrHandler<TObject> &referenceObject)
    {
      m_monitorObject.clear();
      m_monitorObject.set(monitorObject.ptr(), false);
      m_referenceObject.clear();
      m_referenceObject.set(referenceObject.ptr(), false);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::addQualityTest(QTestPtr qualityTest)
    {
      const std::string &name(qualityTest->name());
      auto iter = m_qualityTests.find(name);

      if(m_qualityTests.end() != iter)
        return STATUS_CODE_ALREADY_PRESENT;

      m_qualityTests.insert(QTestMap::value_type(name, qualityTest));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::removeQualityTest(const std::string &name)
    {
      auto iter = m_qualityTests.find(name);

      if(m_qualityTests.end() == iter)
        return STATUS_CODE_NOT_FOUND;

      m_qualityTests.erase(iter);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::runQualityTests(QReportMap &reports)
    {
      try
      {
        for(auto iter : m_qualityTests)
        {
          QReport report;
          iter.second->run(this->shared_from_this(), report);
          reports.insert(QReportMap::value_type(iter.first, report));
        }
      }
      catch(StatusCodeException &exception)
      {
        return exception.getStatusCode();
      }
      catch(...)
      {
        return STATUS_CODE_FAILURE;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElement::runQualityTest(const std::string &name, QReport &report)
    {
      auto iter = m_qualityTests.find(name);

      if(m_qualityTests.end() == iter)
        return STATUS_CODE_NOT_FOUND;

      iter->second->run(this->shared_from_this(), report);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    TDynamicGraph::TDynamicGraph() :
		    TGraph(),
		    m_rangeLength(-1.)
    {
      SetBit(kDynamicRange);
      SetBit(kShrinkToRange);
    }

    //-------------------------------------------------------------------------------------------------

    TDynamicGraph::~TDynamicGraph()
    = default;

    //-------------------------------------------------------------------------------------------------

    void TDynamicGraph::SetRangeLength(Double_t rangeLength)
    {
      m_rangeLength = rangeLength;
    }

    //-------------------------------------------------------------------------------------------------

    void TDynamicGraph::AddPoint(Double_t x, Double_t y)
    {
      Int_t nPoints = this->GetN();
      this->SetPoint(nPoints, x, y);
      this->ShrinkToRange();
    }

    //-------------------------------------------------------------------------------------------------

    void TDynamicGraph::Draw(Option_t *option)
    {
      TGraph::Draw(option);

      if( TestBit(kDynamicRange) )
      {
        Int_t nPoints = this->GetN();

        if( m_rangeLength <= 0 || nPoints == 0)
          return;

        Double_t lastX, lastY;
        Int_t res = this->GetPoint( nPoints-1 , lastX, lastY );

        this->GetXaxis()->SetRangeUser( lastX - m_rangeLength , lastX );

        if(gPad)
          gPad->Update();
      }
    }

    //-------------------------------------------------------------------------------------------------

    void TDynamicGraph::ShrinkToRange()
    {
      if( ! TestBit(kDynamicRange) || ! TestBit(kShrinkToRange) )
        return;

      Int_t nPoints = this->GetN();

      if( m_rangeLength <= 0 || nPoints == 0)
        return;

      Double_t lastX, lastY;
      Int_t res = this->GetPoint( nPoints-1 , lastX, lastY );

      if( res < 0 )
        return;

      Double_t xLimit = lastX - m_rangeLength;

      while(true)
      {
        Double_t x, y;
        res = this->GetPoint(0, x, y);

        if( res < 0 )
          break;

        if( x < xLimit )
          this->RemovePoint(0);
        else
          break;
      }
    }

  }

}
