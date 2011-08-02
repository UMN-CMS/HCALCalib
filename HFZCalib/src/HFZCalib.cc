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
// $Id: HFZCalib.cc,v 1.2 2011/01/18 02:44:14 mansj Exp $
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
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

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
  bool doMC_;
  int nvertexCut_;

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
  hfHits_(iConfig.getUntrackedParameter<edm::InputTag>("hfHits")),
  doMC_(iConfig.getUntrackedParameter<bool>("doMC",false))

{
   //now do what ever initialization is needed
  nvertexCut_ = iConfig.getParameter<int>("nvertexCut");


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

   Handle<reco::VertexCollection> pvHandle;
   iEvent.getByLabel("offlinePrimaryVertices", pvHandle);
   const reco::VertexCollection & vertices = *pvHandle.product();
   static const int minNDOF = 4;
   static const double maxAbsZ = 15.0;
   static const double maxd0 = 2.0;
   
   //count verticies
   int nvertex = 0;
   for(reco::VertexCollection::const_iterator vit = vertices.begin(); vit != vertices.end(); ++vit)
     {
       if(vit->ndof() > minNDOF && ((maxAbsZ <= 0) || fabs(vit->z()) <= maxAbsZ) && ((maxd0 <= 0) || fabs(vit->position().rho()) <= maxd0)) nvertex++;
     }

   loadFromHF(iEvent,iSetup);

   //Cut on number of vertices
   if (nvertex != nvertexCut_ && nvertexCut_ != -1){ // -1 disables the vertex cut. //nvertex != 0){
     return false;
   }

   if (!iEvent.eventAuxiliary().isRealData() && doMC_) {

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
  theAnalysis.setup(doMC_);
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HFZCalib::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(HFZCalib);
