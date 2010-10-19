#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Work/HFZCalib/interface/HFMiscalibration.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include <iostream>
#include <vector>

using namespace std;

HFZRecHitMiscalib::HFZRecHitMiscalib(const edm::ParameterSet& iConfig)
{

  hfLabel_ = iConfig.getParameter<edm::InputTag>("hfInput");

  scenario_ = iConfig.getParameter<int>("miscalibScenario"); 
 
  if (scenario_==6) {
  // hand put in miscalibration scenario
   hand_factors_.reserve(26);
   hand_factors_ = iConfig.getParameter< std::vector<double> >("factors");


   if (hand_factors_.size()!=26) {
     std::cout << "Wrong  Factors: " << hand_factors_.size() << std::endl;
    hand_factors_.resize(26);
    hand_factors_.assign(26,1); 
   };
  }  
  /*if (scenario_ == 6) {
    m_factors=iConfig.getParameter<std::vector<float> >("factors");
    }*/

  //register your products
  produces< HFRecHitCollection >();
}

HFZRecHitMiscalib::~HFZRecHitMiscalib(){
 }

int linid;

float HFZRecHitMiscalib::scaleFactor(int ieta, int scenario){
  //float scaleFactor(int ieta, int scenario){
  //int linid;
  if (ieta < 0) linid = ieta + 41;
  else linid = 13 + (ieta - 29);

  static const float factors1 [26] = { 1.200, 1.168, 1.136, 1.104, 1.072, 1.040, 1.008, .976, .944, .912, .880, .848, .816, 
				       .816, .848, .880, .912, .944, .976, 1.008, 1.040, 1.072, 1.104, 1.136, 1.168, 1.200 };
  static const float factors2 [26] = { .81, .94, .96, .81, .95, .82, .93, 1.02, 1.12, 1.04, 1.19, 1.07, 1.06, .84, 1.06, 1.04, 1.15, 1.10, .92, 1.10, 1.03, .83, .84, 1.08, .82, 1.01 };
  static const float factors3 [26] = {1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800, 1.200, .800};
  static const float factors4 [26] = {1.200, 1.000, .800, 1.200, 1.000, .800, 1.200, 1.000, .800, 1.200, 1.000, .800, 1.200, 1.000, .800, 1.200, 1.000, .800, 1.200, 1.000, .800, 1.200, 1.000, .800, 1.200, 1.000};
  static const float factors5 [26] = {.816, .848, .880, .912, .944, .976, 1.008, 1.040, 1.072, 1.104, 1.136, 1.168, 1.200,1.200, 1.168, 1.136, 1.104, 1.072, 1.040, 1.008, .976, .944, .912, .880, .848, .81};


  switch (scenario) {
  case 1:
    return factors1[linid];         //linearly scales energy by ieta
    break;
  case 2:
    return factors2[linid];         //randomly scales by ieta with rms of .989
    break;
  case 3:
    return factors3[linid];         //sinusoidal wave
    break;
  case 4:
     return factors4[linid];         //sawtooth formation
    break;
  case 5:
    return factors5[linid];         //inverse linearly scales energy by ieta
    break;
  case 6:
    // std::cout << " Hahaha,This is sccenario 6" << std::endl;
    return hand_factors_[linid];         // Hand_input  miscal
    break;
  }
  return 0;
}



void HFZRecHitMiscalib::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
  using namespace edm;
  using namespace std;

  //Read in HFRecHits
  edm::Handle<HFRecHitCollection> orig_HFrecHits;
  iEvent.getByLabel (hfLabel_, orig_HFrecHits);

  //create an empty HFRecHitCollection
  std::auto_ptr< HFRecHitCollection > MiscalibHFRecHitCollection( new HFRecHitCollection );

  //Loop over all the HF recHits
  for( HFRecHitCollection::const_iterator misHF = orig_HFrecHits->begin(); misHF != orig_HFrecHits->end(); ++ misHF )
{

    //calling miscalibration function scaleFactor(ieta, scenarioNumber)
    float ietascale;
    /*if (scenario_==6) ietascale=m_factors[linid];
    else*/

     ietascale = scaleFactor(misHF->id().ieta(),scenario_);

     std::cout << " The new scale of energy is:" << ietascale << std::endl;
    
     HFRecHit aHit(misHF->id(),misHF->energy()*ietascale,misHF->time()); // this is where the Miscallibration factors  affect the energy distrubution directly.
    MiscalibHFRecHitCollection->push_back( aHit );
  }

  iEvent.put( MiscalibHFRecHitCollection );

}


DEFINE_FWK_MODULE(HFZRecHitMiscalib);
// DEFINE_ANOTHER_FWK_MODULE(HFZRecHitMiscalib);
