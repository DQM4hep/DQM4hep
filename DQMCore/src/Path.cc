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
#include <dqm4hep/Path.h>

// -- std headers
#include <algorithm>

namespace dqm4hep {

  namespace core {

    Path::Path(const std::string &path) : 
      m_path(path) {
      Path::normalize(m_path);
    }

    //-------------------------------------------------------------------------------------------------

    std::vector<std::string> Path::getSplitPath() const {
      if (!this->isValid())
        return std::vector<std::string>();

      std::vector<std::string> splitPath;
      dqm4hep::core::tokenize(m_path, splitPath, "/");

      return splitPath;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Path::getPath() const {
      return m_path;
    }

    //-------------------------------------------------------------------------------------------------

    bool Path::isValid() const {
      if (dqm4hep::core::containsSpecialCharacters(m_path))
        return false;

      if (std::string::npos != m_path.find("//"))
        return false;

      return true;
    }

    //-------------------------------------------------------------------------------------------------

    bool Path::isRelative() const {
      if (!this->isValid())
        return false;

      if (m_path.empty())
        return true;

      if (m_path.at(0) == '/')
        return false;

      return true;
    }

    //-------------------------------------------------------------------------------------------------

    std::string &Path::normalize(std::string &path) {
      size_t pos = path.find("//");

      while (std::string::npos != pos) {
        path.replace(pos, 2, "/");
        pos = path.find("//");
      }

      pos = path.find("/./");

      while (std::string::npos != pos) {
        path.replace(pos, 3, "/");
        pos = path.find("/./");
      }

      return path;
    }

    //-------------------------------------------------------------------------------------------------

    Path &Path::operator+=(const Path &path) {
      if (path.m_path.empty())
        return *this;

      if (m_path.empty()) {
        m_path = path.m_path;
        return *this;
      }

      size_t pos = std::string::npos;

      pos = m_path.find_last_not_of("/");
      std::string path1 = (pos == std::string::npos) ? m_path : m_path.substr(0, pos + 1);

      pos = path.m_path.find_first_not_of("/");
      std::string path2 = (pos == std::string::npos) ? path.m_path : path.m_path.substr(pos);

      m_path = path1 + "/" + path2;
      Path::normalize(m_path);

      return *this;
    }

    //-------------------------------------------------------------------------------------------------

    Path &Path::operator=(const Path &path) = default;

    //-------------------------------------------------------------------------------------------------

    Path &Path::operator=(const std::string &path) {
      m_path = path;
      Path::normalize(m_path);

      return *this;
    }
  }
}
