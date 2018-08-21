//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_MODULE_H
#define DQM4HEP_MODULE_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/Run.h"
#include "dqm4hep/Event.h"
#include "dqm4hep/Version.h"
#include "dqm4hep/tinyxml.h"

namespace dqm4hep {

  namespace online {

    class ModuleApplication;
    class ModuleApi;
    struct EOCCondition;

    /** 
     *  @brief  Module base class.
     *          Base class for monitor element booking modules.
     *          See daughter classes for specific implementations.
     */
    class Module {
      friend class ModuleApplication;
      friend class ModuleApi;
    public:
      /** 
       *  @brief  Default constructor
       */
      Module() = default;
      Module(const Module&) = delete;
      Module& operator=(const Module&) = delete;

      /** 
       *  @brief  Destructor
       */
      virtual ~Module() {}

      /** 
       *  @brief  Get the module name
       */
      const std::string &name() const;

      /** 
       *  @brief  Get the detector name
       */
      const std::string &detectorName() const;

      /** 
       *  @brief  Get the module version
       */
      const core::Version &version() const;

      /** 
      *  @brief  Initialize the module.
      *          In this method, user should book monitor elements.
      *          See ModuleApi class.
       */
      virtual void initModule() = 0;

      /** 
       *  @brief  Read the user settings from the xml handle
       *
       *  @brief  handle the xml handle
       */
      virtual void readSettings(const core::TiXmlHandle &handle) = 0;
      
      /**
       *  @brief  Callback function on start of run.
       *          User can get run configuration from the run object
       *
       *  @param  run the run configuration
       */
      virtual void startOfRun(core::Run &run) = 0;

      /** 
       *  @brief  Callback function on start of cycle.
       *          User can take appropriate actions such as monitor element reset, etc ...
       */
      virtual void startOfCycle() = 0;

      /** 
       *  @brief  Callback function on end of cycle
       *          The monitor elements are send to the collector after calling this function
       *
       *  @param  condition the end of cycle conditions
       */
      virtual void endOfCycle(const EOCCondition &condition) = 0;
      
      /**
       *  @brief  Callback function on end of run.
       *          User can get final run configuration from the run object
       *
       *  @param  run the run configuration
       */
      virtual void endOfRun(const core::Run &run) = 0;

      /** 
       *  @brief  Callback function before terminate
       */
      virtual void endModule() = 0;

    protected:
      /** 
       *  @brief  Set the detector name
       *
       *  @param  detectorName the detector name
       */
      void setDetectorName(const std::string &detector);

      /** 
       *  @brief  Set the module name
       *
       *  @brief  name the module name
       */
      void setName(const std::string &module);

      /** 
       *  @brief  Set the module version
       *
       *  @param  major the major version
       *  @param  minor the minor version
       *  @param  patch the patch number
       */
      void setVersion(unsigned int major, unsigned int minor = 0, unsigned int patch = 0);

    private:
      /** 
       *  @brief  Get the module application to which the module is associated
       */
      ModuleApplication *moduleApplication() const;
      
      /** 
       *  @brief  Set the module application to which the module is associated
       *
       *  @param  application the module application
       */
      void setModuleApplication(ModuleApplication *application);

    private:
      /// The module name
      std::string                   m_name = {""};
      /// The detector name for this module
      std::string                   m_detectorName = {""};
      /// The module version
      core::Version                 m_version = {};
      /// The module application instance
      ModuleApplication            *m_moduleApplication = {nullptr};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief ModuleT template class
     *         Template parameter T is the type of item to process in the 
     *         user callback function process()
     */
    template <typename T>
    class ModuleT : public Module {
    public:
      /** 
       *  @brief  Default constructor
       */
      ModuleT() = default;
      ModuleT(const ModuleT&) = delete;
      ModuleT& operator=(const ModuleT&) = delete;

      /** 
       *  @brief  Destructor
       */
      virtual ~ModuleT() {}
      
      /**
       *  @brief  Callback function to process an item
       *
       *  @param  item the item to process
       */
      virtual void process(T item) = 0;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief ModuleT template class. Specialization for void type (no argument)
     */
    template <>
    class ModuleT<void> : public Module {
    public:
      /** 
       *  @brief  Default constructor
       */
      ModuleT() = default;
      ModuleT(const ModuleT&) = delete;
      ModuleT& operator=(const ModuleT&) = delete;

      /** 
       *  @brief  Destructor
       */
      virtual ~ModuleT() {}
      
      /**
       *  @brief  Callback function. Fully user defined
       */
      virtual void process() = 0;
    };
    
    using ModulePtr = std::shared_ptr<Module>;
    using AnalysisModule = ModuleT<core::EventPtr>;
    using StandaloneModule = ModuleT<void>;
  }

} 

#endif  //  DQM4HEP_MODULE_H
