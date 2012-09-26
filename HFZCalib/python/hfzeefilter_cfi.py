import FWCore.ParameterSet.Config as cms

# to access values of EldId cuts
import ElectroWeakAnalysis.WENu.simpleCutBasedElectronIDSpring10_cfi

hfzeefilter = cms.EDFilter('HFZeeFilter',
     myName        = cms.string('HFZeeFilter-simpleEleId80cIso'),
     DoLog         = cms.bool(False),

     ecalElectrons = cms.InputTag("gsfElectrons"),
     hfElectrons   = cms.InputTag("hfRecoEcalCandidate"),
     ecalID        = cms.string('simpleEleId80cIso'),
     idThreshold   = cms.int32(7),
     ecalMinET     = cms.double(30.0),
     hfMinET       = cms.double(20.0),
     zMassWindow   = cms.vdouble(40,130),

     # The following params are, respectively: 
     #   e9e25_loose, e9e25_tight, var2d_loose, var2d_tight, 
     #   eCOREe9_loose, eCOREe9_tight, eSeL_loose, eSeL_tight;
     # Set to -9999/+9999 if you want to neglect a cut, resp eCOREe9/eSeL
     # NOTE: These values are not used by the filter, but could be of interest for plotting
     hfSelParams     =  cms.vdouble(0.90, 0.94, 0.2, 0.40, -9999, -9999, 9999, 9999),

     # Used for plotting only
     eleID95Cuts     = cms.PSet(ElectroWeakAnalysis.WENu.simpleCutBasedElectronIDSpring10_cfi.simpleCutBasedElectronID.robust95cIsoEleIDCutsV20.clone()),
     eleIDFilterCuts = cms.PSet(ElectroWeakAnalysis.WENu.simpleCutBasedElectronIDSpring10_cfi.simpleCutBasedElectronID.robust80cIsoEleIDCutsV20.clone())
)
