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
#include <dqm4hep/UnitTesting.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;

int main(int /*argc*/, char *argv[]) {
  UnitTest unitTest("test-xmlparser");

  XMLParser parser;
  bool parseOkay = true;

  try {
    parser.parse(argv[1]);
  } catch (...) {
    parseOkay = false;
  }

  unitTest.test("PARSE_OKAY", parseOkay);
  
  auto document = parser.document();
  TiXmlPrinter printer;
  document.Accept(&printer);
  std::cout << printer.Str() << std::endl;

  TiXmlElement *root = parser.document().RootElement();
  unitTest.test("ROOT_VALID", root != nullptr);

  std::string user = parser.constantAs<std::string>("user");
  unitTest.test("SUPERMAN_CST", user == "superman");

  int age = parser.constantAs<int>("age", 0);
  unitTest.test("AGE_CST", age == 32);

  std::string incVar1 = parser.constantAs<std::string>("incVar1", "");
  unitTest.test("TOTO_CST", incVar1 == "toto");
  
  std::string incVar2 = parser.constantAs<std::string>("incVar2", "");
  unitTest.test("TATA_CST", incVar2 == "tata");

  std::string badguy = parser.constantAs<std::string>("badguy", "");
  unitTest.test("BADGUY_CST", badguy.empty());

  std::string notFound = parser.constantAs<std::string>("bibou", "missing");
  unitTest.test("BIBOU_CST", notFound == "missing");

  TiXmlElement *heroElement = root->FirstChildElement("hero");

  std::string heroName;
  heroElement->QueryStringAttribute("name", &heroName);
  unitTest.test("CHILD_SUPERMAN", heroName == "superman");

  std::string heroPlanet;
  heroElement->QueryStringAttribute("planet", &heroPlanet);
  unitTest.test("CHILD_KRYPTON", heroPlanet == "krypton");

  return 0;
}
