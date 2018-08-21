//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_MODULEAPI_H
#define DQM4HEP_MODULEAPI_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/Module.h"
#include "dqm4hep/OnlineElement.h"
#include "dqm4hep/QualityTest.h"
#include "dqm4hep/MonitorElementManager.h"
#include "dqm4hep/ModuleApplication.h"
#include "dqm4hep/XmlHelper.h"
#include "dqm4hep/RootHeaders.h"

namespace dqm4hep {

  namespace online {

    /** 
     *  @brief ModuleApi class.
     *
     *  Responsible for direct operation on module application and modules such as monitor
     *  element booking and directory navigation. Monitor element booking should be done via
     *  a Module
     */
    class ModuleApi {
    public:

      /////////////////////////
      // DIRECTORY INTERFACE //
      /////////////////////////

      /** 
       *  @brief  Go back to root directory
       *
       *  @param  module the user module calling this method
       */
      static core::StatusCode cd(const Module *const module);

      /** 
       *  @brief  Go to a specific directory
       *
       *  @param  module the user module calling this method
       *  @param  dirName the directory to enter
       */
      static core::StatusCode cd(const Module *const module, const std::string &dirName);

      /** 
       *  @brief  Create the directory 'dirName'
       *
       *  @param  module the user module calling this method
       *  @param  dirName the directory name to create
       */
      static core::StatusCode mkdir(const Module *const module, const std::string &dirName);
      
      /** 
       *  @brief  Get the current directory name
       *
       *  @param  module the user module calling this method
       */
      static const std::string &pwd(const Module *const module);

      /** 
       *  @brief  Navigate backward in the directory structure
       *
       *  @param  module the user module calling this method
       */
      static core::StatusCode goUp(const Module *const module);

      /** 
       *  @brief  Remove the directory 'dirName'
       *
       *  @param  module the user module calling this method
       *  @param  dirName the directory name to remove
       */
      static core::StatusCode rmdir(const Module *const module, const std::string &dirName);

      /** 
       *  @brief  Whether the directory exists
       *
       *  @param  module the user module calling this method
       *  @param  dirName the directory name to check
       */
      static bool dirExists(const Module *const module, const std::string &dirName);
      
      /**
       *  @brief  Dump the monitor element storage to console
       */
      static core::StatusCode dump(const Module *const module);

    public:

      ///////////////////////
      // BOOKING INTERFACE //
      ///////////////////////

      /**
       *  @brief  Book a monitor element with a custom ROOT object. For histograms and scalars see below.
       *
       *  The arguments 'args' are directly passed to the object constructor, so that you have simple cases without arguments such as:
       *  @code
       *  OnlineElementPtr graphElement;
       *  ModuleApi::bookObject<TGraph>(this, graphElement, "/", "MyGraph", "A title");
       *  @endcode
       *  or more complicated cases with custom arguments such as:
       *  @code
       *  const Int_t nPoints = 5;
       *  Float_t x[] = {0, 2, 4, 9, 15};
       *  Float_t y[] = {4, 12, 7, 14, 75};
       *  OnlineElementPtr graphElement;
       *  ModuleApi::bookObject<TGraph>(this, graphElement, "/", "MyGraph", "A title", nPoints, x, y);
       *  @endcode
       *  
       *  @param  module the user module calling this method
       *  @param  monitorElement the monitor element to receive
       *  @param  path the monitor element directory
       *  @param  name the monitor element name
       *  @param  title the monitor element title
       *  @param  args the arguments to pass in the object constructor (optional)
       */
      template <typename ObjectType, typename... Args>
      static core::StatusCode bookObject(const Module *const module, OnlineElementPtr &monitorElement, const std::string &path, const std::string &name, const std::string &title, Args... args);
      
      /**
       *  @brief  Book a monitor element with a ROOT histogram handled (anything inheriting from TH1). 
       *  
       *  The arguments 'args' are the ones passed in the TH1 constructors after the name and title.
       *  For example:
       *  @code
       *  OnlineElementPtr element;
       *  ModuleApi::bookHisto<TH1F>(this, element, "/", "MyHisto", "A title", 100, 0, 99);
       *  @endcode
       *  books a TH1F object with 100 bins from 0 to 99
       *  
       *  @param  module the user module calling this method
       *  @param  monitorElement the monitor element to receive
       *  @param  path the monitor element directory
       *  @param  name the monitor element name
       *  @param  title the monitor element title
       *  @param  args the arguments to pass in the object constructor (optional)
       */
      template <typename HistoType, typename... Args>
      static core::StatusCode bookHisto(const Module *const module, OnlineElementPtr &monitorElement, const std::string &path, const std::string &name, const std::string &title, Args... args);
      
      /**
       *  @brief  Book a monitor element handling a scalar value (the real type is TScalarObject<T>).
       *
       *  The template parameter ScalarType must be a C++ builtin type like in, float, etc ...
       *  For example:
       *  @code
       *  OnlineElementPtr element;
       *  ModuleApi::bookScalar<int>(this, element, "/", "MyScalar", "The answer", 42);
       *  @endcode
       *  books a TScalarInt object with an initial value of 42.
       *  
       *  @param  module the user module calling this method
       *  @param  monitorElement the monitor element to receive
       *  @param  path the monitor element directory
       *  @param  name the monitor element name
       *  @param  title the monitor element title
       *  @param  args the arguments to pass in the object constructor (optional)
       */
      template <typename ScalarType, typename... Args>
      static core::StatusCode bookScalar(const Module *const module, OnlineElementPtr &monitorElement, const std::string &path, const std::string &name, const std::string &title, Args... args);
      
    public:
      ///////////////////////
      // GETTERS INTERFACE //
      ///////////////////////

      /** 
       *  @brief  Get all the monitor elements already booked by this module in all the directories
       *
       *  @param  module the user module calling this method
       *  @param  monitorElementList the monitor element list to receive
       */
      static core::StatusCode getMonitorElements(const Module *const module, OnlineElementPtrList &monitorElementList);

      /** 
       *  @brief  Get the monitor element in the current directory (result by ptr reference)
       *
       *  @param  module the user module calling this method
       *  @param  monitorElementName the monitor element name
       *  @param  monitorElement the monitor element to receive
       */
      static core::StatusCode getMonitorElement(const Module *const module, const std::string &monitorElementName, OnlineElementPtr &monitorElement);

      /** 
       *  @brief  Get the monitor element in the given directory (result by ptr reference)
       *
       *  @param  module the user module calling this method
       *  @param  dirName the monitor element path
       *  @param  monitorElementName the monitor element name
       *  @param  monitorElement the monitor element to receive
       */
      static core::StatusCode getMonitorElement(const Module *const module, const std::string &dirName, const std::string &monitorElementName, OnlineElementPtr &monitorElement);

      /** 
       *  @brief  Get the monitor element in the current directory.
       *
       *  @param  module the user module calling this method
       *  @param  monitorElementName the monitor element name
       */
      static OnlineElementPtr getMonitorElement(const Module *const module, const std::string &monitorElementName);

      /** 
       *  @brief  Get the monitor element in the given directory.
       *  The element is directly returned without any ptr check
       *
       *  @param  module the user module calling this method
       *  @param  dirName the monitor element path
       *  @param  monitorElementName the monitor element name
       */
      static OnlineElementPtr getMonitorElement(const Module *const module, const std::string &dirName, const std::string &monitorElementName);

    public:
      ////////////////////////
      // UITILITY FUNCTIONS //
      ////////////////////////

      /** 
       *  @brief  Reset all the monitor elements of this module
       *
       *  @param  module the user module calling this method
       */
      static core::StatusCode resetMonitorElements(const Module *const module);
      
      /**
       *  @brief  Add a pre-booked quality test to a particular monitor element  
       *  
       *  @param  module the user module calling this method
       *  @param  monitorElement the monitor element to attach the quality test
       *  @param  qualityTestName the quality test name to attach
       */
      static core::StatusCode addQualityTest(const Module *const module, OnlineElementPtr monitorElement, const std::string &qualityTestName);

    private:
      /**
       *  @brief  Check module validity
       *
       *  @param  module the user module calling this method
       */
      static core::StatusCode checkModule(const Module *const module);
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename ObjectType, typename... Args>
    inline core::StatusCode  ModuleApi::bookObject(const Module *const module, OnlineElementPtr &monitorElement, const std::string &path, const std::string &name, const std::string &title, Args... args) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      if(not module->moduleApplication()->allowBooking()) {
        dqm_error( "Module application allows monitor element booking only in readSettings() or init() functions !" );
        return core::STATUS_CODE_NOT_ALLOWED;
      }
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, module->moduleApplication()->monitorElementManager()->bookObject<ObjectType>(path, name, title, monitorElement, args...));
      monitorElement->setModuleName(module->name());
      return core::STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename HistoType, typename... Args>
    inline core::StatusCode  ModuleApi::bookHisto(const Module *const module, OnlineElementPtr &monitorElement, const std::string &path, const std::string &name, const std::string &title, Args... args) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      if(not module->moduleApplication()->allowBooking()) {
        dqm_error( "Module application allows monitor element booking only in readSettings() or init() functions !" );
        return core::STATUS_CODE_NOT_ALLOWED;
      }
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, module->moduleApplication()->monitorElementManager()->bookHisto<HistoType>(path, name, title, monitorElement, args...));
      monitorElement->setModuleName(module->name());
      return core::STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename ScalarType, typename... Args>
    inline core::StatusCode  ModuleApi::bookScalar(const Module *const module, OnlineElementPtr &monitorElement, const std::string &path, const std::string &name, const std::string &title, Args... args) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      if(not module->moduleApplication()->allowBooking()) {
        dqm_error( "Module application allows monitor element booking only in readSettings() or init() functions !" );
        return core::STATUS_CODE_NOT_ALLOWED;
      }
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, module->moduleApplication()->monitorElementManager()->bookScalar<ScalarType>(path, name, title, monitorElement, args...));
      monitorElement->setModuleName(module->name());
      return core::STATUS_CODE_SUCCESS;
    }

  }

} 

#endif  //  DQM4HEP_MODULEAPI_H
