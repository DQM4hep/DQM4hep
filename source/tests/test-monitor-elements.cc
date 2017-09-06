/// \file test-monitor-elements.cc
/*
 *
 * test-monitor-elements.cc main source file template automatically generated
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
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/Logging.h"

#include "dqm4hep/Graph1D.h"
#include "dqm4hep/Histogram1D.h"
#include "dqm4hep/PieChart.h"
#include "dqm4hep/Scalar.h"
#include "dqm4hep/Summary.h"

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

int main(int argc, char* argv[])
{
  Logger::createLogger("test-monitor-element", {Logger::console()});
  Logger::setMainLogger("test-monitor-element");
  Logger::setLogLevel(spdlog::level::debug);

  //
  // Graph1D testing
  //
  Graph1D *pGraph1D = new Graph1D();
  pGraph1D->setTitle("pol 2 graph");

  for(int i=0 ; i<20 ; i++)
    pGraph1D->addPoint(Graph1D::Point(i, i*i, 0, (i*i)/100));

  Json::StyledWriter writer;
  Json::Value jValue;

  pGraph1D->toJson(jValue, true, true);
  dqm_info("Graph1D json : {}", writer.write(jValue));

  // change graph properties
  pGraph1D->addPoint(Graph1D::Point(20, 20*20, 0, (20*20)/100));
  MarkerAttributes attr(pGraph1D->getMarkerAttributes());
  attr.setMarkerColor(Red);
  pGraph1D->setMarkerAttributes(attr);

  // write again with update mode
  jValue.clear();
  pGraph1D->toJson(jValue, false, true);
  dqm_info("Graph1D json update : {}", writer.write(jValue));

  //
  // Histogram1D testing
  //
  Histogram1D *pHisto1D = new Histogram1D(100, 0, 99);
  pHisto1D->setTitle("Gaussian distribution");

  // generate random numbers from normal distribution
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<float> d(50, 10);

  for(int i=0 ; i<10000 ; i++)
    pHisto1D->fill(d(gen));

  jValue.clear();
  pHisto1D->toJson(jValue, true, false);
  dqm_info("Histogram1D json : {}", writer.write(jValue));

  //
  // PieChart testing
  //
  PieChart *pPieChart = new PieChart();
  pPieChart->setTitle("N years at university");

  pPieChart->addEntry("Licence", Green, 3);
  pPieChart->addEntry("Master", Yellow, 2);
  pPieChart->addEntry("Doctorat", Red, 3);

  const PieChart::EntryMap &entries(pPieChart->getEntries());

  for(auto entry : entries)
    dqm_info("Name : {}, color : {}, value : {}, percentage : {}%", entry.first, entry.second.m_color, entry.second.m_value, entry.second.m_percentage*100);

  jValue.clear();
  pPieChart->toJson(jValue, true, false);
  dqm_info("PieChart json : {}", writer.write(jValue));

  //
  // Scalar testing
  //
  ScalarInt *pScalarInt = new ScalarInt(-42);
  jValue.clear();
  pScalarInt->toJson(jValue, true, false);
  dqm_info("ScalarInt json : {}", writer.write(jValue));

  ScalarUInt *pScalarUInt = new ScalarUInt(42);
  jValue.clear();
  pScalarUInt->toJson(jValue, true, false);
  dqm_info("pScalarUInt json : {}", writer.write(jValue));

  ScalarFloat *pScalarFloat = new ScalarFloat(0.48);
  jValue.clear();
  pScalarFloat->toJson(jValue, true, false);
  dqm_info("pScalarFloat json : {}", writer.write(jValue));

  ScalarDouble *pScalarDouble = new ScalarDouble(0.78);
  jValue.clear();
  pScalarDouble->toJson(jValue, true, false);
  dqm_info("pScalarDouble json : {}", writer.write(jValue));

  ScalarShort *pScalarShort = new ScalarShort(10);
  jValue.clear();
  pScalarShort->toJson(jValue, true, false);
  dqm_info("pScalarShort json : {}", writer.write(jValue));

  ScalarLong *pScalarLong = new ScalarLong(1000000);
  jValue.clear();
  pScalarLong->toJson(jValue, true, false);
  dqm_info("pScalarLong json : {}", writer.write(jValue));

  ScalarString *pScalarString = new ScalarString("A strange bibou");
  jValue.clear();
  pScalarString->toJson(jValue, true, false);
  dqm_info("pScalarString json : {}", writer.write(jValue));

  //
  // Summary testing
  //
  Summary *pSummary = new Summary();
  pSummary->setHeader("Monitor element summary");
  pSummary->setEntry("Temperature", "270 K");
  pSummary->setEntry("Pressure", "1.5 bar");
  pSummary->setEntry("HV", "6900 V");
  pSummary->setEntry("I", "1 µA");

  jValue.clear();
  pSummary->toJson(jValue, true, false);
  dqm_info("Summary json : {}", writer.write(jValue));

  // memory !
  delete pScalarInt;
  delete pScalarUInt;
  delete pScalarFloat;
  delete pScalarDouble;
  delete pScalarString;
  delete pScalarLong;
  delete pScalarShort;
  delete pHisto1D;
  delete pGraph1D;
  delete pSummary;
  delete pPieChart;

  return 0;
}
