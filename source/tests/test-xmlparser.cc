/// \file test-xmlparser.cc
/*
 *
 * test-xmlparser.cc main source file template automatically generated
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
 */

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/XMLParser.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

#define assert_test(Command)                                                                                           \
  if (!(Command)) {                                                                                                    \
    dqm_error("Assertion failed : {0}, line {1}", #Command, __LINE__);                                                 \
    exit(1);                                                                                                           \
  }

int main(int /*argc*/, char *argv[]) {
  Logger::createLogger("test-xmlparser", {Logger::coloredConsole()});
  Logger::setMainLogger("test-xmlparser");
  Logger::setLogLevel(spdlog::level::debug);

  XMLParser parser;
  bool parseOkay = true;

  try {
    parser.parse(argv[1]);
  } catch (...) {
    parseOkay = false;
  }

  assert_test(parseOkay);

  TiXmlElement *root = parser.document().RootElement();
  assert_test(root != nullptr);

  std::string user = parser.constantAs<std::string>("user");
  assert_test(user == "superman");

  int age = parser.constantAs<int>("age", 0);
  assert_test(age == 32);

  std::string badguy = parser.constantAs<std::string>("badguy", "");
  assert_test(badguy == "dracula");

  std::string notFound = parser.constantAs<std::string>("bibou", "missing");
  assert_test(notFound == "missing");

  TiXmlElement *heroElement = root->FirstChildElement("hero");

  std::string heroName;
  heroElement->QueryStringAttribute("name", &heroName);
  assert_test(heroName == "superman");

  std::string heroPlanet;
  heroElement->QueryStringAttribute("planet", &heroPlanet);
  assert_test(heroPlanet == "krypton");

  return 0;
}
