/**
 * ROOT macro to generate test samples.
 *
 * Run with :
 *   root -l
 *   [0] .x GenerateData.C
 *   [1] TBrowser b # to browse histograms
 *
 * To generate the file only :
 *   root -l -q GenerateData.C
 *
 * @author Remi Ete, DESY
 */

void GenerateData()
{
  std::vector<TObject*> writeObject;
  TFile *pTFile = new TFile("test_samples.root", "RECREATE");

  TDirectory *pSubDir = pTFile->mkdir("Gaussians");
  pSubDir->cd();

  //
  // Generate random gaussian distribution
  //
  TH1F *pGaus_Mean10_RMS2 = new TH1F("Gaus_Mean10_RMS2", "Random gaus(10, 2)", 80, 0, 20);
  writeObject.push_back(pGaus_Mean10_RMS2);

  for(unsigned int i=0 ; i<10000 ; i++)
    pGaus_Mean10_RMS2->Fill(gRandom->Gaus(10, 2));


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
