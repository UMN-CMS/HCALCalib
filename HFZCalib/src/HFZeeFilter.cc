// -*- C++ -*-
//
// Package:    HFZeeFilter
// Class:      HFZeeFilter
// 
/**\class HFZeeFilter HFZeeFilter.cc HCALCalib/HFZCalib/src/HFZeeFilter.cc

 Description: Modified version of HFZeeVBTF removing PAT requirements

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jeremy M Mans
//         Created:  Mon May 31 07:00:26 CDT 2010
// $Id: HFZeeFilter.cc,v 1.1 2012/09/20 09:19:44 bdahmes Exp $
//
//

// system include files
#include <memory>

#include <algorithm>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/FileBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaReco/interface/HFEMClusterShape.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/HFEMClusterShapeAssociation.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/Ref.h"

#include "TH1.h"
#include "TH2.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// Remove if possible
// #include "ZShape/HFZeeVBTF/interface/SelectElectron.h"

class HFZeeFilter : public edm::EDFilter {
public:
  explicit HFZeeFilter(const edm::ParameterSet&);
  ~HFZeeFilter();

private:
  virtual void beginJob() ;
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  std::string currentFile_, myName_ ;
  bool dolog_;

  edm::InputTag gsfElectrons_, hfElectrons_ ; 
  std::string electronIDLabel_ ; 
  double electronIDthreshold_ ; 

  double ecalMinEt_;
  double hfMinEt_;
  std::vector<double> hfIdParams_;

  std::vector<double> massWindow_;

  // ----------member data ---------------------------

  // gf set of histograms per each == Z definition ==
  struct HistPerDef {
    //book histogram set w/ common suffix inside the provided TFileDirectory
    void book(TFileDirectory td,const std::string&,const std::vector<double>& win);
    // fill all histos of the set with the two electron candidates
    void fill(reco::GsfElectronCollection::const_iterator ecalE, 
	      const reco::RecoEcalCandidate& hfE, const reco::HFEMClusterShape& hfshape, 
	      const bool hasEleIDPassed, const edm::ParameterSet& myPs, const int isTight, 
	      std::vector<double> hfIdParams);

    TH1* mee, *meeHFP, *meeHFM;
    TH2* mee_vsEta, *mee_vsAbsEta, *meeHFP_vsEta, *meeHFM_vsEta;
    TH1* Yee, *Ptee;
    TH1* ec_eta,       *ec_phi,     *ec_pt;
    TH1* hf_eta,       *hf_phi,     *hf_pt;
    TH1* hfp_eta,      *hfp_phi,    *hfp_pt;
    TH1* hfm_eta,      *hfm_phi,    *hfm_pt;
    TH2* ec_eta_vs_phi, *hfp_eta_vs_phi, *hfm_eta_vs_phi ;
    TH2* hf_eCOREe9eSeL;
    TH1* hf_e9e25,     *hf_eCOREe9, *hf_eSeL, *hf_var2d;
    TH1* hf_seed_eSeL, *hf_e1e9;
    TH1* hf_e,         *hf_e1x1,    *hf_core, *hf_e3x3, *hf_e5x5;
    TH1* el_1_eta,     *el_2_eta;
    TH2* el_1_eta_VS_el_2_eta, *el_1_phi_VS_el_2_phi;
    // gf: add hisos for variables N-1 selected
    TH1* combIsoEB_nmo,  *relTkIsoEB_nmo, *relEcIsoEB_nmo, *relHcIsoEB_nmo, *sigiEtaiEtaEB_nmo, *dphiEB_nmo, *detaEB_nmo, *hOeEB_nmo;
    TH1* combIsoEE_nmo,  *relTkIsoEE_nmo, *relEcIsoEE_nmo, *relHcIsoEE_nmo, *sigiEtaiEtaEE_nmo, *dphiEE_nmo, *detaEE_nmo, *hOeEE_nmo;
    TH1* e9e25_nmo, *var2d_nmo, *eCOREe9_nmo, *eSeL_nmo;
    TH2* eSeL_vs_logEl;
    std::vector<double> zMassWindow;

  };
  std::vector<std::string> HLT_Names;

  // gf set of histo for all Z definitios in a stack
  struct HistStruct {
    TH1 *nelec,*nhf;
    HistPerDef base, basePt ; 
    HistPerDef filteredEvents;
  } hists;

  edm::ParameterSet eleID95Cuts_ps_;
  edm::ParameterSet eleIDFilterCuts_ps_;
};


void HFZeeFilter::HistPerDef::book(TFileDirectory td, const std::string& post, const std::vector<double>& win) {

  zMassWindow = win ; // Assigned for plotting some quantities for Z peak only

  std::string title;
  // For plotting purposes only.  Actual filter requirements defined elsewhere
  double minZrange = 40;
  double maxZrange = 160;
  
  title=std::string("M_{ee} ")+post;
  mee=td.make<TH1D>("mee",title.c_str(),120,minZrange,maxZrange);  
  title=std::string("M_{ee,HF+} ")+post;
  meeHFP=td.make<TH1D>("mee-HFP",title.c_str(),120,minZrange,maxZrange);  
  title=std::string("M_{ee,HF-} ")+post;
  meeHFM=td.make<TH1D>("mee-HFM",title.c_str(),120,minZrange,maxZrange);  

  title=std::string("M_{ee} vs eta")+post;
  mee_vsEta=td.make<TH2D>("mee_vsEta",title.c_str(),50,-5,5,60,minZrange,maxZrange);
  title=std::string("M_{ee,HF} vs abs(eta) ")+post;
  mee_vsAbsEta=td.make<TH2D>("mee_vsAbsEta",title.c_str(),10,3,5,60,minZrange,maxZrange);  
  title=std::string("M_{ee,HF+} vs eta ")+post;
  meeHFP_vsEta=td.make<TH2D>("mee-HFP_vsEta",title.c_str(),10,3,5,60,minZrange,maxZrange);  
  title=std::string("M_{ee,HF-} vs eta ")+post;
  meeHFM_vsEta=td.make<TH2D>("mee-HFM_vsEta",title.c_str(),10,-5,-3,60,minZrange,maxZrange);  

  title=std::string("Y_{ee} ")+post;
  Yee=td.make<TH1D>("yee",title.c_str(),50,-4,4);  
  title=std::string("pT_{ee} ")+post;
  Ptee=td.make<TH1D>("ptee",title.c_str(),100,0,100);  

  title=std::string("eta_{ecal} ")+post;
  ec_eta=td.make<TH1D>("etaecal",title.c_str(),30,-3,3);  
  title=std::string("phi_{ecal} ")+post;
  ec_phi=td.make<TH1D>("phiecal",title.c_str(),30,-3.14159,3.14159);  
  title=std::string("eta_{ecal} vs. phi_{ecal}")+post;
  ec_eta_vs_phi=td.make<TH2D>("etavsphiecal",title.c_str(),30,-3,3,30,-3.14159,3.14159);  
  title=std::string("pt_{ecal} ")+post;
  ec_pt=td.make<TH1D>("ptecal",title.c_str(),120,0,120);

  title=std::string("eta_{hf} ")+post;
  hf_eta=td.make<TH1D>("etahf",title.c_str(),50,-5,5);  
  title=std::string("phi_{hf} ")+post;
  hf_phi=td.make<TH1D>("phihf",title.c_str(),30,-3.14159,3.14159);
  title=std::string("pt_{hf} ")+post;
  hf_pt=td.make<TH1D>("pthf",title.c_str(),120,0,120);  

  title=std::string("eta_{hf+} ")+post;
  hfp_eta=td.make<TH1D>("etahfp",title.c_str(),50,-5,5);  
  title=std::string("phi_{hf+} ")+post;
  hfp_phi=td.make<TH1D>("phihfp",title.c_str(),30,-3.14159,3.14159);
  title=std::string("eta_{hf+} vs. phi_{hf+}")+post;
  hfp_eta_vs_phi=td.make<TH2D>("etavsphihfp",title.c_str(),10,3,5,30,-3.14159,3.14159);  
  title=std::string("pt_{hf+} ")+post;
  hfp_pt=td.make<TH1D>("pthfp",title.c_str(),120,0,120);  

  title=std::string("eta_{hf-} ")+post;
  hfm_eta=td.make<TH1D>("etahfm",title.c_str(),50,-5,5);  
  title=std::string("phi_{hf-} ")+post;
  hfm_phi=td.make<TH1D>("phihfm",title.c_str(),30,-3.14159,3.14159);
  title=std::string("eta_{hf-} vs. phi_{hf-}")+post;
  hfm_eta_vs_phi=td.make<TH2D>("etavsphihfm",title.c_str(),10,-5,-3,30,-3.14159,3.14159);  
  title=std::string("pt_{hf-} ")+post;
  hfm_pt=td.make<TH1D>("pthfm",title.c_str(),120,0,120);  

  title=std::string("iso e9e25 ")+post;
  hf_e9e25=td.make<TH1D>("e9e25",title.c_str(),60,0.5,1.1);
  title=std::string("eldId var2d")+post;
  hf_var2d=td.make<TH1D>("var2d",title.c_str(),75,0,1.5);  
  title=std::string("eCOREe9 ")+post;
  hf_eCOREe9=td.make<TH1D>("eCOREe9",title.c_str(),60,0,1.2);  
  title=std::string("eSeL ")+post;
  hf_eSeL=td.make<TH1D>("eSeL",title.c_str(),75,0,1.5);  

  title=std::string("eCOREe9eSeL ")+post;
  hf_eCOREe9eSeL=td.make<TH2D>("eCOREe9eSeL",title.c_str(),60,0,1.2,75,0,1.5);  

  title=std::string("seed_eSeL ")+post;
  hf_seed_eSeL=td.make<TH1D>("seed_eSeL",title.c_str(),75,0,1.5);  
  title=std::string("e1e9 ")+post;
  hf_e1e9=td.make<TH1D>("e1e9",title.c_str(),60,0,1.2);
  title=std::string("e ")+post;
  hf_e=td.make<TH1D>("e",title.c_str(),150,0,1500);
  title=std::string("e1x1 ")+post;
  hf_e1x1=td.make<TH1D>("e1x1",title.c_str(),150,0,1500);
  title=std::string("e3x3 ")+post;
  hf_e3x3=td.make<TH1D>("e3x3",title.c_str(),150,0,1500);
  title=std::string("e5x5 ")+post;
  hf_e5x5=td.make<TH1D>("e5x5",title.c_str(),150,0,1500);
  title=std::string("core ")+post;
  hf_core=td.make<TH1D>("core",title.c_str(),150,0,1500);

  title=std::string("eta_{el_1} ")+post;
  el_1_eta=td.make<TH1D>("etael1",title.c_str(),50,-5,5);  
  title=std::string("eta_{el_2} ")+post;
  el_2_eta=td.make<TH1D>("etael2",title.c_str(),50,-5,5);  

  title=std::string("eta_{el_1}_VS_eta_{el_2} ")+post;
  el_1_eta_VS_el_2_eta=td.make<TH2D>("etael1-vs-etael2",title.c_str(),50,-5,5,50,-5,5);
  title=std::string("phi_{el_1}_VS_phi_{el_2} ")+post;
  el_1_phi_VS_el_2_phi=td.make<TH2D>("phiel1-vs-phiel2",title.c_str(),30,-3.14159,3.14159,30,-3.14159,3.14159);

  title=std::string("N-1_combIsoEB ")+post;
  combIsoEB_nmo=td.make<TH1D>("N-1_combIsoEB",title.c_str(),100,0,2);
  title=std::string("N-1_relTkIsoEB ")+post;
  relTkIsoEB_nmo=td.make<TH1D>("N-1_relTkIsoEB",title.c_str(),40,0,0.5);  
  title=std::string("N-1_relEcIsoEB ")+post;
  relEcIsoEB_nmo=td.make<TH1D>("N-1_relEcIsoEB",title.c_str(),40,0,0.5);  
  title=std::string("N-1_relHcIsoEB ")+post;
  relHcIsoEB_nmo=td.make<TH1D>("N-1_relHcIsoEB",title.c_str(),40,0,0.5);  
  title=std::string("N-1_sigiEtaiEtaEB ")+post;
  sigiEtaiEtaEB_nmo=td.make<TH1D>("N-1_sigiEtaiEtaEB",title.c_str(),60,0,0.06);  
  title=std::string("N-1_dphiEB ")+post;
  dphiEB_nmo=td.make<TH1D>("N-1_dphiEB",title.c_str(),75,-0.75,0.75);
  title=std::string("N-1_detaEB ")+post;
  detaEB_nmo=td.make<TH1D>("N-1_detaEB",title.c_str(),40,-0.02,0.02);  
  title=std::string("N-1_hOeEB ")+post;
  hOeEB_nmo=td.make<TH1D>("N-1_hOeEB",title.c_str(),40,0,1);
  title=std::string("N-1_combIsoEE ")+post;
  combIsoEE_nmo=td.make<TH1D>("N-1_combIsoEE",title.c_str(),100,0,2); 
  title=std::string("N-1_relTkIsoEE ")+post;
  relTkIsoEE_nmo=td.make<TH1D>("N-1_relTkIsoEE",title.c_str(),40,0,0.5);  
  title=std::string("N-1_relEcIsoEE ")+post;
  relEcIsoEE_nmo=td.make<TH1D>("N-1_relEcIsoEE",title.c_str(),40,0,0.5);
  title=std::string("N-1_relHcIsoEE ")+post;
  relHcIsoEE_nmo=td.make<TH1D>("N-1_relHcIsoEE",title.c_str(),40,0,0.5);  
  title=std::string("N-1_sigiEtaiEtaEE ")+post;
  sigiEtaiEtaEE_nmo=td.make<TH1D>("N-1_sigiEtaiEtaEE",title.c_str(),60,0,0.06);  
  title=std::string("N-1_dphiEE ")+post;
  dphiEE_nmo=td.make<TH1D>("N-1_dphiEE",title.c_str(),75,-0.75,0.75);
  title=std::string("N-1_detaEE ")+post;
  detaEE_nmo=td.make<TH1D>("N-1_detaEE",title.c_str(),40,-0.02,0.02);  
  title=std::string("N-1_hOeEE ")+post;
  hOeEE_nmo=td.make<TH1D>("N-1_hOeEE",title.c_str(),40,0,0.2);

  title=std::string("N-1_HF iso e9e25 ")+post;
  e9e25_nmo=td.make<TH1D>("N-1_e9e25",title.c_str(),40,0,1);
  title=std::string("N-1_HF eldId var2d")+post;
  var2d_nmo=td.make<TH1D>("N-1_var2d",title.c_str(),40,0,1.5);  
  title=std::string("N-1_eCOREe9 ")+post;
  eCOREe9_nmo=td.make<TH1D>("N-1_eCOREe9",title.c_str(),40,0,1);  
  title=std::string("N-1_eSeL ")+post;
  eSeL_nmo=td.make<TH1D>("N-1_eSeL",title.c_str(),75,0,1.5);
  title=std::string("eSeL_vs_logEl ")+post;
  eSeL_vs_logEl=td.make<TH2D>("eSeL_vs_logEl",title.c_str(),25,3,8,75,0,1.5);
}

void HFZeeFilter::HistPerDef::fill(reco::GsfElectronCollection::const_iterator ecalE,  
				   const reco::RecoEcalCandidate& hfE, const reco::HFEMClusterShape& hfshape, 
				   const bool hasEleIDPassed,
				   const edm::ParameterSet& myPs, 
				   const int isTight,
				   std::vector<double> hfIdParams) {

  float e9e25_cut;
  float hf_2d_cut;
  float eCOREe9_cut;
  float eSeL_cut;

  if (isTight==0){     // this is the loose HFeleId
    e9e25_cut   = hfIdParams.at(0);  
    hf_2d_cut   = hfIdParams.at(2);
    eCOREe9_cut = hfIdParams.at(4);
    eSeL_cut    = hfIdParams.at(6);}
  else if (isTight==1){// this is the tight HFeleId
    e9e25_cut   = hfIdParams.at(1);  
    hf_2d_cut   = hfIdParams.at(3);
    eCOREe9_cut = hfIdParams.at(5);
    eSeL_cut    = hfIdParams.at(7);}
  else if (isTight==-1) { // This is no HFeleId
    e9e25_cut=-9999;
    hf_2d_cut=-9999;
    eCOREe9_cut=-9999;
    eSeL_cut=9999;
  } else 
    {assert(0);} // no values otherthan 1 and 0 are supported for the moment
  
  // for debugging
  //  std::cout << "hf_2d_cut: " << hf_2d_cut << " hfIdParams[2]: (loose) " << hfIdParams.at(2) << " hfIdParams[3]: (tight) " << hfIdParams.at(3) << std::endl;
  //  std::cout << "e9e25_cut: " << e9e25_cut << " hfIdParams[0]: (loose) " << hfIdParams.at(0) << " hfIdParams[1]: (tight) " << hfIdParams.at(1) << std::endl;
  //  std::cout << "eCOREe9: " << eCOREe9_cut << "\t" << hfIdParams.at(4) << "\t" << hfIdParams.at(5) << std::endl; 
  //  std::cout << "eSeL: " << eSeL_cut << "\t" << hfIdParams.at(6) << "\t" <<  hfIdParams.at(7) << std::endl; 
  
  

  bool isEb(false);
  int isEe=1;// 0: EB;  1: EE
  if(fabs(ecalE->eta())<1.4442){
    isEb=true;
    isEe=0;
  }
  
  float e9e25      = hfshape.eLong3x3()/hfshape.eLong5x5();
  float var2d      = hfshape.eCOREe9()-(hfshape.eSeL()*9./8.);
  float eCOREe9    = hfshape.eCOREe9();
  float eSeL       = hfshape.eSeL();
  
  reco::Particle::LorentzVector Z(ecalE->p4());
  Z+=hfE.p4();

  // if all selections are passed, fill standard plots
  if (hasEleIDPassed && 
     var2d   > hf_2d_cut   &&
     e9e25   > e9e25_cut   &&
     eCOREe9 > eCOREe9_cut &&
      eSeL    < eSeL_cut    ) { 

    mee          ->Fill(Z.M());
    mee_vsEta    -> Fill(hfE.p4().eta(),Z.M());
    mee_vsAbsEta -> Fill(fabs(hfE.p4().eta()),Z.M());

    if ( hfE.p4().eta() > 0 ) { 
      meeHFP ->Fill(Z.M());
      meeHFP_vsEta -> Fill(hfE.p4().eta(),Z.M());
    } else {
      meeHFM ->Fill(Z.M());
      meeHFM_vsEta -> Fill(hfE.p4().eta(),Z.M());
    }

    if (zMassWindow[0] < Z.M() && Z.M() < zMassWindow[1]) { // make inv. mass requirement explicit
    
      eSeL_vs_logEl-> Fill(log(hfshape.eLong3x3()),hfshape.eSeL());
      
      if(hfE.p4().eta()>0){
	hfp_eta         ->Fill(hfE.p4().eta());
	hfp_phi         ->Fill(hfE.p4().phi());
	hfp_eta_vs_phi  ->Fill(hfE.p4().eta(),hfE.p4().phi());
	hfp_pt          ->Fill(hfE.pt());
      }
      else{
	hfm_eta         ->Fill(hfE.p4().eta());
	hfm_phi         ->Fill(hfE.p4().phi());
	hfm_eta_vs_phi  ->Fill(hfE.p4().eta(),hfE.p4().phi());
	hfm_pt          ->Fill(hfE.pt());
      }
    
      Yee   ->Fill(Z.Rapidity());
      Ptee  ->Fill(Z.pt());
      
      ec_eta->Fill(ecalE->eta());
    
      ec_phi->Fill(ecalE->phi());
      ec_eta_vs_phi->Fill(ecalE->eta(),ecalE->phi());

      ec_pt ->Fill(ecalE->pt());
      hf_eta->Fill(hfE.eta());
      hf_phi->Fill(hfE.phi());
      hf_pt ->Fill(hfE.pt());
      // HF Id variables after the cuts, as a cross check
      hf_e9e25  ->Fill(e9e25);
      hf_var2d  ->Fill(var2d);
      hf_eCOREe9->Fill(hfshape.eCOREe9());
      hf_eSeL   ->Fill(hfshape.eSeL());
    
      hf_eCOREe9eSeL->Fill(hfshape.eCOREe9(),hfshape.eSeL());

      hf_seed_eSeL->Fill(hfshape.eShort1x1()/hfshape.eLong1x1());
      hf_e1e9     ->Fill(hfshape.eLong1x1()/hfshape.eLong3x3());
      hf_e        ->Fill(hfshape.eLong3x3()); // GF: check this with Kevin
      hf_e1x1     ->Fill(hfshape.e1x1());
      hf_e3x3     ->Fill(hfshape.e3x3());
      hf_e5x5     ->Fill(hfshape.e5x5());
      hf_core     ->Fill(hfshape.eCore());

      if(ecalE->pt() > hfE.pt()){
	el_1_eta->Fill(ecalE->eta());
	el_2_eta->Fill(hfE.eta());
	el_1_eta_VS_el_2_eta->Fill(ecalE->eta(),hfE.eta());
	el_1_phi_VS_el_2_phi->Fill(ecalE->phi(),hfE.phi());
      }
      else{
	el_1_eta->Fill(hfE.eta());
	el_2_eta->Fill(ecalE->eta());
	el_1_eta_VS_el_2_eta->Fill(hfE.eta(),ecalE->eta());
	el_1_phi_VS_el_2_phi->Fill(hfE.phi(),ecalE->phi());
      }
    }
  }// END
  // if all selections are passed, fill standard plots
  
  //////////////////////////////////////////////////////////////////////////////////////
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/SimpleCutBasedEleID
  // here follows filling of histograms for selection variables when the other N-1_have passed
  // which cuts values are used? Depends on which parameter set you pass...
  std::vector<double> ebParam = myPs.getParameter< std::vector<double> >("barrel"); 
  std::vector<double> eeParam = myPs.getParameter< std::vector<double> >("endcap"); 


  float combIsolation_cut[2];
  float trackRel03_cut[2];
  float ecalRel03_cut[2];
  float hcalRel03_cut[2];
  float sigiEtaiEta_cut[2];
  float dPhi_cut[2];
  float dEta_cut[2];
  float HoE_cut[2];


  bool verboseDebug(false);
  
  // EB isolation variables
  combIsolation_cut[0] = ebParam.at(17); if(verboseDebug) std::cout  << "\n\ncombIsolationEB: " << combIsolation_cut[0] << std::endl;
  
  trackRel03_cut[0]    = ebParam.at(11); if(verboseDebug) std::cout  << "trackRel03EB: " << trackRel03_cut[0] << std::endl;
  ecalRel03_cut[0]     = ebParam.at(12); if(verboseDebug) std::cout  << "ecalRel03EB: " << ecalRel03_cut[0] << std::endl;
  hcalRel03_cut[0]     = ebParam.at(13); if(verboseDebug) std::cout  << "hcalRel03EB: " << hcalRel03_cut[0] << std::endl;
  
  // EB eleId variables
  sigiEtaiEta_cut[0]  = ebParam.at(1);  if(verboseDebug) std::cout  << "sigiEetaiEtaEB: " << sigiEtaiEta_cut[0] << std::endl;
  dPhi_cut[0]         = ebParam.at(2);  if(verboseDebug) std::cout  << "DphiEB: " << dPhi_cut[0] << std::endl;
  dEta_cut[0]         = ebParam.at(3);  if(verboseDebug) std::cout  << "DetaEB: " << dEta_cut[0] << std::endl;
  HoE_cut[0]          = ebParam.at(0);  if(verboseDebug) std::cout  << "HoEEB: " << HoE_cut[0] << std::endl;

  
  // EE isolation variables
  combIsolation_cut[1] = eeParam.at(17); if(verboseDebug) std::cout  << "\n\ncombIsolationEE: " << combIsolation_cut[1] << std::endl;
  
  trackRel03_cut[1]    = eeParam.at(11); if(verboseDebug) std::cout  << "trackRel03EE: " << trackRel03_cut[1] << std::endl;
  ecalRel03_cut[1]     = eeParam.at(12); if(verboseDebug) std::cout  << "ecalRel03EE: " << ecalRel03_cut[1] << std::endl;
  hcalRel03_cut[1]     = eeParam.at(13); if(verboseDebug) std::cout  << "hcalRel03EE: " << hcalRel03_cut[1] << std::endl;
  
  // EE eleId variables
  sigiEtaiEta_cut[1]  = eeParam.at(1);  if(verboseDebug) std::cout  << "sigiEetaiEtaEE: " << sigiEtaiEta_cut[1] << std::endl;
  dPhi_cut[1]          = eeParam.at(2);  if(verboseDebug) std::cout  << "DphiEE: " << dPhi_cut[1] << std::endl;
  dEta_cut[1]          = eeParam.at(3);  if(verboseDebug) std::cout  << "DetaEE: " << dEta_cut[1] << std::endl;
  HoE_cut[1]           = eeParam.at(0);  if(verboseDebug) std::cout  << "HoEEE: " << HoE_cut[1] << std::endl;
  

  //  //////////////////////////////////////////////////////////////////////////////////////
  //  // now get hold of the actual variables
  //  //hfE

  float combinedEcalIso;
  if (isEb) combinedEcalIso  = ( ecalE->dr03TkSumPt() + std::max(0., ecalE->dr03EcalRecHitSumEt() - 1.) + ecalE->dr03HcalTowerSumEt() ) / ecalE->p4().Pt();
  else      combinedEcalIso  = ( ecalE->dr03TkSumPt() + ecalE->dr03EcalRecHitSumEt() + ecalE->dr03HcalTowerSumEt() ) / ecalE->p4().Pt();

  // relative iso variable
  float trackRel03 = ecalE->dr03TkSumPt()/ecalE->p4().Pt();
  float ecalRel03  = ecalE->dr03EcalRecHitSumEt()/ecalE->p4().Pt(); 
  float hcalRel03  = ecalE->dr03HcalTowerSumEt()/ecalE->p4().Pt();

  // eleID variables  
  float sigiEtaiEta = ecalE->scSigmaIEtaIEta();
  float dEta        = ecalE->deltaEtaSuperClusterTrackAtVtx();
  float dPhi        = ecalE->deltaPhiSuperClusterTrackAtVtx();
  float HoE         = ecalE->hadronicOverEm();

  //////////////////////////////////////////////////////////////////////////////////////
  // now establish whether cuts are passed or not; 0-9 ECAL, 10-19 HF
  bool  cut[20];  for (int v =0; v<20; v++) cut[v]=false;
  short passedCuts=0;
  // ECAL electron cuts  
  if (combinedEcalIso < combIsolation_cut[isEe]) {cut[0] = true; passedCuts++; /*std::cout<<"\t c0 ";*/}
  // if (trackRel03      < trackRel03_cut[isEe])   {cut[1] = true; passedCuts++;/*std::cout<<"c1 ";*/}
  // if (ecalRel03       < ecalRel03_cut[isEe])     {cut[2] = true; passedCuts++;/*std::cout<<"c2 ";*/}
  // if (hcalRel03       < hcalRel03_cut[isEe])     {cut[3] = true; passedCuts++;/*std::cout<<"c3 ";*/}
  if (sigiEtaiEta     < sigiEtaiEta_cut[isEe])   {cut[4] = true; passedCuts++;/*std::cout<<"c4 ";*/}
  if (dEta            < dEta_cut[isEe])          {cut[5] = true; passedCuts++;/*std::cout<<"c5 ";*/}
  if (dPhi            < dPhi_cut[isEe])          {cut[6] = true; passedCuts++;/*std::cout<<"c6 ";*/}
  if (HoE             < HoE_cut[isEe])           {cut[7] = true; passedCuts++;/*std::cout<<"c7 ";*/}

  if (e9e25           > e9e25_cut)               {cut[10] = true; passedCuts++;/*std::cout<<"c10 ";*/}
  if (var2d           > hf_2d_cut)               {cut[11] = true; passedCuts++;/*std::cout<<"c11 ";*/}
  if (eSeL            < eSeL_cut )               {cut[12] = true; passedCuts++;/*std::cout<<"c12 ";*/}
  if (eCOREe9         > eCOREe9_cut)             {cut[13] = true; passedCuts++;/*std::cout<<"c13 ";*/}
  /*std::cout<<std::endl;*/
  short numCuts = 9; // was 7 when eSeL and eCOREe9 were not considered sigularly


  //////////////////////////////////////////////////////////////////////////////////////
  // all cuts passed     OR   all passed except present variable
  if(passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[0])) ) {
    if(isEb) combIsoEB_nmo -> Fill(combinedEcalIso);
    else     combIsoEE_nmo -> Fill(combinedEcalIso);
  }

  if(passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[4])) ) {
    if(isEb) sigiEtaiEtaEB_nmo -> Fill(sigiEtaiEta);
    else     sigiEtaiEtaEE_nmo -> Fill(sigiEtaiEta);
  }

  if(passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[5])) ) {
    if(isEb) detaEB_nmo -> Fill(dEta);
    else     detaEE_nmo -> Fill(dEta);
  }

  if(passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[6])) ) {
    if(isEb) dphiEB_nmo -> Fill(dPhi);
    else     dphiEE_nmo -> Fill(dPhi);
  }

  if(passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[7])) ) {
    if(isEb) hOeEB_nmo -> Fill(HoE);
    else     hOeEE_nmo -> Fill(HoE);
  }

  if(passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[10])) ) {
    e9e25_nmo -> Fill(e9e25);
  }

  // the following three variables are correlated
  // assume that only vard2 or eSeL&eCOREe9 will be used
  // and when not used, give huge values
  
  // 2d var
  if(      ( passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[11]))  )
	   && fabs(hf_2d_cut)<5
	   ) {
    var2d_nmo -> Fill(var2d); eSeL_nmo-> Fill(eSeL); eCOREe9_nmo -> Fill(eCOREe9);
  }
  // eSeL
  if(      ( passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[12]))  )
	   && fabs(eSeL_cut)<5
	   ) {
    var2d_nmo -> Fill(var2d); eSeL_nmo-> Fill(eSeL); eCOREe9_nmo -> Fill(eCOREe9);
  }
  // eCOREe9
  if(      ( passedCuts==numCuts || ( (passedCuts==(numCuts-1)) && (!cut[13]))  )
	   && fabs(eCOREe9_cut)<5
	   ) {
    var2d_nmo -> Fill(var2d); eSeL_nmo-> Fill(eSeL); eCOREe9_nmo -> Fill(eCOREe9);
  }

  
}// end of fill()

// this value for tight is lower than CMS AN-2009/106 to accont for S/L miscalib in data
// see figure 15 therein: turn-on is sharp
/*
static const double hf_2d_loose   = 0.32;
static const double hf_2d_tight   = 0.45;
static const double e9e25_loose   = 0.90;
static const double e9e25_tight   = 0.94;
*/
//////////////////////////////////////////////////////////////////////////////////////
// FIXME
// gf: bring HF variables to PSet as well, as opposed to having them hard-coded



//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HFZeeFilter::HFZeeFilter(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
  gsfElectrons_        = iConfig.getParameter<edm::InputTag>("ecalElectrons");
  hfElectrons_         = iConfig.getParameter<edm::InputTag>("hfElectrons");
  electronIDLabel_     = iConfig.getParameter<std::string>("ecalID");
  electronIDthreshold_ = iConfig.getParameter<int>("idThreshold");
  ecalMinEt_           = iConfig.getParameter<double>("ecalMinET");
  hfMinEt_             = iConfig.getParameter<double>("hfMinET");

  dolog_               = iConfig.getParameter<bool>("DoLog");
  hfIdParams_          = iConfig.getParameter< std::vector<double> >("hfSelParams");
  myName_              = iConfig.getParameter<std::string>("myName");
  massWindow_          = iConfig.getParameter< std::vector<double> >("zMassWindow");

  myName_+=std::string("    ");
  
  edm::Service<TFileService> fs;
  hists.nelec=fs->make<TH1D>("nelec","N_Elec",10,-0.5,9.5);
  hists.nhf=fs  ->make<TH1D>("nhf","N_HF",10,-0.5,9.5);
  hists.base.book(fs->mkdir("base"),"(base)",massWindow_);
  hists.basePt.book(fs->mkdir("basePt"),"(base with pT cuts)",massWindow_);
  hists.filteredEvents.book(fs->mkdir("filteredEvents"),"(user-defined filter)",massWindow_);  

  // import parameter set which carry threshold vaues
  eleID95Cuts_ps_     = iConfig.getParameter<edm::ParameterSet>("eleID95Cuts");
  eleIDFilterCuts_ps_ = iConfig.getParameter<edm::ParameterSet>("eleIDFilterCuts");
}
  
HFZeeFilter::~HFZeeFilter()
{
  
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
bool
HFZeeFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  bool filterOk=false;
  bool baseMinPt = false ; 

  edm::Handle<reco::RecoEcalCandidateCollection> HFElectrons;
  edm::Handle<reco::SuperClusterCollection> SuperClusters;
  edm::Handle<reco::HFEMClusterShapeAssociationCollection> ClusterAssociation;

  iEvent.getByLabel("hfEMClusters",SuperClusters);  
  iEvent.getByLabel("hfEMClusters",ClusterAssociation);
  iEvent.getByLabel(hfElectrons_,HFElectrons);

  edm::Handle<reco::GsfElectronCollection> gsfElectronCollection;
  iEvent.getByLabel(gsfElectrons_, gsfElectronCollection);
  if ( !gsfElectronCollection.isValid()) {
    std::cout << "No electrons found in this event with tag " << gsfElectrons_ << std::endl;
    return false; // RETURN if no elecs in the event
  }

  // if ( dolog_ ) { 
  //   std::cout << "Sizes: " << gsfElectronCollection->size() << ", " << HFElectrons->size() << std::endl ; 
  // }

  edm::Handle<edm::ValueMap<float> > eIDValueMap ; 
  iEvent.getByLabel(electronIDLabel_, eIDValueMap); 
  if ( !eIDValueMap.isValid()) {
    std::cout << "No electron ID value map found in this event with tag " << electronIDLabel_ << std::endl;
    return false; // RETURN if no eID map in the event
  }
  const edm::ValueMap<float> & eIDmap = *eIDValueMap;

  // reco::GsfElectronCollection::const_iterator eCand;
  reco::GsfElectronCollection::const_iterator ecalE=gsfElectronCollection->end();  
  for (unsigned int i=0; i<gsfElectronCollection->size(); i++) {
    // (i==0)?(eCand=gsfElectronCollection->begin()):(eCand++) ; 
    edm::Ref<reco::GsfElectronCollection> electronRef(gsfElectronCollection,i);
    if ( electronRef.isNull() ) { 
      std::cout << "Electron Ref is NULL" << std::endl ; 
      continue ; 
    }
    double electronID = eIDmap[electronRef] ; 
    // if ( dolog_ ) std::cout << "Processing electron " << i+1 << " of " 
    //  			    << gsfElectronCollection->size() << " with pT " 
    //  			    << electronRef->pt() << " and ID value "
    // 			    << electronID << std::endl ; 

    reco::SuperClusterRef scr=electronRef->superCluster();
    if ( scr.isNull() ) {
      std::cout << "Supercluster Ref is NULL" << std::endl ;
      continue;
    }
    double eta_det=scr.get()->eta();

    // if ( dolog_ ) { // Debugging
    //   if ( HFElectrons->size() > 0 && electronRef->pt() > ecalMinEt_ ) {
    //  	std::cout << iEvent.id() << std::endl ;
    //  	std::cout << "Electron with pT " << electronRef->pt() << " GeV has ID value " << electronID << std::endl ; 
    //    }
    // }

    // Explanation of electronID value: https://twiki.cern.ch/twiki/bin/view/CMS/SimpleCutBasedEleID
    // Passes electron iso/Id/convRej; electronIDLabel_ defines working point
    if ( (electronID >= electronIDthreshold_) &&
	 (fabs(eta_det)<1.4442 || fabs(eta_det)>1.560) ) { // ECAL acceptance cut
      // if (dolog_) std::cout << "Found a candidate" << std::endl ; 
      // ecalE = eCand ; // ecalE != end() iff candidate passes ID requirements
      // if (dolog_) std::cout << "ecalE pT is " << ecalE->pt() << std::endl ; 
      ecalE = gsfElectronCollection->begin() + i ; // ecalE != end() iff candidate passes ID requirements
      // if (dolog_) std::cout << "ecalE pT is " << ecalE->pt() << std::endl ; 
      break ; 
    }

  } 
  
  hists.nelec->Fill(gsfElectronCollection->size());
  hists.nhf  ->Fill(HFElectrons->size());

  if ((gsfElectronCollection->size()>0) && (HFElectrons->size()>0)) {
    unsigned int hfEleWithMaxPt = 0;
    for(unsigned int u=0; u<(HFElectrons->size()); u++) 
      if ( (*HFElectrons).at(u).pt() > (*HFElectrons).at(hfEleWithMaxPt).pt() ) hfEleWithMaxPt = u;    
    const reco::RecoEcalCandidate& hfE=(*HFElectrons).at(hfEleWithMaxPt);
    
    reco::SuperClusterRef           hfclusRef      = hfE.superCluster();
    const reco::HFEMClusterShapeRef hfclusShapeRef = (*ClusterAssociation).find(hfclusRef)->val;
    const reco::HFEMClusterShape&   hfshape        = *hfclusShapeRef;

    // if (dolog_) std::cout << "HF candidate index " << hfEleWithMaxPt << " of " << HFElectrons->size()
    //  			  << " with pT " << hfE.pt() << std::endl ; 

    baseMinPt = (hfE.pt() > hfMinEt_) && 
      ((ecalE!=gsfElectronCollection->end())?(ecalE->pt()>ecalMinEt_):(gsfElectronCollection->begin()->pt()>ecalMinEt_)) ;

    if (ecalE!=gsfElectronCollection->end()) {
      // if (dolog_) std::cout << "Filling base histograms with valid ECAL candidate" << std::endl ; 
      hists.base.fill(ecalE,hfE,hfshape,true,eleID95Cuts_ps_,-1,hfIdParams_);
      if (baseMinPt)
	hists.basePt.fill(ecalE,hfE,hfshape,true,eleID95Cuts_ps_,-1,hfIdParams_);
    }
    else {
      // if (dolog_) std::cout << "Filling base histograms with bogus ECAL candidate" << std::endl ; 
      hists.base.fill(gsfElectronCollection->begin(),hfE,hfshape,true,eleID95Cuts_ps_,-1,hfIdParams_);
      if (baseMinPt)
	hists.basePt.fill(gsfElectronCollection->begin(),hfE,hfshape,true,eleID95Cuts_ps_,-1,hfIdParams_);
    }

    if (ecalE!=gsfElectronCollection->end()) { // Found an electron meeting the filter requirements
      reco::Particle::LorentzVector Z(ecalE->p4());
      Z += hfE.p4();

      // if (dolog_) std::cout << "ECAL with pT " << ecalE->pt() << " and HF with pT " << hfE.pt()
      //  			    << " combines to a Z candidate with mass " << Z.M() << std::endl ; 

      if (ecalE->pt()>ecalMinEt_ && hfE.pt()>hfMinEt_) {
	// if (dolog_) std::cout << "I pass the filter!!!" << std::endl ; 
	filterOk = true ; // ECAL electron meets ID requirements, ECAL/HF electrons have sufficient pT

	double var2d=hfshape.eCOREe9()-(hfshape.eSeL()*1.125);
	double eta_det=ecalE->superCluster().get()->eta();
	if (dolog_) {
	  std::cout << myName_  << " ------------------------------------------------------" << std::endl;
	  std::cout << myName_  << " Candidate!: event " << iEvent.id().event() << " run " << iEvent.id().run() << std::endl ;
	  std::cout << myName_  << " m_ee=" << Z.M() << " Y_ee=" << Z.Rapidity() << " pT_ee=" << Z.pt() <<  std::endl;
	  std::cout << myName_  << " file: " << currentFile_ << std::endl;
	
	  // https://twiki.cern.ch/twiki/bin/view/CMS/SimpleCutBasedEleID#Electron_ID_Implementation_in_Re
	  std::cout << myName_  << " ECAL (pt,eta,phi; eta_det) : " 
		    << ecalE->pt() << ", " 
		    << ecalE->eta() << ", " 
		    << ecalE->phi() << " ; " 
		    << eta_det << std::endl;

	  std::cout << myName_  << "\t ECAL ele // REL ISOLATION -  trackRel03: " << ecalE->dr03TkSumPt()/ecalE->p4().Pt() 
		    << "\t ecalRel03: "  << ecalE->dr03EcalRecHitSumEt()/ecalE->p4().Pt() 
		    << "\t hcalRel03: "  <<  ecalE->dr03HcalTowerSumEt()/ecalE->p4().Pt() 
		    << "\n\t Electron ID - sigIetaIeta: " << ecalE->scSigmaIEtaIEta()
		    << "\t Deta: " << ecalE->deltaEtaSuperClusterTrackAtVtx()
		    << "\t Dphi: " << ecalE->deltaPhiSuperClusterTrackAtVtx()
		    << "\t H/E: "  << ecalE->hadronicOverEm()
		    << std::endl;

	  std::cout << myName_  << "  HF (pt, eta, phi): " << hfE.pt() << ", " << hfE.eta() << ", " << hfE.phi() << std::endl;
	  std::cout << myName_  << "  Shape (S/L, ec/e9, e1/e9, e9/e25, 2d) : " 
		    << hfshape.eSeL() << " "
		    << hfshape.eCOREe9() << " "
		    << hfshape.e1x1()/hfshape.e3x3() << " "
		    << hfshape.e9e25() << " "
		    << var2d 
		    << std::endl;

	  std::cout << myName_  << "======================================================" << std::endl;
	}

	hists.filteredEvents.fill(ecalE,hfE,hfshape,true,eleIDFilterCuts_ps_,-1,hfIdParams_);
	filterOk = filterOk && (Z.M()>massWindow_[0] && Z.M()<massWindow_[1]) ; 
      } // If both ECAL and HF electron candidates have sufficient pT
    } // If ECAL electron meets ID requirements 
  } // If ECAL and HF electron candidates present in the event

  return filterOk ;
}

//--- Electron ID reminder:                                            ---//
//--- 0 --> fails                                                      ---//
//--- 1: passes electron ID only                                       ---//
//--- 2: passes electron Isolation only                                ---//
//--- 3: passes electron ID and Isolation only                         ---// 
//--- 4: passes conversion rejection                                   ---//
//--- 5: passes conversion rejection and ID                            ---//
//--- 6: passes conversion rejection and Isolation                     ---//
//--- 7: passes the whole selection                                    ---//
//--- See https://twiki.cern.ch/twiki/bin/view/CMS/SimpleCutBasedEleID ---//

// ------------ method called once each job just before starting event loop  ------------
void HFZeeFilter::beginJob() {
}

// ------------ method called once each job just after ending the event loop  ------------
void HFZeeFilter::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(HFZeeFilter);
