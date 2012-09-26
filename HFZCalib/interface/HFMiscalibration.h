#ifndef _HFMISCALIBRATION_H
#define _HFMISCALIBRATION_H

#include <memory>
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include <vector>

class HFZRecHitMiscalib : public edm::EDProducer {
 public:
  HFZRecHitMiscalib(const edm::ParameterSet& iConfig);
  ~HFZRecHitMiscalib(); 

    void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

private:
    float scaleFactor(int ieta, int scenario);

    edm::InputTag hfLabel_;
    int scenario_;
    std::vector<float> m_factors;
    std::vector<double> hand_factors_;
    
};

#endif
