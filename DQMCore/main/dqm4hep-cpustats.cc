//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : A.Pingault
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <iomanip>
#include <sys/time.h>

#define wfmt_value(width, val) std::setw(width) << std::left << std::setprecision(1) << std::fixed << val
#define fmt_value(val) wfmt_value(10, val)

int main(int /*argc*/, char ** /*argv[]*/) {

  struct timeval timeStart;
  gettimeofday(&timeStart, NULL);
  dqm4hep::core::ProcessStats procInfo;
  procInfo.lastPollTime = timeStart;
  while (1) {
    dqm4hep::core::CpuStats cpuInfo;
    dqm4hep::core::cpuStats(cpuInfo);
    dqm4hep::core::procStats(procInfo);

    // System Info
    std::cout << std::setw(16) << std::left << "System Load" << fmt_value("1m (%)") << fmt_value("5m (%)")
              << fmt_value("15m (%)") << fmt_value("User (%)") << fmt_value("Sys (%)") << fmt_value("Total (%)")
              << fmt_value("Idle (%)") << std::endl;

    std::cout << std::setw(16) << std::left << " " << fmt_value(cpuInfo.load1m) << fmt_value(cpuInfo.load5m)
              << fmt_value(cpuInfo.load15m) << fmt_value(cpuInfo.user) << fmt_value(cpuInfo.sys)
              << fmt_value(cpuInfo.tot) << fmt_value(cpuInfo.idle) << std::endl;

    // Process Info
    std::cout << std::setw(16) << std::left << "Process Load" << fmt_value("User (%)") << fmt_value("Sys (%)") << fmt_value("Tot (%)")
              << wfmt_value(20, "User cpuTime (s)") << wfmt_value(20, "Sys cpuTime (s)") << std::endl;

    std::cout << std::setw(16) << std::left << " " << fmt_value(procInfo.cpuUser) << fmt_value(procInfo.cpuSys) << fmt_value(procInfo.cpuTot) << wfmt_value(20, procInfo.cpuTimeUser) << wfmt_value(20, procInfo.cpuTimeSys) << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
