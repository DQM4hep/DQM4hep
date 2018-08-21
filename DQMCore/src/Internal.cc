//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete, A.Pingault
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h> // Include here to avoid cycling include in the header

// -- ROOT header
#include <TSystem.h>
// #if defined(__linux__)
#include <TPRegexp.h>
// #endif

#include <cstring>
#include <dirent.h>
#include <regex>
#include <sys/resource.h> // getrusage
#include <sys/time.h> // gettimeofday

namespace dqm4hep {

  namespace core {
    
    bool wildcardMatch(const std::string &testString, const std::string &wildcardString, bool caseSensitive) {
      std::string wildcardPattern = wildcardString;
      replaceAll(wildcardPattern, "\\", "\\\\");
      replaceAll(wildcardPattern, "^", "\\^");
      replaceAll(wildcardPattern, ".", "\\.");
      replaceAll(wildcardPattern, "$", "\\$");
      replaceAll(wildcardPattern, "|", "\\|");
      replaceAll(wildcardPattern, "(", "\\(");
      replaceAll(wildcardPattern, ")", "\\)");
      replaceAll(wildcardPattern, "[", "\\[");
      replaceAll(wildcardPattern, "]", "\\]");
      replaceAll(wildcardPattern, "*", "\\*");
      replaceAll(wildcardPattern, "+", "\\+");
      replaceAll(wildcardPattern, "?", "\\?");
      replaceAll(wildcardPattern, "/", "\\/");
      // Convert chars '*?' back to their regex equivalents
      replaceAll(wildcardPattern, "\\?", ".");
      replaceAll(wildcardPattern, "\\*", ".*");
      // regex !
      auto flag = caseSensitive ? std::regex::ECMAScript : std::regex::ECMAScript | std::regex::icase;
      std::regex pattern(wildcardPattern, flag);
      return std::regex_match(testString, pattern);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string &replaceAll(std::string &subject, const std::string& search, const std::string& replace) {
      size_t pos = 0;
      while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
      }
      return subject; 
    }
    
    //-------------------------------------------------------------------------------------------------
    
    // void timeToHMS(std::time_t t, std::string &timeStr) {
    //   timeStr = time::asString(time::asPoint(t));
    // }
    
    
    //-------------------------------------------------------------------------------------------------

    // TimePoint now() {
    //   return time::now();
    // }

    //-------------------------------------------------------------------------------------------------

    // void sleep(const TimeDuration &duration) {
    //   std::this_thread::sleep_for(duration);
    // }

#if defined(__linux__) || defined(__APPLE__)

    /**
     *  @brief  Compute process cpu load for osx and linux
     *  @param  stats the ProcessStats struct to fill
     */
    static void procCpuStats(ProcessStats &stats) {
      struct rusage ru;
      if (getrusage(RUSAGE_SELF, &ru) < 0) {
        dqm_error("[{0}] - Failed to getrusage", __FUNCTION__);
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      } else {

        double prevCpuTimeUser = stats.cpuTimeUser;
        double prevCpuTimeSys = stats.cpuTimeSys;
        stats.cpuTimeUser = ru.ru_utime.tv_sec + ru.ru_utime.tv_usec / 1000000.;
        stats.cpuTimeSys = ru.ru_stime.tv_sec + ru.ru_stime.tv_usec / 1000000.;

        struct timeval timeNow;
        gettimeofday(&timeNow, NULL);
        double start = stats.lastPollTime.tv_sec + stats.lastPollTime.tv_usec / 1000000.;
        double stop = timeNow.tv_sec + timeNow.tv_usec / 1000000.;
        stats.lastPollTime = timeNow;
        if (stop > start) {
          double timeDif = stop - start;
          // Don't compute percentage for the first call
          if (0 != prevCpuTimeUser || 0 != prevCpuTimeSys) {
            stats.cpuUser = (stats.cpuTimeUser - prevCpuTimeUser) / timeDif;
            stats.cpuSys = (stats.cpuTimeSys - prevCpuTimeSys) / timeDif;
            stats.cpuTot = stats.cpuUser + stats.cpuSys;
            stats.cpuUser *= 100.;
            stats.cpuSys *= 100.;
            stats.cpuTot *= 100.;
          }
        } else {
          dqm_error("[{0}] - Something went wrong when comparing clocks", __FUNCTION__);
          stats.cpuUser = -1;
          stats.cpuSys = -1;
          stats.cpuTot = -1;
        }
      }
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Compute network rate and fill NetworkStats struct for osx and linux
     *  @param  stats the NetworkStats struct to fill
     *  @param  tempStats the NetworkStats struct to compare with
     *  @param  sampleTime time between the two NetworkStats reading in \b seconds
     */
    static void fillNetRate(NetworkStats &stats, NetworkStats &tempStats, int sampleTime) {
      for (const auto &ifcIter : tempStats) {
        auto ifcIter2 = stats.find(ifcIter.first);

        if (ifcIter.first != ifcIter2->first) {
          dqm_error("[{0}] - Wrong interface comparison");
          throw core::StatusCodeException(STATUS_CODE_FAILURE);
        }

        ifcIter2->second.rcv_rate_kbytes =
            (ifcIter2->second.tot_rcv_kbytes - ifcIter.second.tot_rcv_kbytes) / sampleTime;
        ifcIter2->second.rcv_rate_packets =
            (ifcIter2->second.tot_rcv_packets - ifcIter.second.tot_rcv_packets) / sampleTime;
        ifcIter2->second.rcv_rate_errs = (ifcIter2->second.tot_rcv_errs - ifcIter.second.tot_rcv_errs) / sampleTime;
        ifcIter2->second.snd_rate_kbytes =
            (ifcIter2->second.tot_snd_kbytes - ifcIter.second.tot_snd_kbytes) / sampleTime;
        ifcIter2->second.snd_rate_packets =
            (ifcIter2->second.tot_snd_packets - ifcIter.second.tot_snd_packets) / sampleTime;
        ifcIter2->second.snd_rate_errs = (ifcIter2->second.tot_snd_errs - ifcIter.second.tot_snd_errs) / sampleTime;
      }
    }
#endif // linux || __APPLE__

      //-------------------------------------------------------------------------------------------------

#if defined(__linux__)

#if defined(DQM4HEP_WITH_PROC_FS)
    // only for unix systems
    int getProcValue(const std::string fname, const std::string &entry) {
      FILE *file = fopen(fname.c_str(), "r");
      int result = -1;
      char line[128];

      while (fgets(line, 128, file) != nullptr) {
        if (strncmp(line, entry.c_str(), entry.size()) == 0) {
          int i = strlen(line);
          const char *p = line;
          while (*p < '0' || *p > '9') {
            p++;
          }
          line[i - 3] = '\0';
          result = atoi(p);
          break;
        }
      }
      fclose(file);
      return result;
    }
#endif // DQM4HEP_WITH_PROC_FS

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Read cpu load for linux
     *  @param  ticks an array of length 4 to store the cpu ticks value
     */
    static void readLinuxCpu(long *ticks) {
      ticks[0] = ticks[1] = ticks[2] = ticks[3] = 0;

      TString s;
      FILE *f = fopen("/proc/stat", "r");

      if (!f) {
        dqm_error("[{0}] - Failed to open '/proc/stat'", __FUNCTION__);
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      }

      s.Gets(f);
      // user, user nice, sys, idle
      sscanf(s.Data(), "%*s %ld %ld %ld %ld", &ticks[0], &ticks[3], &ticks[1], &ticks[2]);
      fclose(f);
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get cpu stats for linux
     *  @param  stats the CpuStats struct to store the data
     *  @param  sampleTime the time between two cpu reading to compute the load in \b seconds
     */
    static void linuxCpuStats(CpuStats &stats, int sampleTime) {
      double avg[3] = {-1.};
#ifndef R__WINGCC
      if (getloadavg(avg, sizeof(avg)) < 0) {
        // ::Error("getLinuxCpuInfo", "getloadavg failed");
        dqm_error("[{0}] - Failed to getloadavg'", __FUNCTION__);
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      } else
#endif // R__WINGCC
      {
        stats.load1m = (float)avg[0];
        stats.load5m = (float)avg[1];
        stats.load15m = (float)avg[2];
      }

      long cpu_ticks1[4] = {0L};
      long cpu_ticks2[4] = {0L};
      readLinuxCpu(cpu_ticks1);
      ::sleep(sampleTime);
      readLinuxCpu(cpu_ticks2);

      long userticks = (cpu_ticks2[0] + cpu_ticks2[3]) - (cpu_ticks1[0] + cpu_ticks1[3]);
      long systicks = cpu_ticks2[1] - cpu_ticks1[1];
      long idleticks = cpu_ticks2[2] - cpu_ticks1[2];
      if (userticks < 0)
        userticks = 0;
      if (systicks < 0)
        systicks = 0;
      if (idleticks < 0)
        idleticks = 0;
      long totalticks = userticks + systicks + idleticks;
      if (totalticks) {
        stats.user = ((float)(100 * userticks)) / ((float)totalticks);
        stats.sys = ((float)(100 * systicks)) / ((float)totalticks);
        stats.tot = stats.user + stats.sys;
        stats.idle = ((float)(100 * idleticks)) / ((float)totalticks);
      }
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get memory stats for linux
     *  @param  stats the MemoryStats struct to store the data
     */
    static void linuxMemStats(MemoryStats &stats) {
      TString s;
      FILE *f = fopen("/proc/meminfo", "r");
      if (!f) {
        dqm_error("[{0}] - Failed to open '/proc/meminfo'", __FUNCTION__);
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      }
      uint64_t memFree = {0LL};
      uint64_t memTotal = {0LL};

      uint64_t swap_total = {0LL};
      uint64_t swap_free = {0LL};

      while (s.Gets(f)) {
        if (s.BeginsWith("MemTotal")) {
          TPRegexp("^.+: *([^ ]+).*").Substitute(s, "$1");
          memTotal = (s.Atoi() / 1024);
        }
        if (s.BeginsWith("MemFree")) {
          TPRegexp("^.+: *([^ ]+).*").Substitute(s, "$1");
          memFree = (s.Atoi() / 1024);
        }
        if (s.BeginsWith("SwapTotal")) {
          TPRegexp("^.+: *([^ ]+).*").Substitute(s, "$1");
          swap_total = (s.Atoi() / 1024);
        }
        if (s.BeginsWith("SwapFree")) {
          TPRegexp("^.+: *([^ ]+).*").Substitute(s, "$1");
          swap_free = (s.Atoi() / 1024);
        }
      }
      fclose(f);

      stats.vmTot = (int)(memTotal + swap_total);
      stats.vmUsed = (int)(memTotal - memFree);
      stats.vmFree = (int)(memFree + swap_free);
      stats.rssTot = (int)(memTotal);
      stats.rssUsed = (int)(memTotal - memFree);
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get process stats for linux
     *  @param  stats the ProcessStats struct to store the data
     */
    static void linuxProcStats(ProcessStats &stats) {

      procCpuStats(stats);

      stats.vm = -1;
      stats.rss = -1;
      TString s;
      FILE *f = fopen(TString::Format("/proc/%d/statm", gSystem->GetPid()), "r");
      if (f) {
        s.Gets(f);
        fclose(f);
        long total = {0L};
        long rss = {0L};
        sscanf(s.Data(), "%ld %ld", &total, &rss);
        stats.vm = total * (getpagesize() / 1024);
        stats.rss = rss * (getpagesize() / 1024);
      }
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Read network stats for linux from /proc/net.dev
     *  @param  stats the NetworkStats struct to store the data
     */
    static void  readLinuxNet(NetworkStats &stats) {
#if defined(DQM4HEP_WITH_PROC_FS)
      FILE *file = fopen("/proc/net/dev", "r");

      // skip first two lines
      if (fscanf(file, "%*[^\n]\n") == EOF or fscanf(file, "%*[^\n]\n") == EOF) {
        return;
      }

      while (1) {
        // get interface name
        std::string iname;
        int c;
        do {
          c = fgetc(file);
          if (c == ':')
            break;
          iname += c;
        } while (c != EOF && c != '\n');

        if (c == EOF)
          break;

        INetworkStats stat;

        // read received stats
        if (fscanf(file, "%lu %lu %lu", &stat.tot_rcv_kbytes, &stat.tot_rcv_packets, &stat.tot_rcv_errs) == EOF) {
          stat.tot_rcv_kbytes = (int) ((stat.tot_rcv_kbytes) >> 10); // divide by 1024
          fclose(file);
          return;
        }

        // skip uneeded fields
        long unsigned int dummy;
        if (EOF == fscanf(file, "%lu %lu %lu %lu %lu", &dummy, &dummy, &dummy, &dummy, &dummy))
          break;

        // read send stats
        if (fscanf(file, "%lu %lu %lu", &stat.tot_snd_kbytes, &stat.tot_snd_packets, &stat.tot_snd_errs) == EOF) {
          stat.tot_snd_kbytes = (int) ((stat.tot_snd_kbytes) >> 10); // divide by 1024
          fclose(file);
          return;
        }

        if (EOF == fscanf(file, "%*[^\n]\n"))
          break;

        stats[iname] = stat;
      }
      fclose(file);
#else // DQM4HEP_WITH_PROC_FS
    static void readLinuxNet(NetworkStats &/*stats*/) {
#endif
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get network stats for linux
     *  @param  stats the NetworkStats struct to store the data
     *  @param  sampleTime the time between two network i/o reading to compute the rate in \b seconds
     */
    static void linuxNetStats(NetworkStats &stats, int sampleTime) {
      NetworkStats tempStats;
      readLinuxNet(tempStats);
      ::sleep(sampleTime);
      readLinuxNet(stats);

      fillNetRate(stats, tempStats, sampleTime);
    }

#endif // __linux__

#if defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <sys/sysctl.h>

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Read cpu load for osx
     *  @param  ticks an array of length 4 to store the cpu ticks value
     */
    static void readDarwinCpu(long *ticks) {
      ticks[0] = ticks[1] = ticks[2] = ticks[3] = 0;

      host_cpu_load_info_data_t cpu;
      mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;

      kern_return_t kr = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpu, &count);
      if (kr != KERN_SUCCESS) {
        dqm_error("[{0}] - Failed to get host_statistics: {1}", __FUNCTION__, mach_error_string(kr));
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      } else {
        ticks[0] = cpu.cpu_ticks[CPU_STATE_USER];
        ticks[1] = cpu.cpu_ticks[CPU_STATE_SYSTEM];
        ticks[2] = cpu.cpu_ticks[CPU_STATE_IDLE];
        ticks[3] = cpu.cpu_ticks[CPU_STATE_NICE];
      }
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get cpu stats for osx
     *  @param  stats the CpuStats struct to store the data
     *  @param  sampleTime the time between two cpu reading to compute the load in \b seconds
     */
    static void darwinCpuStats(CpuStats &stats, int sampleTime) {
      double avg[3] = {-1.};
      if (getloadavg(avg, sizeof(avg)) < 0) {
        dqm_error("[{0}] - Failed to getloadavg", __FUNCTION__);
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      } else {
        stats.load1m = (float)avg[0];
        stats.load5m = (float)avg[1];
        stats.load15m = (float)avg[2];
      }

      long cpu_ticks1[4] = {0L};
      long cpu_ticks2[4] = {0L};
      readDarwinCpu(cpu_ticks1);
      ::sleep(sampleTime);
      readDarwinCpu(cpu_ticks2);

      long userticks = (cpu_ticks2[0] + cpu_ticks2[3]) - (cpu_ticks1[0] + cpu_ticks1[3]);
      long systicks = cpu_ticks2[1] - cpu_ticks1[1];
      long idleticks = cpu_ticks2[2] - cpu_ticks1[2];
      if (userticks < 0)
        userticks = 0;
      if (systicks < 0)
        systicks = 0;
      if (idleticks < 0)
        idleticks = 0;
      long totalticks = userticks + systicks + idleticks;
      if (totalticks) {
        stats.user = ((float)(100 * userticks)) / ((float)totalticks);
        stats.sys = ((float)(100 * systicks)) / ((float)totalticks);
        stats.tot = stats.user + stats.sys;
        stats.idle = ((float)(100 * idleticks)) / ((float)totalticks);
      }
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get memory stats for osx
     *  @param  stats the MemoryStats struct to store the data
     */
    static void darwinMemStats(MemoryStats &stats) {
      vm_statistics_data_t vm_info;
      mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

      kern_return_t kr = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_info, &count);
      if (kr != KERN_SUCCESS) {
        dqm_error("[{0}] - Failed to get host_statistics: {1}", __FUNCTION__, mach_error_string(kr));
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      }
      static int pshift = 0;
      if (pshift == 0) {
        for (int psize = getpagesize(); psize > 1; psize >>= 1) {
          pshift++;
        }
      }

      long long used = (long long)(vm_info.active_count + vm_info.inactive_count + vm_info.wire_count) << pshift;
      long long free = (long long)(vm_info.free_count) << pshift;
      long long total =
          (long long)(vm_info.active_count + vm_info.inactive_count + vm_info.free_count + vm_info.wire_count)
          << pshift;

      // Swap is available at same time as mem, so grab values here.
      long long swap_used = vm_info.pageouts << pshift;

      // Figure out total swap. This adds up the size of the swapfiles */
      static DIR *dirp = nullptr;
      dirp = opendir("/private/var/vm");
      if (!dirp) {
        dqm_error("[{0}] - Failed to get vm file", __FUNCTION__);
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
        // return;
      }

      long long swap_total = 0;
      struct dirent *dp = nullptr;

      while ((dp = readdir(dirp)) != 0) {
        struct stat sb;
        char fname[MAXNAMLEN];
        if (strncmp(dp->d_name, "swapfile", 8))
          continue;
        strlcpy(fname, "/private/var/vm/", MAXNAMLEN);
        strlcat(fname, dp->d_name, MAXNAMLEN);
        if (stat(fname, &sb) < 0)
          continue;
        swap_total += sb.st_size;
      }
      closedir(dirp);

      stats.vmTot = (int)((total + swap_total) >> 20); // divide by 1024 * 1024
      stats.vmUsed = (int)((used + swap_used) >> 20);
      stats.vmFree = (int)((free + swap_total - swap_used) >> 20);
      stats.rssTot = (int)(total >> 20);
      stats.rssUsed = (int)(used >> 20);
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get process stats for osx
     *  @param  stats the ProcessStats struct to store the data
     */
    static void darwinProcStats(ProcessStats &stats) {
#ifdef _LP64
#define vm_region vm_region_64
#endif

// taken from <mach/shared_memory_server.h> which is obsoleted in 10.5
#define GLOBAL_SHARED_TEXT_SEGMENT 0x90000000U
#define GLOBAL_SHARED_DATA_SEGMENT 0xA0000000U
#define SHARED_TEXT_REGION_SIZE 0x10000000
#define SHARED_DATA_REGION_SIZE 0x10000000

      procCpuStats(stats);

      task_t a_task = mach_task_self();
      task_basic_info_data_t ti;
      mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;

      kern_return_t kr = task_info(a_task, TASK_BASIC_INFO, (task_info_t)&ti, &count);
      if (kr != KERN_SUCCESS) {
        dqm_error("[{0}] - Failed to get task_info: {1}", __FUNCTION__, mach_error_string(kr));
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      } else {
        // resident size does not require any calculation. Virtual size
        // needs to be adjusted if traversing memory objects do not include the
        // globally shared text and data regions
        mach_port_t object_name;
        vm_address_t address;
        vm_region_top_info_data_t info;

        vm_size_t rsize = ti.resident_size;
        vm_size_t vsize = ti.virtual_size;
        vm_size_t vprvt = 0;
        vm_size_t size = 0;

        for (address = 0;; address += size) {
          // get memory region
          count = VM_REGION_TOP_INFO_COUNT;
          if (vm_region(a_task, &address, &size, VM_REGION_TOP_INFO, (vm_region_info_t)&info, &count, &object_name) !=
              KERN_SUCCESS) {
            // no more memory regions.
            break;
          }

          if (address >= GLOBAL_SHARED_TEXT_SEGMENT &&
              address < (GLOBAL_SHARED_DATA_SEGMENT + SHARED_DATA_REGION_SIZE)) {
            // This region is private shared.
            // Check if this process has the globally shared
            // text and data regions mapped in. If so, adjust
            // virtual memory size and exit loop.
            if (info.share_mode == SM_EMPTY) {
              vm_region_basic_info_data_64_t b_info;
              count = VM_REGION_BASIC_INFO_COUNT_64;
              if (vm_region_64(a_task, &address, &size, VM_REGION_BASIC_INFO, (vm_region_info_t)&b_info, &count,
                               &object_name) != KERN_SUCCESS) {
                break;
              }

              if (b_info.reserved) {
                vsize -= (SHARED_TEXT_REGION_SIZE + SHARED_DATA_REGION_SIZE);
                // break;  // only for vsize
              }
            }
            // Short circuit the loop if this isn't a shared
            // private region, since that's the only region
            // type we care about within the current address range.
            if (info.share_mode != SM_PRIVATE) {
              continue;
            }
          }
          switch (info.share_mode) {
          case SM_COW: {
            if (info.ref_count == 1) {
              vprvt += size;
            } else {
              vprvt += info.private_pages_resident * getpagesize();
            }
            break;
          }
          case SM_PRIVATE: {
            vprvt += size;
            break;
          }
          default:
            break;
          }
        }

        stats.vm = (long)(rsize / 1024.);
        stats.rss = (long)(vprvt / 1024.);
      }
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Read network stats for osx via sysctl
     *  @param  stats the NetworkStats struct to store the data
     */
    static void readDarwinNet(NetworkStats &stats) {
      double unit = 1024.; // Store everything in kb

      // Get sizing info from sysctl and resize as needed.
      int mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0};
      size_t currentSize{0};
      if (sysctl(mib, 6, NULL, &currentSize, NULL, 0) != 0) {
        const int errNum = errno;
        dqm_error("[{0}] - Failed to get network buffer size: {1}", __FUNCTION__, errNum);
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      }
      size_t sysctlBufferSize{0};
      uint8_t *sysctlBuffer = nullptr;
      if (!sysctlBuffer || (currentSize > sysctlBufferSize)) {
        if (sysctlBuffer)
          free(sysctlBuffer);
        sysctlBufferSize = 0;
        sysctlBuffer = (uint8_t *)malloc(currentSize);
        if (!sysctlBuffer) {
          const int errNum = errno;
          dqm_error("[{0}] - Failed to get network statistics: {1}", __FUNCTION__, errNum);
          throw core::StatusCodeException(STATUS_CODE_FAILURE);
        }
        sysctlBufferSize = currentSize;
      }

      // Read in new data
      if (sysctl(mib, 6, sysctlBuffer, &currentSize, NULL, 0) != 0) {
        const int errNum = errno;
        dqm_error("[{0}] - Failed to read network statistics: {1}", __FUNCTION__, errNum);
        throw core::StatusCodeException(STATUS_CODE_FAILURE);
      }

      // Walk through the reply
      uint8_t *currentData = sysctlBuffer;
      uint8_t *currentDataEnd = sysctlBuffer + currentSize;

      while (currentData < currentDataEnd) {
        // Expecting interface data
        struct if_msghdr *ifmsg = (struct if_msghdr *)currentData;
        if (ifmsg->ifm_type != RTM_IFINFO) {
          currentData += ifmsg->ifm_msglen;
          continue;
        }

        // Only look at link layer items
        struct sockaddr_dl *sdl = (struct sockaddr_dl *)(ifmsg + 1);
        if (sdl->sdl_family != AF_LINK) {
          currentData += ifmsg->ifm_msglen;
          continue;
        }

        // Get interface name
        char ifc_name[32];
        strncpy(ifc_name, sdl->sdl_data, sdl->sdl_nlen);
        ifc_name[sdl->sdl_nlen] = 0;

        INetworkStats stat;
        stat.tot_rcv_kbytes = ifmsg->ifm_data.ifi_ibytes / unit;
        stat.tot_rcv_packets = ifmsg->ifm_data.ifi_ipackets;
        stat.tot_rcv_errs = ifmsg->ifm_data.ifi_ierrors;
        stat.tot_snd_kbytes = ifmsg->ifm_data.ifi_obytes / unit;
        stat.tot_snd_packets = ifmsg->ifm_data.ifi_opackets;
        stat.tot_snd_errs = ifmsg->ifm_data.ifi_oerrors;
        stats[ifc_name] = stat;

        currentData += ifmsg->ifm_msglen;
      }
    }

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get network stats for osx
     *  @param  stats the NetworkStats struct to store the data
     *  @param  sampleTime the time between two network i/o reading to compute the rate in \b seconds
     */
    static void darwinNetStats(NetworkStats &stats, int sampleTime) {

      NetworkStats tempStats;
      readDarwinNet(tempStats);
      ::sleep(sampleTime);
      readDarwinNet(stats);

      fillNetRate(stats, tempStats, sampleTime);
    }

#endif // __APPLE__

#if defined(_WIN32)
    static void winCpuStats(CpuStats &stats, int sampleTime){
      /* WINDOWS implementation: TBD */
      dqm_warning("[{0}] - has not been implemented!", __FUNCTION__);
      throw core::StatusCodeException(STATUS_CODE_NOT_FOUND);
    }

    //-------------------------------------------------------------------------------------------------

    static void winMemStats(MemoryStats &stats) {
      /* WINDOWS implementation: TBD */
      dqm_warning("[{0}] - has not been implemented!", __FUNCTION__);
      throw core::StatusCodeException(STATUS_CODE_NOT_FOUND);
    }

    //-------------------------------------------------------------------------------------------------

    static void winProcStats(ProcessStats &stats) {
      /* WINDOWS implementation: TBD */
      dqm_warning("[{0}] - has not been implemented!", __FUNCTION__);
      throw core::StatusCodeException(STATUS_CODE_NOT_FOUND);
    }

    //-------------------------------------------------------------------------------------------------

    static void winNetStats(NetworkStats &stats, int sampleTime) {
      /* WINDOWS implementation: TBD */
      dqm_warning("[{0}] - has not been implemented!", __FUNCTION__);
      throw core::StatusCodeException(STATUS_CODE_NOT_FOUND);
    }


#endif //_WIN32

    //-------------------------------------------------------------------------------------------------

    void cpuStats(CpuStats &stats, int sampleTime) {
#if defined(__APPLE__)
      darwinCpuStats(stats, sampleTime);
#elif defined(__linux__)
      linuxCpuStats(stats, sampleTime);
#elif defined(_WIN32)
      winCpuStats(stats, sampleTime);
#else
#error "Unrecognized OS plateform (not windows, linux, OSX or unix) !"
#endif
    }

    //-------------------------------------------------------------------------------------------------

    void memStats(MemoryStats &stats) {
#if defined(__APPLE__)
      darwinMemStats(stats);
#elif defined(__linux__)
      linuxMemStats(stats);
#elif defined(_WIN32)
      winMemStats(stats);
#else
#error "Unrecognized OS plateform (not windows, linux, OSX or unix) !"
#endif
    }

    //-------------------------------------------------------------------------------------------------

    void procStats(ProcessStats &stats) {
#if defined(__APPLE__)
      darwinProcStats(stats);
#elif defined(__linux__)
      linuxProcStats(stats);
#elif defined(_WIN32)
      winProcStats(stats);
#else
#error "Unrecognized OS plateform (not windows, linux, OSX or unix) !"
#endif
    }

    //-------------------------------------------------------------------------------------------------

    void netStats(NetworkStats &stats, int sampleTime) {
#if defined(__APPLE__)
      darwinNetStats(stats, sampleTime);
#elif defined(__linux__)
      linuxNetStats(stats, sampleTime);
#elif defined(_WIN32)
      winNetStats(stats, sampleTime);
#else
#error "Unrecognized OS plateform (not windows, linux, OSX or unix) !"
#endif
    }
  } // namespace core
} // namespace dqm4hep
