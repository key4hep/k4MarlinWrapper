#include "converters/LCIO2EDM4hep.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <vector>
#include <string>
#include <ctime>
#include <chrono>

#include <LCEventWrapper.h>
#include <EVENT/LCEvent.h>

#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/ReconstructedParticleImpl.h"
#include "IMPL/TrackImpl.h"
#include "IMPL/TrackStateImpl.h"
#include "IMPL/ParticleIDImpl.h"

DECLARE_COMPONENT(LCIO2EDM4hep)

LCIO2EDM4hep::LCIO2EDM4hep(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  // declareProperty("Particle", mcps_handle, "");
}

StatusCode LCIO2EDM4hep::initialize() {
  info() << "start LCIO2EDM4hep::initialize()\n";

  return GaudiAlgorithm::initialize();
}

StatusCode LCIO2EDM4hep::execute() {
  info() << "LCIO2EDM4hep::execute()\n";

  // TODO for TrackCollection
  // Tracks handle
  DataHandle<edm4hep::TrackCollection> tracks_handle {
    "EFlowTrack", Gaudi::DataHandle::Reader, this};
  const auto tracks_coll = tracks_handle.get();

  std::cout << "TrackCollection size: " << tracks_coll->size() << "\n\n";

  // ReconstructedParticles handle
  DataHandle<edm4hep::ReconstructedParticleCollection> recos_handle {
    "ReconstructedParticles", Gaudi::DataHandle::Reader, this};
  const auto recos_coll = recos_handle.get();

  std::cout << "ReconstructedParticles size: " << recos_coll->size() << "\n\n";

  std::vector<lcio::TrackImpl*> lcio_tracks_vec;

  // Loop over EDM4hep tracks converting them to lcio tracks
  // including the track states
  for (const auto edm_tr = tracks_coll->begin(); edm_tr != tracks_coll->end(); ++edm_tr) {

    auto* lcio_tr = new lcio::TrackImpl;

    lcio_tr->setTypeBit( edm_tr->getType() );
    lcio_tr->setChi2( edm_tr->getChi2() );
    lcio_tr->setNdf( edm_tr->getNdf() );
    lcio_tr->setdEdx( edm_tr->getDEdx() );
    lcio_tr->setdEdxError( edm_tr->getDEdxError() );
    lcio_tr->setRadiusOfInnermostHit( edm_tr->getRadiusOfInnermostHit() );

    // Loop over the track states in the track
    podio::RelationRange<edm4hep::TrackState> edm_track_states = edm_tr->getTrackStates();
    for (auto& tr_state : edm_track_states) {

      std::array<float, 15> cov = {}; // zero-init
      cov[0]  = tr_state.covMatrix[0];
      cov[2]  = tr_state.covMatrix[5];
      cov[5]  = tr_state.covMatrix[9];
      cov[9]  = tr_state.covMatrix[12];
      cov[14] = tr_state.covMatrix[14];
      std::array<float, 15> refP = {
        tr_state.referencePoint.x, tr_state.referencePoint.y, tr_state.referencePoint.z};

      auto* lcio_tr_state = new lcio::TrackStateImpl(
        tr_state.location,
        tr_state.D0,
        tr_state.phi ,
        tr_state.omega,
        tr_state.Z0,
        tr_state.tanLambda,
        cov.data(),
        refP.data()
      );

      lcio_tr->addTrackState( lcio_tr_state ) ;

      // Default TrackState?
      // lcio_tr_state->setLocation( tr_state.location );
      // lcio_tr->setD0( tr_state.D0 );
      // lcio_tr->setPhi( tr_state.phi );
      // lcio_tr->setOmega( tr_state.omega );
      // lcio_tr->setZ0( tr_state.Z0 );
      // lcio_tr->setTanLambda( tr_state.tanLambda );
      // lcio_tr->setCovMatrix( cov.data() );
      // lcio_tr->setReferencePoint( refP.data() );
    }

    lcio_tracks_vec.push_back(lcio_tr);

  }


  // LCIO event to populate
  const std::string detector_name {"ToyTracker"};
  auto lcio_event = new lcio::LCEventImpl;
  // lcio_event->setEventNumber(0);
  lcio_event->setDetectorName(detector_name);
  lcio_event->setRunNumber(1);
  const auto p1 = (std::chrono::system_clock::now().time_since_epoch()).count();
  lcio_event->setTimeStamp(p1);

  // // Register LCIO event in TES
  // auto pO = std::make_unique<LCEventWrapper>(lcio_event);
  // const StatusCode sc = eventSvc()->registerObject("/Event/LCEvent", pO.release());
  // if (sc.isFailure()) {
  //   error() << "Failed to store the LCEvent" << endmsg;
  //   return sc;
  // }

  // // Populate LCIO event with EDM4hep info
  // auto* recops = new lcio::LCCollectionVec(lcio::LCIO::RECONSTRUCTEDPARTICLE);
  // auto* tracks = new lcio::LCCollectionVec(lcio::LCIO::TRACK);


  // TODO
  // Loop over reconstructed particles
  for (size_t i=0; i < recos_coll->size(); ++i) {
    const auto p = recos_coll->at(i);

    auto* lcio_recp = new lcio::ReconstructedParticleImpl;

    lcio_recp->setType(p.getType());
    float m[3] = {p.getMomentum()[0], p.getMomentum()[1], p.getMomentum()[2]};
    lcio_recp->setMomentum(m);
    lcio_recp->setEnergy(p.getEnergy());

    lcio_recp->setCovMatrix(p.getCovMatrix().data());

    lcio_recp->setMass(p.getMass());
    lcio_recp->setCharge(p.getCharge());
    float rp[3] = {p.getReferencePoint()[0], p.getReferencePoint()[1], p.getReferencePoint()[2]};
    lcio_recp->setReferencePoint(rp);

    // ::edm4hep::ConstParticleID getParticleIDUsed()
    // lcio_recp->setParticleIDUsed(EVENT::ParticleID*  pid);

    lcio_recp->setGoodnessOfPID(p.getGoodnessOfPID());

    // ::edm4hep::ConstVertex getStartVertex()
    // lcio_recp->setStartVertex(EVENT::Vertex * sv);

    // Add lcio converted tracks
    for (auto& lcio_tr : lcio_tracks_vec) {

      // addTrack( EVENT::Track* track)

    }

  }

  // StatusCode sc = eventSvc()->registerObject("/Event/LCEvent", event);
  // put(event, "/Event/SomeData");
  // lcio_handle.put(event);

  return StatusCode::SUCCESS;
}

StatusCode LCIO2EDM4hep::finalize() {
  info() << "LCIO2EDM4hep::finalize()\n";
  return StatusCode::SUCCESS;
}