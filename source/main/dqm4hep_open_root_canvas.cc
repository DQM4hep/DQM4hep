  /// \file dqm4hep_open_root_canvas.cc
/*
 *
 * dqm4hep_open_root_canvas.cc main source file template automatically generated
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

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- root headers
#include "TObject.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TBrowser.h"

using namespace std;

int main(int argc, char* argv[])
{
	// initialize root
	TApplication app("dqm4hep_open_root_canvas", NULL, NULL);

	std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
	TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ');

	TCLAP::ValueArg<std::string> rootFileNameArg(
				  "f"
				 , "root-file"
				 , "The root file to open"
				 , true
				 , ""
				 , "string");
	pCommandLine->add(rootFileNameArg);

	// parse command line
	pCommandLine->parse(argc, argv);

	std::string rootFileName = rootFileNameArg.getValue();

	TFile *pFile = TFile::Open(rootFileName.c_str());

	if(!pFile)
		return 1;

	TObject *pObject = pFile->Get("dqm4hep_open_root_canvas");

	if(!pObject)
		return 1;

	pObject->Draw(pObject->GetDrawOption());
	gPad->SetTitle(pObject->GetName());

	while(1)
	{
		gSystem->ProcessEvents();

		if(!gPad)
			break;

		timespec timesleep;
		timesleep.tv_sec = 0;
		timesleep.tv_nsec = 100000L;
		nanosleep(&timesleep, NULL);
	}

	pFile->Close();
	delete pFile;

	return 0;
}
