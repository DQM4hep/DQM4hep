//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_ROOTHEADERS_H
#define DQM4HEP_ROOTHEADERS_H

// base headers
#include <Rtypes.h>
#include <TObject.h>
#include <TNamed.h>
#include <TClass.h>
#include <TFile.h>
#include <TROOT.h>

// histogram related
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TProfile3D.h>
#include <THStack.h>
#include <TH2Poly.h>

// graph related
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraph2D.h>
#include <TGraph2DErrors.h>
#include <TGraphAsymmErrors.h>
#include <TMultiGraph.h>

// function related
#include <TF1.h>
#include <TF2.h>
#include <TF3.h>

// miscellaneous
#include <TPie.h>
#include <TTree.h>
#include <TPaveText.h>

#endif //  DQM4HEP_ROOTHEADERS_H
