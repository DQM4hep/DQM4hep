//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_MONITORELEMENTMANAGER_H
#define DQM4HEP_MONITORELEMENTMANAGER_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/QualityTest.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Storage.h>
#include <dqm4hep/Path.h>
#include <dqm4hep/XmlHelper.h>
#include <dqm4hep/AllocatorHelper.h>
#include <dqm4hep/RootHeaders.h>
#include <dqm4hep/Directory.h>
#include <dqm4hep/RootStyle.h>
#include <dqm4hep/Archiver.h>

namespace dqm4hep {

  namespace core {

    /** 
     *  @brief  MonitorElementManager class
     */
    class MonitorElementManager {
      typedef std::shared_ptr<QualityTest> QualityTestPtr;
      typedef std::shared_ptr<QualityTestFactory> QualityTestFactoryPtr;
      typedef std::map<const std::string, QualityTestPtr> QualityTestMap;
      typedef std::map<const std::string, const QualityTestFactoryPtr> QualityTestFactoryMap;
    public:
      /** 
       *  @brief  Constructor
       */
      MonitorElementManager();

      /** 
       *  @brief  Destructor
       */
      ~MonitorElementManager();

      /////////////////////////
      // DIRECTORY INTERFACE //
      /////////////////////////

      /** 
       *  @brief  Go back to root directory
       */
      void cd();

      /** 
       *  @brief  Go to specific directory. 
       *          If the directory name starts with with '/' the path is absolute 
       *          and refers to the root directory, else it refers to the current directory  
       *
       *  @param  dirName the directory name to enter 
       */
      StatusCode cd(const std::string &dirName);

      /** 
       *  @brief  Create a directory
       *          If the directory name starts with with '/' the path is absolute 
       *          and refers to the root directory, else it refers to the current directory
       *
       *  @param  dirName the directory name
       */
      StatusCode mkdir(const std::string &dirName);

      // void ls(bool recursive = false);

      /** 
       *  @brief  Get the current directory name
       */
      const std::string &pwd();

      /** 
       *  @brief  Get the current directory full name (parents included)
       */
      const std::string &fullPwd();

      /** 
       *  @brief  Navigate backward in the directory structure. 
       *          Equivalent of 'cd ..' in a shell.
       */
      StatusCode goUp();

      /** 
       *  @brief  Remove a directory
       *          If the directory name starts with with '/' the path is absolute 
       *          and refers to the root directory, else it refers to the current directory
       *
       *  @param  dirName the directory name 
       */
      StatusCode rmdir(const std::string &dirName);

      /** 
       *  @brief  Whether the directory exists or not.
       *          If the directory name starts with with '/' the path is absolute 
       *          and refers to the root directory, else it refers to the current directory
       *
       *  @param  dirName the directory name
       */
      bool dirExists(const std::string &dirName) const;

    public:
      ///////////////////////////////
      // MONITOR ELEMENT INTERFACE //
      ///////////////////////////////

      /** 
       *  @brief  Add a monitor element from an external source.
       *          WARNING : The ROOT object is owned by the framework.
       *          The caller must NOT delete the object
       *
       *  @param  path the monitor element path in the directory structure
       *  @param  object the ROOT object to wrap in a monitor element
       *  @param  monitorElement the monitor element pointer to receive
       */
      template <typename T>
      StatusCode addMonitorElement(const std::string &path, TObject *object, std::shared_ptr<T> &monitorElement);

      /** 
       *  @brief  Read TObject from file and add it to list.
       *          The ROOT TObject is owned by the manager.
       *          Note that this method might not be so efficient of called mutiple 
       *          times as a TFile is opened and closed for each call
       *
       *  @param  fileName the ROOT file name to read
       *  @param  path the path to the ROOT object to read and wrap in the monitor element
       *  @param  name the name of the ROOT object
       *  @param  monitorElement the monitor element to receive 
       */
      template <typename T>
      StatusCode readMonitorElement(const std::string &fileName, const std::string &path, const std::string &name,
                                    std::shared_ptr<T> &monitorElement);

      /** 
       *  @brief  Read TObject from file and add it to list.
       *          The ROOT TObject is owned by the manager.
       *
       *  @param  pTFile the ROOT file instance to read the monitor element from
       *  @param  path the TObject path in the file
       *  @param  name the TObject name in the file
       *  @param  monitorElement the monitor element instance to receive  
       */
      template <typename T>
      StatusCode readMonitorElement(TFile *pTFile, const std::string &path, const std::string &name,
                                    std::shared_ptr<T> &monitorElement);

      /** 
       *  @brief  Book a monitor element using the ROOT TClass facility.
       *          The className is passed to TClass::GetClass() to get the corresponding
       *          TClass object handler. Note that to allocate the TObject:
       *          - the default constructor is used
       *          - the ROOT object is not referenced anywhere in the ROOT framework (see TObject::SetObjectStat and TH1::AddDirectory)
       *          The resulting allocated TObject is thus fully owned by the manager
       *
       *  @param  className the ROOT class name (i.e "TGraph") to allocate
       *  @param  path the path where to store the monitor element
       *  @param  name the TObject name to set
       *  @param  monitorElement the monitor element pointer to receive 
       */
      template <typename T>
      StatusCode bookMonitorElement(const std::string &className, const std::string &path, const std::string &name,
                                    std::shared_ptr<T> &monitorElement);

      /** 
       *  @brief  Handle and wrap the TObject in a monitor element.
       *          WARNING : The ROOT object is NOT owned by the framework.
       *          The caller must delete the object on termination.
       *          You might use this function when the TObject is owned by another framework. 
       *
       *  @param  path the path where to store the monitor element
       *  @param  pObject the input ROOT TObject to wrap
       *  @param  monitorElement the monitor element pointer to receive 
       */
      template <typename T>
      StatusCode handleMonitorElement(const std::string &path, TObject *pObject, std::shared_ptr<T> &monitorElement);
      
      /**
       *  @brief  Create a custom ROOT object and wrap it into a monitor element. 
       *          WARNING: The root class must inherit form TNamed in order to set the name and title corectly.
       *          The created TObject is owned by the monitor element and must not be delete by users.
       *          The allocation is performed using the ROOT object constructor possibly with arguments.
       *          For histogram classes like TH1, TH2, TH3 and TProfile and daughters, use the bookHisto()
       *          method instead. For scalar types (int, double, ...) use the bookScalar() method instead.
       *
       *  Example:
       *  @code{.cpp}
       *  // Without arguments. Use the TGraph::TGraph() constructor
       *  MonitorElement graphElement = nullptr;
       *  mgr->bookObject<TGraph>("/MyDirectory", "SuperGraph", "A supeeeeer graph !", graphElement);
       *  // With 42 points. Use the constructor Use the TGraph::TGraph(Int_t n) constructor
       *  MonitorElement graphElement2 = nullptr;
       *  mgr->bookObject<TGraph>("/MyDirectory", "SuperGraph2", "A supeeeeer graph with n points set!", graphElement2, 42);
       *  @endcode
       *  
       *  @param  path the path where to store the monitor element
       *  @param  name the monitor element name
       *  @param  title the monitor element title
       *  @param  monitorElement the monitor element pointer to receive
       *  @param  args optional arguments to pass to the ROOT class constructor
       */
      template <typename ObjectType, typename T, typename... Args>
      StatusCode bookObject(const std::string &path, const std::string &name, const std::string &title, std::shared_ptr<T> &monitorElement, Args... args);
      
      /**
      *  @brief  Create a ROOT histogram and wrap it into a monitor element. 
      *          The created histogram is owned by the monitor element and must not be delete by users.
      *          The allocation is performed using the ROOT object constructor possibly with arguments.
      *          For scalar types (int, double, ...) use the bookScalar() method instead. For other custom 
      *          classes, use the bookObject() function. The last argument args are the arguments passed to 
      *          the histogram constructor after the "name" and "title" arguments.
      *
      *  Example:
      *  @code{.cpp}
      *  // Use the constructor TH1F::TH1F(const char *name, const char *title, Int_t nBins, Double_t min, Double_t max)
      *  MonitorElement histo1DElement = nullptr;
      *  mgr->bookHisto<TH1F>("/MyDirectory", "SuperHisto", "A supeeeeer histo !", histo1DElement, 10, 0., 9.);
      *  @endcode
      *  
      *  @param  path the path where to store the monitor element
      *  @param  name the monitor element name
      *  @param  title the monitor element title
      *  @param  monitorElement the monitor element pointer to receive
      *  @param  args optional arguments to pass to the histogram class after the name and title arguments
       */
      template <typename HistoType, typename T, typename... Args>
      StatusCode bookHisto(const std::string &path, const std::string &name, const std::string &title, std::shared_ptr<T> &monitorElement, Args... args);
      
      /**
      *  @brief  Create a TScalarObject<ScalarType> of type T and wrap it into a monitor element. 
      *          The created scalar object is owned by the monitor element and must not be delete by users.
      *          The allocation is performed using the TScalarObject<ScalarType> constructor possibly with arguments.
      *          For histogram classes like TH1, TH2, TH3 and TProfile and daughters, use the bookHisto()
      *          method instead. For other custom classes, use the bookObject() function. The last argument 
      *          args are the arguments passed to the TScalarObject<ScalarType> constructor.
      *
      *  Example:
      *  @code{.cpp}
      *  // Use the constructor TScalarObject<int>::TScalarObject<int>(int value)
      *  MonitorElement intScalarElement = nullptr;
      *  mgr->bookScalar<int>("/MyDirectory", "SuperScalar", "A supeeeeer scalar !", intScalarElement, 42);
      *  @endcode
      *  
      *  @param  path the path where to store the monitor element
      *  @param  name the monitor element name
      *  @param  title the monitor element title
      *  @param  monitorElement the monitor element pointer to receive
      *  @param  args optional arguments to pass to the TScalarObject class
       */
      template <typename ScalarType, typename T, typename... Args>
      StatusCode bookScalar(const std::string &path, const std::string &name, const std::string &title, std::shared_ptr<T> &monitorElement, Args... args);

      /**
       *  @brief  Find reference file using the refId, look for a reference object and attach it to the monitor element.
       *          The path and name of the reference must be the same as the monitor element
       *
       *  @param  monitorElement the monitor element to attach the reference
       *  @param  refId the reference file id
       */
      template <typename T>
      StatusCode attachReference(std::shared_ptr<T> monitorElement, const std::string &refId);

      /**
       *  @brief  Find reference file using the refId, look for a reference object and attach it to the monitor element.
       *
       *  @param  monitorElement the monitor element to attach the reference
       *  @param  refId the reference file id
       *  @param  path the path to reference object in file
       *  @param  name the name to reference object in file
       */
      template <typename T>
      StatusCode attachReference(std::shared_ptr<T> monitorElement, const std::string &refId, const std::string &path, const std::string &name);
      
      /**
       *  @brief  Write all monitor elements in the storage to json
       *  
       *  @param  object the json object to receive
       */
      void monitorElementsToJson(json &object) const;
      
      /**
       *  @brief  Add a reference file under the specified id
       *  
       *  @param  refId the reference file id
       *  @param  fname the reference file name
       */
      StatusCode addReferenceFile(const std::string &refId, const std::string &fname);
      
      /**
       *  @brief  Iterate over the monitor element directory structure and call the user callback function.
       *          The user function must have the form void f(std::shared_ptr<T> ptr), where T stand for the 
       *          monitor element type, i.e MonitorElement.
       *
       *  Example:
       *  @code{.cpp}
       *  // Use a lambda function
       *  mgr->iterate<MonitorElement>([](std::shared_ptr<MonitorElement> ptr){
       *    std::cout << "Got element: type: " << ptr->type() << ", name: " << ptr->name() << std::endl;
       *  });
       *  @endcode
       *
       *  @param  function a user of the form void f(std::shared_ptr<T> ptr);
       */
      template <typename T, typename Function>
      void iterate(Function function);
      
      /**
       *  @brief  Dump the monitor element storage in the console.
       *          Uses the macro dqm_info() for printing out
       */
      void dumpStorage();
      
      /**
       *  @brief  Parse the xml section describing:
       *          - monitor element style in <style>
       *          - reference files in <references>
       *          - quality tests configuration in <qtests>
       *          - monitor element to read/book in <monitorElements>
       *          The template type T reference to the monitor element type to create
       *          
       * @param  xmlElement the input xml element
       */
      template <typename T>
      StatusCode parseStorage(TiXmlElement *xmlElement);
      
      /**
       *  @brief  Archive the current monitor element content in a root file
       *
       *  @param  archiver the archiver performing the write operation
       *  @param  withReferences whether to write references with monitor elements
       */
      StatusCode archive(Archiver &archiver, bool withReferences = true);
      
    private:
      /**
       *  @brief  Read the style settings from the XML element
       * 
       *  @param  xmlElement the input XML element
       */
      StatusCode parseStyle(TiXmlElement *xmlElement);
      
      /**
       *  @brief  Book a monitor element from the xml element
       *  
       *  @param  xmlElement the xml element to parse
       *  @param  monitorelement the monitor element to receive
       */
      template <typename T>
      StatusCode bookMonitorElement(TiXmlElement *xmlElement, std::shared_ptr<T> &monitorElement);
      
      /**
       *  @brief  Read monitor element described by the xml element
       *
       *  @param  pXmlElement the xml element describing the monitor elements
       */
      template <typename T>
      StatusCode parseMonitorElements(TiXmlElement *xmlElement);
      
      /**
       *  @brief  Read monitor element described by the xml element from a root file
       *
       *  @param  pXmlElement the xml element describing the objects to read
       *  @param  monitorelement the monitor element to receive
       */
      template <typename T>
      StatusCode readMonitorElement(const std::string &fileName, TiXmlElement *xmlElement, std::shared_ptr<T> &monitorElement);
      
      /**
       *  @brief  Read reference files from XML element
       *          Feed a map of reference id (string) and reference file.
       *          No reference association is made.
       *  
       *  @param  element the XML element to read
       */
      StatusCode parseReferences(TiXmlElement *xmlElement);
      
      /**
       *  @brief  Read a single reference file the from XML element
       *          Feed the map of reference id (string) -> reference file.
       *          No reference association is made.
       *  
       *  @param  element the XML element to read
       */
      StatusCode parseReference(TiXmlElement *xmlElement);
      
      /** 
       *  @brief  Create quality tests from the xml element
       *
       *  @param  xmlElement the XML element to parse
       */
      StatusCode parseQualityTests(TiXmlElement *xmlElement);   

    public:
      /** 
       *  @brief  Get all the monitor elements already booked by this module in all the directories
       *
       *  @param  monitorElements the monitor elements to receive
       */
      template <typename T>
      void getMonitorElements(std::vector<std::shared_ptr<T>> &monitorElements) const;

      /** 
       *  @brief  Get the monitor element in the current directory (result by ptr reference)
       *
       *  @param  name the name of the monitor element
       *  @param  monitorElement the monitor element to receive
       */
      template <typename T>
      StatusCode getMonitorElement(const std::string &name, std::shared_ptr<T> &monitorElement) const;

      /** 
       *  @brief  Get the monitor element in the given directory (result by ptr reference)
       *
       *  @param  path the path to the monitor element to find
       *  @param  name the name of the monitor element to find
       *  @param  monitorElement the monitor element to receive
       */
      template <typename T>
      StatusCode getMonitorElement(const std::string &path, const std::string &name,
                                   std::shared_ptr<T> &monitorElement) const;
      
      /**
       *  @brief  Reset all monitor elements
       */
      void resetMonitorElements();

      /** 
       *  @brief  Remove a specific monitor element
       *
       *  @param  path the path to the monitor element
       *  @param  name the name of the monitor element
       */
      StatusCode removeMonitorElement(const std::string &path, const std::string &name);

    public:
      /** 
       *  @brief  Create a quality test from the xml element.
       *          The xml element must contain the attribute 'type' and 'name'
       *
       *  @param  pXmlElement the XML element to parse
       *  @param  warning the warning threshold level for this particular qtest (optional)
       *  @param  error the error threshold level for this particular qtest (optional)
       */
      StatusCode createQualityTest(TiXmlElement *const pXmlElement, float warning = QualityTest::defaultWarningLimit(), float error = QualityTest::defaultErrorLimit());

      /** 
       *  @brief  Attach an already created quality test to the monitor element
       *
       *  @param  path the path to the monitor element
       *  @param  name the name of the monitor element
       *  @param  qualityTestName the name of the quality test to attach
       */
      StatusCode addQualityTest(const std::string &path, const std::string &name, const std::string &qualityTestName);

      /** 
       *  @brief  Remove a quality test from the monitor element
       *
       *  @param  path the path to the monitor element
       *  @param  name the name of the monitor element
       *  @param  qualityTestName the name of the quality test to remove
       */
      StatusCode removeQualityTest(const std::string &path, const std::string &name,
                                   const std::string &qualityTestName);

      /**
       *  @brief  Run all quality tests for all monitor elements and receive qtest reports
       *
       *  @param  reports the quality test reports to receive
       */
      StatusCode runQualityTests(QReportStorage &reports);

      /**
       *  @brief  Run all quality tests attached to a particular monitor element
       *
       *  @param  path the path to the monitor element
       *  @param  name the name of the monitor element
       *  @param  reports the quality test reports to receive
       */
      StatusCode runQualityTests(const std::string &path, const std::string &name, QReportStorage &reports);

      /**
       *  @brief  Run a particular quality test on one monitor element
       *
       *  @param  path the path to the monitor element
       *  @param  name the name of the monitor element
       *  @param  qualityTestName the name of the quality test to run
       *  @param  reports the quality test reports to receive
       */
      StatusCode runQualityTest(const std::string &path, const std::string &name, const std::string &qualityTestName,
                                QReportStorage &reports);

    private:
      /**
       *  @brief  Check if the target class is compatible with the DQM4hep requirements.
       *          See implementation for details
       * 
       *  @param  cls the ROOT TClass to check
       */
      bool checkClass(TClass *cls);
      
      /**
       *  @brief  Add a monitor element to the internal storage
       *  
       *  @param  path the path where to add this monitor element
       *  @param  element the monitor element to add
       */
      StatusCode addToStorage(const std::string &path, MonitorElementPtr element);
      
      /**
       *  @brief  Create a monitor element and add it the internal storage
       *  
       *  @param  path the path where to add this monitor element
       *  @param  pObject the ROOT object pointer to wrap in the monitor element
       *  @param  monitorElement the monitor element to receive
       *  @param  owner whether the monitor element should owns the ROOT object
       */
      template <typename T>
      StatusCode createAndAddMonitorElement(const std::string &path, TObject *pObject, std::shared_ptr<T> &monitorElement, bool owner);

      /**
       *  @brief  Attach a reference, describe by the XML element, to the target monitor element
       *  
       *  @param  monitorElement the monitor element to which to attach the reference
       *  @param  xmlElement the XML element from which to extract the reference
       */
      template <typename T>
      StatusCode attachReference(std::shared_ptr<T> monitorElement, TiXmlElement *xmlElement);
      
      /**
       *  @brief  Performs the actual monitor element memory allocation.
       *          The actual type T should inherit the MonitorElement class and provides
       *          the same make_shared interface as the MonitorElement class (see MonitorElement.h)
       *  
       *  @param  args the arguments to forward to the allocator function
       */
      template <typename T, typename... Args>
      std::shared_ptr<T> createMonitorElement(Args ...args);
      
      /**
       *  @brief  Get the monitor element storage
       */
      const Storage<MonitorElement> &getStorage() const;
      
      /**
       *  @brief  Perform a user action after having disabling any ROOT framework
       *          memory handling such as the one implicitly done with TObject::GetObjectStat()
       *          or with TH1::AddDirectoryStatus(). The ROOT memory handling is restored after 
       *          the function has been called. The function must have the form 'void f()'.
       *  
       *  @param  function a user call back function
       */
      template <typename T>
      void doROOTNotOwner(T function);
      
    private:
      typedef std::shared_ptr<TObjectXMLAllocator> XMLAllocatorPtr;
      typedef std::map<MonitorElementPtr, QualityTestMap> MonitorElementToQTestMap;
      typedef std::map<std::string, XMLAllocatorPtr> XMLAllocatorMap;
      typedef std::map<std::string, std::shared_ptr<TFile>> ReferenceMap;

      /// The internal monitor element storage
      Storage<MonitorElement>      m_storage = {};
      /// The quality test factory map (plugins)
      QualityTestFactoryMap        m_qualityTestFactoryMap = {};
      /// The actual allocated quality test map
      QualityTestMap               m_qualityTestMap = {};
      /// The XML allocator map to create monitor elements from XML description
      XMLAllocatorMap              m_xmlAllocatorMap = {};
      /// The default XML allocator function
      XMLAllocatorPtr              m_defaultXMLAllocator = {nullptr};
      /// The current monitor element object style
      RootStyle                    m_objectStyle = {};
      /// The current monitor element reference style
      RootStyle                    m_referenceStyle = {};
      /// The map of reference ROOT files currently handled
      ReferenceMap                 m_references = {};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void MonitorElementManager::getMonitorElements(std::vector<std::shared_ptr<T>> &monitorElements) const {
      m_storage.getObjects(monitorElements);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementManager::getMonitorElement(const std::string &name,
                                                        std::shared_ptr<T> &monitorElement) const {
      monitorElement = std::dynamic_pointer_cast<T>(m_storage.findObject([&name](const MonitorElementPtr &elt) { return (elt->name() == name); }));

      if (nullptr == monitorElement)
        return STATUS_CODE_NOT_FOUND;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode MonitorElementManager::getMonitorElement(const std::string &dirName, const std::string &name,
                                                        std::shared_ptr<T> &monitorElement) const {
      monitorElement = std::dynamic_pointer_cast<T>(
          m_storage.findObject(dirName, [&name](const MonitorElementPtr &elt) { return (elt->name() == name); }));

      if (nullptr == monitorElement)
        return STATUS_CODE_NOT_FOUND;

      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename ObjectType, typename T, typename... Args>
    inline StatusCode MonitorElementManager::bookObject(const std::string &path, const std::string &name, const std::string &title, std::shared_ptr<T> &monitorElement, Args... args) {
      monitorElement = nullptr;
      if(not checkClass(ObjectType::Class())) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      TObject *pTObject = nullptr;
      doROOTNotOwner([&](){
        pTObject = new ObjectType(args...);
      });
      if (!pTObject) {
        dqm_warning("Couldn't allocate monitor element of type '{0}', path '{1}', name '{2}'", ObjectType::Class_Name(),
                    path, name);
        return STATUS_CODE_FAILURE;
      }
      ((TNamed *)pTObject)->SetNameTitle(name.c_str(), title.c_str());
      m_objectStyle.applyTo(pTObject);
      return addMonitorElement<T>(path, pTObject, monitorElement);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename HistoType, typename T, typename... Args>
    inline StatusCode MonitorElementManager::bookHisto(const std::string &path, const std::string &name, const std::string &title, std::shared_ptr<T> &monitorElement, Args... args) {
      monitorElement = nullptr;
      if(not checkClass(HistoType::Class())) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      TObject *pTObject = nullptr;
      doROOTNotOwner([&](){
        pTObject = new HistoType(name.c_str(), title.c_str(), args...);
      });
      if (!pTObject) {
        dqm_warning("Couldn't allocate monitor element of type '{0}', path '{1}', name '{2}'", HistoType::Class_Name(), path, name);
        return STATUS_CODE_FAILURE;
      }
      m_objectStyle.applyTo(pTObject);
      return addMonitorElement<T>(path, pTObject, monitorElement);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename ScalarType, typename T, typename... Args>
    inline StatusCode MonitorElementManager::bookScalar(const std::string &path, const std::string &name, const std::string &title, std::shared_ptr<T> &monitorElement, Args... args) {
      monitorElement = nullptr;
      if(not checkClass(TScalarObject<ScalarType>::Class())) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      TObject *pTObject = nullptr;
      doROOTNotOwner([&](){
        pTObject = new TScalarObject<ScalarType>(args...);
      });
      if (!pTObject) {
        dqm_warning("Couldn't allocate monitor element of type '{0}', path '{1}', name '{2}'", TScalarObject<ScalarType>::Class_Name(), path, name);
        return STATUS_CODE_FAILURE;
      }
      ((TNamed *)pTObject)->SetNameTitle(name.c_str(), title.c_str());
      m_objectStyle.applyTo(pTObject);
      return addMonitorElement<T>(path, pTObject, monitorElement);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T, typename... Args>
    inline std::shared_ptr<T> MonitorElementManager::createMonitorElement(Args ...args) {
      return T::make_shared(args...);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementManager::createAndAddMonitorElement(const std::string &path, TObject *pObject, std::shared_ptr<T> &monitorElement, bool owner) {
      PtrHandler<TObject> ptrObject(pObject, owner);
      monitorElement = nullptr;
      TClass *cls = TClass::GetClass(pObject->ClassName());
      if(not checkClass(cls)) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      monitorElement = createMonitorElement<T>(ptrObject);
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, addToStorage(path, monitorElement));
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    StatusCode MonitorElementManager::addMonitorElement(const std::string &path, TObject *pObject, std::shared_ptr<T> &monitorElement) {
      return createAndAddMonitorElement<T>(path, pObject, monitorElement, true);
    }
    
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode MonitorElementManager::handleMonitorElement(const std::string &path, TObject *pObject,
                                                           std::shared_ptr<T> &monitorElement) {
      return createAndAddMonitorElement<T>(path, pObject, monitorElement, false);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementManager::readMonitorElement(const std::string &fileName, const std::string &path,
                                                         const std::string &name, std::shared_ptr<T> &monitorElement) {
      monitorElement = nullptr;
      std::unique_ptr<TFile> rootFile(new TFile(fileName.c_str(), "READ"));
      return readMonitorElement<T>(rootFile.get(), path, name, monitorElement);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementManager::readMonitorElement(TFile *pTFile, const std::string &path, const std::string &name, std::shared_ptr<T> &monitorElement) {
      monitorElement = nullptr;
      Path fullName = path;
      fullName += name;
      const std::string queryPath = (path == "/") ? name : fullName.getPath();
      TObject *pTObject = nullptr;
      doROOTNotOwner([&](){
        auto local = pTFile->Get(queryPath.c_str());
        if(nullptr != local) {
          pTObject = local->Clone();
        }
      });
      if (pTObject == nullptr) {
        return STATUS_CODE_NOT_FOUND;
      }
      m_objectStyle.applyTo(pTObject);
      return addMonitorElement<T>(path, pTObject, monitorElement);
    }
    
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode MonitorElementManager::bookMonitorElement(const std::string &className, const std::string &path,
                                                         const std::string &name, std::shared_ptr<T> &monitorElement) {
      monitorElement = nullptr;
      TClass *pTClass = TClass::GetClass(className.c_str());
      if(not checkClass(pTClass)) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      TObject *pTObject = nullptr;
      doROOTNotOwner([&](){
        pTObject = (TObject *)pTClass->New();
      });
      if (pTObject == nullptr) {
        dqm_error("MonitorElementManager::bookMonitorElement: Couldn't allocate ROOT class '{0}' from TClass facility",
                  className);
        return STATUS_CODE_FAILURE;
      }
      ((TNamed *)pTObject)->SetName(name.c_str());
      m_objectStyle.applyTo(pTObject);
      return addMonitorElement<T>(path, pTObject, monitorElement);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementManager::bookMonitorElement(TiXmlElement *xmlElement, std::shared_ptr<T> &monitorElement) {
      if(nullptr == xmlElement) {
        return STATUS_CODE_INVALID_PTR;
      }
      std::string className, name, path, title;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(xmlElement, "type", className));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(xmlElement, "name", name));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(xmlElement, "path", path));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(xmlElement, "title", title));
      
      auto findIter = m_xmlAllocatorMap.find(className);
      XMLAllocatorPtr allocator = (m_xmlAllocatorMap.end() == findIter) ? m_defaultXMLAllocator : findIter->second;
      TObject *pTObject = nullptr;
      doROOTNotOwner([&](){
        pTObject = allocator->create(xmlElement);
      });
      if(nullptr == pTObject) {
        dqm_error( "Object of type '{0}' couldn't be created from xml element !", className );
        return STATUS_CODE_FAILURE;
      }
      dynamic_cast<TNamed *>(pTObject)->SetNameTitle(name.c_str(), title.c_str());
      m_objectStyle.applyTo(pTObject);
      return addMonitorElement<T>(path, pTObject, monitorElement);
    }
    
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode MonitorElementManager::attachReference(std::shared_ptr<T> monitorElement, const std::string &refId) {
      if (nullptr == monitorElement) {
        return STATUS_CODE_INVALID_PTR;
      }
      return attachReference(monitorElement, refId, monitorElement->path(), monitorElement->name());
    }
    
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode MonitorElementManager::attachReference(std::shared_ptr<T> monitorElement, const std::string &refId, const std::string &path, const std::string &name) {
      if (nullptr == monitorElement){
        return STATUS_CODE_INVALID_PTR;
      }
      Path fullName = path;
      fullName += name;
      dqm_debug("MonitorElementManager::attachReference: looking for element {0}", fullName.getPath());
      
      auto findIter = m_references.find(refId);
      if(m_references.end() == findIter) {
        dqm_error( "MonitorElementManager::attachReference: refId '{0}' not found !", refId );
        return STATUS_CODE_NOT_FOUND;
      }
      auto rootFile = findIter->second;

      const std::string queryPath = (path == "/") ? name : fullName.getPath();
      TObject *pTObject = nullptr;
      doROOTNotOwner([&](){
        auto local = rootFile->Get(queryPath.c_str());
        if(nullptr != local) {
          pTObject = local->Clone();
        }
      });
      if (pTObject == nullptr)
        return STATUS_CODE_NOT_FOUND;
      
      m_referenceStyle.applyTo(pTObject);
      monitorElement->setReferenceObject(pTObject);

      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementManager::parseMonitorElements(TiXmlElement *xmlElement) {
      if(nullptr == xmlElement) {
        dqm_warning( "MonitorElementManager::readMonitorElements: No monitor elements to read from XML" );
        return STATUS_CODE_SUCCESS;
      }
      for (TiXmlElement *child = xmlElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        std::shared_ptr<T> monitorElement;
        if(child->ValueStr() == "bookElement") {
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, bookMonitorElement<T>(child, monitorElement));
          auto referenceElement = child->FirstChildElement("reference");
          if(nullptr != referenceElement) {
            RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, attachReference(monitorElement, referenceElement));
          }
        }
        else if(child->ValueStr() == "fileElement") {
          std::string rootFile;
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(child, "file", rootFile));
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, readMonitorElement<T>(rootFile, child, monitorElement));
          auto referenceElement = child->FirstChildElement("reference");
          if(nullptr != referenceElement) {
            RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, attachReference(monitorElement, referenceElement));
          }
        }
        else if(child->ValueStr() == "file") {
          std::string rootFile;
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(child, "name", rootFile));
          for (TiXmlElement *child2 = child->FirstChildElement("fileElement"); child2 != nullptr; child2 = child2->NextSiblingElement("fileElement")) {
            // read sub-element
            std::shared_ptr<T> monitorElement2;
            RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, readMonitorElement<T>(rootFile, child2, monitorElement2));
            auto referenceElement = child2->FirstChildElement("reference");
            if(nullptr != referenceElement) {
              RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, attachReference(monitorElement2, referenceElement));
            }
            // attach quality tests if option set
            for (TiXmlElement *qtest = child2->FirstChildElement("qtest"); qtest != nullptr; qtest = qtest->NextSiblingElement("qtest")) {
              std::string qTestName;
              RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(qtest, "name", qTestName));
              RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, addQualityTest(monitorElement2->path(), monitorElement2->name(), qTestName));
            }          
          }
          continue;
        }
        else {
          continue;
        }
        // attach quality tests if option set
        for (TiXmlElement *qtest = child->FirstChildElement("qtest"); qtest != nullptr; qtest = qtest->NextSiblingElement("qtest")) {
          std::string qTestName;
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(qtest, "name", qTestName));
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, addQualityTest(monitorElement->path(), monitorElement->name(), qTestName));
        }          
      }
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementManager::readMonitorElement(const std::string &fileName, TiXmlElement *const pXmlElement, std::shared_ptr<T> &monitorElement) {
      std::string path, name;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", name));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(pXmlElement, "path", path));
      // read element from root file
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, readMonitorElement<T>(fileName, path, name, monitorElement));
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    void MonitorElementManager::doROOTNotOwner(T userFunction) {
      const bool objectStat(TObject::GetObjectStat());
      const bool directoryStatus(TH1::AddDirectoryStatus());
      TObject::SetObjectStat(false);
      TH1::AddDirectory(false);
      userFunction();
      TH1::AddDirectory(directoryStatus);
      TObject::SetObjectStat(objectStat);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T, typename Function>
    inline void MonitorElementManager::iterate(Function function) {
      m_storage.iterate([&](const Directory<MonitorElement>::DirectoryPtr &, MonitorElementPtr monitorElement){
        auto castPointer = std::dynamic_pointer_cast<T>(monitorElement);
        return function(castPointer);
      });
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    StatusCode MonitorElementManager::attachReference(std::shared_ptr<T> monitorElement, TiXmlElement *element) {
      std::string refId, path, name;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(element, "id", refId));
      auto pathCode = XmlHelper::getAttribute(element, "path", path);
      if(STATUS_CODE_NOT_FOUND == pathCode) {
        path = monitorElement->path();
      }
      else if(STATUS_CODE_SUCCESS != pathCode) {
        return pathCode;
      }
      auto nameCode = XmlHelper::getAttribute(element, "name", name);
      if(STATUS_CODE_NOT_FOUND == nameCode) {
        name = monitorElement->name();
      }
      else if(STATUS_CODE_SUCCESS != nameCode) {
        return nameCode;
      }      
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, attachReference(monitorElement, refId, path, name));
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    StatusCode MonitorElementManager::parseStorage(TiXmlElement *xmlElement) {
      if(nullptr == xmlElement) {
        dqm_error( "MonitorElementManager::parseStorage: Got nullptr as input xml element !" );
        return STATUS_CODE_INVALID_PTR;
      }
      // parse the monitor element style
      auto styleElement = xmlElement->FirstChildElement("style");
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, parseStyle(styleElement));
      // parse the references
      auto referencesElement = xmlElement->FirstChildElement("references");
      if(nullptr != referencesElement) {
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, parseReferences(referencesElement));
      }
      // parse the quality tests
      auto qtestsElement = xmlElement->FirstChildElement("qtests");
      if(nullptr != qtestsElement) {
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, parseQualityTests(qtestsElement));        
      }
      // parse the monitor elements
      auto monitorElementsElement = xmlElement->FirstChildElement("monitorElements");
      if(nullptr != monitorElementsElement) {
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, parseMonitorElements<T>(monitorElementsElement));        
      }
      return STATUS_CODE_SUCCESS;
    }
    
  }
  
}

#endif //  DQM4HEP_MONITORELEMENTMANAGER_H
