
// execute: 
// gSystem->Load("../lib/libDQMCore.so"); 
// before running the macro
// Example from local directory : 
// $ root -l
// root [0] gSystem->Load("../lib/libDQMCore.so"); 
// root [1] .x DrawStyleThemes.C


#include "../include/dqm4hep/RootStyle.h"

void DrawStyleThemes() {
  
  dqm4hep::core::RootStyle objectStyle, referenceStyle;
  std::vector<std::string> themes = {
    dqm4hep::core::RootStyle::Theme::DEFAULT,
    dqm4hep::core::RootStyle::Theme::THICK,
    dqm4hep::core::RootStyle::Theme::POLAR,
    dqm4hep::core::RootStyle::Theme::SUNRISE,
    dqm4hep::core::RootStyle::Theme::FOREST
  };
  TH1F *object = new TH1F("object", "Object", 101, -10, 10);
  TH1F *reference = new TH1F("reference", "Reference", 101, -10, 10);
  TRandom rnd;
  
  for(int i=0 ; i<10000 ; i++) {
    object->Fill(rnd.Gaus(0, 2));
  }
  
  for(int i=0 ; i<10000 ; i++) {
    reference->Fill(rnd.Gaus(2, 2));
  }
  
  for(auto theme : themes) {
    TCanvas *canvas = new TCanvas((theme + "_theme").c_str(), (theme + "_theme").c_str());
    canvas->cd();
    
    dqm4hep::core::RootStyle::builtinStyle(theme, objectStyle, referenceStyle);
    objectStyle.applyTo(object);
    referenceStyle.applyTo(reference);
    
    object->Draw();
    reference->Draw("same");
    
    TLegend *legend = canvas->BuildLegend(0.1, 0.7, 0.4, 0.9, ("Theme: " + theme).c_str());
    
    canvas->SaveAs( ("Theme_" + theme + ".png").c_str() );
    canvas->SaveAs( ("Theme_" + theme + ".pdf").c_str() );
    delete canvas;
  }
  
  delete object;
  delete reference;
}