  /// \file dqm4hep_start_run_control.cc
/*
 *
 * dqm4hep_start_run_control.cc main source file template automatically generated
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

#include <iostream>

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMLogging.h"
#include "dqm4hep/vis/DQMRunControlWidget.h"
#include "dqm4hep/vis/DQMLoggerWidget.h"

// -- qt headers
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLayout>

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

using namespace std;
using namespace dqm4hep;

int main(int argc, char* argv[])
{
	DQM4HEP::screenSplash();

	std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
	TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4HEP_VERSION_STR);

	TCLAP::ValueArg<std::string> runControlNameArg(
				  "r"
				 , "run-control-name"
				 , "The run control name for run control services"
				 , false
				 , "DEFAULT"
				 , "string");
	pCommandLine->add(runControlNameArg);

	TCLAP::ValueArg<std::string> settingsFileNameArg(
				  "f"
				 , "settings-file"
				 , "The run control settings file"
				 , false
				 , ""
				 , "string");
	pCommandLine->add(settingsFileNameArg);

	TCLAP::ValueArg<std::string> serverNameArg(
				  "d"
				 , "server-name"
				 , "The dim server name to start"
				 , false
				 , "DQM4HEP/RunControl"
				 , "string");
	pCommandLine->add(serverNameArg);

	TCLAP::ValueArg<std::string> verbosityArg(
				  "v"
				 , "verbosity"
				 , "The verbosity used for this application"
				 , false
				 , "WARNING"
				 , "string");
	pCommandLine->add(verbosityArg);

	// parse command line
	streamlog_out(MESSAGE) << "Parsing command line ... " << std::endl;
	pCommandLine->parse(argc, argv);

	std::string verbosity = verbosityArg.getValue();
	streamlog_init( "DQM4HEP RUN CONTROL" , verbosity );

	QApplication app(argc, argv);

	QMainWindow mainWindow;

	QString iconDir = QString(DQM4HEP_DIR) + "/icons";
	mainWindow.setWindowIcon(QIcon(iconDir + "/RUN_CTRL.png"));

	DQMLoggerWidget *pLoggerWidget = new DQMLoggerWidget();
	DQMRunControlWidget *pRunControlWidget = new DQMRunControlWidget(pLoggerWidget);

	pRunControlWidget->layout()->addWidget(pLoggerWidget);
	pRunControlWidget->setRunControlName( runControlNameArg.getValue().c_str() );
	pRunControlWidget->start();

	mainWindow.setCentralWidget(pRunControlWidget);
	QMenu *pFileMenu = mainWindow.menuBar()->addMenu("&File");
	pFileMenu->addAction("&Import", pRunControlWidget, SLOT(importFile()));
	pFileMenu->addAction("&Export", pRunControlWidget, SLOT(exportFile()));

	DimServer::start( serverNameArg.getValue().c_str() );

	mainWindow.show();
	mainWindow.setWindowTitle("Run control (" + QString(runControlNameArg.getValue().c_str()) + ")");

	if(settingsFileNameArg.isSet())
		pRunControlWidget->importFile(settingsFileNameArg.getValue());

	return app.exec();
}
