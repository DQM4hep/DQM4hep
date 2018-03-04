/*
 *
 * Internal.cc source template automatically generated by a class generator
 * Creation date : lun. ao�t 17 2015
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
 */

// -- dqm4hep headers
#include <dqm4hep/Internal.h>

#include <cstring>

namespace dqm4hep {

  namespace core {
    
    // only for unix systems
    int getProcValue(const std::string fname, const std::string &entry) {
      FILE* file = fopen(fname.c_str(), "r");
      int result = -1;
      char line[128];

      while (fgets(line, 128, file) != nullptr) {
        if (strncmp(line, entry.c_str(), entry.size()) == 0) {
          int i = strlen(line);
          const char* p = line;
          while (*p <'0' || *p > '9') {
            p++;
          }
          line[i-3] = '\0';
          result = atoi(p);
          break;
        }
      }
      fclose(file);
      return result;
    }

#ifdef _WIN32
    void memStats(MemoryStats &) {
      /* WINDOWS implementation: TBD */
    }
#elif __APPLE__
    void memStats(MemoryStats &) {
      /* MACOSX implementation: TBD */
    }
#elif __linux__ || defined(_POSIX_VERSION)
    void memStats(MemoryStats &stats) {
      struct sysinfo memInfo;
      sysinfo (&memInfo);
      
      // total virtual memory
      stats.vmtot = memInfo.totalram;
      stats.vmtot += memInfo.totalswap;
      stats.vmtot *= memInfo.mem_unit;

      // total virtual memory in use
      stats.vmused = memInfo.totalram - memInfo.freeram;
      stats.vmused += memInfo.totalswap - memInfo.freeswap;
      stats.vmused *= memInfo.mem_unit;
            
      // Total physical memory
      stats.pmtot = memInfo.totalram;
      stats.pmtot *= memInfo.mem_unit;
      
      // Total physical memory in use
      stats.pmused = memInfo.totalram - memInfo.freeram;
      stats.pmused *= memInfo.mem_unit;
      
      // Process virtual memory size and physical memory size
      std::stringstream procstat; procstat << "/proc/" << core::pid() << "/status";
      stats.vmproc = getProcValue(procstat.str().c_str(), "VmSize:");
      stats.pmproc = getProcValue(procstat.str().c_str(), "VmRSS:");
    }
#elif __unix__
    void memStats(MemoryStats &) {
      /* nop */
    }
#else
    void memStats(MemoryStats &) {
      /* nop */
    }
#endif
  }
}
