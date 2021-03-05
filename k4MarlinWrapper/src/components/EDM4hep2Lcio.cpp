#include "converters/EDM4hep2Lcio.h"


DECLARE_COMPONENT(EDM4hep2LcioTool);


EDM4hep2LcioTool::EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IEDM4hep2LcioTool>(this);
}

EDM4hep2LcioTool::~EDM4hep2LcioTool() { ; }

StatusCode EDM4hep2LcioTool::initialize() {
  return GaudiTool::initialize();
}

StatusCode EDM4hep2LcioTool::finalize() {
  return GaudiTool::finalize();
}


// Add EDM4hep to LCIO converted tracks to vector
void EDM4hep2LcioTool::addLCIOConvertedTracks(
  std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& m_lcio_tracks_vec,
  const std::string& name,
  const std::string& lcio_collection_name,
  lcio::LCEventImpl* lcio_event)
{
  DataHandle<edm4hep::TrackCollection> tracks_handle {
    name, Gaudi::DataHandle::Reader, this};
  const auto tracks_coll = tracks_handle.get();

  auto* tracks = new lcio::LCCollectionVec(lcio::LCIO::TRACK);

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
    // FIXME, correctly assign hitnumbers
    lcio_tr->subdetectorHitNumbers().resize(50);
    for(int i =0;i<50;++i) {
      lcio_tr->subdetectorHitNumbers()[i] = 0 ;
    }

    // Loop over the track states in the track
    const podio::RelationRange<edm4hep::TrackState> edm_track_states = edm_tr.getTrackStates();
    for (const auto& tr_state : edm_track_states) {

      std::array<float, 15> cov = tr_state.covMatrix;
      std::array<float, 3> refP = {
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

    // Save intermediate tracks ref
    m_lcio_tracks_vec.emplace_back(
      std::make_pair(lcio_tr, edm_tr)
    );

    // Add to lcio tracks collection
    tracks->addElement(lcio_tr);
  }

  // Add all tracks to event
  lcio_event->addCollection(tracks, lcio_collection_name);
}



// Add EDM4hep to LCIO converted vertex to vector
void EDM4hep2LcioTool::addLCIOVertices(
  std::vector<std::pair<lcio::VertexImpl*, edm4hep::Vertex>>& m_lcio_vertex_vec,
  const std::string& name,
  const std::string& lcio_collection_name,
  lcio::LCEventImpl* lcio_event)
{
  DataHandle<edm4hep::VertexCollection> vertex_handle {
    name, Gaudi::DataHandle::Reader, this};
  const auto vertex_coll = vertex_handle.get();

  auto* vertices = new lcio::LCCollectionVec(lcio::LCIO::VERTEX);

  // Loop over EDM4hep vertex converting them to lcio vertex
  for (int i = 0; i < vertex_coll->size(); ++i) {

    const edm4hep::Vertex edm_vertex = (*vertex_coll)[i];

    auto* lcio_vertex = new lcio::VertexImpl();
    lcio_vertex->setPrimary( edm_vertex.getPrimary() );
    #warning "AlgoritymType conversion from int to string"
    lcio_vertex->setAlgorithmType( std::string{edm_vertex.getAlgorithmType()} ); // TODO std::string(int)
    lcio_vertex->setChi2( edm_vertex.getChi2() );
    lcio_vertex->setProbability( edm_vertex.getProbability() );
    lcio_vertex->setPosition( edm_vertex.getPosition()[0], edm_vertex.getPosition()[1], edm_vertex.getPosition()[2]  );
    lcio_vertex->setCovMatrix( edm_vertex.getCovMatrix().data() );

    // Associated particle to the vertex
    edm4hep::ConstReconstructedParticle vertex_rp = edm_vertex.getAssociatedParticle();
    if (vertex_rp.isAvailable()) {
      // TODO add rp
    }

    for (int i=0; i < edm_vertex.parameters_size(); ++i) {
      lcio_vertex->addParameter(edm_vertex.getParameters(i));
    }

    // Save intermediate vertex ref
    m_lcio_vertex_vec.emplace_back(
      std::make_pair(lcio_vertex, edm_vertex)
    );

    // Add to lcio tracks collection
    vertices->addElement(lcio_vertex);
  }

  // Add all tracks to event
  lcio_event->addCollection(vertices, lcio_collection_name);
}




void EDM4hep2LcioTool::addLCIOParticleIDs(
  std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& m_lcio_particleIDs_vec,
  const std::string& name,
  const std::string& lcio_collection_name,
  lcio::LCEventImpl* lcio_event)
{
  // ParticleIDs handle
  DataHandle<edm4hep::ParticleIDCollection> pIDs_handle {
    name, Gaudi::DataHandle::Reader, this};
  const auto pIDs_coll = pIDs_handle.get();

  auto* particleIDs = new lcio::LCCollectionVec(lcio::LCIO::PARTICLEID);

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

    m_lcio_particleIDs_vec.emplace_back(
      std::make_pair(lcio_pID, edm_pid)
    );

    // Add to lcio particleIDs collection
    particleIDs->addElement(lcio_pID);
  }

  // Add all particles to event
  lcio_event->addCollection(particleIDs, lcio_collection_name);
}


void EDM4hep2LcioTool::addLCIOReconstructedParticles(
  std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& m_lcio_rec_particles_vec,
  const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& m_lcio_particleIDs_vec,
  const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& m_lcio_tracks_vec,
  const std::string& name,
  const std::string& lcio_collection_name,
  lcio::LCEventImpl* lcio_event)
{
  // ReconstructedParticles handle
  DataHandle<edm4hep::ReconstructedParticleCollection> recos_handle {
    name, Gaudi::DataHandle::Reader, this};
  const auto recos_coll = recos_handle.get();

  auto* recops = new lcio::LCCollectionVec(lcio::LCIO::RECONSTRUCTEDPARTICLE);

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
        for (auto& particleID : m_lcio_particleIDs_vec) {
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
      for (auto& track : m_lcio_tracks_vec) {
        if (track.second == edm_rp_tr) {
          lcio_recp->addTrack(track.first);
        }
      }
    }

    // Add converted LCIO RecoParticle, and associated EDM4hep RecoParticle
    m_lcio_rec_particles_vec.emplace_back(
      std::make_pair(lcio_recp, edm_rp)
    );

    // Add to reconstructed particles collection
    recops->addElement(lcio_recp);
  }

  // Add all reconstructed particles to event
  lcio_event->addCollection(recops, lcio_collection_name);
}


void EDM4hep2LcioTool::convertAdd(
  const std::string& type,
  const std::string& name,
  const std::string& lcio_collection_name,
  lcio::LCEventImpl* lcio_event)
{

  // Types are edm4hep::<Name>Collection
  if (type == "Track") {
    addLCIOConvertedTracks(m_lcio_tracks_vec, name, lcio_collection_name, lcio_event);
  } else
  if (type == "ParticleID") {
    addLCIOParticleIDs(m_lcio_particleIDs_vec, name, lcio_collection_name, lcio_event);
  } else
  if (type == "Vertex") {
    addLCIOVertices(m_lcio_vertex_vec, name, lcio_collection_name, lcio_event);
  } else
  if (type == "ReconstructedParticle") {
    addLCIOReconstructedParticles(
      m_lcio_rec_particles_vec,
      m_lcio_particleIDs_vec,
      m_lcio_tracks_vec,
      name,
      lcio_collection_name,
      lcio_event);
  } else {
    error() << "Error trying to convert requested " << type << " with name " << name << endmsg;
    error() << "List of supported types: Track, ParticleID, ReconstructedParticle." << endmsg;
  }
}


bool EDM4hep2LcioTool::collectionExist(
  const std::string& collection_name,
  lcio::LCEventImpl* lcio_event)
{
  auto* coll = lcio_event->getCollectionNames();
  for (int i = 0; i < coll->size(); ++i) {
    if (collection_name == coll->at(i)) {
      return true;
    }
  }
  return false;
}


StatusCode EDM4hep2LcioTool::convertCollections(
  lcio::LCEventImpl* lcio_event)
{
  if (m_edm2lcio_params.size() % 3 != 0) {
    error() << " Error processing conversion parameters. 3 arguments per collection expected. " << endmsg;
    return StatusCode::FAILURE;
  }

  for (int i = 0; i < m_edm2lcio_params.size(); i=i+3) {
    if (! collectionExist(m_edm2lcio_params[i+2], lcio_event)) {
      convertAdd(
        m_edm2lcio_params[i],
        m_edm2lcio_params[i+1],
        m_edm2lcio_params[i+2],
        lcio_event);
    } else {
      debug() << " Collection " << m_edm2lcio_params[i+2] << " already in place, skipping conversion. " << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
