//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_ONLINEELEMENT_H
#define DQM4HEP_ONLINEELEMENT_H

#include <dqm4hep/MonitorElement.h>

namespace dqm4hep {
  
  namespace online {

    class ModuleApi;
    class ModuleApplication;
    class OnlineElement;
    typedef std::shared_ptr<OnlineElement> OnlineElementPtr;
    typedef std::vector<OnlineElementPtr> OnlineElementPtrList;

    /** 
     *  @brief  OnlineElement class
     */ 
    class OnlineElement : public core::MonitorElement {
      friend class ModuleApi;
      friend class ModuleApplication;
    public:
      /** 
       *  @brief  Make a shared pointer of OnlineElement
       */
      static OnlineElementPtr make_shared();

      /** 
       *  @brief  Make a shared pointer of OnlineElement
       *
       *  @param  pMonitorObject a valid ROOT object
       */
      static OnlineElementPtr make_shared(TObject *pMonitorObject);

      /** 
       *  @brief  Make a shared pointer of OnlineElement
       *
       *  @param  pMonitorObject a valid ROOT object
       *  @param  pReferenceObject a valid ROOT object as reference
       */
      static OnlineElementPtr make_shared(TObject *pMonitorObject, TObject *pReferenceObject);

      /** 
       *  @brief  Make a shared pointer of OnlineElement
       *
       *  @param  monitorObject a valid ROOT object pointer
       */
      static OnlineElementPtr make_shared(const core::PtrHandler<TObject> &monitorObject);

      /**
       *  @brief  Make a shared pointer of OnlineElement
       *
       *  @param  monitorObject a valid ROOT object pointer 
       *  @param  referenceObject a valid ROOT object pointer as reference
       */
      static OnlineElementPtr make_shared(const core::PtrHandler<TObject> &monitorObject,
                                                 const core::PtrHandler<TObject> &referenceObject);

      /**
       *  @brief  Destructor 
       */
      virtual ~OnlineElement() {}
      
      /**
       *  @brief  Set the run number
       * 
       *  @param runNum the run number
       */
      void setRunNumber(int runNum);
      
      /**
       *  @brief  Get the run number
       */
      int runNumber() const;
        
      /**
       *  @brief  Get the collector name.
       *          This property is set only if the element was collected
       */
      const std::string &collectorName() const;
      
      /**
       *  @brief  Get the module name that have booked this element
       */
      const std::string &moduleName() const;
      
      /**
       *  @brief  Set the description
       * 
       *  @param  desc the element description
       */
      void setDescription(const std::string &desc);
      
      /**
       *  @brief  Get the description
       */
      const std::string &description() const;
      
      /**
       *  @brief  Set whether to monitor element has to be published at end of cycle (user flag)
       *  
       *  @param  publish whether to publish it
       */
      void setPublish(bool publish);
      
      /**
       *  @brief  Whether the monitor element has to be published
       */
      bool publish() const;
      
      /**
       *  @brief  Whether the element has been subscribed by a shifter (runtime flag)
       */
      bool subscribed() const;
      
      /**
       *  @brief  Reset the monitor element
       *
       *  @param  resetQtests whether to also reset the quality tests 
       */
      virtual void reset(bool resetQtests = true) override;

      /**
       *  @brief  Convert the monitor element to json
       *  
       *  @param  object the json object to receive
       */
      virtual void toJson(core::json &object) const override;
      
// #if ROOT_VERSION_CODE >= ROOT_VERSION(6, 14, 0)
//       /**
//        *  @brief  Parse the json object and set monitor element properties
//        *  
//        *  @param  object the json object to parse
//        */
//       virtual void fromJson(const core::json &value);
// #endif
      
      /**
       *  @brief  Write monitor element to device
       *  
       *  @param  device the device to write to
       */
      virtual core::StatusCode write(TBuffer &buffer) const override;
      
      /**
       *  @brief  Read the monitor element from device
       * 
       *  @param  device the device to read from
       */
      virtual core::StatusCode read(TBuffer &buffer) override;
     
    protected:
      OnlineElement();
      OnlineElement(TObject *pMonitorObject);
      OnlineElement(TObject *pMonitorObject, TObject *pReferenceObject);
      OnlineElement(const core::PtrHandler<TObject> &monitorObject);
      OnlineElement(const core::PtrHandler<TObject> &monitorObject, const core::PtrHandler<TObject> &referenceObject);
     
      /**
       *  @brief  Set the collector name.
       *          Called by a monitor element collector on collect
       *
       *  @param  colName the collector name
       */
      void setCollectorName(const std::string &colName);
     
      /**
       *  @brief  Set the module name.
       *          Called by the module application on element booking
       *
       *  @param  modName the module name
       */
      void setModuleName(const std::string &modName);
      
      /**
       *  @brief  Set the element as subscribed
       *  
       *  @param  sub the subscription flag 
       */
      void setSubscribed(bool sub);
      
      /** 
       *  @brief  Run all quality tests
       *
       *  @param  reports the list of quality test reports to receive
       */
      core::StatusCode runQualityTests(core::QReportMap &reports) override;

      /** 
       *  @brief  Run a specific quality test
       *
       *  @param  name the quality test name to run
       *  @param  report the quality test report to receive
       */
      core::StatusCode runQualityTest(const std::string &name, core::QReport &report) override;

    private:
      /// The run number
      int                           m_runNumber = {0};
      /// The monitor element collector
      std::string                   m_collectorName = {""};
      /// The module name that have produced it
      std::string                   m_moduleName = {""};
      /// The monitor element description
      std::string                   m_description = {""};
      /// The quality test reports obtained at end of cycle
      core::QReportMap              m_reports = {};
      /// Whether to publish the element at end of cycle (user flag)
      bool                          m_publish = {true};
      /// Whether a shifter has subscribed to this element
      bool                          m_subscribed = {false};
    }; 

  }
  
} 

#endif  //  DQM4HEP_ONLINEELEMENT_H
