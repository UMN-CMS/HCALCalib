
static const float numbers[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
static const float widths[] = {0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5};

static const float corrections_mc[] = {1.0, 1.0, 1.054, 1.027, 1.072, 1.046, 1.056, 1.039, 1.039, 1.034, 1.041, 1.110, 1.0, 
				       1.0, 1.160, 1.027, 1.004, 1.051, 1.015, 1.014, 1.013, 1.040, 1.016, 1.012, 1.0, 1.0};

#include "TMath.h"
#include "TFile.h"
#include "TROOT.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooGlobalFunc.h"
#include "RooNumConvPdf.h"

double MCpred(TH1* mcdist, double sf, double x) {
  double effx=x*sf;
  int ibin=mcdist->FindBin(effx);
  double x2,x1=mcdist->GetBinCenter(ibin);
  double y2,y1=mcdist->GetBinContent(ibin);

  if (effx<mcdist->GetBinCenter(ibin)) {
    x2=mcdist->GetBinCenter(ibin-1);
    y2=mcdist->GetBinContent(ibin-1);
  } else {
    x2=mcdist->GetBinCenter(ibin+1);
    y2=mcdist->GetBinContent(ibin+1);
  }
  return (effx-x1)*(y2-y1)/(x2-x1)+y1;
}


const double factorsRelVertex[] = { 1.0, 1.0, 1.041, 1.065, 1.074,
1.113, 1.089, 1.057, 1.084, 1.081, 1.089, 1.178, 1.0,
1.0, 1.157, 1.063, 1.040, 1.080, 1.051, 1.047, 1.023, 1.067, 0.995,
1.017, 1.0, 1.0};

void studyLongShort(TFile* dataf, TFile* mcf, bool doFit=true) {
  gROOT->SetStyle("Plain");
  char name[1025];

  float longF[26], longFCorr[26], slData[26],slMC[26];
  float slDataError[26],slMCError[26];
  float corrRatio[26],rawRatio[26];
  float corrRatioErrors[26],rawRatioErrors[26];
  TH1* rawRatioH=new TH1F("RawRatioH","RawRatioH",20,0.8,1.2);
  TH1* corrRatioH=new TH1F("CorrRatioH","CorrRatioH",20,0.8,1.3);

  TF1* f1=new TF1("f1","[0]*(x-[1])*(x-[1])+[2]",0.8,1.19);

  for (int signeta=-1; signeta<=1; signeta+=2) { // gives -1/+1
    
    sprintf(name,"c%d",signeta+3);
    TCanvas *cfit=new TCanvas(name,name,1200,950);
    cfit->Divide(4,3);
      
    for (int absieta=30; absieta<40; absieta++) { // 29 doesn't calibrate this way
      int ieta=signeta*absieta;
      //      if (ieta!=33) continue;
      int index=(signeta<0)?(41-absieta):(absieta-16);
      cfit->cd(absieta-29);
 
      char hname[50],htitle[50];
      sprintf(hname,"Tower%d",ieta);
      sprintf(htitle,"Tower %d",ieta);


      longF[index]=factorsRelVertex[index]/corrections_mc[index];
      longFCorr[index]=factorsRelVertex[index];
      
      // got the final long fiber number now.

      sprintf(hname,"TowerSL%d",ieta);
      sprintf(htitle,"TowerSL %d",ieta);

      sprintf(name,"calib/ShortLongRatio%d",ieta);  //Collect in Order of such names all the Histograms in <filename>.root file

      TH1* sldata=(TH1F*)(dataf->Get(name)->Clone(hname));

      sldata->Rebin(4);

      sprintf(hname,"TowerSL%d",ieta);
      sprintf(htitle,"TowerSL %d",ieta);

      TH1* slmc=(TH1F*)(mcf->Get(name)->Clone(hname));

      slmc->Rebin(4);

      if (slmc->GetEntries()>0) {
	slmc->Scale(sldata->GetEntries()/slmc->GetEntries());

	TGraph tool;
	int np=0;
	double lowest=10000, lowestsf=1.0;	
	for (double sf=0.8; sf<=1.2; sf+=0.02) {
	  double chi2=0;
	  int ndof=0;
	 
	  for (int i=1; i<=sldata->GetNbinsX(); i++) {
	    double nd=sldata->GetBinContent(i);
	    if (nd<20) continue;
	    
	    double MCP=MCpred(slmc,sf,sldata->GetBinCenter(i));
	    
	    chi2+=pow(nd-MCP,2)/nd;
	    
	    ndof++;
	  }
	  if (chi2<lowest) {
	    lowest=chi2;
	    lowestsf=sf;
	  }
	  if (ndof>4) {
	    tool.SetPoint(np++,sf,chi2);
	    printf("%f %f %f\n",sf,chi2,lowest);
	  }
	}
	if (np>4) {
	  f1->SetParameter(0,10);
	  f1->SetParameter(1,1.0);
	  f1->SetParameter(2,lowest);
	  f1->SetParLimits(0,5,1000000);
	  f1->SetParLimits(1,0.9,1.2);
	  f1->SetParLimits(2,10.0,100000);
	  tool.Fit(f1,"","",lowestsf-0.05,lowestsf+0.05);
	  rawRatio[index]=f1->GetParameter(1);
	  rawRatioErrors[index]=sqrt(1.0/f1->GetParameter(0));
	} else {
	  rawRatio[index]=-1;
	  rawRatioErrors[index]=0;
	}
      }

      sldata->SetMarkerStyle(24);

      sldata->Draw("E0");
      //      f2->Draw("SAME");
      slmc->Draw("HISTSAME");
      //      f3->Draw("SAME");

      corrRatio[index]=rawRatio[index]/longFCorr[index];
      corrRatioErrors[index]=rawRatioErrors[index];

      rawRatioH->Fill(rawRatio[index]);
      corrRatioH->Fill(corrRatio[index]);

      printf("%d %f %f %f %f\n",ieta,rawRatio[index],rawRatioErrors[index],longFCorr[index],corrRatio[index]);
      //      return;
    }
    if (signeta>0) {
      cfit->Print("sl_mc_fit_results_posieta.pdf");
      cfit->Print("sl_mc_fit_results_posieta.eps");
      cfit->Print("sl_mc_fit_results_posieta.png");
    } else {
      cfit->Print("sl_mc_fit_results_negieta.pdf");
      cfit->Print("sl_mc_fit_results_negieta.eps");
      cfit->Print("sl_mc_fit_results_negieta.png");
    }
  }

  TH2* dummy1=new TH2F("dummy1","",26,-0.5,25.5,10,0.70,1.25);
  TH2* dummy2=new TH2F("dummy2","",26,-0.5,25.5,10,0.70,1.25);

  dummy1->SetStats(0);
  dummy1->GetXaxis()->SetTitle("Ieta");
  dummy1->GetXaxis()->CenterTitle();
  dummy1->GetYaxis()->SetTitle("Reco SL Ratio/MC SL Ratio");
  dummy1->GetYaxis()->CenterTitle();
  dummy1->GetXaxis()->SetBinLabel(3,"-39");
  dummy1->GetXaxis()->SetBinLabel(24,"39");
  dummy1->GetXaxis()->SetBinLabel(3+4,"-35");
  dummy1->GetXaxis()->SetBinLabel(24-4,"35");
  dummy1->GetXaxis()->SetBinLabel(3+9,"-30");
  dummy1->GetXaxis()->SetBinLabel(24-9,"30");

  dummy2->SetStats(0);
  dummy2->GetXaxis()->SetTitle("Ieta");
  dummy2->GetXaxis()->CenterTitle();
  dummy2->GetYaxis()->SetTitle("Reco SL/MC SL (LF Calibrated)");
  dummy2->GetYaxis()->CenterTitle();
  dummy2->GetXaxis()->SetBinLabel(3,"-39");
  dummy2->GetXaxis()->SetBinLabel(24,"39");
  dummy2->GetXaxis()->SetBinLabel(3+4,"-35");
  dummy2->GetXaxis()->SetBinLabel(24-4,"35");
  dummy2->GetXaxis()->SetBinLabel(3+9,"-30");
  dummy2->GetXaxis()->SetBinLabel(24-9,"30");


  TCanvas* c3= new TCanvas("c3","c3",800,800);

  c3->Divide(2,2);

  c3->cd(1);

  TGraphErrors* tge1=new TGraphErrors(26,numbers,rawRatio,widths,rawRatioErrors);
  dummy1->Draw();
  tge1->SetMaximum(1.7);
  tge1->SetMinimum(0.7);
  tge1->SetMaximum(1.5);
  tge1->SetMinimum(0.5);
  tge1->SetMarkerStyle(20);
 
  tge1->Draw("P");

  c3->cd(2);

  TGraphErrors* tge2=new TGraphErrors(26,numbers,corrRatio,widths,corrRatioErrors);
  dummy2->Draw();
  tge2->SetMaximum(1.7);
  tge2->SetMinimum(0.7);
  tge2->SetMaximum(1.5);
  tge2->SetMinimum(0.5);
  tge2->SetMarkerStyle(20);

  tge2->Draw("P");

  c3->cd(3);

  rawRatioH->Draw("HIST");

  c3->cd(4);

  corrRatioH->Draw("HIST");

  c3->Print("sl_mc_data_comparison.pdf");
  c3->Print("sl_mc_data_comparison.eps");
  c3->Print("sl_mc_data_comparison.png");

 
  for (int signeta=-1; signeta<=1; signeta+=2) { // gives -1/+1
    
     for (int absieta=30; absieta<40; absieta++) { // 29 doesn't calibrate this way
      int ieta=signeta*absieta;
      int index=(signeta<0)?(41-absieta):(absieta-16);

      printf("%d,%.3f,%.3f\n",ieta,corrRatio[index],corrRatioErrors[index]);
     }     
  }

 
}
