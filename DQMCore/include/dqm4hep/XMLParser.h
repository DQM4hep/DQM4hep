//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_XMLPARSER_H
#define DQM4HEP_XMLPARSER_H

// -- dqm4hep headers
#include <dqm4hep/DBInterface.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/tinyxml.h>

namespace dqm4hep {

  namespace core {

    /**
     *  @brief  XMLParser class.
     */
    class XMLParser {
    public:
      /**
       *  @brief  Constructor
       */
      XMLParser();

      /**
       *  @brief  Parse the xml file. See the different parsing options below
       *
       *  @param fname the xml file name
       */
      void parse(const std::string &fname);

      /**
       *  @brief  Get the last parsed file name
       */
      const std::string &fileName() const;

      /**
       *  @brief  Get the constants map.
       */
      const StringMap &constants() const;

      /**
       *  @brief  Get a constant value to specified type
       *
       *  @param  name the constant name
       *  @param  fallback fallback value if constant doesn't exists
       */
      template <typename T>
      T constantAs(const std::string &name, const T &fallback = T()) const;

      /**
       *  @brief  Get the xml document
       */
      const TiXmlDocument &document() const;

      /**
       *  @brief  Get the xml document
       */
      TiXmlDocument &document();

      /**
       *  @brief  Reset the parser: clear the constants and the xml document
       */
      void reset();

      /**
       *  @brief  Set whether to process <include> element while parsing
       *
       *  @param  process whether to process includes
       */
      void setProcessIncludes(bool process);

      /**
       *  @brief  Whether to process <include> elements
       */
      bool processIncludes() const;

      /**
       *  @brief  Set whether to allow nested <include> elements, meaning include
       *          elements in included files
       *
       *  @param  allow whether to allow nested includes
       */
      void setAllowNestedIncludes(bool allow);

      /**
       *  @brief  Whether to allow nested <include> elements meaning include
       *          elements in included files
       */
      bool allowNestedIncludes() const;

      /**
       *  @brief  Set whether to process constants sections while parsing
       *
       *  @param  process whether to process constants
       */
      void setProcessConstants(bool process);

      /**
       *  @brief  Whether to process constants sections while parsing
       */
      bool processConstants() const;

      /**
       *  @brief  Set whether to process <database> elements
       *
       *  @param  process whether to process <database> elements
       */
      void setProcessDatabase(bool process);

      /**
       *  @brief  Whether to process <database> elements
       */
      bool processDatabase() const;

      /**
       *  @brief  Set whether to allow the use of environment variable while resolving constants
       *
       *  @param  allow whether to allow the use of environment variable
       */
      void setAllowEnvVariables(bool allow);

      /**
       *  @brief  Whether to allow the use of environment variable while resolving constants
       */
      bool allowEnvVariables() const;
      
      /**
       *  @brief  Set whether to process XML for loop elements
       *
       *  @param  process whether to process for loops
       */
      void setProcessForLoops(bool process);

      /**
       *  @brief  Whether to process XML for loop elements
       */
      bool processForLoops() const;

    private:
      // parsing functions
      void resolveIncludes();
      void resolveForLoops();
      void resolveConstants();
      void resolveDbParameters();

      // include related functions
      void resolveIncludes(const std::string &referencePath, TiXmlNode *node);
      void resolveInclude(const std::string &referencePath, TiXmlElement *element, TiXmlDocument &document);
      void getFilePath(const std::string &fileName, std::string &filePath);
      void getRelativeFileName(const std::string &fileName, const std::string &relativeTo,
                               std::string &relativeFileName);

      // constants related functions
      void readConstants();
      void replaceAllConstants();
      void readConstantsSection(TiXmlElement *constants);
      void resolveConstantsInElement(TiXmlElement *element);
      void performConstantReplacement(std::string &value);
      void performEnvVariableReplacement(std::string &value);

      // database related functions
      void readDatabases();
      void resolveAllDbSelect(TiXmlElement *pXmlElement);
      
      // for loop related
      void resolveForLoops(TiXmlElement *element);
      void resolveForLoop(TiXmlNode *node, const std::string &id, int value);

    private:
      typedef std::map<std::string, std::shared_ptr<DBInterface>> DBInterfaceMap;

      bool m_processIncludes = {true};     ///< Whether to process includes elements
      bool m_allowNestedIncludes = {true}; ///< Whether to allow nested include elements
      bool m_processConstants = {true};    ///< Whether to process constants elements
      bool m_processDatabase = {true};     ///< Whether to process database elements
      bool m_allowEnvVariables = {true};   ///< Whether to allow the use of environment variables
      bool m_processForLoops = {true};     ///< Whether to process for loops
      std::string m_fileName = {""};       ///< The file name of the last parsed file
      TiXmlDocument m_document = {};       ///< The xml document
      StringMap m_constants = {};          ///< The constants map
      DBInterfaceMap m_databases = {};     ///< The map of databases
    };

    //----------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------

    template <typename T>
    inline T XMLParser::constantAs(const std::string &name, const T &fallback) const {
      T value(fallback);
      auto iter = m_constants.find(name);

      if (m_constants.end() != iter)
        dqm4hep::core::stringToType(iter->second, value);

      return value;
    }
  }
}

#endif //  DQM4HEP_XMLHELPER_H
