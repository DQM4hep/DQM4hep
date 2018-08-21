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

#define wfmt_value(width, val) std::setw(width) << std::left << std::setprecision(1) << std::fixed << val
#define fmt_value(val) wfmt_value(20, val)

int main(int /*argc*/, char ** /*argv[]*/) {

  while (1) {

    dqm4hep::core::NetworkStats nStats;
    dqm4hep::core::netStats(nStats, 1);

    std::cout << std::setw(68) << "" << std::setw(68) << "Total" << std::setw(50) << "|\t" << std::setw(60) << "Rate"
              << std::endl;
    std::cout << std::setw(16) << std::left << "Interface" << fmt_value("Sent (kb)") << fmt_value("Received (kb)")
              << fmt_value("Sent packets") << fmt_value("Rcvd packets") << fmt_value("Rcvd errors")
              << fmt_value("Sent errors") << "|\t" << fmt_value("Sent (kb/s)") << fmt_value("Rcvd (kb/s)")
              << fmt_value("Sent (packets/s)") << fmt_value("Rcvd (packets/s)") << fmt_value("Sent (error/s)")
              << fmt_value("Rcvd (error/s)") << std::endl;

    for (const auto &stat : nStats) {
      std::cout << std::setw(16) << std::left << stat.first << fmt_value(stat.second.tot_snd_kbytes)
                << fmt_value(stat.second.tot_rcv_kbytes) << fmt_value(stat.second.tot_snd_packets)
                << fmt_value(stat.second.tot_rcv_packets) << fmt_value(stat.second.tot_rcv_errs)
                << fmt_value(stat.second.tot_snd_errs) << "|\t" << fmt_value(stat.second.rcv_rate_kbytes)
                << fmt_value(stat.second.rcv_rate_packets) << fmt_value(stat.second.rcv_rate_errs)
                << fmt_value(stat.second.snd_rate_kbytes) << fmt_value(stat.second.snd_rate_packets)
                << fmt_value(stat.second.snd_rate_errs) << std::endl;
    }
    dqm4hep::core::time::sleep(std::chrono::seconds(2));
  }

  return 0;
}
