import FWCore.ParameterSet.Config as cms

process = cms.Process("HFCALIB")

#--- Steps available to this configuration       ---#
#--- (1) Trigger filter.  Input data RAW/AOD     ---#
#--- (2) Reconstruction, assuming RAW input data ---#
#--- (3) Filtering on ECAL+HF, Z mass            ---#
#--- (4) HF Calibration analysis                 ---#
triggerFilter = True
doRerecoOnRaw = True
zFilterEcalHF = True
calibAnalysis = True

#--- Testing flag ---#
testing = True

#--- Flag for running on data/MC ---#
isData = True

#--- Flag for keeping the skim results ---#
keepSkimEvents = True

#--- Implement specific conditions not in the Global Tag ---#
hfPhiSymCorrections = True

## Import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("RecoEgamma.EgammaHFProducers.hfEMClusteringSequence_cff")
if isData:
    process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
    process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
    process.load('Configuration.StandardSequences.L1Reco_cff')
    process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
else:
    process.load('SimGeneral.MixingModule.mixNoPU_cfi')
    process.load('Configuration.StandardSequences.MagneticField_38T_cff')
    process.load('Configuration.StandardSequences.RawToDigi_cff')
    process.load('Configuration.StandardSequences.Reconstruction_cff')

## Global tags:
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
if isData:
    if doRerecoOnRaw:
        # process.GlobalTag.globaltag = cms.string('GR_R_53_V2::All')
        process.GlobalTag.globaltag = cms.string('FT_R_53_V6::All')
        # process.GlobalTag.globaltag = cms.string('FT_R_53_V10::All')
    else:
        process.GlobalTag.globaltag = cms.string('GR_P_V41_AN2::All')
else:
    process.GlobalTag.globaltag = cms.string('UNKNOWN::All')

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    eventsToProcess = cms.untracked.VEventRange(),
    fileNames = cms.untracked.vstring( 'file:input.root' )
)
process.source.eventsToProcess.extend([
'191226:864:1158913821-191226:864:1158913821',
'191226:866:1161616410-191226:866:1161616410',
'191226:866:1161233536-191226:866:1161233536',
'191226:866:1161308200-191226:866:1161308200',
'191226:868:1163352130-191226:868:1163352130',
'191226:868:1164035205-191226:868:1164035205',
'191226:869:1165040197-191226:869:1165040197',
'191226:869:1164727492-191226:869:1164727492',
'191226:872:1168041287-191226:872:1168041287'
])

process.out = cms.OutputModule( "PoolOutputModule",
    fileName = cms.untracked.string("output.root"),
    SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
    outputCommands = cms.untracked.vstring( 'keep *' )
)

###--- (1) HLT Skim ---###
process.hltPickTriggered = cms.EDFilter('TriggerResultsFilter',
    hltResults            = cms.InputTag('TriggerResults','','HLT'),  # Ignore HLT if empty
    l1tResults            = cms.InputTag(''),                         # Ignore L1 if empty
    l1tIgnoreMask         = cms.bool(False),                          # Ignore L1 mask if True
    l1techIgnorePrescales = cms.bool(False),                          # Ignore prescales for L1Tech if True
    daqPartitions         = cms.uint32(0x01),                         # Used by the definition of the L1 mask
    throw                 = cms.bool(False),                          # Exception on unknown trigger names if True
    triggerConditions     = cms.vstring( 
        'HLT_Ele27_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele15_CaloIdT_CaloIsoVL_trackless_v*',
        'HLT_Ele27_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_HFT15_v*',
        'HLT_Ele23_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_HFT30_v*',
        'HLT_Ele20_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_SC4_Mass50_v*'
    )
)
process.triggerFilterSequence = cms.Sequence(process.hltPickTriggered)

###--- (2) Re-RECO from RAW ---###
### Auto generated configuration file using Revision: 1.381.2.6 
### Source: /local/reps/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
### with command line options: -s RAW2DIGI,RECO ...

#--- New HF phi-symmetry corrections ---#
if hfPhiSymCorrections:
    process.es_pool_hf = cms.ESSource("PoolDBESSource",  
        process.CondDBSetup,
        timetype = cms.string('runnumber'),
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string("HcalRespCorrsRcd"),
                tag = cms.string("HcalRespCorrs_v4.3_offline")
            ),
            cms.PSet(
                record = cms.string('HcalGainsRcd'),
                tag = cms.string('HcalGains_v5.06_offline')
            ),
        ),
        connect = cms.string('frontier://FrontierProd/CMS_COND_31X_HCAL'),
        authenticationMethod = cms.untracked.uint32(0)
    )
    process.es_prefer_es_pool = cms.ESPrefer('PoolDBESSource','es_pool_hf')

if doRerecoOnRaw:
    process.reconstructionFromRawSequence = cms.Sequence(process.RawToDigi * process.L1Reco * process.reconstruction)

###--- (3) Require Z->ee, ECAL+HF ---###
if zFilterEcalHF:
    process.load('RecoJets.JetProducers.kt4PFJets_cfi') # For isolation calculation
    process.kt6PFJetsForZeeCalib = process.kt4PFJets.clone(
        rParam = cms.double(0.6),
        doRhoFastjet = True,
        Rho_EtaMax = cms.double(2.5),
    )

    import ElectroWeakAnalysis.WENu.simpleCutBasedElectronIDSpring10_cfi
    process.load("RecoLocalCalo.EcalRecAlgos.EcalSeverityLevelESProducer_cfi")
    process.load("ElectroWeakAnalysis.WENu.simpleEleIdSequence_cff")
    process.getElectronIDs = cms.Sequence(process.kt6PFJetsForZeeCalib * process.simpleEleIdSequence)

    import HCALCalib.HFZCalib.hfzeefilter_cfi
    process.hfzeeForCalib = HCALCalib.HFZCalib.hfzeefilter_cfi.hfzeefilter.clone()
    process.hfzeeForCalib.Zmass  = cms.vdouble(70,120)
    process.hfzeeForCalib.ecalID = cms.string('simpleEleId80cIso')
    process.hfzeeForCalib.DoLog  = cms.bool(True)
    # For testing ONLY!!!
    if testing:
        process.hfzeeForCalib.idThreshold = cms.int32(1)

    ## Medium cut values
    process.load("RecoEgamma.EgammaHFProducers.hfRecoEcalCandidate_cfi")
    process.hfRecoEcalCandidate.intercept2DCut   = 0.875
    process.hfRecoEcalCandidate.intercept2DSlope = 0.275
    process.hfRecoEcalCandidate.e9e25Cut         = 0.96

    process.calibInit = cms.Sequence( process.getElectronIDs * process.hfRecoEcalCandidate ) 
    process.zCalibFilterSequence = cms.Sequence( process.calibInit * process.hfzeeForCalib ) 

###--- (4) HF Calibration Analysis ---###
if calibAnalysis:
    import HCALCalib.HFZCalib.hfzcalib_cfi
    process.calib = HCALCalib.HFZCalib.hfzcalib_cfi.hfzcalib.clone(
        hfHits     = cms.untracked.InputTag("notused"),
        minHFET    = cms.untracked.double(20.0), # Value fixed from the Z->ee filter
        nvertexCut = cms.int32(-1)
    )

    ###--- Histograms ---###
    process.TFileService = cms.Service("TFileService",
        fileName = cms.string('HFZCalib_from_data.root')
    )

###--- Assemble everything ---###
process.boolTrue = cms.EDFilter( 'HLTBool',
    result = cms.bool( True )
)
process.calibPreSequence = cms.Sequence(process.boolTrue)

if triggerFilter:
    process.calibPreSequence += process.triggerFilterSequence
if doRerecoOnRaw:
    process.calibPreSequence += process.reconstructionFromRawSequence
if zFilterEcalHF:
    process.calibPreSequence += process.zCalibFilterSequence

process.p = cms.Path( process.calibPreSequence )

if zFilterEcalHF and calibAnalysis:
    for nvtx in range(0,50):
        calibByNvtx = process.calib.clone( nvertexCut = cms.untracked.int32(nvtx) )
        modLabel = "calibByNvtx" + str(nvtx)
        setattr(process, modLabel, calibByNvtx)

        calibPathByNvtx = cms.Path( process.calibPreSequence * getattr(process,modLabel) )
        pathLabel = "calibPathByNvtx" + str(nvtx)
        setattr(process, pathLabel, calibPathByNvtx)

if keepSkimEvents:
    process.finalOutput = cms.EndPath( process.out )
