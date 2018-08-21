//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_PATH_H
#define DQM4HEP_PATH_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>

namespace dqm4hep {

  namespace core {

    /** Path class
     */
    class Path {
    public:
      /** Constructor
       */
      Path() = default;
      Path(const Path &path) = default;

      /** Constructor with path
       */
      Path(const std::string &path);

      /** Get the path, split by '/' in a vector
       */
      std::vector<std::string> getSplitPath() const;

      /** Get the path as string
       */
      const std::string &getPath() const;

      /** Whether the path is valid
       */
      bool isValid() const;

      /** Whether the path is relative (does not start with '/')
       */
      bool isRelative() const;

      /** Normalize the path. Replace all "//" and "/./" by "/" only characters
       */
      static std::string &normalize(std::string &path);

      /** += operator
       */
      Path &operator+=(const Path &path);

      /** Assignment operator with path
       */
      Path &operator=(const Path &path);

      /** Assignment operator with string
       */
      Path &operator=(const std::string &path);

      /** + operator
       */
      friend Path operator+(const Path &lhs, const Path &rhs);

      /** << operator
       */
      friend std::ostream &operator<<(std::ostream &out, const Path &path);

      /** == operator
       */
      friend bool operator==(const Path &lhs, const Path &rhs);

    private:
      std::string m_path = {""};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline Path operator+(const Path &lhs, const Path &rhs) {
      Path path(lhs.m_path);
      path += rhs;

      return path;
    }

    //-------------------------------------------------------------------------------------------------

    inline std::ostream &operator<<(std::ostream &out, const Path &path) {
      out << path.m_path;

      return out;
    }

    //-------------------------------------------------------------------------------------------------

    inline bool operator==(const Path &lhs, const Path &rhs) {
      return (lhs.getSplitPath() == rhs.getSplitPath());
    }
  }
}

#endif //  DQM4HEP_PATH_H
