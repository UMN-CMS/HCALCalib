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
  void setup();
 private:
  struct HFCalibData {
    CLHEP::HepLorentzVector cluster_p4;
    double clus_eta, clus_phi;
    double ringOf,ringCentral,ringForward;
    double ringOf_t,ringCentral_t,ringForward_t;
    HcalDetId clus_seed;
    CLHEP::HepLorentzVector gen_p4;
    bool gen_valid;
  };

  CLHEP::HepLorentzVector ecal_gen_p4;
  bool ecal_gen_valid;//if Ecal generated is valid?
  bool gen_is_EcalHF;// if generated is ECalHF
  bool wasUseful_;  // return wasteful if lectron was usless for analysis.

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

  PlotPerRing m_canHFEnergyPlus98, m_expHFEnergyPlus98, m_delExpCanHFEnergyPlus98, m_genHFEnergyPlus98, m_delExpGenHFEnergyPlus98, m_delExpCanHFEnergyECALTruthPlus98, m_delExpCanHFEnergyHFTruthPlus98, m_ringOf_t_Plus98, m_ringCentral_t_Plus98, m_ringForward_t_Plus98, m_canHFEnergyMinus98, m_expHFEnergyMinus98, m_delExpCanHFEnergyMinus98, m_genHFEnergyMinus98, m_delExpGenHFEnergyMinus98, m_delExpCanHFEnergyECALTruthMinus98, m_delExpCanHFEnergyHFTruthMinus98, m_ringOf_t_Minus98, m_ringCentral_t_Minus98, m_ringForward_t_Minus98, m_canHFEnergy, m_expHFEnergy, m_delExpCanHFEnergy, m_genHFEnergy, m_delExpGenHFEnergy, m_delExpCanHFEnergyECALTruth, m_delExpCanHFEnergyHFTruth, m_ringOf_t, m_ringCentral_t, m_ringForward_t, m_ringRatio_t, m_delCanRingHFEnergy, m_diElectronMassRing, m_invariantMassZ;

  CalibrationObjects m_calibObjects, m_calibObjectsW;
  double m_recoFactor;


  /* TH1F* m_genHFieta_30Energy, * m_genHFieta_31Energy, * m_genHFieta_32Energy, * m_genHFieta_33Energy, * m_genHFieta_34Energy, * m_genHFieta_35Energy, * m_genHFieta_36Energy, * m_genHFieta_37Energy, * m_genHFieta_38Energy, * m_genHFieta_39Energy, * m_genHFieta_40Energy, * m_genHFieta30Energy, * m_genHFieta31Energy, * m_genHFieta32Energy, * m_genHFieta33Energy, * m_genHFieta34Energy, * m_genHFieta35Energy, * m_genHFieta36Energy, * m_genHFieta37Energy, * m_genHFieta38Energy, * m_genHFieta39Energy, * m_genHFieta40Energy, * m_delHFieta_30Energy, * m_delHFieta_31Energy, * m_delHFieta_32Energy, * m_delHFieta_33Energy, * m_delHFieta_34Energy, * m_delHFieta_35Energy, * m_delHFieta_36Energy, * m_delHFieta_37Energy, * m_delHFieta_38Energy, * m_delHFieta_39Energy, * m_delHFieta_40Energy, * m_delHFieta30Energy, * m_delHFieta31Energy, * m_delHFieta32Energy, * m_delHFieta33Energy, * m_delHFieta34Energy, * m_delHFieta35Energy, * m_delHFieta36Energy, * m_delHFieta37Energy, * m_delHFieta38Energy, * m_delHFieta39Energy, * m_delHFieta40Energy, * m_delExpGenHFieta30, * m_delExpGenHFieta31, * m_delExpGenHFieta32, * m_delExpGenHFieta33, * m_delExpGenHFieta34, * m_delExpGenHFieta35, * m_delExpGenHFieta36, * m_delExpGenHFieta37, * m_delExpGenHFieta38, * m_delExpGenHFieta39, * m_delExpGenHFieta40; */
};


#endif // HFZCalibAnalysis_h_included 
