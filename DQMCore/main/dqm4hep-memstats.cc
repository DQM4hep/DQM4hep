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

#define fmt_value(val) std::setw(16) << std::left << std::setprecision(1) << std::fixed << val

int main(int /*argc*/, char ** /*argv[]*/) {

  while (1) {

    dqm4hep::core::MemoryStats memInfo;
    dqm4hep::core::memStats(memInfo);

    dqm4hep::core::ProcessStats procInfo;
    dqm4hep::core::procStats(procInfo);

    std::cout << std::setw(10) << std::left << "Memory" << fmt_value("Total (Mb)") << fmt_value("Used (Mb)")
              << fmt_value("Process (Mb)") << fmt_value("Used (%)") << fmt_value("Proc used (%)") << std::endl;

    std::cout << std::setw(10) << std::left << "Resident" << fmt_value(memInfo.rssTot) << fmt_value(memInfo.rssUsed)
              << fmt_value(procInfo.rss / 1024.) << fmt_value((memInfo.rssUsed / (memInfo.rssTot * 1.)) * 100)
              << fmt_value(((procInfo.rss / 1024.) / (memInfo.rssTot * 1.)) * 100) << std::endl;

    std::cout << std::setw(10) << std::left << "Virtual" << fmt_value(memInfo.vmTot) << fmt_value(memInfo.vmUsed)
              << fmt_value(procInfo.vm / 1024.) << fmt_value((memInfo.vmUsed / (memInfo.vmTot * 1.)) * 100)
              << fmt_value(((procInfo.vm / 1024.) / (memInfo.vmTot * 1.)) * 100.) << std::endl;

    std::cout << std::endl;
    dqm4hep::core::time::sleep(std::chrono::seconds(2));
  }

  return 0;
}
