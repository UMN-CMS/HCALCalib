// -*- C++ -*-
//
// Package:    HFZCalib
// Class:      HFZCalib
// 
/**\class HFZCalib HFZCalib.cc MyWork/HFZCalib/src/HFZCalib.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Perrie Cole
//         Created:  Wed Jun 17 15:21:36 CDT 2009
// $Id: HFZCalib.cc,v 1.1 2010/10/19 19:39:29 mansj Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "Work/HFZCalib/interface/HFZCalibAnalysis.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "DataFormats/EgammaReco/interface/HFEMClusterShapeAssociation.h"
#include "DataFormats/EgammaReco/interface/HFEMClusterShape.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
//#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include <iostream>
#include <vector>

//
// class decleration
//

//   HFZCalib is my Filter
class HFZCalib : public edm::EDFilter {
   public:
      explicit HFZCalib(const edm::ParameterSet&);
      ~HFZCalib();


   private:
      virtual void beginJob() ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void loadFromHF(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

  std::string selectedPatElectrons_;
  edm::InputTag hfRecoEcalCandidate_,hfClusterShapes_,hfHits_;
      // ----------member data ---------------------------
  HFZCalibAnalysis theAnalysis;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HFZCalib::HFZCalib(const edm::ParameterSet& iConfig) :
  selectedPatElectrons_(iConfig.getUntrackedParameter<std::string>("selectedPatElectrons")),
  hfRecoEcalCandidate_(iConfig.getUntrackedParameter<edm::InputTag>("hfRecoEcalCandidate")),
  hfClusterShapes_(iConfig.getUntrackedParameter<edm::InputTag>("hfClusterShapes")),
  hfHits_(iConfig.getUntrackedParameter<edm::InputTag>("hfHits"))

{
   //now do what ever initialization is needed


}


HFZCalib::~HFZCalib()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------


void HFZCalib::loadFromHF(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  Handle<reco::RecoEcalCandidateCollection> HFElectrons;
  iEvent.getByLabel(hfRecoEcalCandidate_,HFElectrons);
  Handle<reco::SuperClusterCollection> SuperClusters;
  iEvent.getByLabel(hfClusterShapes_,SuperClusters);
  Handle<reco::HFEMClusterShapeAssociationCollection> ClusterAssociation;
  iEvent.getByLabel(hfClusterShapes_,ClusterAssociation);
  Handle<HFRecHitCollection> hits;
  iEvent.getByLabel(hfHits_,hits);
  
  theAnalysis.loadFromHF(*HFElectrons,*SuperClusters,*ClusterAssociation,*hits);
  //std::cout << "I've got " << HFElectrons->size() << " reco::Electrons!  How about you?\n" ;

}

bool
HFZCalib::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   Handle<pat::ElectronCollection> patElectrons;
   iEvent.getByLabel(selectedPatElectrons_,patElectrons);
   
   // std::cout << "I've got " << patElectrons->size() << " pat::Electrons!  How about you?\n";  

   loadFromHF(iEvent,iSetup);

   if (!iEvent.eventAuxiliary().isRealData()) {

     Handle<HepMCProduct> hepMCEvt;
     iEvent.getByLabel("generator",hepMCEvt);
     const HepMC::GenEvent* genEvt=hepMCEvt->GetEvent();

     theAnalysis.loadFromGen(*genEvt);
   }

   theAnalysis.analyze(*patElectrons);


   return theAnalysis.eventWasUseful();

}


// ------------ method called once each job just before starting event loop  ------------
void 
HFZCalib::beginJob()
{
  theAnalysis.setup();
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HFZCalib::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(HFZCalib);
