#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include <math.h>
#include <algorithm>
#include <list>
#include <iostream>
#include "TROOT.h"
using std::cin;
using std::cout;
using std::endl;
using namespace std;

// std::vector<double> factors6; // factors defined as a 26 component vector
float factors6[26]; // factors defined as a 26 component vector


// Give miscalibration constants for all the different scenarios
static const float factors_nomiscal[] = { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00}; //  no miscalibration yet or assign all miscalibration factors to 1


//corrections to no miscalibration case ie Corrections from Callibration process
static const float corrections_mc[] = {1.00, 1.00, 1.08696, 0.979119, 0.969235, 0.986741, 0.961573, 0.947524, 0.986307, 0.998243, 0.986077, 1.08743, 1.00, 1.00, 1.08399, 0.986606, 1.01257, 0.975674, 0.968379, 0.949467, 0.963255, 0.960102, 0.960654, 1.12329, 1.00, 1.00 };

static const float numbers[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};


void exposureResults(TFile* t) //, double& rms_ratio, double& fit_ratio, double& ietaRms_ratio, double& correctedRms_ratio)

 {

   gROOT->SetStyle("Plain");

  const float* corrections = 0;

  //    corrections = factors_nomiscal;   
  corrections = corrections_mc;   

  // what will these  new histograms contain?
  TH1* reco_pred=new TH1F("RecoPred","Reco/Pred",180,0,2); // first make a histogram of just calibration constants or ratio of reco/pred
  reco_pred->SetDirectory(0);
  TH1* reco_pred_oexp=new TH1F("RecoPredMultExp","Reco/Pred*Exp",180,0,2);  // histogram with calibration constant multiplied by expression, which exp?
  reco_pred_oexp->SetDirectory(0);
  TH1* reco_pred_mexp=new TH1F("RecoPredMinusExp","Reco/Pred-Exp",180,-1,1); // histogram with calib constant minus  by miscalib factors
  reco_pred_mexp->SetDirectory(0);
  //TH1* reco_pred_norm=new TH1F("RecoPredNorm","Reco/Pred/Error",40,0,40);
  //reco_pred_norm->SetDirectory(0);
  //TH1* reco_pred_oexp_norm=new TH1F("RecoPredOverExpNorm","Reco/Pred/Exp/Error",40,0,40);
  //reco_pred_oexp_norm->SetDirectory(0);
  //TH1* reco_pred_mexp_norm=new TH1F("RecoPredMinusExpNorm","Reco/Pred-Exp/Error",40,-10,10);
  //reco_pred_mexp_norm->SetDirectory(0);
  TH1* reco_pred_ietaRatio=new TH1F("RecoPredIetaRatio","Reco/Pred +ieta/-ieta",180,0,2);  // histogram with reco.pred ratio +/- ieta rati
  reco_pred_ietaRatio->SetDirectory(0);
  TH1* reco_pred_oexp_ietaRatio=new TH1F("RecoPredMultExpIetaRatio","Reco/Pred*Exp +ieta/-ieta",180,0,2); // histogram with reco/pred multiplied by ieta ratio
  reco_pred_oexp_ietaRatio->SetDirectory(0);
  // TH1* reco_pred_mexp_ietaRatio=new TH1F("RecoPredMinusExpIetaRatio","Reco/Pred-Exp +ieta/-ieta",180,0,2);
  //reco_pred_mexp_ietaRatio->SetDirectory(0);
  TH1* reco_pred_oexp_corrected=new TH1F("RcoPredMultExpCorrected","Reco/Pred*Exp with correction at 200 pb-1",180,0,2); // correction ratio at maximum events or luminocity? but what do we mean by corrected? Ratio reco/pred times Corrections. for diff ieta towers
  reco_pred_oexp_corrected->SetDirectory(0);

  std::cout << "Histograms created" << std::endl ; 

  double x[26],ex[26];
  double y[26],yc[26],ey[26];
  double yRatio[26],yDifference[26];
  double ietaPlus[13],ietaMinus[13];

  for (int i=0; i<26; i++) {
    x[i]=0; y[i]=0; yc[i]=0; ex[i]=0; ey[i]=0; yRatio[i]=0; yDifference[i]=0; 
  }
  for (int i=0; i<13; i++) {
    ietaPlus[i]=0; ietaMinus[i]=0; 
  }
  std::cout <<" proper initialization" << std::endl;
  char name[128];

  for (int signeta=-1; signeta<=1; signeta+=2) { // gives -1/+1

    sprintf(name,"c%d",signeta+3);
    TCanvas *cfit=new TCanvas(name,name,1000,750);
    cfit->Divide(4,3);
    
    for (int absieta=29; absieta<42; absieta++) {
      int ieta=signeta*absieta;

      cfit->cd(absieta-29+1); 

      // index is -41..-29,29,..41 linearly
      int index=(signeta<0)?(41-absieta):(absieta-16);
      

      sprintf(name,"calib/delExpCanHF%dEnergy",ieta);  //Collect in Order of such names all the Histograms in <filename>.root file

      std::cout << "Read data from Histograms" << std::endl;
      TH1F* h1 = (TH1F*)t->Get(name); 

      x[index]=index;
      
      if (h1->GetEntries()<2) continue; 

      TF1* f1=new TF1("f1","gaus"); 
      h1->Fit(f1,"L");
      double mean = f1->GetParameter(1);

 
      std ::cout << " Read the mean of fitted   histograms" << std::endl;
      x[index]=index;
      y[index]=mean;     
      yc[index]=mean*corrections[index];   
      //??      ietaPlus[i-29]=mean*corrections[index];
      ey[index]=f1->GetParError(1);
      if (ey[index]<y[index]*0.1/sqrt(h1->GetEntries()))
	ey[index]=y[index]*0.1/sqrt(h1->GetEntries());

      ex[index]=0.5;
      yRatio[index]=mean*corrections[index];  //divide mean by corresponding input Miscalibration factor of ieta tower
      yDifference[index]=mean;
      std::cout << "  Redefined the Miscal factors" << std::endl;
      if (h1->GetEntries()>10)	{
      // this is actually where I need to  get new Corrections to no miscal and factors.
	reco_pred->Fill(1/mean); // inverse of  mean is the calibration factor
	reco_pred_oexp->Fill(1/yRatio[index]); // fill  histogram with distrubution of inverse of new mean ie Calibration constants
	reco_pred_mexp->Fill(1/yDifference[index]);
	reco_pred_oexp_corrected->Fill(yRatio[index]*corrections[index]); // New corrected miss calibration factor = original  Ereco/Epred times Corrections
	//reco_pred_norm->Fill(mean/ey[i-16]);
	//reco_pred_oexp_norm->Fill(mean/factors[i-16]/ey[i-16]);
	//reco_pred_mexp_norm->Fill((mean-factors[i-16])/ey[i-16]);
	
      } // end of if statement
 
    } // end of for statement 
  }

  std::cout << "  Now make Graphs" << std::endl;
// plotting graphs for seperate miscal scen   
  TGraphErrors* tge1=new TGraphErrors(26,x,y,ex,ey);
  TGraphErrors* tge2=new TGraphErrors(26,x,yRatio,ex,ey);
  TGraphErrors* tge3=new TGraphErrors(26,x,yDifference,ex,ey);

  tge1->SetMaximum(1.5);
  tge1->SetMinimum(0.5);
  tge1->SetMarkerStyle(20);
  tge2->SetMaximum(1.5);
  tge2->SetMinimum(0.5);
  tge2->SetMarkerStyle(20);
  tge3->SetMaximum(0.5);
  tge3->SetMinimum(-0.5);
  tge3->SetMarkerStyle(20);
    
  TCanvas *MyC = new TCanvas("MyC","Test Canvas",900,600);
  MyC->Divide(2,2);
  TCanvas *MyC1 = new TCanvas("MyC1","Test Canvas",900,600);

  TH2* dummy1=new TH2F("dummy1","",26,-0.5,25.5,10,0.5,1.5);
  TH2* dummy2=new TH2F("dummy2","",26,-0.5,25.5,10,0.5,1.5);

  dummy1->SetStats(0);
  dummy1->GetXaxis()->SetTitle("Ieta");
  dummy1->GetXaxis()->CenterTitle();
  dummy1->GetYaxis()->SetTitle("Mean Reco/Pred HF Energy");
  dummy1->GetYaxis()->CenterTitle();
  dummy1->GetXaxis()->SetBinLabel(3,"-39");
  dummy1->GetXaxis()->SetBinLabel(24,"39");
  dummy1->GetXaxis()->SetBinLabel(3+4,"-35");
  dummy1->GetXaxis()->SetBinLabel(24-4,"35");
  dummy1->GetXaxis()->SetBinLabel(3+9,"-30");
  dummy1->GetXaxis()->SetBinLabel(24-9,"30");


  dummy2->GetXaxis()->SetTitle("Ieta");
  dummy2->GetXaxis()->CenterTitle();
  dummy2->GetYaxis()->SetTitle("Mean Reco/Pred HF Energy (MC Corrected)");
  dummy2->GetYaxis()->CenterTitle();
  dummy2->GetXaxis()->SetBinLabel(3,"-39");
  dummy2->GetXaxis()->SetBinLabel(24,"39");
  dummy2->GetXaxis()->SetBinLabel(3+4,"-35");
  dummy2->GetXaxis()->SetBinLabel(24-4,"35");
  dummy2->GetXaxis()->SetBinLabel(3+9,"-30");
  dummy2->GetXaxis()->SetBinLabel(24-9,"30");

  MyC1->cd(1);
  reco_pred->Draw("HIST");
  reco_pred->GetXaxis()->SetTitle("Mean of Reco/Pred HF Energy");
  reco_pred->GetXaxis()->CenterTitle();
  reco_pred->GetYaxis()->SetTitle("Entries");
  reco_pred->GetYaxis()->CenterTitle();
  MyC->cd(1);
  dummy1->Draw("");
  tge1->Draw("SAMEP");
  MyC->cd(2);
  reco_pred->Draw("HIST");
  MyC->cd(3);
  dummy2->Draw("");
  tge2->Draw("SAMEP");
  MyC->cd(4);
  reco_pred_oexp->Draw("HIST");
  //  float rms=reco_pred_oexp->TH1::GetRMS(1);   //   get  RMS of every graph
  //float fit=reco_pred_oexp->TH1::GetMean(1);
  TF1* g1=new TF1("g1","gaus");
  reco_pred_oexp->Fit(g1,"L");          //  fit a     new  gaussian on this graph and get the  fit parameter
  //float fit = g1->GetParameter(2);
  //reco_pred_oexp->SetOptStat("mr");


for (int i=0; i<26; i++) // for each ieta tower
 {
   if (yRatio[i] == 0)
    cout << i << " " << 0 << endl; //    yRatio is the  new Mean while 1/yRatio the NewFactors/Calibration cosntants
    else cout << i << " " << 1/yRatio[i] << endl;
 }
}   
      
void exposureData(TFile* t)

{
  /*
  int mode;
  double result1;
  double result2;
  double result3;
  double result4;
  int pileup ;

  */
  exposureResults(t);//,0,0,pileup,result1,result2,result3,result4);
}


