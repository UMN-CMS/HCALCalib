#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TLegend.h"
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

static const float numbers[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
static const float widths[] = {0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5};

static const float corrections_mcs[] = {1.0, 1.0, 1.054, 1.027, 1.072, 1.046, 1.056, 1.039, 1.039, 1.034, 1.041, 1.110, 1.0, 
				       1.0, 1.160, 1.027, 1.004, 1.051, 1.015, 1.014, 1.013, 1.040, 1.016, 1.012, 1.0, 1.0};

#include "TMath.h"
#include "TFile.h"
#include "TROOT.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraphErrors.h"

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

/*void shortLongCalib(TFile* dataf, TFile* mcf, double outputRatios[26], double outputRatioErrors[26]) {
  for(int i = 0; i < 26; i++){
    outputRatios[i] = 1.0;
    outputRatioErrors[i] = 1.0;
  }

  gROOT->SetStyle("Plain");
  char name[1025];

  float longF[26], longFCorr[26];
  float corrRatio[26],rawRatio[26];
  float corrRatioErrors[26],rawRatioErrors[26];
  TH1* rawRatioH=new TH1F("RawRatioH","RawRatioH",20,0.8,1.2);
  TH1* corrRatioH=new TH1F("CorrRatioH","CorrRatioH",20,0.8,1.3);

  TF1* f1=new TF1("f1","[0]*(x-[1])*(x-[1])+[2]",0.8,1.19);

  for (int signeta=-1; signeta<=1; signeta+=2) { // gives -1/+1
    
    sprintf(name,"c%d",signeta+3);
    //TCanvas *cfit=new TCanvas(name,name,1200,950);
    //cfit->Divide(4,3);
      
    for (int absieta=30; absieta<40; absieta++) { // 29 doesn't calibrate this way
      int ieta=signeta*absieta;
      //      if (ieta!=33) continue;
      int index=(signeta<0)?(41-absieta):(absieta-16);
      //cfit->cd(absieta-29);
 
      char hname[50],htitle[50];
      sprintf(hname,"Tower%d",ieta);
      sprintf(htitle,"Tower %d",ieta);


      longF[index]=factorsRelVertex[index]/corrections_mcs[index];
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
	double lowest=10000;
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
	  if (chi2<lowest) lowest=chi2;
	  if (ndof>4) {
	    tool.SetPoint(np++,sf,chi2);
	    //printf("%f %f %f\n",sf,chi2,lowest);
	  }
	}
	if (np>4) {
	  f1->SetParameter(0,10);
	  f1->SetParameter(1,1.0);
	  f1->SetParameter(2,lowest);
	  f1->SetParLimits(0,5,1000000);
	  f1->SetParLimits(1,0.9,1.2);
	  f1->SetParLimits(2,10.0,100000);
	  tool.Fit(f1, "Q");
	  rawRatio[index]=f1->GetParameter(1);
	  rawRatioErrors[index]=sqrt(1.0/f1->GetParameter(0));
	} else {
	  rawRatio[index]=-1;
	  rawRatioErrors[index]=0;
	}
      }

      sldata->SetMarkerStyle(24);

      //sldata->Draw("E0");
      //      f2->Draw("SAME");
      //slmc->Draw("HISTSAME");
      //      f3->Draw("SAME");

      corrRatio[index]=rawRatio[index]/longFCorr[index];
      corrRatioErrors[index]=rawRatioErrors[index];

      rawRatioH->Fill(rawRatio[index]);
      corrRatioH->Fill(corrRatio[index]);

      //printf("%d %f %f %f %f\n",ieta,rawRatio[index],rawRatioErrors[index],longFCorr[index],corrRatio[index]);
      //      return;
    }
    if (signeta>0) {
      //cfit->Print("sl_mc_fit_results_posieta.pdf");
      //cfit->Print("sl_mc_fit_results_posieta.eps");
      //cfit->Print("sl_mc_fit_results_posieta.png");
    } else {
      //cfit->Print("sl_mc_fit_results_negieta.pdf");
      //cfit->Print("sl_mc_fit_results_negieta.eps");
      //cfit->Print("sl_mc_fit_results_negieta.png");
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


  //TCanvas* c3= new TCanvas("c3","c3",800,800);

  //c3->Divide(2,2);

  //c3->cd(1);

  TGraphErrors* tge1=new TGraphErrors(26,numbers,rawRatio,widths,rawRatioErrors);
  //dummy1->Draw();
  tge1->SetMaximum(1.7);
  tge1->SetMinimum(0.7);
  tge1->SetMaximum(1.5);
  tge1->SetMinimum(0.5);
  tge1->SetMarkerStyle(20);
 
  //tge1->Draw("P");

  //c3->cd(2);

  TGraphErrors* tge2=new TGraphErrors(26,numbers,corrRatio,widths,corrRatioErrors);
  //dummy2->Draw();
  tge2->SetMaximum(1.7);
  tge2->SetMinimum(0.7);
  tge2->SetMaximum(1.5);
  tge2->SetMinimum(0.5);
  tge2->SetMarkerStyle(20);

  //tge2->Draw("P");

  //c3->cd(3);

  //rawRatioH->Draw("HIST");

  //c3->cd(4);

  //corrRatioH->Draw("HIST");

  //c3->Print("sl_mc_data_comparison.pdf");
  //c3->Print("sl_mc_data_comparison.eps");
  //c3->Print("sl_mc_data_comparison.png");

 
  for (int signeta=-1; signeta<=1; signeta+=2) { // gives -1/+1
    
     for (int absieta=30; absieta<40; absieta++) { // 29 doesn't calibrate this way
      int ieta=signeta*absieta;
      int index=(signeta<0)?(41-absieta):(absieta-16);

      //printf("%d,%.3f,%.3f\n",ieta,corrRatio[index],corrRatioErrors[index]);
      outputRatios[index] = corrRatio[index];
      outputRatioErrors[index] = (corrRatioErrors[index]>=0)?corrRatioErrors[index]:-corrRatioErrors[index];
     }     
  }

 
  }*/


//Non-Vertex Numbers for the sake of comparison with the vertex fits
//const double factorsRelMC[] = {1.003, 1.022, 1.042, 1.048, 1.051, 1.032, 1.036, 1.036, 1.042, 1.142, 1.142, 1.025, 0.998, 1.037, 1.009, 0.996, 0.985, 1.031, 0.980, 0.974};
const int ietaArray [] = {	-39	,	-38	,	-37	,	-36	,	-35	,	-34	,	-33	,	-32	,	-31	,	-30	,	30	,	31	,	32	,	33	, 34, 	35	,	36	,	37	,	38	,	39	};
//const int sizeArray [] = {	3	,	6	,	7	,	8	,	9	,	10	,	10	,	10	,	11	,	10	,	10	,   	11	,	10	,	9	, 9,	9	,	8	,	8	,	5	,	3	};



// std::vector<double> factors6; // factors defined as a 26 component vector
float factors6[26]; // factors defined as a 26 component vector


// Give miscalibration constants for all the different scenarios
static const float factors_nomiscal[] = { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00}; //  no miscalibration yet or assign all miscalibration factors to 1


//corrections to no miscalibration case ie Corrections from Callibration process
// original static const float corrections_mc[] = {1.00, 1.00, 1.08696, 0.979119, 0.969235, 0.986741, 0.961573, 0.947524, 0.986307, 0.998243, 0.986077, 1.08743, 1.00, 1.00, 1.08399, 0.986606, 1.01257, 0.975674, 0.968379, 0.949467, 0.963255, 0.960102, 0.960654, 1.12329, 1.00, 1.00 };

// Fall10 
// older static const float corrections_mc[] = {1.00, 1.00, 1.08721, 1.00361, 1.00494, 0.999059, 1.00132, 0.998315, 1.01525, 1.03643, 1.02008, 1.10051, 1.00, 1.00, 1.10069, 1.02149, 1.03306, 1.0145, 0.9999745, 1.00531, 1.00052, 1.00288, 1.02014, 1.07499, 1.00, 1.00 };

// 2011 Numbers
static const float corrections_mc[] = { 1.0, 1.0, 1.044, 1.011, 0.995, 0.990, 0.988, 0.985, 0.993, 1.014, 0.991, 1.086, 1.0, 1.0, 1.088, 0.993, 1.013, 0.993, 0.986, 0.980, 0.987, 0.996, 1.005, 1.052, 1.0, 1.0};

//static const float numbers[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};


void exposureResults(TFile* t, int startVertex, int numberOfVertices, bool allVertex,  double* outputRatios, double* outputRatioErrors, int* populations) //, double& rms_ratio, double& fit_ratio, double& ietaRms_ratio, double& correctedRms_ratio)

 {

   //cout << "S: " << startVertex << ", N: " << numberOfVertices << endl;

   gROOT->SetStyle("Plain");

  const float* corrections = 0;

  //    corrections = factors_nomiscal;   
  corrections = corrections_mc;   

  //std::cout << "Histograms created" << std::endl ; 

  // double x[26],ex[26];
  double y[26],ey[26],eyc[26];
  double yRatio[26]; //,yDifference[26],yRatioRaw[26];
   //  double ietaPlus[13],ietaMinus[13];
  //int populations[20];

  for (int i=0; i<26; i++) {
    y[i]=0; ey[i]=0; eyc[i]=0; yRatio[i]=0;
    //yDifference[i]=0; 
    // yRatioRaw[i]=0;
  }
  /*
  for (int i=0; i<13; i++) {
    ietaPlus[i]=0; ietaMinus[i]=0; 
  }
  */
  //std::cout <<" proper initialization" << std::endl;
  char name[128];

  for (int signeta=-1; signeta<=1; signeta+=2) { // gives -1/+1

    sprintf(name,"c%d",signeta+3);
    //TCanvas *cfit=new TCanvas(name,name,1000,750);
    //cfit->Divide(4,3);
    
    for (int absieta=30; absieta<42; absieta++) { // 29 doesn't calibrate this way
      int ieta=signeta*absieta;

      //cfit->cd(absieta-29); 

      // index is -41..-29,29,..41 linearly
      int index=(signeta<0)?(41-absieta):(absieta-16);

      if(!allVertex){
	if(startVertex >= 10){
	  sprintf(name,"v%d/delExpCanHF%dEnergy",startVertex,ieta);
	}else if(startVertex < 10){
	  sprintf(name,"v0%d/delExpCanHF%dEnergy",startVertex,ieta);
	}
      }else if(allVertex){
	sprintf(name,"vall/delExpCanHF%dEnergy",ieta);
      }
      
      TH1F * h1 = (TH1F*)t->Get(name);

      char hname[50],htitle[50];
      sprintf(hname,"Tower%d",ieta);
      sprintf(htitle,"Tower %d",ieta);

      h1=(TH1F*)(h1->Clone(hname));
      h1->SetTitle(htitle);
      
      if(numberOfVertices > 0 && !allVertex)
	for(int v = startVertex + 1; v <= startVertex + numberOfVertices; v++){
	  
	  //	sprintf(name,"calib/delExpCanHF%dEnergy",v,ieta);  //Collect in Order of such names all the Histograms in <filename>.root file
	  if(v >= 10){
	      sprintf(name,"v%d/delExpCanHF%dEnergy",v,ieta);
	    }else if(v < 10){
	      sprintf(name,"v0%d/delExpCanHF%dEnergy",v,ieta);
	    }
	    
	    //std::cout << "Read data from Histograms" << std::endl;
	    h1->Add((TH1F*)t->Get(name));
	    //cout << "Vertex: " << v << " ";
	}
      

      //      x[index]=index;

      if(absieta < 40){
	//std::cout << ieta << ": " << h1->GetEntries() << ", ";
	if(ieta < 0){
	  populations[ieta + 39] = (int) h1->GetEntries();
	}else if(ieta > 0){
	  populations[ieta - 20] = (int) h1->GetEntries();
	}
      }

      if (h1->GetEntries()<2) continue; 


      TF1 f1("f1","gaus",0.6,1.4); 
      h1->Fit(&f1,"L R Q N"); //Added R and Q
      //h1->Draw();
      h1->GetXaxis()->SetRangeUser(0.25,1.75);
      double mean = f1.GetParameter(1);

 
      //     std ::cout << " Read the mean of fitted   histograms" << std::endl;
      //x[index]=index;
      y[index]=mean;     
      //yc[index]=mean*corrections[index];   
      //??      ietaPlus[i-29]=mean*corrections[index];
      ey[index]=f1.GetParError(1);
      if (ey[index]<y[index]*0.1/sqrt(h1->GetEntries()))
	ey[index]=y[index]*0.1/sqrt(h1->GetEntries());
      eyc[index]=ey[index]*corrections[index];

      // ex[index]=0.5;
      //yRatioRaw[index]=mean;  //divide mean by corresponding input Miscalibration factor of ieta tower
      yRatio[index]=mean*corrections[index];  //divide mean by corresponding input Miscalibration factor of ieta tower
      //      yDifference[index]=mean;

      delete h1; // done with this!
    } // end of for statement 

    //cout << std::endl;

  }



  for (int i=0; i<26; i++) {
    //if (yRatio[i]==0) printf("1.0");
    //else printf("%.3f",1/yRatio[i]);
    //if (i!=25) printf(", ");
    //if (i==12) printf("\n");
    if (yRatio[i]!=0) outputRatios[i] = 1/yRatio[i];
    if (yRatio[i]==0) outputRatios[i] = 1.0;
  }
  //printf("};\n");
  
  //printf("const double factorsRelMCErrors[] = { ");
  for (int i=0; i<26; i++) {
    //if (yRatio[i]==0) printf("1.0");
    //else printf("%.3f",eyc[i]/pow(yRatio[i],2));
    //if (i!=25) printf(", ");
    //if (i==12) printf("\n");
    if (yRatio[i]!=0) outputRatioErrors[i] = eyc[i]/pow(yRatio[i],2);
    if (yRatio[i]==0) outputRatioErrors[i] = 1.0;
  }
 }

void exposureDataVertices(TFile * t) //TFile* t)
  
{
  //starting vertex (35 is all), number of vertices, ieta
  double correctionRatios[36][35][26];
  double correctionRatioErrors[36][35][26];
  //  double correctionRatiosShort[36][35][26];
  // double correctionRatioErrorsShort[36][35][26];
  int populations[36][35][20];
  //TFile * noCut;
  //TFile * files[12][12];
  //char filePath[128];
  int i, j, k;

  // TFile * mcFile = TFile::Open("/local/cms/user/hanson/MC_Iteration1_Combined/MC_Iteration1.root");

  for(i = 0; i < 36; i++)
    for(j = 0; j < 35; j++)
      for(k = 0; k < 26; k++)
	{
	  correctionRatios[i][j][k] = 0.0;
	  correctionRatioErrors[i][j][k] = 0.0;
	  if (k<20) populations[i][j][k] = 0;
	  //correctionRatiosShort[i][j][k] = 0.0;
	  //correctionRatioErrorsShort[i][j][k] = 0.0;
	}

  // /local/cms/user/hanson/VertexCuts2D35/merged/VertexCuts2D35_??.root
  //cout << "Here." << endl;

  //sprintf(filePath, "/local/cms/user/hanson/VertexCuts2D35/merged/VertexCuts2D35_0.root");
  //noCut = TFile::Open(filePath);
  exposureResults(t, 0, 0, true, correctionRatios[35][0], correctionRatioErrors[35][0], populations[35][0]);
  //shortLongCalib(noCut,mcFile, correctionRatios[12][0], correctionRatioErrors[12][0]);

  for(i = 0; i < 35; i++){
    for(j = 0; j < 35 - i; j++){
      //cout << "Vertex: " << i+1 << ", Grouping: " << j+1 << endl;
      //sprintf(filePath, "/local/cms/user/hanson/VertexCuts2D35/merged/VertexCuts2D35_%d_%d.root", j+1,i+1);
      //files[j][i] = TFile::Open(filePath);
      exposureResults(t, j+1, i, false, correctionRatios[j][i], correctionRatioErrors[j][i], populations[j][i]);
      //shortLongCalib(files[j][i],mcFile,correctionRatios[j][i], correctionRatioErrors[j][i]);
      //cout << i << ", " << j << ": ";
      for(int l = 0; l < 26; l++){
	//cout << l << ": " << populations[j][i][l] << "; ";
	//cout << l << ": " << correctionRatios[j][i][l] << "+-" << correctionRatioErrors[j][i][l] << ", ";
      }
      //cout << endl;
    }
  }


  //  t->TFile::Close();
  /*for(i = 0; i < 12; i++){
    for(j = 0; j < 12 - i; j++){
      files[j][i]->TFile::Close();
    }
    }*/


  for(j = 0; j < 35; j++){
    for(i = 0; i < 35 - j; i++){
      //cout << "Vertex: " << i+1 << ", Grouping: " << j+1 << ": " << endl;
      for(k = 0; k < 26; k++){
	//cout << correctionRatios[i][j][k] << "+-" << correctionRatioErrors[i][j][k] << ", ";
      }
      //cout << endl;
    }
    //cout << endl;
  }

  double correctionRatios2[36][35][20];
  double correctionRatioErrors2[36][35][20];
  for(i = 0; i < 36; i++){
    for(j = 0; j < 35; j++){
      for(k = 0 ; k < 10; k++){
	//cout << "correctionRatios";
	correctionRatios2[i][j][k] = correctionRatios[i][j][k+2];
	correctionRatios2[i][j][k+10] = correctionRatios[i][j][k+14];
	correctionRatioErrors2[i][j][k] = correctionRatioErrors[i][j][k+2];
	correctionRatioErrors2[i][j][k+10] = correctionRatioErrors[i][j][k+14];
      }
    }
  }
  
    
  int startingVertices[20][35];
  for(i = 0; i < 20; i++){
    if (populations[2][1][i]>0) {
      startingVertices[i][0] = 1;
      startingVertices[i][1] = 2;
    } else {
      startingVertices[i][0] = 1;
      startingVertices[i][1] = -1;
    }
    for(j = 2; j < 35; j++){
    //cout << "startingVertices";
      startingVertices[i][j] = -1;
    }
    int a = 1;
    int b = 0;
    int c = 2;
    while(a + b < 35){
      //cout << ietaArray[i] << " A, B, C: " << a << ", " << b << ", " << c << ". Correction Ratio: " << correctionRatios2[a][b][i] << endl;
      /*if( b > 7 ){
	//cout << "Danger Will Robinson, danger!" << endl;
	}*/
      if( populations[a][b][i] < 50){
	b++;
      } else if( populations[a][b][i] >= 50 ){
	startingVertices[i][c] = ((a + b + 2) != 36) ? (a + b + 2) : -1;
	a = a + b + 1;
	b = 0;
	c++;
      }
      }
  }
  
  

 int sizeArray[20];
 
 for(i = 0; i < 20; i++){
   int binCount = 0;
   //cout << "Ieta " << ietaArray[i] << ": ";
   for(j = 0; j < 35; j++){
     if(startingVertices[i][j] > 0 ){
       binCount++;
     }
     sizeArray[i] = binCount; // max(binCount - 1, 1);
     //cout << startingVertices[i][j] << " ";
   }
   //cout << " " << binCount << endl;
 }

 
 
 //return;



 
 double effVertex[20][34];
 double factorRelMC[20][34];
 double factorRelMCErrors[20][34];
 //double VertexNumber[20][11];
 double VertexErrors[20][34]; // {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 double intercepts[20];
 double interceptErrors[20];
 double slopes[20];
 double slopeErrors[20];
 
 for(i = 0; i < 20; i++){
   for(j = 0; j < 34; j++){
     //cout << "Initializing";
     effVertex[i][j] = 0;
     factorRelMC[i][j] = 0;
     factorRelMCErrors[i][j] = 0;
   }
 }

 double tempSum = 0.0;
 double tempSum2 = 0.0;
 double verr2 = 0.0;

 for(i = 0; i < 20; i++){
   //cout << "Outermost" << endl;
   effVertex[i][0] = 1.0;
   j = 0;
   while(startingVertices[i][j] != -1){
     VertexErrors[i][j] = 0.5;

     //cout << "Middle" << endl;
     tempSum = 0.0;
     tempSum2 = 0.0;
     verr2=0;
     bool DidYouLoop = false;
     for(k = startingVertices[i][j] - 1; k < ( (startingVertices[i][j+1] != -1) ? startingVertices[i][j+1] : 20 ) - 1; k++){
       //cout << "Indices: " << i << ", " << j << ", " << k << ", Top: " << ( (startingVertices[i][j+1] != -1) ? startingVertices[i][j+1] : 13 ) - 1;
       DidYouLoop = true;
       tempSum += (double) (populations[k][0][i]) * (k + 1.0);
       tempSum2 += (double) (populations[k][0][i]);
       verr2+= (populations[k][0][i]) * pow(k + 1.0,2);
       //cout << ", Population: " << populations[k][0][i] << endl; // << populations[0][0][k] << ", " << populations[1][0][k] << ", " << populations[2][0][k] << ", " << populations[3][0][k] << ", " << populations[4][0][k] << ", " << populations[5][0][k] << ", " << populations[6][0][k] << ", " << populations[7][0][k] << ", " << populations[8][0][k] << ", " << populations[9][0][k] << ", " << populations[10][0][k] << ", " << populations[11][0][k] << endl;
     }
     if(DidYouLoop && tempSum2 != 0) {
       effVertex[i][j] = (1.0 * tempSum) / (1.0 * tempSum2);
       VertexErrors[i][j] = sqrt(0.5*0.5+(verr2/tempSum2-effVertex[i][j]*effVertex[i][j]));
     }
     else
       effVertex[i][j] = startingVertices[i][j] - 1;
     cout << "StartV: " << startingVertices[i][j] - 1 << " EffV: " << effVertex[i][j] << " Values: " << tempSum << " " << tempSum2 << " ";
     j++;
   }
   cout << endl;
 }

 // for(i = 0; i < 20; i++){
 //   factorRelMC[i][0] = correctionRatios2[0][0][i];
 //   factorRelMCErrors[i][0] = correctionRatioErrors2[0][0][i];
 // }

 for(i = 0; i < 20; i++){
   for(j = 0; j < 34; j++){
     if(startingVertices[i][j] != -1){
       factorRelMC[i][j] = correctionRatios2[startingVertices[i][j] - 1][ ( ( (startingVertices[i][j+1] != -1) ? startingVertices[i][j+1] : 13 ) - startingVertices[i][j] ) - 1 ][i];
       //cout << startingVertices[i][j] - 1 << " " << ( ( (startingVertices[i][j+1] != -1) ? startingVertices[i][j+1] : 13 ) - startingVertices[i][j] ) - 1 << " " << i << ", ";
       factorRelMCErrors[i][j] = correctionRatioErrors2[startingVertices[i][j] - 1][ ( ( (startingVertices[i][j+1] != -1) ? startingVertices[i][j+1] : 13 ) - startingVertices[i][j] ) - 1 ][i];
     }
   }
   //cout << endl;

   if (sizeArray[i]>1 && factorRelMC[i][sizeArray[i]-1]<0.3) sizeArray[i]--;

 }

 for(i = 0; i < 20; i++){
   if (effVertex[i][0]==0) { // problem that happens for small numbers of vertices
     for (j=0; j<sizeArray[i]; j++) {
       effVertex[i][j]=effVertex[i][j+1];
       factorRelMC[i][j]=factorRelMC[i][j+1];
       VertexErrors[i][j]=VertexErrors[i][j+1];
       factorRelMCErrors[i][j]=factorRelMCErrors[i][j+1];
     }
     sizeArray[i]--;
   }
   //cout << endl << endl;
 }


  gROOT->SetStyle("Plain");

  double lowerBound;
  double upperBound;

  TCanvas * fits = new TCanvas("c1","c1",1250, 1250);
  fits->Divide(5,2);
  for(i = 0; i < 10; i++){
    //cout << i << endl;
    fits->cd(i+1);
    char name[128];
    char fitName[128];
    sprintf(name, "Tower %d", ietaArray[i]);
    sprintf(fitName, "FitOfTower%d", ietaArray[i]);
    TGraphErrors * FitGraph = new TGraphErrors(sizeArray[i], effVertex[i], factorRelMC[i], VertexErrors[i], factorRelMCErrors[i]);
    cout << i << ": " << sizeArray[i] << " ";

    //TF1 * FitOfGraph = new TF1("FitOfTower","[0]*(x-1)+[1]",0.5,36);
    TF1 * FitOfGraph = new TF1("FitOfTower","pol1",0.5,effVertex[i][sizeArray[i]-1]+0.5);
    TF1 * OldValue = new TF1("OldValue","[0]",0,36);
    //TF1 * OldValue2 = new TF1("OldValue2","[0]",0,13);
    //OldValue->SetParameter(0,factorsRelMC[i]);
    //OldValue->SetLineStyle(2);
    OldValue->SetParameter(0,correctionRatios2[35][0][i]);
    OldValue->SetLineStyle(2);
    lowerBound = 0.9;
    if(i == 0){
      lowerBound = 0.84;
    }
    upperBound = 1.2;
    if(i == 9){
      upperBound = 1.3;
      lowerBound = 1.05;
    }
    TH2D * dummy1 = new TH2D("dummy1",name,20,0,36,20,lowerBound,upperBound);

    dummy1->SetStats(0);
    dummy1->GetXaxis()->SetTitle("Effective Number of Vertices");
    dummy1->GetYaxis()->SetTitle("Correction Ratio (PRED/RECO)");
    fits->SetLeftMargin(0.12);
    dummy1->GetYaxis()->SetTitleOffset(1.5);
    
    dummy1->Draw();
    FitGraph->SetMarkerStyle(24);
    FitGraph->SetMarkerColor(kBlack);
    FitGraph->SetFillColor(0);
    FitGraph->Draw("SAME P");
    
    
    FitOfGraph->SetParameter(1,0);
    FitOfGraph->SetParLimits(1,-10,0.0001);

    FitOfGraph->SetParameter(0,1);
    FitOfGraph->SetParLimits(0,0.5,1.5);

    if (i==0 || i==9) FitOfGraph->FixParameter(1,0);

    if (sizeArray[i]<4) {
      FitOfGraph->FixParameter(1,-0.005);
    }
    
    FitGraph->SetMinimum(0.85);
    FitGraph->SetMaximum(1.2);

    FitGraph->Fit(FitOfGraph, "Q R M");
    //    FitGraph->Fit(FitOfGraph, "Q R M");
    //FitGraph->Fit(FitOfGraph, "Q R M");
    FitOfGraph->Draw("SAME L");

    OldValue->Draw("SAME L");
    //OldValue2->Draw("SAME L");

    TLegend * leg = new TLegend(.4,.7,.9,.9);
    leg->AddEntry(FitGraph, "Run2011A-05Jul2011ReReco-HF");
    leg->AddEntry(FitOfGraph, "Best Fit");
    leg->AddEntry(OldValue, "Overall Ratio");
    leg->SetTextSize(.04);
    leg->Draw("SAME");
    
    intercepts[i] = FitOfGraph->GetParameter(0)+FitOfGraph->GetParameter(1);
    interceptErrors[i] = FitOfGraph->GetParError(0);
    slopes[i] = FitOfGraph->GetParameter(1);
    slopeErrors[i] = FitOfGraph->GetParError(1);

    //delete dummy1;
    
  }
  //fits->Print("VertexFitsNegative2.png");

  TCanvas * fits2 = new TCanvas("c2","c2",1250, 1250);
  fits2->Divide(5,2);
  for(i = 10; i < 20; i++){
    //cout << i << endl;
    fits2->cd(i-9);
    char name[128];
    char fitName[128];
    sprintf(name, "Tower +%d", ietaArray[i]);
    sprintf(fitName, "FitOfTower%d", ietaArray[i]);
    TGraphErrors * FitGraph = new TGraphErrors(sizeArray[i], effVertex[i], factorRelMC[i], VertexErrors[i], factorRelMCErrors[i]);
    cout << i << ": " << sizeArray[i] << " ";

    printf("Fitting %d/%d :\n",i,sizeArray[i]);
    for (int kk=0; kk<sizeArray[i]; kk++) printf(" %f ",effVertex[i][kk]);
    printf("\n");
    for (int kk=0; kk<sizeArray[i]; kk++) printf(" %f ",VertexErrors[i][kk]);
    printf("\n");
    for (int kk=0; kk<sizeArray[i]; kk++) printf(" %f ",factorRelMC[i][kk]);
    printf("\n");
    for (int kk=0; kk<sizeArray[i]; kk++) printf(" %f ",factorRelMCErrors[i][kk]);
    printf("\n ------ \n");


    //    TF1 * FitOfGraph = new TF1("FitOfTower","[0]*(x-1)+[1]",0,36);
    TF1 * FitOfGraph = new TF1("FitOfTower","pol1",0.5,effVertex[i][sizeArray[i]-1]+0.5);
    TF1 * OldValue = new TF1("OldValue","[0]",0,36);
    //TF1 * OldValue2 = new TF1("OldValue2","[0]",0,13);
    //OldValue->SetParameter(0,factorsRelMC[i]);
    //OldValue->SetLineStyle(2);
    OldValue->SetParameter(0,correctionRatios2[35][0][i]);
    OldValue->SetLineStyle(2);    
    lowerBound = 0.9;
    upperBound = 1.2;
    if(i == 10){
      upperBound = 1.3;
      lowerBound = 1.05;
    }
    TH2D * dummy1 = new TH2D("dummy1",name,20,0,36,20,lowerBound,upperBound);

    dummy1->SetStats(0);
    dummy1->GetXaxis()->SetTitle("Effective Number of Vertices");
    dummy1->GetYaxis()->SetTitle("Correction Ratio (PRED/RECO)");
    fits2->SetLeftMargin(0.12);
    dummy1->GetYaxis()->SetTitleOffset(1.5);
    dummy1->Draw();
    FitGraph->SetMarkerStyle(24);
    FitGraph->SetMarkerColor(kBlack);
    FitGraph->SetFillColor(0);
    FitGraph->Draw("SAME P");
    
    FitOfGraph->SetParameter(1,0);
    FitOfGraph->SetParLimits(1,-1,0.001);
    FitOfGraph->SetParameter(0,1);
    FitOfGraph->SetParLimits(0,0.5,1.5);
       
    if (i==10 || i==19) FitOfGraph->FixParameter(1,0);

    if (sizeArray[i]<4) {
      FitOfGraph->FixParameter(1,-0.005);
    }

    FitGraph->SetMinimum(0.85);
    FitGraph->SetMaximum(1.2);

    FitGraph->Fit(FitOfGraph, "Q M R");
    FitOfGraph->Draw("SAME L");
    
    OldValue->Draw("SAME L");
    //OldValue2->Draw("SAME L");

    TLegend * leg = new TLegend(.4,.7,.9,.9);
    leg->AddEntry(FitGraph, "Run2011A-05Jul2011ReReco-HF");
    leg->AddEntry(FitOfGraph, "Best Fit");
    leg->AddEntry(OldValue, "Overall Ratio");
    leg->SetTextSize(0.04);
    leg->Draw("SAME");

    /*
    intercepts[i] = FitOfGraph->GetParameter(1);
    interceptErrors[i] = FitOfGraph->GetParError(1);
    slopes[i] = FitOfGraph->GetParameter(0);
    slopeErrors[i] = FitOfGraph->GetParError(0);
    */
    intercepts[i] = FitOfGraph->GetParameter(0)+FitOfGraph->GetParameter(1);
    interceptErrors[i] = FitOfGraph->GetParError(0);
    slopes[i] = FitOfGraph->GetParameter(1);
    slopeErrors[i] = FitOfGraph->GetParError(1);

    //delete dummy1;

  }
  //fits2->Print("VertexFitsPositive2.png");
  

  /*for(int j = 0; j < 20; j++){
    for(int k = 0; k < 12; k++){
      cout << factorRelMC[j][k] << "+-" << factorRelMCErrors[j][k] << "\t";
    }
    cout << endl;
    }*/

  cout << "Intercepts (at x = 1):" << endl;


  for(i = 0; i < 20; i++){
    cout << ietaArray[i] << ": " << intercepts[i] << "+-" << interceptErrors[i] << endl;
  }
  cout << endl << "Slopes:" << endl;

  for(i = 0; i < 20; i++){
    cout << ietaArray[i] << ": " << slopes[i] << "+-" << slopeErrors[i] << endl;
  }
  cout << endl;

  for(i = 0; i < 20; i++){
    cout << intercepts[i] << ", ";
  }
  cout << endl;
  for(i = 0; i < 20; i++){
    cout << interceptErrors[i] << ", ";
  }
  cout << endl;

  TCanvas * c5 = new TCanvas("c5","c5",800,800);

  c5->cd();
  TH2* dummy2 = new TH2D("dummy2","",26,0.0,26.0,10,0.9,1.2);

  double x[26] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
  double ex[26] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  double intercepts2[26];
  double interceptErrors2[26];

  intercepts2[0] = 0; intercepts2[1] = 0;
  intercepts2[12] = 0; intercepts2[13] = 0;
  intercepts2[24] = 0; intercepts2[25] = 0;
  interceptErrors2[0] = 0; interceptErrors2[1] = 0;
  interceptErrors2[12] = 0; interceptErrors2[13] = 0;
  interceptErrors2[24] = 0; interceptErrors2[25] = 0;

  for(i = 0; i < 10; i++){
    intercepts2[i+2] = intercepts[i];
    intercepts2[i+14] = intercepts[i+10];
    interceptErrors2[i+2] = interceptErrors[i];
    interceptErrors2[i+14] = interceptErrors[i+10];
  }

  TGraphErrors * OnceThrough = new TGraphErrors(26, x, intercepts2, ex, interceptErrors2);
  
  OnceThrough->SetMaximum(1.5);
  OnceThrough->SetMinimum(0.5);
  OnceThrough->SetMarkerStyle(20);
  OnceThrough->SetFillColor(0);

  dummy2->SetStats(0);
  dummy2->GetXaxis()->SetTitle("Ieta");
  dummy2->GetXaxis()->CenterTitle();
  dummy2->GetYaxis()->SetTitle("Correction Ratio Pred/Reco");
  dummy2->GetYaxis()->CenterTitle();
  dummy2->GetXaxis()->SetBinLabel(3,"-39");
  dummy2->GetXaxis()->SetBinLabel(24,"39");
  dummy2->GetXaxis()->SetBinLabel(3+4,"-35");
  dummy2->GetXaxis()->SetBinLabel(24-4,"35");
  dummy2->GetXaxis()->SetBinLabel(3+9,"-30");
  dummy2->GetXaxis()->SetBinLabel(24-9,"30");
  c5->SetLeftMargin(0.12);
  dummy2->GetYaxis()->SetTitleOffset(1.5);

  //  TLegend * leg2 = new TLegend(.6,.65,.9,.9,"Dead Material Corrected");

  //  leg2->AddEntry(OnceThrough, "1 Iteration");

  dummy2->Draw();
  OnceThrough->Draw("SAME P");
  //  leg2->Draw("SAME");

  //c5->Print("FinalVertexCorrectedResult.eps");

 printf("const double factorsRelVertex[] = { ");
 for (int ii=0; ii<26; ii++) {
   if (intercepts2[ii]==0) printf("1.0");
   else printf("%.3f",intercepts2[ii]);
   if (ii!=25) printf(", ");
   if (ii==12) printf("\n");
 }
 printf("};\n");



























}
