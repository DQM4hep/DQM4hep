//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
// 
//  * ROOT macro to generate test samples.
//
// Run with :
//  root -l
//    [0] .x GenerateData.C
//    [1] TBrowser b # to browse histograms
//
// To generate the file only :
//  root -l -q GenerateData.C
//
// Author     : R.Ete
//====================================================================

void GenerateData()
{
  std::vector<TObject*> writeObject;
  TFile *pTFile = new TFile("test_samples.root", "RECREATE");

  TDirectory *pSubDir = pTFile->mkdir("Gaussians");
  pSubDir->cd();

  TDirectory *pSubDir2 = pTFile->mkdir("TGraphs");

  //
  // Generate TGraph
  //
  const int n1 = 10000;
  double x1[n1], y1[n1];
  const double mean1 = 0;
  const double stddev1 = 2;
  const double pi = 3.1415;
  const double en = 2.7183;
  for (unsigned int i=0 ; i<10000 ; i++) {
    x1[i] = -5+(i*0.001);
    y1[i] = (1/sqrt(2*pi*pow(stddev1,2)))*pow(en,-1*pow(x1[i]-mean1,2)/(2*pow(stddev1,2)));
  }

  TGraph *pGaus_Mean0_RMS2 = new TGraph(n1,x1,y1);
  writeObject.push_back(pGaus_Mean0_RMS2);

  pGaus_Mean0_RMS2->SetName("Gaus_Mean0_RMS2");
  pGaus_Mean0_RMS2->SetTitle("Random gaus (0, 2)");

  pSubDir2->Add(pGaus_Mean0_RMS2);


  //
  // Generate TGraph with offset mean
  //
  const int n2 = 10000;
  double x2[n2], y2[n2];
  const double mean2 = 3;
  const double stddev2 = 2;
  for (unsigned int i=0 ; i<10000 ; i++) {
    x2[i] = -5+(i*0.001);
    y2[i] = (1/sqrt(2*pi*pow(stddev2,2)))*pow(en,-1*pow(x2[i]-mean2,2)/(2*pow(stddev2,2)));
  }

  TGraph *pGaus_Mean3_RMS2 = new TGraph(n2,x2,y2);
  writeObject.push_back(pGaus_Mean3_RMS2);

  pGaus_Mean3_RMS2->SetName("Gaus_Mean3_RMS2");
  pGaus_Mean3_RMS2->SetTitle("Random gaus (3, 2)");

  pSubDir2->Add(pGaus_Mean3_RMS2);


  //
  // Generate random gaussian distribution
  //
  TH1F *pGaus_Mean10_RMS2 = new TH1F("Gaus_Mean10_RMS2", "Random gaus(10, 2)", 80, 0, 20);
  writeObject.push_back(pGaus_Mean10_RMS2);

  for(unsigned int i=0 ; i<10000 ; i++)
    pGaus_Mean10_RMS2->Fill(gRandom->Gaus(10, 2));


  //
  // Generate random gaussian distribution
  //
  TH1F *pGaus_Mean8_RMS2 = new TH1F("Gaus_Mean8_RMS2", "Random gaus(8, 2)", 80, 0, 20);
  writeObject.push_back(pGaus_Mean8_RMS2);

  for(unsigned int i=0 ; i<10000 ; i++) {
    if(rand() / float(RAND_MAX) > 0.8)
      pGaus_Mean8_RMS2->Fill(gRandom->Gaus(6, 1));
    else
      pGaus_Mean8_RMS2->Fill(gRandom->Gaus(10, 2));
  }

  //
  // Same as before but with uniform background
  //
  TH1F *pGaus_Mean10_RMS2_bck = new TH1F("Gaus_Mean10_RMS2_bck", "Random gaus(10, 2) with uniform background", 80, 0, 20);
  writeObject.push_back(pGaus_Mean10_RMS2_bck);

  for(unsigned int i=0 ; i<10000 ; i++)
    pGaus_Mean10_RMS2_bck->Fill(gRandom->Gaus(10, 2));

  for(unsigned int i=0 ; i<1000 ; i++)
    pGaus_Mean10_RMS2_bck->Fill(gRandom->Uniform(0, 20));


  //
  // Double gaussian with same mean but different RMS
  //
  TH1F *pDblGaus_Mean15_RMS2_RMS5 = new TH1F("DblGaus_Mean15_RMS2_RMS5", "Random gaus(15, 2)+gaus(15, 5)", 300, 0, 30);
  writeObject.push_back(pDblGaus_Mean15_RMS2_RMS5);

  for(unsigned int i=0 ; i<10000 ; i++)
  {
    pDblGaus_Mean15_RMS2_RMS5->Fill(gRandom->Gaus(15, 2));
    pDblGaus_Mean15_RMS2_RMS5->Fill(gRandom->Gaus(15, 5));
  }

  //
  // Random gaussian with exponential background
  //
  TH1F *pGaus_Mean15_RMS1_ExpBck = new TH1F("Gaus_Mean15_RMS1_ExpBck", "Random gaus(15, 1) + Exp background", 150, 0, 30);
  writeObject.push_back(pGaus_Mean15_RMS1_ExpBck);

  for(unsigned int i=0 ; i<10000 ; i++)
  {
    if(i%10==0)
      pGaus_Mean15_RMS1_ExpBck->Fill(gRandom->Gaus(15, 1));
    pGaus_Mean15_RMS1_ExpBck->Fill(gRandom->Exp(12));
  }

  pTFile->cd();

  //
  // Random Landau
  //
  TH1F *pLandau_Mean10_RMS2 = new TH1F("Landau_Mean10_RMS2", "Random landau(10, 2)", 300, 0, 30);
  writeObject.push_back(pLandau_Mean10_RMS2);

  for(unsigned int i=0 ; i<10000 ; i++)
    pLandau_Mean10_RMS2->Fill(gRandom->Landau(10, 2));


  //
  // Random exponential
  //
  TH1F *pExp_Dev5 = new TH1F("Exp_Dev5", "Random exp(-x/5)", 300, 0, 30);
  writeObject.push_back(pExp_Dev5);
  TH1F *pExp_Dev5_SmallBck = new TH1F("Exp_Dev5_SmallBck", "Random exp(-x/5) with small uniform background", 300, 0, 30);
  writeObject.push_back(pExp_Dev5_SmallBck);
  TH1F *pExp_Dev5_HugeBck = new TH1F("Exp_Dev5_HugeBck", "Random exp(-x/5) with huge uniform background", 300, 0, 30);
  writeObject.push_back(pExp_Dev5_HugeBck);

  for(unsigned int i=0 ; i<10000 ; i++)
  {
    pExp_Dev5->Fill(gRandom->Exp(5));
    pExp_Dev5_SmallBck->Fill(gRandom->Exp(5));
    pExp_Dev5_HugeBck->Fill(gRandom->Exp(5));
  }

  for(unsigned int i=0 ; i<1000 ; i++)
    pExp_Dev5_SmallBck->Fill(gRandom->Uniform(0, 30));

  for(unsigned int i=0 ; i<5000 ; i++)
    pExp_Dev5_HugeBck->Fill(gRandom->Uniform(0, 30));



  //
  // Random cos function and cos function in range [-0.8, 0.8]
  //
  TF1 *pCosFunction = new TF1("CosFunction", "cos(x)", -1, 1);
  TH1F *pCosDistribution = new TH1F("CosDistribution", "", 200, -1, 1);
  TH1F *pCosDistributionCut080 = new TH1F("CosDistributionCut080", "", 200, -1, 1);
  writeObject.push_back(pCosDistribution);
  writeObject.push_back(pCosDistributionCut080);

  for(unsigned int i=0 ; i<10000 ; i++)
  {
    pCosDistribution->Fill(pCosFunction->GetRandom());
    pCosDistributionCut080->Fill(pCosFunction->GetRandom(-0.8, 0.8));
  }

  delete pCosFunction;




  // //
  // // Write content to file
  // //
  // for(unsigned int i=0 ; i<writeObject.size() ; i++)
  // {
  //   TObject *pTObject = writeObject.at(i);
  //   pTObject->Write();
  // }

  pTFile->Write();
  pTFile->Close();
  delete pTFile;
}
