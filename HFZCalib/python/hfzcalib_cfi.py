import FWCore.ParameterSet.Config as cms

hfzcalib = cms.EDFilter('HFZCalib',
                        hfClusterShapes = cms.untracked.InputTag("hfEMClusters"),
                        hfRecoEcalCandidate = cms.untracked.InputTag("hfRecoEcalCandidate"),
                        hfHits = cms.untracked.InputTag("hfreco"),
                        selectedElectrons = cms.untracked.string('gsfElectrons'),
                        doHits = cms.untracked.bool(False),
                        minHFET = cms.untracked.double(12.0),
                        nvertexCut = cms.untracked.int32(-1)
)
