import FWCore.ParameterSet.Config as cms

process = cms.Process("Calib1")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(         )
)

process.load("RecoEgamma.EgammaHFProducers.hfEMClusteringSequence_cff")

process.hfRecoEcalCandidate.intercept2DCut=0.38

process.calib = cms.EDFilter('HFZCalib',
                             hfClusterShapes = cms.untracked.InputTag("hfEMClusters"),
                             hfRecoEcalCandidate = cms.untracked.InputTag("hfRecoEcalCandidate"),
                             hfHits = cms.untracked.InputTag("hfreco"),
                             selectedPatElectrons = cms.untracked.string('patElectrons')
)

process.TFileService = cms.Service("TFileService",
      fileName = cms.string('HFZCalib_from_data.root')
)


process.p = cms.Path(process.hfRecoEcalCandidate*process.calib)
