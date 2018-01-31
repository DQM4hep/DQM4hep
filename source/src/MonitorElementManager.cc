/// \file MonitorElementManager.cc
/*
 *
 * MonitorElementManager.cc source template automatically generated by a class generator
 * Creation date : lun. ao�t 17 2015
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
#include <dqm4hep/MonitorElementManager.h>
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/Storage.h>
#include <dqm4hep/Directory.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/QualityTest.h>

// -- root headers
#include <TH1F.h>
#include <TH1I.h>
#include <TH1S.h>
#include <TH1C.h>
#include <TH2F.h>
#include <TH2I.h>
#include <TH2S.h>
#include <TH2C.h>
#include <TH3F.h>
#include <TH3I.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TROOT.h>
#include <TClass.h>
#include <TObject.h>
#include <TNamed.h>
#include <TFile.h>

// -- std headers
#include <stdexcept>

namespace dqm4hep {

  namespace core {

    MonitorElementManager::MonitorElementManager() :
      m_storage()
    {
      PluginManager *pPluginMgr(PluginManager::instance());
      StringVector factoryNames(pPluginMgr->pluginNamesMatchingType<QualityTestFactory>());

      for(auto factoryName : factoryNames)
      {
        auto factory(pPluginMgr->create<QualityTestFactory>(factoryName));

        if(nullptr == factory)
        {
          dqm_error( "MonitorElementManager::MonitorElementManager: Couldn't create qtest factory '{0}'. This is normally not possible !", factoryName );
          continue;
        }

        m_qualityTestFactoryMap.insert(QualityTestFactoryMap::value_type(factoryName, factory));
      }
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElementManager::~MonitorElementManager()
    {
      m_qualityTestFactoryMap.clear();
      m_qualityTestMap.clear();
    }

    //-------------------------------------------------------------------------------------------------

    void MonitorElementManager::cd()
    {
      m_storage.cd();
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::cd(const std::string &dirName)
    {
      return m_storage.cd(dirName);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::mkdir(const std::string &dirName)
    {
      return m_storage.mkdir(dirName);
    }

    //-------------------------------------------------------------------------------------------------

    // void MonitorElementManager::ls(bool recursive)
    // {
    //   m_pMonitorElementStorage->ls(recursive);
    // }

    //-------------------------------------------------------------------------------------------------

    const std::string &MonitorElementManager::pwd()
    {
      return m_storage.pwd();
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &MonitorElementManager::fullPwd()
    {
      return m_storage.current()->fullPath().getPath();
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::goUp()
    {
      return m_storage.goUp();
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::rmdir(const std::string &dirName)
    {
      return m_storage.rmdir(dirName);
    }

    //-------------------------------------------------------------------------------------------------

    bool MonitorElementManager::dirExists(const std::string &dirName) const
    {
      return m_storage.dirExists(dirName);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::addMonitorElement(const std::string &path, TObject *pObject, MonitorElementPtr &monitorElement)
    {
      PtrHandler<TObject> ptrObject(pObject, true);
      monitorElement = nullptr;

      try
      {
        if(!pObject->Class()->HasDictionary())
        {
          dqm_error( "Couldn't book object of type '{0}', because this class has no dictionnary!", pObject->ClassName() );
          throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);
        }

        if(pObject->Class()->IsForeign())
        {
          dqm_error( "Couldn't book object of type '{0}', because this class is foreign ( does not have a Streamer method))!", pObject->ClassName() );
          throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);
        }

        if(!pObject->InheritsFrom("TNamed"))
        {
          dqm_error( "Couldn't book object of type '{0}', because this class doesn't inherit TNamed (required to call SetName())!", pObject->ClassName() );
          throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);
        }

        monitorElement = MonitorElement::make_shared(ptrObject);

        if(nullptr != m_storage.findObject(path, [&](MonitorElementPtr obj){ return obj->name() == pObject->GetName(); }) )
        {
          dqm_error( "Monitor element '{0}' in directory '{1}' already booked !", pObject->GetName(), path );
          throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
        }

        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_storage.add(path, monitorElement));
        monitorElement->setPath(path);
      }
      catch(StatusCodeException &e)
      {
        if(nullptr == monitorElement)
        {
          // TObject should have been owned by the MonitorElement
          // but creation failed, we need to delete it manually
          delete pObject;
        }

        return e.getStatusCode();
      }
      catch(...)
      {
        if(nullptr == monitorElement)
        {
          // TObject should have been owned by the MonitorElement
          // but creation failed, we need to delete it manually
          delete pObject;
        }

        return STATUS_CODE_FAILURE;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::handleMonitorElement(const std::string &path, TObject *pObject, MonitorElementPtr &monitorElement)
    {
      PtrHandler<TObject> ptrObject(pObject, false);
      monitorElement = MonitorElement::make_shared(ptrObject);
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_storage.add(path, monitorElement));
      monitorElement->setPath(path);
      
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::readMonitorElement(const std::string &fileName, const std::string &path, const std::string &name, MonitorElementPtr &monitorElement)
    {
      monitorElement = nullptr;
      std::unique_ptr<TFile> rootFile(new TFile(fileName.c_str(), "READ"));
      return this->readMonitorElement(rootFile.get(), path, name, monitorElement);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::readMonitorElement(TFile *pTFile, const std::string &path, const std::string &name, MonitorElementPtr &monitorElement)
    {
      monitorElement = nullptr;

      Path fullName = path;
      fullName += name;
      dqm_debug( "MonitorElementManager::readMonitorElement: looking for element {0}", fullName.getPath() );

      const bool objectStat(TObject::GetObjectStat());
      TObject::SetObjectStat(false);
      TObject *pTObject = pTFile->Get(fullName.getPath().c_str());
      TObject::SetObjectStat(objectStat);

      if(pTObject == nullptr)
        return STATUS_CODE_NOT_FOUND;

      return this->addMonitorElement(path, pTObject, monitorElement);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::bookMonitorElement(const std::string &className, const std::string &path, const std::string &name, MonitorElementPtr &monitorElement)
    {
      monitorElement = nullptr;
      TClass *pTClass = TClass::GetClass(className.c_str());

      if(!pTClass->IsTObject())
      {
        dqm_error( "MonitorElementManager::bookMonitorElement: ROOT class '{0}' does not inherit from TObject. Can only handle TObject object type", className );
        return STATUS_CODE_FAILURE;
      }

      const bool objectStat(TObject::GetObjectStat());
      TObject::SetObjectStat(false);
      TObject *pTObject = (TObject*)pTClass->New();
      TObject::SetObjectStat(objectStat);

      if(pTObject == nullptr)
      {
        dqm_error( "MonitorElementManager::bookMonitorElement: Couldn't allocate ROOT class '{0}' from TClass facility", className );
        return STATUS_CODE_FAILURE;
      }

      if(pTObject->InheritsFrom("TNamed"))
      {
        dqm_debug( "Set name of object {0} to '{1}'", (void*) pTObject, name );
        ((TNamed*)pTObject)->SetName(name.c_str());
      }

      return this->addMonitorElement(path, pTObject, monitorElement);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::attachReference(MonitorElementPtr monitorElement, const std::string &fileName)
    {
      if(nullptr == monitorElement)
        return STATUS_CODE_INVALID_PTR;

      Path fullName = monitorElement->path();
      fullName += monitorElement->name();
      dqm_debug( "MonitorElementManager::attachReference: looking for element {0}", fullName.getPath() );

      std::unique_ptr<TFile> rootFile(new TFile(fileName.c_str(), "READ"));

      const bool objectStat(TObject::GetObjectStat());
      TObject::SetObjectStat(false);
      TObject *pTObject(nullptr);

      if(monitorElement->path() == "/")
        pTObject = (TObject *) rootFile->Get(monitorElement->name().c_str());
      else
        pTObject = (TObject *) rootFile->Get(fullName.getPath().c_str());

      TObject::SetObjectStat(objectStat);

      if(pTObject == nullptr)
        return STATUS_CODE_NOT_FOUND;

      monitorElement->setReferenceObject(pTObject);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    void MonitorElementManager::getMonitorElements(MonitorElementList &monitorElements) const
    {
      m_storage.getObjects(monitorElements);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::getMonitorElement(const std::string &name, MonitorElementPtr &monitorElement) const
    {
      monitorElement = m_storage.findObject(
        [&name](const MonitorElementPtr &elt){
          return (elt->name() == name);
        });

      if(nullptr == monitorElement)
        return STATUS_CODE_NOT_FOUND;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::getMonitorElement(const std::string &dirName, const std::string &name, MonitorElementPtr &monitorElement) const
    {
      monitorElement = m_storage.findObject(dirName,
        [&name](const MonitorElementPtr &elt){
          return (elt->name() == name);
        });

      if(nullptr == monitorElement)
        return STATUS_CODE_NOT_FOUND;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::removeMonitorElement(const std::string &path, const std::string &name)
    {
      MonitorElementPtr monitorElement;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElement(path, name, monitorElement));

      // look in qtest map first
      auto iter = m_monitorElementToQTestMap.find(monitorElement);

      if(m_monitorElementToQTestMap.end() != iter)
        m_monitorElementToQTestMap.erase(iter);

      // remove the element form storage. Call delete operator
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_storage.remove(path, [&name](const MonitorElementPtr &monitorElement){
        return (monitorElement->name() == name);
      }));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::createQualityTest(TiXmlElement *const pXmlElement)
    {
      std::string name, type;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", name));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "type", type));

      auto findIter = m_qualityTestMap.find(name);

      if(m_qualityTestMap.end() != findIter)
        return STATUS_CODE_ALREADY_PRESENT;

      auto findFactoryIter = m_qualityTestFactoryMap.find(type);

      if(m_qualityTestFactoryMap.end() == findFactoryIter)
        return STATUS_CODE_NOT_FOUND;

      QualityTestPtr qualityTest = findFactoryIter->second->createQualityTest(name);

      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, qualityTest->readSettings(TiXmlHandle(pXmlElement)));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, qualityTest->init());

      if(!m_qualityTestMap.insert(QualityTestMap::value_type(name, qualityTest)).second)
        return STATUS_CODE_FAILURE;

      return STATUS_CODE_SUCCESS;
    }


    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::addQualityTest(const std::string &path, const std::string &name, const std::string &qualityTestName)
    {
      auto findIter = m_qualityTestMap.find(qualityTestName);

      if(m_qualityTestMap.end() == findIter)
        return STATUS_CODE_NOT_FOUND;

      MonitorElementPtr monitorElement;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElement(path, name, monitorElement));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElement->addQualityTest(findIter->second));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::removeQualityTest(const std::string &path, const std::string &name, const std::string &qualityTestName)
    {
      MonitorElementPtr monitorElement;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElement(path, name, monitorElement));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElement->removeQualityTest(qualityTestName));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::runQualityTests(QReportStorage &reports)
    {
      try
      {
        m_storage.iterate([&reports](const MonitorElementDir &, MonitorElementPtr monitorElement){
          QReportMap reportMap;
          THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElement->runQualityTests(reportMap));
          reports.addReports(reportMap);
          return true;
        });
      }
      catch(StatusCodeException &exception)
      {
        dqm_error( "Failed to process qtests: {0}", exception.toString() );
        return exception.getStatusCode();
      }
      catch(...)
      {
        dqm_error( "Failed to process qtests: Unknown exception" );
        return STATUS_CODE_FAILURE;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::runQualityTests(const std::string &path, const std::string &name, QReportStorage &reports)
    {
      MonitorElementPtr monitorElement;
      QReportMap reportMap;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElement(path, name, monitorElement));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElement->runQualityTests(reportMap));
      reports.addReports(reportMap);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementManager::runQualityTest(const std::string &path, const std::string &name, const std::string &qualityTestName, QReportStorage &reports)
    {
      MonitorElementPtr monitorElement;
      QReport report;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElement(path, name, monitorElement));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElement->runQualityTest(qualityTestName, report));
      reports.addReport(report);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    const Storage<MonitorElement> &MonitorElementManager::getStorage() const
    {
      return m_storage;
    }

  }

}
