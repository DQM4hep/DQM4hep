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
#include <dqm4hep/Version.h>

namespace dqm4hep {

  namespace core {

    Version::Version() : m_major(0), m_minor(0), m_patch(0) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    Version::Version(unsigned int major, unsigned int minor, unsigned int patch) {
      set(major, minor, patch);
    }

    //-------------------------------------------------------------------------------------------------

    Version::~Version() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    unsigned int Version::getMajor() const {
      return m_major;
    }

    //-------------------------------------------------------------------------------------------------

    unsigned int Version::getMinor() const {
      return m_minor;
    }

    //-------------------------------------------------------------------------------------------------

    unsigned int Version::getPatch() const {
      return m_patch;
    }

    //-------------------------------------------------------------------------------------------------

    void Version::set(unsigned int major, unsigned int minor, unsigned int patch) {
      m_major = major;
      m_minor = minor;
      m_patch = patch;
      std::stringstream ss;
      ss << m_major << "." << m_minor << "." << m_patch;
      m_versionString = ss.str();
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Version::toString() const {
      return m_versionString;
    }

    //-------------------------------------------------------------------------------------------------

    Version &Version::operator=(const Version &version) {
      m_major = version.m_major;
      m_minor = version.m_minor;
      m_patch = version.m_patch;
      m_versionString = version.toString();

      return *this;
    }

    //-------------------------------------------------------------------------------------------------

    bool operator<(const Version &lhs, const Version &rhs) {
      if (lhs.getMajor() < rhs.getMajor())
        return true;

      if (lhs.getMinor() < rhs.getMinor())
        return true;

      if (lhs.getPatch() < rhs.getPatch())
        return true;

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    bool operator<=(const Version &lhs, const Version &rhs) {
      if (lhs < rhs)
        return true;

      if (lhs.getMajor() == rhs.getMajor() && lhs.getMinor() == rhs.getMinor() && lhs.getPatch() == rhs.getPatch())
        return true;

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    bool operator>(const Version &lhs, const Version &rhs) {
      return !(lhs <= rhs);
    }

    //-------------------------------------------------------------------------------------------------

    bool operator>=(const Version &lhs, const Version &rhs) {
      return !(lhs < rhs);
    }

    //-------------------------------------------------------------------------------------------------

    bool operator==(const Version &lhs, const Version &rhs) {
      return (lhs.getMajor() == rhs.getMajor() && lhs.getMinor() == rhs.getMinor() && lhs.getPatch() == rhs.getPatch());
    }

    //-------------------------------------------------------------------------------------------------

    bool operator!=(const Version &lhs, const Version &rhs) {
      return !(lhs == rhs);
    }

  }
  
}
