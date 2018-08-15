/// \file dqm4hep-dump-plugins.cc
/*
 *
 * dqm4hep-dump-plugins.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2014
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 *
 * Create a quality test template file a ROOT file.
 * Parses the different directories and populate the
 * qtest file with found objects.
 */

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
