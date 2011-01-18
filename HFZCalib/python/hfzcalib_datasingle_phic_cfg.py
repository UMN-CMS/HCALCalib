import FWCore.ParameterSet.Config as cms

process = cms.Process("Calib1")

process.load("FWCore.MessageService.MessageLogger_cfi")

#from PhysicsTools.PatAlgos.tools.coreTools import *
## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

## Standard PAT Configuration File
#process.load("PhysicsTools.PatAlgos.patSequences_cff")

## global tag for data
process.GlobalTag.globaltag = 'START38_V7::All' ## needed for CMSSW_3_8_0 due to changes in the DB access for JEC ## process.GlobalTag.globaltag = cms.string('GR_R_35X_V8B::All')


#from PhysicsTools.PatAlgos.tools.metTools import *
#removeMCMatching(process, ['All'], outputInProcess = False)

#process.selectedPatElectrons.cut = 'pt > 20. '

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(110000) )

process.source = cms.Source("PoolSource",
                            eventsToProcess = cms.untracked.VEventRange('1:1-148655:max'),
                            fileNames = cms.untracked.vstring(         )
                            )

process.es_ascii = cms.ESSource("HcalTextCalibrations",
                        input = cms.VPSet(
        cms.PSet(
            object = cms.string('RespCorrs'),
            file = cms.FileInPath('Work/HFRescaler/data/testCombined_period1_v2.txt')
            )
        )
                        )
process.prefer("es_ascii")

process.hfrecalib=cms.EDProducer('HFRescaler',
                                 input = cms.InputTag('hfreco'),
                                 invert = cms.bool(False)
)
process.load("RecoEgamma.EgammaHFProducers.hfEMClusteringSequence_cff")

process.hfEMClusters.hits=cms.InputTag("hfrecalib")
process.hfRecoEcalCandidate.intercept2DCut=0.35

process.calib = cms.EDFilter('HFZCalib',
                             hfClusterShapes = cms.untracked.InputTag("hfEMClusters"),
                             hfRecoEcalCandidate = cms.untracked.InputTag("hfRecoEcalCandidate"),
                             hfHits = cms.untracked.InputTag("hfrecalib"),
                             selectedPatElectrons = cms.untracked.string('patElectrons')
)

process.TFileService = cms.Service("TFileService",
      fileName = cms.string('HFZCalib_from_data.root')
)


#process.p = cms.Path(process.patDefaultSequence*process.calib)
process.p = cms.Path(process.hfrecalib*process.hfEMClusteringSequence*process.calib)
