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
// $Id: HFZCalib.cc,v 1.5 2011/08/02 19:41:44 mansj Exp $
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
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
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
  int maxPU_;
  bool doMC_, doHits_;
  double minHFET_;
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
  maxPU_(iConfig.getUntrackedParameter<int>("maxPU",-1)),
  doMC_(iConfig.getUntrackedParameter<bool>("doMC",false)),
  doHits_(iConfig.getUntrackedParameter<bool>("doHits",false)),
  minHFET_(iConfig.getUntrackedParameter<double>("minHFET",12.0)),
  nvertexCut_(iConfig.getUntrackedParameter<int>("nvertexCut",-1))
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

  const HFRecHitCollection* phits=0;
  
  if (doHits_) {

    Handle<HFRecHitCollection> hits;
    iEvent.getByLabel(hfHits_,hits);
    phits=&(*hits);
  }
  
  theAnalysis.loadFromHF(*HFElectrons,*SuperClusters,*ClusterAssociation,phits);

}

bool
HFZCalib::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   Handle<pat::ElectronCollection> patElectrons;
   iEvent.getByLabel(selectedPatElectrons_,patElectrons);
   
   // std::cout << "I've got " << patElectrons->size() << " pat::Electrons!  How about you?\n";  

   loadFromHF(iEvent,iSetup);

   bool okPU=true;

   int npu=-1;

   if (!iEvent.eventAuxiliary().isRealData()) {
     
     if (doMC_) {

       Handle<HepMCProduct> hepMCEvt;
       iEvent.getByLabel("generator",hepMCEvt);
       const HepMC::GenEvent* genEvt=hepMCEvt->GetEvent();
       
       theAnalysis.loadFromGen(*genEvt);
     }

     if (maxPU_>=0) {
       edm::Handle<std::vector<PileupSummaryInfo> > pPU;
       iEvent.getByLabel("addPileupInfo", pPU);
       if(pPU.isValid() && pPU->size() > 0)
	 {
	   npu = pPU->at(0).getPU_NumInteractions();
	 }
       okPU=(npu<=maxPU_);
       
     }
     theAnalysis.fillPU(npu,okPU);
     if (!okPU) return false;   
   }

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


   //Cut on number of vertices
   if (nvertex != nvertexCut_ && nvertexCut_ != -1){ // -1 disables the vertex cut. //nvertex != 0){
     return false;
   }



   theAnalysis.analyze(*patElectrons);


   return theAnalysis.eventWasUseful();

}


// ------------ method called once each job just before starting event loop  ------------
void 
HFZCalib::beginJob()
{
  theAnalysis.setup(doMC_,doHits_,minHFET_);
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HFZCalib::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(HFZCalib);


