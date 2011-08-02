#ifndef HFZCalibAnalysis_h_included 
#define HFZCalibAnalysis_h_included 1
#include "TH2.h"
#include "TH1.h"
#include "TFile.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/EgammaReco/interface/HFEMClusterShapeAssociation.h"
#include "DataFormats/EgammaReco/interface/HFEMClusterShape.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
//#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
//#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include <map>
#include <list>
#include "CLHEP/Vector/LorentzVector.h"

class HFZCalibAnalysis {
 public:

  bool eventWasUseful() { return wasUseful_; }
  
  void loadFromGen(const HepMC::GenEvent& genE);
  void loadFromHF(const reco::RecoEcalCandidateCollection& theHFelecs,
		  const reco::SuperClusterCollection& SuperCluster,
		  const reco::HFEMClusterShapeAssociationCollection& AssocShape,
		  const HFRecHitCollection& theHits);
  void analyze(const pat::ElectronCollection& elecs
	       /*,
	       const HepMC::GenEvent& genE */);
  void fillPU(int npu, bool ok) {
    m_nPU->Fill(npu*1.0);
    if (ok) m_nPUu->Fill(npu*1.0);
  }
  void setup(bool doMC);
 private:
  struct HFCalibData {
    CLHEP::HepLorentzVector cluster_p4;
    double clus_eta, clus_phi;
    double ringOf,ringCentral,ringForward;
    double ringOf_t,ringCentral_t,ringForward_t;

    double ringOf_short,ringCentral_short,ringForward_short;

    HcalDetId clus_seed;
    CLHEP::HepLorentzVector gen_p4;
    bool gen_valid;
  };

  CLHEP::HepLorentzVector ecal_gen_p4;
  bool ecal_gen_valid;//if Ecal generated is valid?
  bool gen_is_EcalHF;// if generated is ECalHF
  bool wasUseful_;  // return wasteful if lectron was usless for analysis.
  bool m_doMC;

  std::vector<HFCalibData> m_calibs;
  TFile* m_file; //Histogram file

  struct OneDPlot {
    TH1D* numberOfEvents;
    //TH1D* NumOfZ;
    //TH1D* NumOfZOneECALOneHF;
    //TH1D* NumOfZECALId;
    //TH1D* NumOfZHFId;
    //TH1D* NumOfZPassZCut;
    void book(TFileDirectory& dir, const char* basename, const char* title, int bins, double low, double high);
    void fill(double x);
  };

  struct PlotPerRing {
    TH1D* plots[13*2];
    void book(TFileDirectory& dir, const char* basename, const char* title, int bins, double low, double high);
    void fill(double x, HcalDetId id);
  };
  // Matrix Calibration  scheme here
  struct CalibrationObjects {
    TH2D* matrix[2];
    TH1D* bvector[2];
    void book(TFileDirectory& dir, const char* basename, const char* title);
    void fill(double Ac, double Ao, double Af, double E, HcalDetId id);
  };

  OneDPlot m_numberOfEvents, m_NumOfZ, m_NumOfZOneECALOneHF, m_NumOfZECALId, m_NumOfZHFId, m_NumOfZPassZCut;// in this plot we are plotting number of Events, Number of Zs, Number of Z in Ecal and HF, The numnber of Z in Ecal id, Number of  Z in HFid, Number of Z which actually pass the Cut.
  TH1* m_nPU, *m_nPUu;

  PlotPerRing m_canHFEnergy, m_expHFEnergy, m_delExpCanHFEnergy, m_genHFEnergy, m_delExpGenHFEnergy, m_delExpCanHFEnergyECALTruth, m_delExpCanHFEnergyHFTruth, m_ringOf_t, m_ringCentral_t, m_ringForward_t, m_ringRatio_t, m_delCanRingHFEnergy, m_diElectronMassRing, m_invariantMassZ;

  PlotPerRing m_shortLongRatio;

  CalibrationObjects m_calibObjects, m_calibObjectsW;
  double m_recoFactor;



};


#endif // HFZCalibAnalysis_h_included 
