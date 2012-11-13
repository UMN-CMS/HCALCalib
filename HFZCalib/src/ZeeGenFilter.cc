// -*- C++ -*-
//
// Package:    ZeeGenFilter
// Class:      ZeeGenFilter
// 
/**\class ZeeGenFilter ZeeGenFilter.cc HCALCalib/HFZCalib/src/ZeeGenFilter.cc

 Description: Modified version of HFZeeVBTF removing PAT requirements

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jeremy M Mans
//         Created:  Mon May 31 07:00:26 CDT 2010
// $Id: ZeeGenFilter.cc,v 1.1 2012/09/20 09:19:44 bdahmes Exp $
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

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

class ZeeGenFilter : public edm::EDFilter {
public:
  explicit ZeeGenFilter(const edm::ParameterSet&);
  ~ZeeGenFilter();

private:
  virtual void beginJob() ;
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  bool requireZeeHF_ ; 
  double minPt_;
  std::vector<double> massWindow_;

  bool dolog_;
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
ZeeGenFilter::ZeeGenFilter(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed
  requireZeeHF_        = iConfig.getParameter<bool>("requireZeeHF");
  minPt_               = iConfig.getParameter<double>("electronMinET");
  massWindow_          = iConfig.getParameter< std::vector<double> >("zMassWindow");
  dolog_               = iConfig.getParameter<bool>("doLog");
}
  
ZeeGenFilter::~ZeeGenFilter()
{
  
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
bool
ZeeGenFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  bool filterOk = false;

  edm::Handle<edm::HepMCProduct> HepMC;

  if (!iEvent.isRealData()) {
    iEvent.getByLabel("generator",HepMC);
    const HepMC::GenEvent* genE=HepMC->GetEvent();

    HepMC::GenEvent::vertex_const_iterator vtex;
    HepMC::GenVertex::particles_out_const_iterator Pout;
    HepMC::GenParticle* ge0=0;
    HepMC::GenParticle* ge1=0;
    HepMC::GenParticle* Z=0;
    for (vtex=genE->vertices_begin(); vtex!=genE->vertices_end(); vtex++){
      if (((*vtex)->particles_in_size())==1) {
	if ((*((*vtex)->particles_in_const_begin()))->pdg_id()==23){
	  Z=(*((*vtex)->particles_in_const_begin()));
	  for (Pout=(*vtex)->particles_out_const_begin();
	       Pout!=(*vtex)->particles_out_const_end(); Pout++){
	    if (abs((*Pout)->pdg_id())==11){
	      if(ge0==0){
		ge0=*Pout;
	      } else {
		ge1=*Pout;
	      }
	    }
	  }
	}
      }
    }

    double mass = Z->momentum().m() ; 
    
    if ( ge0 != 0 && ge1 != 0 ) { 
      if ( dolog_ ) { 
	std::cout << iEvent.id() << std::endl ; 
	std::cout << "Electron 1 with pT " << ge0->momentum().perp() << ", eta " << ge0->momentum().eta() 
		  << "; Electron 2 with pT " << ge1->momentum().perp() << ", eta " << ge1->momentum().eta() 
		  << "; Z with mass " << mass << std::endl ; 
      }
      if ( mass > massWindow_[0] && mass < massWindow_[1] ) { 
	if ( ge0->momentum().perp() > minPt_ && 
	     ge1->momentum().perp() > minPt_ ) { 
	  if ( requireZeeHF_ ) { 
	    bool ecalE = ( fabs(ge0->momentum().eta()) < 2.5 || fabs(ge1->momentum().eta()) < 2.5 ) ; 
	    bool hfE   = ( ( fabs(ge0->momentum().eta()) > 3.0 && fabs(ge0->momentum().eta()) < 5.0 ) ||
			   ( fabs(ge1->momentum().eta()) > 3.0 && fabs(ge1->momentum().eta()) < 5.0 ) ) ; 
	    filterOk = (ecalE && hfE) ; 
	    if ( filterOk && dolog_ ) 
	      std::cout << "Event with ECAL-HF Z: " << iEvent.id() << std::endl ; 
	  } else filterOk = true ; 
	}
      }
    }
  }

  return filterOk ; 
}

// ------------ method called once each job just before starting event loop  ------------
void ZeeGenFilter::beginJob() {
}

// ------------ method called once each job just after ending the event loop  ------------
void ZeeGenFilter::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(ZeeGenFilter);
