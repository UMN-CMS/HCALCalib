// -*- C++ -*-
//
// Package:    HFZCalib
// Class:      SimpleNVertexFilter
// 
/**\class SimpleNVertexFilter SimpleNVertexFilter.cc HCALCalib/HFZCalib/src/SimpleNVertexFilter.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Perrie Cole
//         Created:  Wed Jun 17 15:21:36 CDT 2009
// $Id: SimpleNVertexFilter.cc,v 1.8 2012/09/20 09:19:44 bdahmes Exp $
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
#include "FWCore/ServiceRegistry/interface/Service.h"
#include <iostream>
#include <vector>
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"
//
// class decleration
//

class SimpleNVertexFilter : public edm::EDFilter {
public:
  explicit SimpleNVertexFilter(const edm::ParameterSet&);
  ~SimpleNVertexFilter();

private:
  virtual void beginJob() ;
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  int min_, max_;

  struct HistStruct {
    TH1 *nvtx;
  } hists;

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
SimpleNVertexFilter::SimpleNVertexFilter(const edm::ParameterSet& iConfig) :
  min_(iConfig.getParameter<int>("minNvtx")),
  max_(iConfig.getParameter<int>("maxNvtx")) {

  edm::Service<TFileService> fs;
  hists.nvtx=fs->make<TH1D>("nvtx","N_vtx",50,-0.5,49.5);
}


SimpleNVertexFilter::~SimpleNVertexFilter() {
}


//
// member functions
//

bool SimpleNVertexFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   edm::Handle<reco::VertexCollection> pvHandle;
   iEvent.getByLabel("offlinePrimaryVertices", pvHandle);
   const reco::VertexCollection & vertices = *pvHandle.product();
   static const int    minNDOF = 4;
   static const double maxAbsZ = 15.0;
   static const double maxd0   = 2.0;
   
   // Count verticies
   int nvertex = 0;
   for (reco::VertexCollection::const_iterator vit = vertices.begin(); vit != vertices.end(); ++vit) {
     if ( (vit->ndof() > minNDOF) && 
	  ((maxAbsZ <= 0) || (fabs(vit->z()) <= maxAbsZ)) && 
	  ((maxd0 <= 0) || (fabs(vit->position().rho()) <= maxd0)) ) nvertex++;
   }
   hists.nvtx->Fill(nvertex);

   // Escape: max < 0 or max < min passes all vertices
   if ( max_ < 0 || (max_ < min_) ) return true ; 

   bool keepEvent = ( nvertex >= min_ ) && ( nvertex <= max_ ) ; 
   return keepEvent ; 
}


// ------------ method called once each job just before starting event loop  ------------
void 
SimpleNVertexFilter::beginJob() {
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SimpleNVertexFilter::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(SimpleNVertexFilter);


