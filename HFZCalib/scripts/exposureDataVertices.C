#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH2.h"
// #include "TPDF.h"
#include "TText.h"
#include "TLatex.h"
#include <math.h>
#include <algorithm>
#include <list>
#include <iostream>
#include "TROOT.h"
using std::cin;
using std::cout;
using std::endl;
using namespace std;

#define MAXNVTX 30 

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

//Non-Vertex Numbers for the sake of comparison with the vertex fits
//const double factorsRelMC[] = {1.003, 1.022, 1.042, 1.048, 1.051, 1.032, 1.036, 1.036, 1.042, 1.142, 1.142, 1.025, 0.998, 1.037, 1.009, 0.996, 0.985, 1.031, 0.980, 0.974};
const int ietaArray [] = {-39,-38,-37,-36,-35,-34,-33,-32,-31,-30,30,31,32,33,34,35,36,37,38,39};
//const int sizeArray [] = {	3	,	6	,	7	,	8	,	9	,	10	,	10	,	10	,	11	,	10	,	10	,   	11	,	10	,	9	, 9,	9	,	8	,	8	,	5	,	3	};

float factors6[26]; // factors defined as a 26 component vector

// Give miscalibration constants for all the different scenarios
static const float factors_nomiscal[] = { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00}; //  no miscalibration yet or assign all miscalibration factors to 1


//corrections to no miscalibration case ie Corrections from Calibration process
// original static const float corrections_mc[] = {1.00, 1.00, 1.08696, 0.979119, 0.969235, 0.986741, 0.961573, 0.947524, 0.986307, 0.998243, 0.986077, 1.08743, 1.00, 1.00, 1.08399, 0.986606, 1.01257, 0.975674, 0.968379, 0.949467, 0.963255, 0.960102, 0.960654, 1.12329, 1.00, 1.00 };

// Fall10 
// older static const float corrections_mc[] = {1.00, 1.00, 1.08721, 1.00361, 1.00494, 0.999059, 1.00132, 0.998315, 1.01525, 1.03643, 1.02008, 1.10051, 1.00, 1.00, 1.10069, 1.02149, 1.03306, 1.0145, 0.9999745, 1.00531, 1.00052, 1.00288, 1.02014, 1.07499, 1.00, 1.00 };

// 2011 Numbers --> Taken from Summer11 Monte Carlo
static const float corrections_mc[] = { 1.0, 1.0, 1.044, 1.011, 0.995, 0.990, 0.988, 0.985, 0.993, 1.014, 0.991, 1.086, 1.0, 1.0, 1.088, 0.993, 1.013, 0.993, 0.986, 0.980, 0.987, 0.996, 1.005, 1.052, 1.0, 1.0};

// Corrections taken from Summer 2012 MC after correcting HF cluster position, 
// with medium 2D requirements and e9/e25 > 0.96
static const float corrections_mc2012[] = { 1.0, 1.0, 1.0, 1.0, 1.08275, 1.07908, 1.0882, 1.1084, 1.03076, 0.985403, 0.986382, 1.02339, 1.0, 1.0, 0.993659, 0.972947, 0.986048, 1.02084, 1.11887, 1.00406, 1.10159, 1.03964, 1.0, 1.0, 1.0, 1.0 };

//static const float numbers[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};


void exposureResults(TFile* t, int startVertex, int numberOfVerticesToAdd, bool allVertex,  
		     double* outputRatios, double* outputRatioErrors, int* populations) {

  gROOT->SetStyle("Plain");

  const float* corrections = 0;

  // std::cout << "Exposure results: " ; 
  // if ( allVertex ) std::cout << "All vertices" ; 
  // else std::cout << "Start vertex " << startVertex << " and " << numberOfVerticesToAdd << " vertices" ; 
  // std::cout << std::endl ; 

  corrections = corrections_mc;
  // corrections = corrections_mc2012;
  // TCanvas* cDebug = new TCanvas("cDebug") ; 

  double y[26],ey[26],eyc[26];
  double yRatio[26]; 

  for (int i=0; i<26; i++) {
    y[i]=0; ey[i]=0; eyc[i]=0; yRatio[i]=0;
  }
  char name[128];

  for (int signeta=-1; signeta<=1; signeta+=2) { // gives -1/+1
    for (int absieta=30; absieta<42; absieta++) { // 29 doesn't calibrate this way
      int ieta=signeta*absieta;

      // index is -41..-29,29,..41 linearly
      int index=(signeta<0)?(41-absieta):(absieta-16);

      if (!allVertex) {
	sprintf(name,"calibByNvtx%d/delExpCanHF%dEnergy",startVertex,ieta) ; 
      } else { // Always expect at least one vertex
	sprintf(name,"calibByNvtx1/delExpCanHF%dEnergy",ieta);
      }
      
      TH1F * h1 = (TH1F*)t->Get(name);

      char hname[50],htitle[50];
      sprintf(hname,"Tower%d",ieta);
      sprintf(htitle,"Tower = %d",ieta);

      h1=(TH1F*)(h1->Clone(hname));
      h1->SetTitle(htitle);
      
      if ( allVertex ) { // Need to sum everything together for the "all" case
	for (int v=2; v<MAXNVTX; v++) { // Total of up to 50 vertices available, 1 already used
	  sprintf(name,"calibByNvtx%d/delExpCanHF%dEnergy",v,ieta);
	  h1->Add((TH1F*)t->Get(name));
	}
      }

      if (numberOfVerticesToAdd > 0 && !allVertex) {
	// Shift starting point by 1 as adding to already defined histogram
	for(int v = startVertex + 1; v <= startVertex + numberOfVerticesToAdd; v++){
	  sprintf(name,"calibByNvtx%d/delExpCanHF%dEnergy",v,ieta);
	  h1->Add((TH1F*)t->Get(name));
	}
      }

      if (absieta < 40){
	if (ieta < 0){
	  populations[ieta + 39] = (int) h1->GetEntries();
	} else {
	  populations[ieta - 20] = (int) h1->GetEntries();
	}
      }

      // Skip if too few events for given N(vtx) range (previously 2)
      if ( h1->GetEntries()<5 || (allVertex && h1->GetEntries()<10) ) continue; 

      // Fit each distribution to a gaussian in a limited range, save mean
      // TF1 f1("f1","gaus",0.6,1.4); 
      // h1->Fit(&f1,"L R Q N"); //Added R and Q
      TF1 f1("f1","gaus",0.6,h1->GetXaxis()->GetXmax()); 
      h1->Fit(&f1,"L Q R N"); //Added R and Q
      double mean = f1.GetParameter(1);

      // h1->Draw();
      h1->GetXaxis()->SetRangeUser(0.25,1.75);
      sprintf(name,"%f",mean) ; 
      TText* txt = new TText(0.50,0.50,name) ;
      txt->SetNDC() ; 
      // txt->Draw() ; 
      // cDebug->Update(); 

      // std::cout << "Mean " << mean << " and error " << f1.GetParError(1) << " on fit" << std::endl ; 

      y[index]=mean;     
      ey[index]=f1.GetParError(1);

      if ( ey[index] < (y[index]*0.1/sqrt(h1->GetEntries())) ) {
	// std::cout << "WARNING: Error found to be too small.  Correcting..." << std::endl ;
	ey[index] = y[index] * (0.1/sqrt(h1->GetEntries()));
      }

      eyc[index]=ey[index]*corrections[index];
      yRatio[index]=mean*corrections[index];  //divide mean by corresponding input Miscalibration factor of ieta tower

      if ( allVertex ) {
	std::cout << "Index value: " << index 
		  << " ieta: " << (signeta < 0 ? "-":"+")
		  << absieta << " mean: " << mean 
		  << " entries: " << h1->GetEntries()
		  << " and correction: " << corrections[index] 
		  << " yielding yRatio: " << yRatio[index] << std::endl ; 
      }

      delete h1; // done with this!
    } // end of for statement 
  }

  for (int i=0; i<26; i++) {
    if (yRatio[i]!=0) outputRatios[i] = 1/yRatio[i];
    if (yRatio[i]==0) outputRatios[i] = 1.0;
  }

  for (int i=0; i<26; i++) {
    if (yRatio[i]!=0) outputRatioErrors[i] = eyc[i]/pow(yRatio[i],2);
    if (yRatio[i]==0) outputRatioErrors[i] = 1.0;
  }
 }

void exposureDataVertices(TFile * t, bool makePlots) 
  
{
  // starting vertex (MAXNVTX is all), number of vertices, ieta
  double correctionRatios[MAXNVTX+1][MAXNVTX][26];
  double correctionRatioErrors[MAXNVTX+1][MAXNVTX][26];
  int populations[MAXNVTX+1][MAXNVTX][20];

  int i, j, k;

  for(i = 0; i <= MAXNVTX; i++)
    for(j = 0; j < MAXNVTX; j++)
      for(k = 0; k < 26; k++)
	{
	  correctionRatios[i][j][k] = 0.0;
	  correctionRatioErrors[i][j][k] = 0.0;
	  if (k<20) populations[i][j][k] = 0;
	}

  // Calculate results for all vertices (1->MAXNVTX)
  exposureResults(t, 0, 0, true, correctionRatios[MAXNVTX][0], correctionRatioErrors[MAXNVTX][0], populations[MAXNVTX][0]);

  for(k = 0; k < 26; k++) {
    std::cout << correctionRatios[MAXNVTX][0][k] << " +/- " << correctionRatioErrors[MAXNVTX][0][k] ; 
    if (k<20) std::cout << " with population " << populations[MAXNVTX][0][k] ; 
    std::cout << std::endl ; 
  }

  // Results as a function of number of vertices.  
  // Note that results NEVER calculated for N(vtx) = 0
  // Offset --> Array entry N gives results for N+1 vertices
  for (i=0; i<MAXNVTX; i++) // Other than starting vertex, include i extra vertices
    for (j=0; j<(MAXNVTX-i-1); j++) { // Starting vertex j 
      if ( j==0 ) std::cout << "Step " << i+1 << " of " << MAXNVTX << std::endl ;
      exposureResults(t, j+1, i, false, correctionRatios[j][i], correctionRatioErrors[j][i], populations[j][i]);
    }

  // Recast corrections and errors for relevant ieta towers
  // abs(ieta) = 29-41 --> 30-39
  double correctionRatios2[MAXNVTX+1][MAXNVTX][20];
  double correctionRatioErrors2[MAXNVTX+1][MAXNVTX][20];
  for(i = 0; i <= MAXNVTX; i++){
    for(j = 0; j < MAXNVTX; j++){
      for(k = 0 ; k < 10; k++){
	correctionRatios2[i][j][k] = correctionRatios[i][j][k+2];
	correctionRatios2[i][j][k+10] = correctionRatios[i][j][k+14];
	correctionRatioErrors2[i][j][k] = correctionRatioErrors[i][j][k+2];
	correctionRatioErrors2[i][j][k+10] = correctionRatioErrors[i][j][k+14];
      }
    }
  }
  
    
  int startingVertices[20][MAXNVTX];
  for(i = 0; i < 20; i++){
    // if ( i==7 ) std::cout << "i==7 populations[2][1][7] = " << populations[2][1][i] << std::endl ; 

    if (populations[2][1][i]>0) {
      startingVertices[i][0] = 1;
      startingVertices[i][1] = 2;
    } else {
      startingVertices[i][0] = 1;
      startingVertices[i][1] = -1;
    }
    for(j = 2; j < MAXNVTX; j++){
      startingVertices[i][j] = -1;
    }
    int a = 1;
    int b = 0;
    int c = 2;
    while(a + b < MAXNVTX){
      // if ( i==7 ) std::cout << a << " " << b << " " << c << " " << populations[a][b][i] << std::endl ; 
      if( populations[a][b][i] < 50){
	b++;
      } else if( populations[a][b][i] >= 50 ){
	startingVertices[i][c] = ((a + b + 2) != MAXNVTX+1) ? (a + b + 2) : -1;
	a = a + b + 1;
	b = 0;
	c++;
      }
      // if ( i==7 ) std::cout << "Next step: " << a << " " << b << " " << c << std::endl ; 
    }
  }
  
  // std::cout << "Dump of starting vertices information: " << std::endl ; 
  // for (int i=0; i<20; i++) { 
  //   std::cout << i << ": " ; 
  //   for (int j=0; j<MAXNVTX; j++) { 
  //     std::cout << startingVertices[i][j] << " " ; 
  //   }
  //   std::cout << std::endl ; 
  // }

 int sizeArray[20];
 
 for(i = 0; i < 20; i++){
   int binCount = 0;
   for(j = 0; j < MAXNVTX; j++){
     if(startingVertices[i][j] > 0 ){
       binCount++;
     }
     sizeArray[i] = binCount; // max(binCount - 1, 1);
   }
 }

 double effVertex[20][MAXNVTX-1];
 double factorRelMC[20][MAXNVTX-1];
 double factorRelMCErrors[20][MAXNVTX-1];
 double VertexErrors[20][MAXNVTX-1]; 
 double intercepts[20];
 double interceptErrors[20];
 double slopes[20];
 double slopeErrors[20];
 
 for(i = 0; i < 20; i++){
   for(j = 0; j < MAXNVTX-1; j++){
     effVertex[i][j] = 0;
     factorRelMC[i][j] = 0;
     factorRelMCErrors[i][j] = 0;
   }
 }

 double tempSum = 0.0;
 double tempSum2 = 0.0;
 double verr2 = 0.0;

 for(i = 0; i < 20; i++){
   effVertex[i][0] = 1.0;
   j = 0;
   // if ( i==7 ) std::cout << "Checking what is happening" << std::endl ; 
   while(startingVertices[i][j] != -1){
     VertexErrors[i][j] = 0.5;

     // if ( i==7 ) std::cout << "In while loop with i=" << i << ", j=" << j << " and startingVertices=" << startingVertices[i][j] << std::endl ; 

     tempSum = 0.0;
     tempSum2 = 0.0;
     verr2=0;
     bool DidYouLoop = false;
     for(k = startingVertices[i][j] - 1; k < ( (startingVertices[i][j+1] != -1) ? startingVertices[i][j+1] : MAXNVTX ) - 1; k++){
       DidYouLoop = true;
       // if ( i==7 ) std::cout << "In the loop, k is " << k << std::endl ; 
       // if ( i==7 ) std::cout << "tempSum is " << tempSum << " and adding " << populations[k][0][i] << "*" << (k+1.0) << std::endl ; 
       // if ( i==7 ) std::cout << "verr2 is " << verr2 << " and increasing by " << (populations[k][0][i] * pow(k+1.0,2)) << std::endl ; 
       // Used to calculate effective number of vertices for graphs later
       tempSum  += (double) (populations[k][0][i]) * (k + 1.0);
       tempSum2 += (double) (populations[k][0][i]);
       verr2+= (populations[k][0][i]) * pow(k + 1.0,2);
     }
     if(DidYouLoop && tempSum2 != 0) {
       effVertex[i][j] = (1.0 * tempSum) / (1.0 * tempSum2); // Weighted average: effective number of vertices
       VertexErrors[i][j] = sqrt(0.5*0.5+(verr2/tempSum2-effVertex[i][j]*effVertex[i][j]));
       // if ( i==7 ) std::cout << "Looped and effVertex is " << effVertex[i][j] << " and errors " << VertexErrors[i][j] << std::endl ; 
     }
     else {
       // if ( i==7 ) std::cout << "Did not loop, so effVertex is " << (startingVertices[i][j] - 1) << std::endl ; 
       effVertex[i][j] = startingVertices[i][j] - 1;
     }
     // cout << "StartV: " << startingVertices[i][j] - 1 << " EffV: " << effVertex[i][j] << " Values: " << tempSum << " " << tempSum2 << " ";
     j++;
   }
   // std::cout << std::endl;
   // std::cout << "End for i=" << i << std::endl ;  
 }

 for(i = 0; i < 20; i++){
   for(j = 0; j < MAXNVTX-1; j++){
     if(startingVertices[i][j] != -1){
       factorRelMC[i][j] = correctionRatios2[startingVertices[i][j] - 1][ ( ( (startingVertices[i][j+1] != -1) ? startingVertices[i][j+1] : MAXNVTX ) - startingVertices[i][j] ) - 1 ][i];
       factorRelMCErrors[i][j] = correctionRatioErrors2[startingVertices[i][j] - 1][ ( ( (startingVertices[i][j+1] != -1) ? startingVertices[i][j+1] : MAXNVTX ) - startingVertices[i][j] ) - 1 ][i];
     }
   }

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
 }


  gROOT->SetStyle("Plain");

  double lowerBound;
  double upperBound;

  TCanvas* fits = new TCanvas("c1","c1",1250, 1250);
  fits->Divide(5,2);
  for(i = 0; i < 10; i++){
    cout << i << endl;
    fits->cd(i+1)->SetLeftMargin(0.22);
    fits->cd(i+1)->SetBottomMargin(0.15);
    char name[128];
    char fitName[128];
    sprintf(name, "i#eta = %d", ietaArray[i]);
    sprintf(fitName, "FitOfTower%d", ietaArray[i]);
    TGraphErrors * FitGraph = new TGraphErrors(sizeArray[i], effVertex[i], factorRelMC[i], VertexErrors[i], factorRelMCErrors[i]);
    cout << i << ": " << sizeArray[i] << " ";

    int sizeArrayIdx = sizeArray[i] - 1 ; 
    if ( sizeArrayIdx < 0 ) sizeArrayIdx = 0 ; 
    TF1 * FitOfGraph = new TF1("FitOfTower","pol1",0.5,effVertex[i][sizeArrayIdx]+0.5);
    TF1 * OldValue = new TF1("OldValue","[0]",0,51);

    OldValue->SetParameter(0,correctionRatios2[MAXNVTX][0][i]);
    OldValue->SetLineStyle(2);
    OldValue->SetLineColor(kBlue);
    lowerBound = 0.9;
    upperBound = 1.2;
    if (i < 6) { upperBound = 1.5; lowerBound = 0.70; }
    if (i == 6) { upperBound = 1.2; lowerBound = 0.80; }
    // if(i == 9){
    //   upperBound = 1.3;
    //   lowerBound = 1.05;
    // }

    TH2D* dummy1 = new TH2D("dummy1","dummy1",20,0,MAXNVTX+1,20,lowerBound,upperBound);
    dummy1->SetStats(0);
    dummy1->SetTitle(";Effective Number of Vertices;Correction Ratio (Pred/Reco HF Energy)") ; 
    dummy1->GetXaxis()->CenterTitle() ; 
    dummy1->GetXaxis()->SetLabelSize(0.06) ; 
    dummy1->GetYaxis()->SetLabelSize(0.06) ; 
    dummy1->GetYaxis()->CenterTitle() ; 
    dummy1->GetYaxis()->SetTitleOffset(1.7);
    dummy1->GetYaxis()->SetTitleSize(0.06);
    dummy1->GetXaxis()->SetTitleSize(0.06);
    
    dummy1->DrawCopy();
    FitGraph->SetMarkerStyle(20);
    FitGraph->SetMarkerSize(0.8);
    FitGraph->SetMarkerColor(kBlack);
    FitGraph->SetFillColor(0);
    FitGraph->Draw("SAME P");
    
    
    FitOfGraph->SetParameter(1,0);
    FitOfGraph->SetParLimits(1,-10,0.0001);

    FitOfGraph->SetParameter(0,1);
    FitOfGraph->SetParLimits(0,0.5,1.5);

    if (i==0 || i==9) FitOfGraph->FixParameter(1,0);

    // if (sizeArray[i]<4) {
    //   FitOfGraph->FixParameter(1,-0.005);
    // }
    
    FitGraph->SetMinimum(0.85);
    FitGraph->SetMaximum(1.2);

    if (sizeArray[i] >= 4) { 
      FitGraph->Fit(FitOfGraph, "Q R M 0");
      FitOfGraph->SetLineColor(kRed) ; 
      FitOfGraph->SetLineWidth(2) ; 
      FitOfGraph->Draw("SAME L");
    }

    OldValue->Draw("SAME L");
    //OldValue2->Draw("SAME L");

    TLegend * leg = new TLegend(0.22,0.75,0.9,0.9);
    leg->AddEntry(FitGraph, "Run 2012","p");
    if (sizeArray[i] >= 4) leg->AddEntry(FitOfGraph, "Best Fit","l");
    leg->AddEntry(OldValue, "Overall Ratio","l");
    leg->SetTextSize(0.065);
    leg->SetFillColor(kWhite) ; 
    // leg->Draw("SAME");
    leg->Draw();
    
    TLatex *latex = new TLatex(0.55,0.68,name) ; 
    latex->SetNDC() ; 
    latex->SetTextSize(0.10) ; 
    latex->Draw() ; 

    intercepts[i] = FitOfGraph->GetParameter(0)+FitOfGraph->GetParameter(1);
    interceptErrors[i] = FitOfGraph->GetParError(0);
    slopes[i] = FitOfGraph->GetParameter(1);
    slopeErrors[i] = FitOfGraph->GetParError(1);

    std::cout << i << " *** " << std::endl ; 
  }

  if ( makePlots && false ) { 
    std::cout << "Trying to make png plot" << std::endl ; 
    c1->Print("/home/bdahmes/Work/HFCalib/plots/negEtaByVertex.png") ; 
    std::cout << "Trying to make pdf plot" << std::endl ; 
    c1->Print("/home/bdahmes/Work/HFCalib/plots/negEtaByVertex.pdf") ; 
    std::cout << "Trying to make the rest" << std::endl ; 
    c1->Print("/home/bdahmes/Work/HFCalib/plots/negEtaByVertex.eps") ; 
    c1->Print("/home/bdahmes/Work/HFCalib/plots/negEtaByVertex.C") ; 
  }

  std::cout << "Got here" << std::endl ; 

 //fits->Print("VertexFitsNegative2.png");

  TCanvas* fits2 = new TCanvas("fits2","fits2",1250, 1250);
  fits2->Divide(5,2);
  for(i = 10; i < 20; i++){
    //cout << i << endl;
    fits2->cd(i-9)->SetLeftMargin(0.22);
    fits2->cd(i-9)->SetBottomMargin(0.15);
    char name[128];
    char fitName[128];
    sprintf(name, "i#eta = +%d", ietaArray[i]);
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
    TF1 * OldValue = new TF1("OldValue","[0]",0,MAXNVTX+1);
    //TF1 * OldValue2 = new TF1("OldValue2","[0]",0,13);
    //OldValue->SetParameter(0,factorsRelMC[i]);
    //OldValue->SetLineStyle(2);
    OldValue->SetParameter(0,correctionRatios2[MAXNVTX][0][i]);
    OldValue->SetLineStyle(2);    
    OldValue->SetLineColor(kBlue);    
    // lowerBound = 0.9;
    // upperBound = 1.2;
    lowerBound = 0.8;
    upperBound = 1.5;
    // if (i == 14) { lowerBound = 0.7; }
    if (i == 16) { lowerBound = 0.7; }
    // if (i == 17) { lowerBound = 0.7; }

    TH2D* dummy2 = new TH2D("dummy2","dummy2",20,0,MAXNVTX+1,20,lowerBound,upperBound);
    dummy2->SetStats(0);
    dummy2->SetTitle(";Effective Number of Vertices;Correction Ratio (Pred/Reco HF Energy)") ; 
    // dummy2->GetXaxis()->SetTitle("Effective Number of Vertices");
    // dummy2->GetYaxis()->SetTitle("Correction Ratio (Pred/Reco HF Energy)");
    dummy2->GetYaxis()->SetTitleOffset(1.7);
    dummy2->GetYaxis()->SetTitleSize(0.06);
    dummy2->GetXaxis()->SetTitleSize(0.06);
    dummy2->GetYaxis()->CenterTitle();
    dummy2->GetXaxis()->CenterTitle();
    dummy2->GetXaxis()->SetLabelSize(0.06) ; 
    dummy2->GetYaxis()->SetLabelSize(0.06) ; 
    dummy2->Draw();
    FitGraph->SetMarkerStyle(20);
    FitGraph->SetMarkerColor(kBlack);
    FitGraph->SetMarkerSize(0.8);
    FitGraph->Draw("SAME P");
    
    FitOfGraph->SetParameter(1,0);
    FitOfGraph->SetParLimits(1,-1,0.001);
    FitOfGraph->SetParameter(0,1);
    FitOfGraph->SetParLimits(0,0.5,1.5);
       
    if (i==10 || i==19) FitOfGraph->FixParameter(1,0);

    // if (sizeArray[i]<4) {
    //   FitOfGraph->FixParameter(1,-0.005);
    // }

    if ( sizeArray[i] >= 4 ) { 
      FitGraph->SetMinimum(0.85);
      FitGraph->SetMaximum(1.2);

      FitGraph->Fit(FitOfGraph, "Q M R 0");
      FitOfGraph->SetLineColor(kRed);
      FitOfGraph->SetLineWidth(2);
      FitOfGraph->Draw("SAME L");
    }

    OldValue->Draw("SAME L");
    //OldValue2->Draw("SAME L");

    TLegend * leg = new TLegend(0.22,0.75,0.9,0.9);
    leg->AddEntry(FitGraph, "Run 2012","p");
    if ( sizeArray[i] >= 4 ) leg->AddEntry(FitOfGraph, "Best Fit","l");
    leg->AddEntry(OldValue, "Overall Ratio","l");
    leg->SetTextSize(0.065);
    leg->SetFillColor(kWhite) ; 
    // leg->Draw("SAME");
    leg->Draw();

    TLatex *latex = new TLatex(0.55,0.68,name) ; 
    latex->SetNDC() ; 
    latex->SetTextSize(0.10) ; 
    latex->Draw() ; 

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

    //delete dummy2;

  }

  if ( makePlots && false ) { 
    std::cout << "Trying here" << std::endl ; 
    fits2->Print("/home/bdahmes/Work/HFCalib/plots/posEtaByVertex.png") ; 
    fits2->Print("/home/bdahmes/Work/HFCalib/plots/posEtaByVertex.pdf") ; 
    fits2->Print("/home/bdahmes/Work/HFCalib/plots/posEtaByVertex.eps") ; 
    fits2->Print("/home/bdahmes/Work/HFCalib/plots/posEtaByVertex.C") ; 
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

  // c5->cd();
  TH2* dummy3 = new TH2D("dummy3","",26,0.5,26.5,10,0.8,1.2);

  double x[26] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
  double ex[26] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  double ex2[26] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};

  double intercepts2[26];
  double interceptErrors2[26];

  double oldvalues[26] ; 
  double olderrors[26] ; 

  intercepts2[0] = 0; intercepts2[1] = 0;
  intercepts2[12] = 0; intercepts2[13] = 0;
  intercepts2[24] = 0; intercepts2[25] = 0;
  interceptErrors2[0] = 0; interceptErrors2[1] = 0;
  interceptErrors2[12] = 0; interceptErrors2[13] = 0;
  interceptErrors2[24] = 0; interceptErrors2[25] = 0;

  oldvalues[0] = 0; oldvalues[1] = 0;
  oldvalues[12] = 0; oldvalues[13] = 0;
  oldvalues[24] = 0; oldvalues[25] = 0;
  olderrors[0] = 0; olderrors[1] = 0;
  olderrors[12] = 0; olderrors[13] = 0;
  olderrors[24] = 0; olderrors[25] = 0;

  for(i = 0; i < 10; i++){
    intercepts2[i+2] = intercepts[i];
    intercepts2[i+14] = intercepts[i+10];
    interceptErrors2[i+2] = interceptErrors[i];
    interceptErrors2[i+14] = interceptErrors[i+10];
    oldvalues[i+2]  = correctionRatios2[MAXNVTX][0][i];
    oldvalues[i+14] = correctionRatios2[MAXNVTX][0][i+10];
    olderrors[i+2]  = correctionRatioErrors2[MAXNVTX][0][i];
    olderrors[i+14] = correctionRatioErrors2[MAXNVTX][0][i+10]; 
  }

  for (int i=0; i<26; i++) { 
    std::cout << "old: " << oldvalues[i] << " +/- " << olderrors[i] << std::endl ; 
  }

  TGraphErrors * OnceThrough   = new TGraphErrors(26, x, intercepts2, ex, interceptErrors2);
  TGraphErrors * VtxIntegrated = new TGraphErrors(26, x, oldvalues, ex2, olderrors);
  
  OnceThrough->SetMaximum(1.5);
  OnceThrough->SetMinimum(0.5);
  OnceThrough->SetMarkerStyle(20);
  OnceThrough->SetFillColor(0);

  VtxIntegrated->SetMaximum(1.5);
  VtxIntegrated->SetMinimum(0.5);
  VtxIntegrated->SetMarkerStyle(24);
  VtxIntegrated->SetMarkerColor(kBlue);
  VtxIntegrated->SetLineColor(kBlue);
  VtxIntegrated->SetFillColor(kBlue);
  VtxIntegrated->SetFillStyle(3002);

  std::cout << "VtxIntegrated N: " << VtxIntegrated->GetN() << std::endl ; 

  dummy3->SetStats(0);
  dummy3->GetXaxis()->SetTitle("i#eta");
  dummy3->GetXaxis()->CenterTitle();
  dummy3->GetXaxis()->SetLabelSize(0.037);
  dummy3->GetYaxis()->SetTitle("Correction Ratio (Pred/Reco)");
  dummy3->GetYaxis()->CenterTitle();
  dummy3->GetXaxis()->SetBinLabel(3,"-39");
  dummy3->GetXaxis()->SetBinLabel(24,"39");
  dummy3->GetXaxis()->SetBinLabel(3+4,"-35");
  dummy3->GetXaxis()->SetBinLabel(24-4,"35");
  dummy3->GetXaxis()->SetBinLabel(3+9,"-30");
  dummy3->GetXaxis()->SetBinLabel(24-9,"30");
  dummy3->GetYaxis()->SetTitleOffset(1.5);

  c5->SetLeftMargin(0.12);

  dummy3->DrawCopy();
  VtxIntegrated->Draw("SAME E2");
  OnceThrough->Draw("SAME P");

  // for (int i=0; i<VtxIntegrated->GetN(); i++) { 
  //   double x, y ; 
  //   VtxIntegrated->GetPoint(i,x,y) ; 
  //   std::cout << i << ": " << x < ", " << y << std::endl ; 
  // }

  TLegend* leg2 = new TLegend(.65,.80,.9,.9);  
  leg2->AddEntry(OnceThrough, "N_{vtx}=1 (result from fit)","p");
  leg2->AddEntry(VtxIntegrated, "All vertices","f");
  leg2->SetFillColor(kWhite) ; 
  leg2->Draw();

  if ( makePlots ) { 
    c5->Print("/home/bdahmes/Work/HFCalib/plots/correctionRatioByVertex.png") ; 
    c5->Print("/home/bdahmes/Work/HFCalib/plots/correctionRatioByVertex.pdf") ; 
    c5->Print("/home/bdahmes/Work/HFCalib/plots/correctionRatioByVertex.eps") ; 
    c5->Print("/home/bdahmes/Work/HFCalib/plots/correctionRatioByVertex.C") ; 
  } 

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
