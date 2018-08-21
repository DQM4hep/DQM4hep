//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_MONITORELEMENTSTREAMER_H
#define DQM4HEP_MONITORELEMENTSTREAMER_H

// -- dqm4hep headers
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/QualityTest.h>
#include <dqm4hep/Logging.h>

// -- root headers
#include <TBuffer.h>

namespace dqm4hep {

  namespace core {
    
    /**
     *  @brief  MonitorElementStreamer class
     */
    class MonitorElementStreamer {
    public:
      /**
       *  @brief  Default constructor
       */
      MonitorElementStreamer() = default;
      
      /**
       *  @brief  Default destructor
       */
      ~MonitorElementStreamer() = default;
      
      /**
       *  @brief  Write a monitor element using an xdrstream device.
       *          
       *  @param  monitorElement a shared pointer on a monitor element or sub-class of it
       *  @param  device the xdrstream device to write with
       */
      template <typename T>
      StatusCode writeMonitorElement(std::shared_ptr<T> monitorElement, TBuffer &buffer);
      
      /**
       *  @brief  Read a monitor element using an xdrstream device.
       *          
       *  @param  monitorElement a shared pointer on a monitor element or sub-class of it
       *  @param  device the xdrstream device to read with
       */
      template <typename T>
      StatusCode readMonitorElement(std::shared_ptr<T> &monitorElement, TBuffer &buffer);
      
      /**
       *  @brief  Write a list of monitor elements using an xdrstream device.
       * 
       *  @param  elements a vector of monitor elements to write
       *  @param  device the xdrstream device to write with
       */
      template <typename T>
      StatusCode writeMonitorElements(const std::vector<std::shared_ptr<T>> &elements, TBuffer &buffer);
      
      /**
       *  @brief  Read a list of monitor elements using an xdrstream device.
       * 
       *  @param  elements a vector of monitor elements to receive
       *  @param  device the xdrstream device to read with
       */
      template <typename T>
      StatusCode readMonitorElements(std::vector<std::shared_ptr<T>> &elements, TBuffer &buffer);
      
      /**
       *  @brief  Write a map of monitor elements using an xdrstream device.
       * 
       *  @param  elements a map of monitor element vector to write
       *  @param  device the xdrstream device to write with
       */
      template <typename T>
      StatusCode writeMonitorElements(const std::map<std::string,std::vector<std::shared_ptr<T>>> &elements, TBuffer &buffer);
      
      /**
       *  @brief  Read a map of monitor elements using an xdrstream device.
       * 
       *  @param  elements a map of monitor element vector to receive
       *  @param  device the xdrstream device to read with
       */
      template <typename T>
      StatusCode readMonitorElements(std::map<std::string,std::vector<std::shared_ptr<T>>> &elements, TBuffer &buffer);
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementStreamer::writeMonitorElement(std::shared_ptr<T> monitorElement, TBuffer &buffer) {
      if(nullptr == monitorElement) {
        dqm_error( "Couldn't write monitor element to device: nullptr !" );
        return STATUS_CODE_INVALID_PTR;
      }
      return monitorElement->write(buffer);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementStreamer::readMonitorElement(std::shared_ptr<T> &monitorElement, TBuffer &buffer) {
      std::shared_ptr<T> tmp = T::make_shared();
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, tmp->read(buffer));
      monitorElement = tmp;
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementStreamer::writeMonitorElements(const std::vector<std::shared_ptr<T>> &elements, TBuffer &buffer) {
      buffer.WriteInt(elements.size());
      for(auto element : elements) {
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, writeMonitorElement(element, buffer));
      }
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode MonitorElementStreamer::readMonitorElements(std::vector<std::shared_ptr<T>> &elements, TBuffer &buffer) {
      Int_t nElements = 0;
      buffer.ReadInt(nElements);
      for(unsigned int i=0 ; i<nElements ; ++i) {
        std::shared_ptr<T> element;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, readMonitorElement(element, buffer));
        elements.push_back(element);
      }
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementStreamer::writeMonitorElements(const std::map<std::string,std::vector<std::shared_ptr<T>>> &elements, TBuffer &buffer) {
      buffer.WriteInt(elements.size());
      for(auto &iter : elements) {
        buffer.WriteStdString(&iter.first);
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, writeMonitorElements(iter.second, buffer));
      }
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StatusCode MonitorElementStreamer::readMonitorElements(std::map<std::string,std::vector<std::shared_ptr<T>>> &elementMap, TBuffer &buffer) {
      Int_t nElements = 0;
      buffer.ReadInt(nElements);
      for(unsigned int i=0 ; i<nElements ; ++i) {
        std::string key;
        buffer.ReadStdString(&key);
        std::vector<std::shared_ptr<T>> elements;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, readMonitorElements(elements, buffer));
        elementMap[key] = elements;
      }
      return STATUS_CODE_SUCCESS;
    }
    

  }
  
}

#endif //  DQM4HEP_MONITORELEMENTSTREAMER_H
