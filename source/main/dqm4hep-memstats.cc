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
    dqm4hep::core::sleep(std::chrono::seconds(2));
  }

  return 0;
}
