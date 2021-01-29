#include "converters/LCIO2EDM4hep.h"


DECLARE_COMPONENT(LCIO2EDM4hep);

LCIO2EDM4hep::LCIO2EDM4hep(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {}

// Add EDM4hep to LCIO converted tracks to vector
void LCIO2EDM4hep::addLCIOConvertedTracks(
  std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& lcio_tracks_vec)
{
  DataHandle<edm4hep::TrackCollection> tracks_handle {
    "EFlowTrack", Gaudi::DataHandle::Reader, this};
  const auto tracks_coll = tracks_handle.get();

  // Loop over EDM4hep tracks converting them to lcio tracks
  for (int i = 0; i < tracks_coll->size(); ++i) {

    const edm4hep::Track edm_tr = (*tracks_coll)[i];

    auto* lcio_tr = new lcio::TrackImpl();

    lcio_tr->setTypeBit( edm_tr.getType() );
    lcio_tr->setChi2( edm_tr.getChi2() );
    lcio_tr->setNdf( edm_tr.getNdf() );
    lcio_tr->setdEdx( edm_tr.getDEdx() );
    lcio_tr->setdEdxError( edm_tr.getDEdxError() );
    lcio_tr->setRadiusOfInnermostHit( edm_tr.getRadiusOfInnermostHit() );

    // Loop over the track states in the track
    const podio::RelationRange<edm4hep::TrackState> edm_track_states = edm_tr.getTrackStates();
    for (const auto& tr_state : edm_track_states) {

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
    }

    lcio_tracks_vec.emplace_back(
      std::make_pair(lcio_tr, edm_tr)
    );
  }
}


void LCIO2EDM4hep::addLCIOParticleIDs(
  std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& lcio_particleIDs_vec)
{
  // ParticleIDs handle
  DataHandle<edm4hep::ParticleIDCollection> pIDs_handle {
    "ParticleIDs", Gaudi::DataHandle::Reader, this};
  const auto pIDs_coll = pIDs_handle.get();

  // for (const auto edm_pid = pIDs_coll->begin(); edm_pid != pIDs_coll->end(); ++edm_pid) {
  for (int i = 0; i < pIDs_coll->size(); ++i) {

    const edm4hep::ParticleID edm_pid = (*pIDs_coll)[i];

    auto* lcio_pID = new lcio::ParticleIDImpl;

    lcio_pID->setType(edm_pid.getType());
    lcio_pID->setPDG(edm_pid.getPDG());
    lcio_pID->setLikelihood(edm_pid.getLikelihood());
    lcio_pID->setAlgorithmType(edm_pid.getAlgorithmType());
    podio::RelationRange<float> pID_params = edm_pid.getParameters();
    for (auto& param : pID_params) {
      lcio_pID->addParameter(param);
    }

    lcio_particleIDs_vec.emplace_back(
      std::make_pair(lcio_pID, edm_pid)
    );
  }
}


void LCIO2EDM4hep::addLCIOReconstructedParticles(
  std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& lcio_rec_particles_vec,
  const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& lcio_particleIDs_vec,
  const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& lcio_tracks_vec )
{
  // ReconstructedParticles handle
  DataHandle<edm4hep::ReconstructedParticleCollection> recos_handle {
    "ReconstructedParticles", Gaudi::DataHandle::Reader, this};
  const auto recos_coll = recos_handle.get();

  for (int i = 0; i < recos_coll->size(); ++i) {

    const edm4hep::ReconstructedParticle edm_rp = (*recos_coll)[i];

    auto* lcio_recp = new lcio::ReconstructedParticleImpl;
    if (edm_rp.isAvailable()) {
      lcio_recp->setType(edm_rp.getType());
      float m[3] = {edm_rp.getMomentum()[0], edm_rp.getMomentum()[1], edm_rp.getMomentum()[2]};
      lcio_recp->setMomentum(m);
      lcio_recp->setEnergy(edm_rp.getEnergy());
      lcio_recp->setCovMatrix(edm_rp.getCovMatrix().data()); // TODO Check lower or upper
      lcio_recp->setMass(edm_rp.getMass());
      lcio_recp->setCharge(edm_rp.getCharge());
      float rp[3] = {edm_rp.getReferencePoint()[0], edm_rp.getReferencePoint()[1], edm_rp.getReferencePoint()[2]};
      lcio_recp->setReferencePoint(rp);
      lcio_recp->setGoodnessOfPID(edm_rp.getGoodnessOfPID());

      // ParticleID
      edm4hep::ConstParticleID pIDUsed = edm_rp.getParticleIDUsed();
      if (pIDUsed.isAvailable()) {
        for (auto& particleID : lcio_particleIDs_vec) {
          if (particleID.second == pIDUsed) {
            lcio_recp->setParticleIDUsed(particleID.first);
            break;
          }
        }
      }
    }

    // Associated Vertex
    edm4hep::ConstVertex vertex = edm_rp.getStartVertex();
    if (vertex.isAvailable()) {
      auto* lcio_vertex = new lcio::VertexImpl;
      lcio_vertex->setPrimary( vertex.getPrimary() );
      #warning "AlgoritymType conversion from int to string"
      lcio_vertex->setAlgorithmType( std::string{vertex.getAlgorithmType()} ); // TODO std::string(int)
      lcio_vertex->setChi2( vertex.getChi2() );
      lcio_vertex->setProbability( vertex.getProbability() );
      lcio_vertex->setPosition( vertex.getPosition()[0], vertex.getPosition()[1], vertex.getPosition()[2]  );
      lcio_vertex->setCovMatrix( vertex.getCovMatrix().data() );
      lcio_recp->setStartVertex(lcio_vertex);

      // Associated particle to the vertex
      edm4hep::ConstReconstructedParticle vertex_rp = vertex.getAssociatedParticle();
      if (vertex_rp.isAvailable()) {
        // TODO add rp
      }
    }

    // Tracks
    for (int j=0; j < edm_rp.tracks_size(); ++j) {
      edm4hep::ConstTrack edm_rp_tr = edm_rp.getTracks(j);
      for (auto& track : lcio_tracks_vec) {
        if (track.second == edm_rp_tr) {
          lcio_recp->addTrack(track.first);
        }
      }
    }

    // Add converted LCIO RecoParticle, and associated EDM4hep RecoParticle
    lcio_rec_particles_vec.emplace_back(
      std::make_pair(lcio_recp, edm_rp)
    );
  }
}



StatusCode LCIO2EDM4hep::initialize() {
  info() << "start LCIO2EDM4hep::initialize()\n";

  return GaudiAlgorithm::initialize();
}

StatusCode LCIO2EDM4hep::execute() {
  info() << "LCIO2EDM4hep::execute()\n";

  // Save pointer to converted element, and pointer to original element in a std::pair
  std::vector<std::pair<
    lcio::TrackImpl*, edm4hep::Track>> lcio_tracks_vec;
  std::vector<std::pair<
    lcio::ParticleIDImpl*, edm4hep::ParticleID>> lcio_particleIDs_vec;
  std::vector<std::pair<
    lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>> lcio_rec_particles_vec;

  addLCIOConvertedTracks(lcio_tracks_vec);
  addLCIOParticleIDs(lcio_particleIDs_vec);
  addLCIOReconstructedParticles(
    lcio_rec_particles_vec,
    lcio_particleIDs_vec,
    lcio_tracks_vec);

  // // LCIO event to populate
  // const std::string detector_name {"ToyTracker"};
  // auto lcio_event = new lcio::LCEventImpl;
  // // lcio_event->setEventNumber(0);
  // lcio_event->setDetectorName(detector_name);
  // lcio_event->setRunNumber(1);
  // const auto p1 = (std::chrono::system_clock::now().time_since_epoch()).count();
  // lcio_event->setTimeStamp(p1);

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

  // StatusCode sc = eventSvc()->registerObject("/Event/LCEvent", event);
  // put(event, "/Event/SomeData");
  // lcio_handle.put(event);

  return StatusCode::SUCCESS;
}

StatusCode LCIO2EDM4hep::finalize() {
  info() << "LCIO2EDM4hep::finalize()\n";

  // TODO add deletes
  return StatusCode::SUCCESS;
}