#include "TF1.h"
#include "Work/HFZCalib/interface/HFZCalibAnalysis.h"
#include <sstream>
#include <algorithm>
#include <list>
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Math/interface/deltaPhi.h"

using namespace reco;


// This is my  Analyzer

void HFZCalibAnalysis::setup()

{
  
  edm::Service<TFileService> fs;
  m_canHFEnergy.book(*fs,"recoHFieta%+dEnergy","Reco HF Energy",100,0,2000);  
  m_expHFEnergy.book(*fs,"predHFieta%+dEnergy","Predicted HF Energy",100,0,2000);
  m_delExpCanHFEnergy.book(*fs,"delExpCanHF%dEnergy","Reco/Pred HF Energy",500,0,5);
  m_genHFEnergy.book(*fs,"genHFieta%+dEnergy","Gen HF Energy",100,0,2000);
  m_delExpGenHFEnergy.book(*fs,"delExpGenHF%dEnergy","Gen/Pred HF Energy",500,0,5);
  m_delExpCanHFEnergyECALTruth.book(*fs,"delExpCanHF%dEnergyECALTruth","Reco/Pred HF Energy w/ ECAL truth",500,0,5);
  m_delExpCanHFEnergyHFTruth.book(*fs,"delExpCanHF%dEnergyHFTruth","Reco/Pred HF Energy w/ HF Truth",500,0,5);
  m_ringOf_t.book(*fs,"ringOf_t%d","ringOf Et",100,0,100);
  m_ringCentral_t.book(*fs,"ringCentral_t%d","ringCentral Et",100,0,100);
  m_ringForward_t.book(*fs,"ringForward_t%d","ringForward Et",100,0,100);
  m_ringRatio_t.book(*fs,"ringRatio_t%d","Ratio of RingOf Et to the sum of Ets",100,0,5);
  m_diElectronMassRing.book(*fs,"diElectronMassRing%d","dielectron mass",100,0,200);
  m_invariantMassZ.book(*fs,"invariantMass%d","Invariant Mass of Z",100,0,200);
  m_numberOfEvents.book(*fs,"numberOfEvents","Number of Events",100,0,1000000);
  m_NumOfZ.book(*fs,"NumOfZ","Number Of Z Events",10,-0.5,9.5);
  m_NumOfZOneECALOneHF.book(*fs,"NumOfZOneECALOneHF","Number of Z Events with One ECAL and One HF Electron",100,0,100000);
  m_NumOfZECALId.book(*fs,"NumOfZECALId","Number of Z Events where ECAL electron passes Id cuts",100,0,100000);
  m_NumOfZHFId.book(*fs,"NumOfZHFId","Number of Z Events where HF electron passes Id cuts",100,0,100000);
  m_NumOfZPassZCut.book(*fs,"NumOfZPassZCut","Number of Z Events that pass mass cut",100,0,100000);
  
  m_canHFEnergyPlus98.book(*fs,"recoHFieta%+dEnergyPlus98","Reco HF Energy>=98% of Energy in Seed Cell",100,0,2000);  
  m_expHFEnergyPlus98.book(*fs,"predHFieta%+dEnergyPlus98","Predicted HF Energy>=98% of Energy in Seed Cell",100,0,2000);
  m_delExpCanHFEnergyPlus98.book(*fs,"delExpCanHF%dEnergyPlus98","Reco/Pred HF Energy>=98% of Energy in Seed Cell",500,0,5);
  m_genHFEnergyPlus98.book(*fs,"genHFieta%+dEnergyPlus98","Gen HF Energy>=98% of Energy in Seed Cell",100,0,2000);
  m_delExpGenHFEnergyPlus98.book(*fs,"delExpGenHF%dEnergyPlus98","Gen/Pred HF Energy>=98% of Energy in Seed Cell",500,0,5);
  m_delExpCanHFEnergyECALTruthPlus98.book(*fs,"delExpCanHF%dEnergyECALTruthPlus98","Reco/Pred HF Energy w/ ECAL truth>=98% of Energy in Seed Cell",500,0,5);
  m_delExpCanHFEnergyHFTruthPlus98.book(*fs,"delExpCanHF%dEnergyHFTruthPlus98","Reco/Pred HF Energy w/ HF Truth>=98% of Energy in Seed Cell",500,0,5);
  m_ringOf_t_Plus98.book(*fs,"ringOf_t%d_Plus98","ringOf Et>=98% of Energy in Seed Cell",100,0,100);
  m_ringCentral_t_Plus98.book(*fs,"ringCentral_t%d_Plus98","ringCentral Et>=98% of Energy in Seed Cell",100,0,100);
  m_ringForward_t_Plus98.book(*fs,"ringForward_t%d_Plus98","ringForward Et>=98% of Energy in Seed Cell",100,0,100);
  
  m_canHFEnergyMinus98.book(*fs,"recoHFieta%+dEnergyMinus98","Reco HF Energy<98% of Energy in Seed Cell",100,0,2000);  
  m_expHFEnergyMinus98.book(*fs,"predHFieta%+dEnergyMinus98","Predicted HF Energy<98% of Energy in Seed Cell",100,0,2000);
  m_delExpCanHFEnergyMinus98.book(*fs,"delExpCanHF%dEnergyMinus98","Reco/Pred HF Energy<98% of Energy in Seed Cell",500,0,5);
  m_genHFEnergyMinus98.book(*fs,"genHFieta%+dEnergyMinus98","Gen HF Energy<98% of Energy in Seed Cell",100,0,2000);
  m_delExpGenHFEnergyMinus98.book(*fs,"delExpGenHF%dEnergyMinus98","Gen/Pred HF Energy<98% of Energy in Seed Cell",500,0,5);
  m_delExpCanHFEnergyECALTruthMinus98.book(*fs,"delExpCanHF%dEnergyECALTruthMinus98","Reco/Pred HF Energy w/ ECAL truth<98% of Energy in Seed Cell",500,0,5);
  m_delExpCanHFEnergyHFTruthMinus98.book(*fs,"delExpCanHF%dEnergyHFTruthMinus98","Reco/Pred HF Energy w/ HF Truth<98% of Energy in Seed Cell",500,0,5);
  m_ringOf_t_Minus98.book(*fs,"ringOf_t%d_Minus98","ringOf Et<98% of Energy in Seed Cell",100,0,100);
  m_ringCentral_t_Minus98.book(*fs,"ringCentral_t%d_Minus98","ringCentral Et<98% of Energy in Seed Cell",100,0,100);
  m_ringForward_t_Minus98.book(*fs,"ringForward_t%d_Minus98","ringForward Et<98% of Energy in Seed Cell",100,0,100);
  
  m_delCanRingHFEnergy.book(*fs,"delCanRingHFieta%dEnergy","Reco/Sum of Ring Energies",500,0,5);

  m_calibObjects.book(*fs,"Standard","Standard calibration objects");
  m_calibObjectsW.book(*fs,"Weighted","Weighted (reco-like) calibration objects");

  ecal_gen_valid=false;

  // hardcode because I'm a bad person (Jeremy)
  m_recoFactor=1.0/0.544;
}

int numOfEvents=0;  // Number of events
int NumOfZECALId=0; //   Number of Z in Ecal Id
int NumOfZHFId=0; //    number of Z in HF id
int NumOfZPassZCut=0; // Number of Z  that Pass the Z cut
int NumOfZ=0;       // Number of Z
int NumOfZOneECALOneHF=0; // Number of Z one in Ecal and one in HF

void HFZCalibAnalysis::analyze(const pat::ElectronCollection& elecs
			       /*
			       const HepMC::GenEvent& genE */)
 {

  bool reject=false;
  wasUseful_=false;

  if (elecs.size() != 1 || elecs[0].et()<=15) {
    reject=true;
  }// reject any electron with energy less than or equal to 15  GeV
 else {
   NumOfZECALId++;// go to the next  ECal identity
    if (!reject)
      {
	m_NumOfZ.fill(3); // if you did not reject electron then fill  it in plot
	if (ecal_gen_valid) m_NumOfZ.fill(4); // fill number Z if Generated Z in Ecal is valid
    }
  }
  
  if (m_calibs.size() != 1 || (m_calibs[0].cluster_p4.e()/cosh(m_calibs[0].cluster_p4.eta()))<=10) {
    reject=true;
  } else {
    NumOfZHFId++;
    if (!reject) {
      m_NumOfZ.fill(5);// fill in    Number of Z that actually reached   HF
      if (ecal_gen_valid) m_NumOfZ.fill(6);// if  the electron went to Ecal then  fill  that number of Z in Histogram
    }
  }
  
  if (reject) return;

  wasUseful_=true;

  const pat::Electron& theECAL=elecs[0];
  HFCalibData& theHF=m_calibs[0];

  if(elecs.size() != 0 || m_calibs.size() != 0){
    numOfEvents++;
      }

  m_numberOfEvents.fill(numOfEvents);
  m_NumOfZECALId.fill(NumOfZECALId);
  m_NumOfZHFId.fill(NumOfZHFId);

  std::cout << "Found single ECAL electron and single HF cluster." << std::endl;
  if (theHF.gen_valid) {
    std::cout << theHF.cluster_p4.eta() << " " << theHF.gen_p4.eta() << " ";
    std::cout << theHF.cluster_p4.phi() << " " << theHF.gen_p4.phi() << " ";
    std::cout << theHF.cluster_p4.e() << " " << theHF.gen_p4.e() << " ";
    std::cout << std::endl;
  }

  if (ecal_gen_valid) {
    std::cout << theECAL.eta() << " " << ecal_gen_p4.eta() << " ";
    std::cout << theECAL.phi() << " " << ecal_gen_p4.phi() << " ";
    std::cout << theECAL.energy() << " " << ecal_gen_p4.e() << " ";
    std::cout << std::endl;
  }

  double Zmass = 91.2;
  double deleta = cosh(elecs[0].eta() - theHF.clus_eta);
  double delphi = cos(deltaPhi(elecs[0].phi(),theHF.clus_phi));
  double ECALgenDelEta = cosh(ecal_gen_p4.eta() - theHF.cluster_p4.eta());
  double ECALgenDelPhi = cos(deltaPhi(ecal_gen_p4.phi(),theHF.cluster_p4.phi()));
  double HFgenDelEta = cosh(theECAL.eta() - theHF.gen_p4.eta());
  double HFgenDelPhi = cos(deltaPhi(theECAL.phi(),theHF.gen_p4.phi()));

  double HFelEcan = (Zmass*Zmass*cosh(theECAL.eta())*cosh(theHF.clus_eta))/(2*theECAL.energy()*(deleta - delphi));
      
  double HFelEgenECALTruth = (Zmass*Zmass*cosh(ecal_gen_p4.eta())*cosh(theHF.cluster_p4.eta()))/(2*ecal_gen_p4.e()*(ECALgenDelEta - ECALgenDelPhi));

  double HFelEgenHFTruth = (Zmass*Zmass*cosh(theECAL.eta())*cosh(theHF.gen_p4.eta()))/(2*theECAL.energy()*(HFgenDelEta - HFgenDelPhi));

  double diElectronMassRing = sqrt(2*(theHF.ringOf+theHF.ringForward+theHF.ringCentral)*theECAL.energy()*(deleta - delphi)/(cosh(theECAL.eta())*cosh(theHF.clus_eta)));

  double InvariantMassZ = sqrt(2*theHF.cluster_p4.e()*theECAL.energy()*(deleta - delphi)/(cosh(theECAL.eta())*cosh(theHF.clus_eta)));

  if (65 < InvariantMassZ && InvariantMassZ < 110)   //ifinvariant mass of z is between 65 and 110
        {
	  NumOfZPassZCut++;
	  m_NumOfZ.fill(7);
	  if (ecal_gen_valid) m_NumOfZ.fill(8);
	}
  m_NumOfZPassZCut.fill(NumOfZPassZCut);

  /*double HighCut, LowCut;
  std::cout << "Please enter high cut, then low cut values for the Reco/Pred graph" << std::endl;
  std::cin >> HighCut >> LowCut;
  std::cout << "High Cut is: " << HighCut << " Low Cut is: " << LowCut << std::endl;*/
  
  if (65 < InvariantMassZ && InvariantMassZ < 110){

    double scaleCutRatio=theHF.cluster_p4.e()/std::max(0.0001,HFelEcan);

    if (0.2 < scaleCutRatio && scaleCutRatio < 2) 

{

  m_calibObjects.fill(theHF.ringCentral,theHF.ringOf,theHF.ringForward,HFelEcan,theHF.clus_seed);
  m_calibObjectsW.fill(theHF.ringCentral*m_recoFactor,theHF.ringOf*m_recoFactor,theHF.ringForward*m_recoFactor,HFelEcan,theHF.clus_seed);
  }

  m_canHFEnergy.fill(theHF.cluster_p4.e(),theHF.clus_seed);
  m_expHFEnergy.fill(HFelEcan,theHF.clus_seed);
  m_delExpCanHFEnergy.fill(theHF.cluster_p4.e()/HFelEcan,theHF.clus_seed);
  m_genHFEnergy.fill(theHF.gen_p4.e(),theHF.clus_seed);
  m_delExpGenHFEnergy.fill(theHF.gen_p4.e()/HFelEcan,theHF.clus_seed);
  m_delExpCanHFEnergyECALTruth.fill(theHF.cluster_p4.e()/HFelEgenECALTruth,theHF.clus_seed);
  m_delExpCanHFEnergyHFTruth.fill(theHF.cluster_p4.e()/HFelEgenHFTruth,theHF.clus_seed);
  m_ringOf_t.fill(theHF.ringOf_t,theHF.clus_seed);
  m_ringCentral_t.fill(theHF.ringCentral_t,theHF.clus_seed);
  m_ringForward_t.fill(theHF.ringForward_t,theHF.clus_seed);
  m_ringRatio_t.fill(theHF.ringOf_t/std::max(0.0001,theHF.ringOf_t+theHF.ringCentral_t+theHF.ringForward_t),theHF.clus_seed);
  m_delCanRingHFEnergy.fill(theHF.cluster_p4.e()/std::max(0.0001,theHF.ringOf*m_recoFactor+theHF.ringCentral*m_recoFactor+theHF.ringForward*m_recoFactor),theHF.clus_seed);
  }
  
  //if(70 < diElectronMassRing < 110){
  m_diElectronMassRing.fill(diElectronMassRing,theHF.clus_seed);
  //}

  //if(70 < InvariantMassZ < 110) {
  m_invariantMassZ.fill(InvariantMassZ,theHF.clus_seed);
  //}

  if(65 < InvariantMassZ && InvariantMassZ < 110){

if(theHF.ringOf_t/std::max(0.0001,theHF.ringOf_t+theHF.ringCentral_t+theHF.ringForward_t) >= .98)

 {
    m_canHFEnergyPlus98.fill(theHF.cluster_p4.e(),theHF.clus_seed);
    m_expHFEnergyPlus98.fill(HFelEcan,theHF.clus_seed);
    m_delExpCanHFEnergyPlus98.fill(theHF.cluster_p4.e()/HFelEcan,theHF.clus_seed);
    m_genHFEnergyPlus98.fill(theHF.gen_p4.e(),theHF.clus_seed);
    m_delExpGenHFEnergyPlus98.fill(theHF.gen_p4.e()/HFelEcan,theHF.clus_seed);
    m_delExpCanHFEnergyECALTruthPlus98.fill(theHF.cluster_p4.e()/HFelEgenECALTruth,theHF.clus_seed);
    m_delExpCanHFEnergyHFTruthPlus98.fill(theHF.cluster_p4.e()/HFelEgenHFTruth,theHF.clus_seed);
    m_ringOf_t_Plus98.fill(theHF.ringOf_t,theHF.clus_seed);
    m_ringCentral_t_Plus98.fill(theHF.ringCentral_t,theHF.clus_seed);
    m_ringForward_t_Plus98.fill(theHF.ringForward_t,theHF.clus_seed);
  } 
			 
  if(theHF.ringOf_t/std::max(0.0001,theHF.ringOf_t+theHF.ringCentral_t+theHF.ringForward_t) < .98)

 {
    m_canHFEnergyMinus98.fill(theHF.cluster_p4.e(),theHF.clus_seed);
    m_expHFEnergyMinus98.fill(HFelEcan,theHF.clus_seed);
    m_delExpCanHFEnergyMinus98.fill(theHF.cluster_p4.e()/HFelEcan,theHF.clus_seed);
    m_genHFEnergyMinus98.fill(theHF.gen_p4.e(),theHF.clus_seed);
    m_delExpGenHFEnergyMinus98.fill(theHF.gen_p4.e()/HFelEcan,theHF.clus_seed);
    m_delExpCanHFEnergyECALTruthMinus98.fill(theHF.cluster_p4.e()/HFelEgenECALTruth,theHF.clus_seed);
    m_delExpCanHFEnergyHFTruthMinus98.fill(theHF.cluster_p4.e()/HFelEgenHFTruth,theHF.clus_seed);
    m_ringOf_t_Minus98.fill(theHF.ringOf_t,theHF.clus_seed);
    m_ringCentral_t_Minus98.fill(theHF.ringCentral_t,theHF.clus_seed);
    m_ringForward_t_Minus98.fill(theHF.ringForward_t,theHF.clus_seed);
  }
}
}

//  I really can't undersatnd what this function that is returning nothing doing? God Help Me.
void HFZCalibAnalysis::loadFromHF(const reco::RecoEcalCandidateCollection& theHFelecs,
				  const reco::SuperClusterCollection& SuperCluster,
				  const reco::HFEMClusterShapeAssociationCollection& AssocShape,
				  const HFRecHitCollection& theHits){
  m_calibs.clear();
  HFCalibData anItem;
  reco::RecoEcalCandidateCollection::const_iterator hf;

  for (hf=theHFelecs.begin(); hf!=theHFelecs.end(); hf++)
 {
    anItem.clus_eta=hf->eta();
    anItem.clus_phi=hf->phi();
    anItem.ringOf=0;
    anItem.ringCentral=0;
    anItem.ringForward=0;
    anItem.ringOf_t=0;
    anItem.ringCentral_t=0;
    anItem.ringForward_t=0;

    anItem.gen_valid=false;    
    
    //   std::cout << hf->pt() << " " << hf->eta() << std::endl;
    
    anItem.cluster_p4=CLHEP::HepLorentzVector(hf->px(),
					      hf->py(),
					      hf->pz(),
					      hf->energy());
    
    SuperClusterRef theClusRef=hf->superCluster();
    const HFEMClusterShapeRef clusShapeRef=AssocShape.find(theClusRef)->val;
    const HFEMClusterShape& clusShape=*clusShapeRef;
    
    anItem.clus_seed=clusShape.seed();
    HFRecHitCollection::const_iterator ahit; // actually putting the calibration constants by hand! but how do the affect base  case
    static const float ietaEtas[] = {-5.054, -4.8165, -4.626, -4.4645, -4.2905, -4.1155, -3.94, -3.765, -3.59, -3.415, -3.2395, -3.064, -2.921, 2.921, 3.064, 3.2395, 3.415, 3.59, 3.765, 3.94, 4.1155, 4.2905, 4.4645, 4.626, 4.8165, 5.054};

    for (int de=-1; de<=1; de++) {
      for (int dp=-2; dp<=2; dp+=2) {
	const int ieta=anItem.clus_seed.ieta()+(de*anItem.clus_seed.zside());
	int iphi=anItem.clus_seed.iphi()+dp;

	if (iphi<1) iphi+=72;
	if (iphi>72) iphi-=72;
	// this doesn't handle towers 40,41 right!
	HcalDetId target(HcalForward,ieta,iphi,1); // my target
	ahit=theHits.find(target); // aiming...
	if (ahit==theHits.end()) continue; // missed!
	if (ahit->energy()<3.0) continue; // remove very low energy hits
	if (de==-1) { // more central
	  anItem.ringCentral+=ahit->energy();
	  int index=(ieta<0)?(ieta+41):(ieta-16);
	  anItem.ringCentral_t+=ahit->energy()/cosh(ietaEtas[index]);
	} else if (de==0) {
	  anItem.ringOf+=ahit->energy();
	  int index=(ieta<0)?(ieta+41):(ieta-16);
	  anItem.ringOf_t+=ahit->energy()/cosh(ietaEtas[index]);
	} else {
	  anItem.ringForward+=ahit->energy();
	  int index=(ieta<0)?(ieta+41):(ieta-16);
	  anItem.ringForward_t+=ahit->energy()/cosh(ietaEtas[index]);
	}
      }
    }

    m_calibs.push_back(anItem); // copy into the list (last thing!)
  }
}
// I also really do not know what this stupid code is doing here! God where are you?
void HFZCalibAnalysis::loadFromGen(const HepMC::GenEvent& genE)

 {
  //----------------------------------------------------
  //Now we look for the particle at the generator level
  HepMC::GenEvent::vertex_const_iterator vtex;
  
  HepMC::GenVertex::particles_out_const_iterator Pout;
  
  std::vector<HepMC::GenParticle*> Ecales;
  std::vector<HepMC::GenParticle*> Hfs;
  
  bool hadz=false;
  gen_is_EcalHF=false;

  for (vtex=genE.vertices_begin(); vtex!=genE.vertices_end(); vtex++){
    if(((*vtex)->particles_in_size())==1){
      if((*((*vtex)->particles_in_const_begin()))->pdg_id()==23){
	// now we have the Z
	for(Pout=(*vtex)->particles_out_const_begin();Pout!=(*vtex)->particles_out_const_end();Pout++){
	  NumOfZ++;
	  hadz=true;
	  if (abs((*Pout)->pdg_id())==11){
	    double abseta=fabs((*Pout)->momentum().eta());
	    
	    if (abseta>=3 && abseta<=5) { // we're in HF
	      Hfs.push_back(*Pout);
	    } else if (abseta<=3) { // we're in ECAL
	      Ecales.push_back(*Pout);
	    }
	  }
	}
	
      } 
    }
  }

  m_NumOfZ.fill(0); // we saw the event
  if (hadz) m_NumOfZ.fill(1);

  ecal_gen_valid=false;
  bool hadhfz=false;

  if (Hfs.size()>=1 && m_calibs.size()==1) { // only case where we care!
    m_calibs[0].gen_p4=CLHEP::HepLorentzVector(Hfs[0]->momentum().px(),
					       Hfs[0]->momentum().py(),
					       Hfs[0]->momentum().pz(),
					       Hfs[0]->momentum().e());
    m_calibs[0].gen_valid=true;
    if (Ecales.size()>=1) {
      ecal_gen_p4=CLHEP::HepLorentzVector(Ecales[0]->momentum().px(),
					  Ecales[0]->momentum().py(),
					  Ecales[0]->momentum().pz(),
					  Ecales[0]->momentum().e());
      ecal_gen_valid=true;
      NumOfZOneECALOneHF++;
      hadhfz=true;
    }    
  }
  if (hadhfz) m_NumOfZ.fill(2);

}

 
void HFZCalibAnalysis::OneDPlot::book(TFileDirectory& dir, const char* basename, const char* title, int bins, double low, double high)

 {
   numberOfEvents=dir.make<TH1D>(basename,title,bins,low,high); // makse histograms of what? may be of events
  //NumOfZ=dir.make<TH1D>(basename,title,bins,low,high);
  //NumOfZOneECALOneHF=dir.make<TH1D>(basename,title,bins,low,high);
  //NumOfZECALId=dir.make<TH1D>(basename,title,bins,low,high);
  //NumOfZHFId=dir.make<TH1D>(basename,title,bins,low,high);
  //NumOfZPassZCut=dir.make<TH1D>(basename,title,bins,low,high);
}

void HFZCalibAnalysis::OneDPlot::fill(double x) {
  numberOfEvents->Fill(x);
  //NumOfZ->Fill(x);
  //NumOfZOneECALOneHF->Fill(x);
  //NumOfZECALId->Fill(x);
  //NumOfZHFId->Fill(x);
  //NumOfZPassZCut->Fill(x);
}
   
 void HFZCalibAnalysis::PlotPerRing::book(TFileDirectory& dir, const char* basename, const char* title, int bins, double low, double high) {
  for (int i=-41; i<=41; i++) {
    if (i>-29 && i<29) continue;
    int index=(i<0)?(i+41):(i-(29-13));


    char name[1024];
    sprintf(name,basename,i);
    
    plots[index]=dir.make<TH1D>(name,title,bins,low,high);
  }
}


void HFZCalibAnalysis::PlotPerRing::fill(double x, HcalDetId id) {
  int i=id.ieta();
  if (i>-29 && i<29) return;
  int index=(i<0)?(i+41):(i-(29-13));

  plots[index]->Fill(x);
}

void HFZCalibAnalysis::CalibrationObjects::book(TFileDirectory& dir, const char* basename, const char* title) {

  char name[1024];
  sprintf(name,"m%s+",basename);
  matrix[0]=dir.make<TH2D>(name,title,13,28.5,41.5,13,28.5,41.5);
  sprintf(name,"m%s-",basename);
  matrix[1]=dir.make<TH2D>(name,title,13,-41.5,-28.5,13,-41.5,-28.5);

  sprintf(name,"b%s+",basename);
  bvector[0]=dir.make<TH1D>(name,title,13,28.5,41.5);
  sprintf(name,"b%s-",basename);
  bvector[1]=dir.make<TH1D>(name,title,13,-41.5,-28.5);

}


void HFZCalibAnalysis::CalibrationObjects::fill(double Ac, double Ao, double Af, double E, HcalDetId id) {

  int of=id.ieta();
  int forward=(of<0)?(of-1):(of+1);
  int central=(of<0)?(of+1):(of-1);

  if (abs(of)<30 || abs(of)>40) return; // need edge bins!

  int index=(of<0)?(1):(0);

  matrix[index]->Fill(forward,forward,Af*Af);
  matrix[index]->Fill(forward,of,Af*Ao);
  matrix[index]->Fill(forward,central,Af*Ac);

  matrix[index]->Fill(of,forward,Ao*Af);
  matrix[index]->Fill(of,of,Ao*Ao);
  matrix[index]->Fill(of,central,Ao*Ac);

  matrix[index]->Fill(central,forward,Ac*Af);
  matrix[index]->Fill(central,of,Ac*Ao);
  matrix[index]->Fill(central,central,Ac*Ac);

  bvector[index]->Fill(forward,E*Af);
  bvector[index]->Fill(of,E*Ao);
  bvector[index]->Fill(central,E*Ac);
}
