  /// \file dqm4hep_start_dqm_control.cc
/*
 *
 * dqm4hep_start_dqm_control.cc main source file template automatically generated
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

#include <QApplication>
#include <QMetaType>
#include <QTextCharFormat>
#include <QTextCursor>

#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/vis/DQMJobInterfaceWidget.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

using namespace dqm4hep;

int main(int argc, char* argv[])
{
	DQM4HEP::screenSplash();

	std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
	TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4HEP_VERSION_STR);

	TCLAP::ValueArg<std::string> settingsFileArg(
				  "f"
				 , "settings-file"
				 , "The xml file containing a run configuration and/or a list of applications expected to be running on the network"
				 , false
				 , ""
				 , "string");
	pCommandLine->add(settingsFileArg);

	pCommandLine->parse(argc, argv);

	QApplication app(argc, argv);

	qRegisterMetaType<QTextCharFormat>("QTextCharFormat");
	qRegisterMetaType<QTextCursor>("QTextCursor");

	QString iconDir = QString(DQM4HEP_DIR) + "/icons";

	DQMJobInterfaceWidget jobInterface;
	jobInterface.setWindowIcon(QIcon(iconDir + "/MON_CTRL_MAIN.png"));
	jobInterface.setWindowTitle("Job interface");
	jobInterface.show();
	jobInterface.resize(900, 500);

	if(settingsFileArg.isSet())
	{
		jobInterface.loadJsonFile(settingsFileArg.getValue());
	}

	return app.exec();
}
