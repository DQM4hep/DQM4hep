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
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/StatusCodes.h>

using namespace std;
using namespace dqm4hep::core;

int main(int argc, char *argv[]) {
  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PluginManager::instance()->loadLibraries());
    PluginManager::instance()->dump();
  } catch (StatusCodeException &e) {
    dqm_error("While loading libraries : Caught {0}", e.toString());
    return e.getStatusCode();
  } catch (...) {
    dqm_error("While loading libraries : Caught unknown error");
    return 1;
  }

  return 0;
}
