//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_XMLHELPER_H
#define DQM4HEP_XMLHELPER_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/tinyxml.h>

namespace dqm4hep {

  namespace core {

    /**
     *  @brief  XmlHelper class
     */
    class XmlHelper {
    public:
      /**
       *  @brief  Read a value from an xml handle
       *
       *  @param  xmlHandle the xml handle
       *  @param  xmlElementName the xml element name
       *  @param  t the value to receive
       */
      template <typename T>
      static StatusCode readValue(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t);

      /**
       *  @brief  Read a vector of values from a (space separated) list in an xml handle
       *
       *  @param  xmlHandle the xml handle
       *  @param  xmlElementName the xml element name
       *  @param  vector the vector of values to receive
       */
      template <typename T>
      static StatusCode readVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName,
                                           std::vector<T> &vector);

      /**
       *  @brief  Read a two-dimensional array of values into a vector of vectors. Each row of values must be contained
       *          within <rowname></rowname> xml tags, whilst the values in the row must be space separated
       *
       *  @param  xmlHandle the xml handle
       *  @param  xmlElementName the xml element name
       *  @param  rowName the element tag of each row
       *  @param  vector the vector of values to receive (matrix like)
       */
      template <typename T>
      static StatusCode read2DVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName,
                                             const std::string &rowName, std::vector<std::vector<T>> &vector);

      /**
       *  @brief  Get the attribute of the xml element
       *
       *  @param  pXmlElement the xml element
       *  @param  attributeName the attribute name
       *  @param  attributeValue the attribute value to receive
       */
      template <typename T>
      static StatusCode getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName,
                                     T &attributeValue);

      /**
       *  @brief  Get the attribute of the xml element and use a predicate to validate the value
       *
       *  @param  pXmlElement the xml element
       *  @param  attributeName the attribute name
       *  @param  attributeValue the attribute value to receive
       *  @param  validator a unary predicate to validate the attribute value
       */
      template <typename T, typename Validator>
      static StatusCode getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName,
                                     T &attributeValue, Validator validator);

      /**
       *  @brief  Read a parameter value from an xml element.
       *          Looks for a xml element <parameter> with the attributes 'name' (mandatory) and 'value'.
       *          If the attribute 'value' is not found the inner text in the element is use instead.
       *
       *  @param  xmlHandle the xml handle
       *  @param  parameterName the parameter name
       *  @param  t the parameter value to receive
       */
      template <typename T>
      static StatusCode readParameter(const TiXmlHandle &xmlHandle, const std::string &parameterName, T &t);

      /**
       *  @brief  Read a parameter value from an xml element and use a predicate to validate the value
       *          Looks for a xml element <parameter> with the attributes 'name' (mandatory) and 'value'.
       *          If the attribute 'value' is not found the inner text in the element is use instead.
       *
       *  @param  xmlHandle the xml handle
       *  @param  parameterName the parameter name
       *  @param  t the parameter value to receive
       *  @param  validator a unary predicate to validate the parameter value
       */
      template <typename T, typename Validator>
      static StatusCode readParameter(const TiXmlHandle &xmlHandle, const std::string &parameterName, T &t,
                                      Validator validator);

      /**
       *  @brief  Read a vector of values for a parameter from a (space separated) list in an xml element
       *          Looks for a xml element <parameter> with the attributes 'name' (mandatory) and 'value'.
       *          If the attribute 'value' is not found the inner text in the element is use instead.
       *
       *  @param  xmlHandle the xml handle
       *  @param  parameterName the parameter name
       *  @param  vector the parameter values to receive
       */
      template <typename T>
      static StatusCode readParameters(const TiXmlHandle &xmlHandle, const std::string &parameterName,
                                       std::vector<T> &vector);

      /**
       *  @brief  Read a vector of values for a parameter from a (space separated) list in an xml element
       *          Looks for a xml element <parameter> with the attributes 'name' (mandatory) and 'value'.
       *          If the attribute 'value' is not found the inner text in the element is use instead.
       *
       *  @param  xmlHandle the xml handle
       *  @param  parameterName the parameter name
       *  @param  vector the parameter values to receive
       *  @param  validator a unary predicate to validate the parameter values
       */
      template <typename T, typename Validator>
      static StatusCode readParameters(const TiXmlHandle &xmlHandle, const std::string &parameterName,
                                       std::vector<T> &vector, Validator validator);

      /**
       *  @brief  Parse a <parameter> XML element and extract the name and value of the parameter
       *
       *  @param  element the XML element to parse
       *  @param  name the parameter name to receive
       *  @param  value the parameter value to receive 
       */
      template <typename T>
      static StatusCode parseParameterElement(const TiXmlElement *const element, std::string &name, T &value);
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::readValue(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t) {
      const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

      if (nullptr == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

      if (!dqm4hep::core::stringToType(pXmlElement->GetText(), t))
        return STATUS_CODE_FAILURE;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline StatusCode XmlHelper::readValue<bool>(const TiXmlHandle &xmlHandle, const std::string &xmlElementName,
                                                 bool &t) {
      const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

      if (nullptr == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

      const std::string xmlElementString = pXmlElement->GetText();

      if ((xmlElementString == "1") || (xmlElementString == "true")) {
        t = true;
      } else if ((xmlElementString == "0") || (xmlElementString == "false")) {
        t = false;
      } else {
        return STATUS_CODE_FAILURE;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::readVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName,
                                                    std::vector<T> &vector) {
      const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

      if (nullptr == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

      StringVector tokens;
      dqm4hep::core::tokenize(pXmlElement->GetText(), tokens);

      for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter) {
        T t;

        if (!dqm4hep::core::stringToType(*iter, t))
          return STATUS_CODE_FAILURE;

        vector.push_back(t);
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::read2DVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName,
                                                      const std::string &rowName, std::vector<std::vector<T>> &vector) {
      TiXmlElement *pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

      if (nullptr == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

      TiXmlElement *pXmlRowElement = TiXmlHandle(pXmlElement).FirstChild(rowName).Element();

      if (nullptr == pXmlRowElement)
        return STATUS_CODE_NOT_FOUND;

      for (; nullptr != pXmlRowElement; pXmlRowElement = pXmlRowElement->NextSiblingElement(rowName)) {
        std::vector<T> rowVector;

        StringVector tokens;
        dqm4hep::core::tokenize(pXmlRowElement->GetText(), tokens);

        for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter) {
          T t;

          if (!dqm4hep::core::stringToType(*iter, t))
            return STATUS_CODE_FAILURE;

          rowVector.push_back(t);
        }

        vector.push_back(rowVector);
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName,
                                              T &attributeValue) {
      if (nullptr == pXmlElement)
        return STATUS_CODE_INVALID_PTR;

      const char *pAttributePtr(pXmlElement->Attribute(attributeName.c_str()));

      if (nullptr == pAttributePtr)
        return STATUS_CODE_NOT_FOUND;

      std::string attributeStr(pAttributePtr);

      if (!dqm4hep::core::stringToType(attributeStr, attributeValue))
        return STATUS_CODE_FAILURE;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T, typename Validator>
    inline StatusCode XmlHelper::getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName,
                                              T &attributeValue, Validator validator) {
      const StatusCode statusCode(XmlHelper::getAttribute<T>(pXmlElement, attributeName, attributeValue));
      
      if(STATUS_CODE_SUCCESS != statusCode)
        return statusCode;

      if (!validator(attributeValue))
        return STATUS_CODE_INVALID_PARAMETER;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline StatusCode XmlHelper::getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName,
                                              std::string &attributeValue) {
      if (nullptr == pXmlElement)
        return STATUS_CODE_INVALID_PTR;

      const char *pAttributePtr(pXmlElement->Attribute(attributeName.c_str()));

      if (nullptr == pAttributePtr)
        return STATUS_CODE_NOT_FOUND;

      attributeValue = pAttributePtr;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::readParameter(const TiXmlHandle &xmlHandle, const std::string &parameterName, T &t) {
      for (TiXmlElement *pXmlElement = xmlHandle.FirstChild("parameter").Element(); nullptr != pXmlElement;
           pXmlElement = pXmlElement->NextSiblingElement("parameter")) {
        std::string name;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", name));

        if (name != parameterName)
          continue;

        std::string value;

        if (STATUS_CODE_SUCCESS != XmlHelper::getAttribute(pXmlElement, "value", value))
          value = pXmlElement->GetText() ? pXmlElement->GetText() : "";

        if (!dqm4hep::core::stringToType(value, t))
          return STATUS_CODE_FAILURE;

        return STATUS_CODE_SUCCESS;
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T, typename Validator>
    inline StatusCode XmlHelper::readParameter(const TiXmlHandle &xmlHandle, const std::string &parameterName, T &t,
                                               Validator validator) {
     const StatusCode statusCode(XmlHelper::readParameter(xmlHandle, parameterName, t));
     
     if(STATUS_CODE_SUCCESS != statusCode)
       return statusCode;

      if (!validator(t))
        return STATUS_CODE_INVALID_PARAMETER;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::readParameters(const TiXmlHandle &xmlHandle, const std::string &parameterName,
                                                std::vector<T> &vector) {
      for (TiXmlElement *pXmlElement = xmlHandle.FirstChild("parameter").Element(); nullptr != pXmlElement;
           pXmlElement = pXmlElement->NextSiblingElement("parameter")) {
        std::string name;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", name));

        if (name != parameterName)
          continue;

        std::string value;

        if (STATUS_CODE_SUCCESS != XmlHelper::getAttribute(pXmlElement, "value", value))
          value = pXmlElement->GetText() ? pXmlElement->GetText() : "";

        StringVector tokens;
        dqm4hep::core::tokenize(value, tokens);

        for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter) {
          T t;

          if (!dqm4hep::core::stringToType(*iter, t))
            return STATUS_CODE_FAILURE;

          vector.push_back(t);
        }

        return STATUS_CODE_SUCCESS;
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T, typename Validator>
    inline StatusCode XmlHelper::readParameters(const TiXmlHandle &xmlHandle, const std::string &parameterName,
                                                std::vector<T> &vector, Validator validator) {
      
      const StatusCode statusCode(XmlHelper::readParameters(xmlHandle, parameterName, vector));
      
      if(STATUS_CODE_SUCCESS != statusCode)
        return statusCode;

      if (!validator(vector))
        return STATUS_CODE_INVALID_PARAMETER;

      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    StatusCode XmlHelper::parseParameterElement(const TiXmlElement *const element, std::string &name, T &value) {
      if(nullptr == element) {
        return STATUS_CODE_INVALID_PTR;
      }
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(element, "name", name));
      std::string valueStr;
      if (STATUS_CODE_SUCCESS != XmlHelper::getAttribute(element, "value", valueStr)) {
        valueStr = element->GetText() ? element->GetText() : "";
      }
      if (not dqm4hep::core::stringToType(valueStr, value)) {
        return STATUS_CODE_FAILURE;        
      }
      return STATUS_CODE_SUCCESS;
    }
    
  }
  
}

#endif //  DQM4HEP_XMLHELPER_H
