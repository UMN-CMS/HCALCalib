import FWCore.ParameterSet.Config as cms

process = cms.Process("HFCALIB")

#--- Steps available to this configuration       ---#
#--- (1) Trigger filter.  Input data RAW/AOD     ---#
#--- (2) N(vertex) filter.  Only valid for MC    ---#
#--- (3) Reconstruction, assuming RAW input data ---#
#--- (4) Filtering on ECAL+HF, Z mass            ---#
#--- (5) HF Calibration analysis                 ---#
triggerFilter = True
nvertexFilter = False
doRerecoOnRaw = True
zFilterEcalHF = True
calibAnalysis = True

#--- Filter out events with no HF EM cluster above 5 GeV ---#
doHFclusterFiltering = True

#--- Which data era? ---#
#--- 532, not isRun2012C: Using data re-reco, Run 2012A/B, CMSSW version = 5_3_2_patch4         ---#
#--- 532, isRun2012C:     Using prompt reco, Run 2012C, Version 1, CMSSW version = 5_3_2_patch4 ---#
#--- 533, isRun2012C:     Using prompt reco, Run 2012C, Version 2, CMSSW version >= 5_3_3       ---#
#--- 533, not isRun2012C: Using prompt reco, Run 2012D, CMSSW version >= 5_3_3                  ---#
cmsswVersion = 532
isRun2012C   = True

#--- Cut levels: ---#
# 0: Testing only
# 1: Loose 2D,  e9/e25 = 0.80
# 2: Loose 2D,  e9/e25 = 0.92 <--- Recommended cut level
# 3: Medium 2D, e9/e25 = 0.96 
cutLevel = 2

#--- Testing flag: Do not enable unless needed! ---#
testing = False

#-----------------------------------#
#--- Flag for running on data/MC ---#
#-----------------------------------#
isData = False

#--- If running on MC, and nvertexFilter is True, then set the ---#
#--- inclusive range of vertices considered: [vtxMin,vtxMax]   ---#
vtxMin = 0 
vtxMax = 3
#--- If nvertexFilter is False, simply count vertices ---#
if not nvertexFilter: 
    vtxMin = 0
    vtxMax = -1

#--- Flag for keeping the skim results ---#
keepSkimEvents = False

#--- Implement specific conditions not in the Global Tag ---#
hfPhiSymCorrections = True

#--- Consistency checks
if isData: 
    if nvertexFilter:
        print "WARNING: Running on data but selecting subset of vertices."  
        print "FIX:     Histogram produced but setting vtxMax < 0."
        vtxMin = 0 
        vtxMax = -1 
else:
    if keepSkimEvents and not testing:
        print "WARNING: Running on MC but saving skimmed events.  FIX: Disabling keepSkimEvents."
        keepSkimEvents = False
    if triggerFilter:
        print "WARNING: Running on MC but using trigger filtering.  FIX: Disabling triggerFilter."
        triggerFilter = False
    if doRerecoOnRaw:
        print "WARNING: Running on MC but trying to re-reco.  FIX: Disabling doRerecoOnRaw."
        doRerecoOnRaw = False
    if hfPhiSymCorrections:
        print "WARNING: Running on MC but using new conditions.  FIX: Disabling hfPhiSymCorrections."
        hfPhiSymCorrections = False    

## Import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')

process.load("RecoEgamma.EgammaHFProducers.hfEMClusteringSequence_cff")
if cutLevel == 0:
    process.hfRecoEcalCandidate.e9e25Cut = 0.0 
    process.hfRecoEcalCandidate.intercept2DCut   = 0.0 
    process.hfRecoEcalCandidate.intercept2DSlope = 0.0 
if cutLevel == 1:
    process.hfRecoEcalCandidate.e9e25Cut = 0.80 
    process.hfRecoEcalCandidate.intercept2DCut   = 0.815
    process.hfRecoEcalCandidate.intercept2DSlope = 0.475
if cutLevel == 2:
    process.hfRecoEcalCandidate.e9e25Cut = 0.92
    process.hfRecoEcalCandidate.intercept2DCut   = 0.815
    process.hfRecoEcalCandidate.intercept2DSlope = 0.475
if cutLevel == 3:
    process.hfRecoEcalCandidate.e9e25Cut = 0.96
    process.hfRecoEcalCandidate.intercept2DCut   = 0.875
    process.hfRecoEcalCandidate.intercept2DSlope = 0.275

if isData and doRerecoOnRaw:
    process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
    process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
    process.load('Configuration.StandardSequences.L1Reco_cff')
    process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
# Not needed as MC from RECO, not RAW
# else:
#     process.load('SimGeneral.MixingModule.mixNoPU_cfi')
#     process.load('Configuration.StandardSequences.MagneticField_38T_cff')
#     process.load('Configuration.StandardSequences.RawToDigi_cff')
#     process.load('Configuration.StandardSequences.Reconstruction_cff')

## Global tags:
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
if isData:
    if doRerecoOnRaw:
        if cmsswVersion == 533:
            if not isRun2012C: # Run 2012D
                process.GlobalTag.globaltag = cms.string('GR_P_V42_AN2::All')
            else: # Run 2012C, v2
                process.GlobalTag.globaltag = cms.string('GR_P_V41_AN2::All')
        else:
            if cmsswVersion != 532:
                print "WARNING: Unrecognized CMSSW version.  Assuming 532 and continuing"
            if isRun2012C: # Run 2012C, v1
                process.GlobalTag.globaltag = cms.string('GR_P_V40_AN2::All')
            else:
                process.GlobalTag.globaltag = cms.string('FT_R_53_V6::All')
    else:
        process.GlobalTag.globaltag = cms.string('GR_P_V41_AN2::All')
else:
    # Note that this assumes the 53X Monte Carlo
    process.GlobalTag.globaltag = cms.string('START53_V7A::All')

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # eventsToProcess = cms.untracked.VEventRange(),
    fileNames = cms.untracked.vstring( 'file:input.root' )
)

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

###--- (2) N(vertex) Skim ---###
process.vtxFilter = cms.EDFilter('SimpleNVertexFilter',
    minNvtx = cms.int32(vtxMin),
    maxNvtx = cms.int32(vtxMax)
)
process.vertexFilterSequence = cms.Sequence(process.vtxFilter)

###--- (3) Re-RECO from RAW ---###
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

## Need to redo the HF clustering on Monte Carlo: requires RECO, not AOD
if not isData:
    process.redoHFclustering = cms.Sequence(process.hfEMClusteringSequence)

if doHFclusterFiltering:
    # turn HF superclusters into candidates
    process.hfCandidateSuperClusters = cms.EDProducer("ConcreteEcalCandidateProducer",
        src = cms.InputTag("hfEMClusters"),
        particleType = cms.string("gamma")
    )
    process.hfSC = cms.EDFilter("CandViewSelector",
        src = cms.InputTag("hfCandidateSuperClusters"),
        cut = cms.string('abs( eta ) > 2.9 & abs( eta ) < 5.1')
    )
    # select HF sucperclusters above a certain Et
    process.hfCut = cms.EDFilter("EtaPtMinCandViewSelector",
        src    = cms.InputTag("hfSC"),
        etaMin = cms.double(-5.1),
        etaMax = cms.double(5.1),
        ptMin  = cms.double(5.0)
    )
    # this is the FILTER which requires at least N=1 HF SC above a certain Et 
    process.hfSel = cms.EDFilter("CandViewCountFilter",
        src = cms.InputTag("hfCut"),
        minNumber = cms.uint32(1)
    )
    process.hfFilterSequence = cms.Sequence( process.hfCandidateSuperClusters + process.hfSC + process.hfCut + process.hfSel )
    if doRerecoOnRaw:
        process.rawHFfilterSequence = cms.Sequence(process.RawToDigi + process.hfreco + process.hfEMClusters + process.hfFilterSequence )

###--- (4) Require Z->ee, ECAL+HF ---###
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
    process.simpleCutBasedElectronID.rhoCorrection = cms.InputTag("kt6PFJetsForZeeCalib","rho")
    process.getElectronIDs = cms.Sequence(process.kt6PFJetsForZeeCalib * process.simpleEleIdSequence)

    import HCALCalib.HFZCalib.hfzeefilter_cfi
    process.hfzeeForCalib = HCALCalib.HFZCalib.hfzeefilter_cfi.hfzeefilter.clone()
    process.hfzeeForCalib.Zmass  = cms.vdouble(70,120)
    process.hfzeeForCalib.ecalID = cms.string('simpleEleId80cIso')
    process.hfzeeForCalib.DoLog  = cms.bool(True)
    # For testing ONLY!!!
    # if testing:
    #     process.hfzeeForCalib.idThreshold = cms.int32(1)

    process.calibInit = cms.Sequence( process.getElectronIDs ) 
    process.zCalibFilterSequence = cms.Sequence( process.calibInit * process.hfzeeForCalib ) 

###--- (5) HF Calibration Analysis ---###
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
    if not isData:
        process.TFileService.fileName = cms.string('HFZCalib_from_MC.root')

###--- Debugging ---###
if testing:
    process.genFilter = cms.EDFilter( 'ZeeGenFilter', 
        requireZeeHF  = cms.bool( True ),
        electronMinET = cms.double( 20.0 ),
        zMassWindow   = cms.vdouble(60,120),
        doLog         = cms.bool( True )
    )
    process.calibDebuggingSequence = cms.Sequence( process.genFilter )

###--- Assemble everything ---###
process.boolTrue = cms.EDFilter( 'HLTBool',
    result = cms.bool( True )
)
process.calibPreSequence = cms.Sequence(process.boolTrue)

if triggerFilter:
    process.calibPreSequence += process.triggerFilterSequence
if doHFclusterFiltering:
    if doRerecoOnRaw:
        process.calibPreSequence += process.rawHFfilterSequence 
    else:
        process.calibPreSequence += process.hfFilterSequence
if doRerecoOnRaw:
    process.calibPreSequence += process.reconstructionFromRawSequence
process.calibPreSequence += process.vertexFilterSequence    
if not isData:
    process.calibPreSequence += process.redoHFclustering
if zFilterEcalHF:
    process.calibPreSequence += process.zCalibFilterSequence

if not isData and testing:
    process.p = cms.Path( process.calibDebuggingSequence ) 
    process.q = cms.Path( process.calibPreSequence )
else:
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
