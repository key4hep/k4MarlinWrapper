#include "converters/EDM4hep2Lcio.h"


DECLARE_COMPONENT(EDM4hep2LcioTool);


EDM4hep2LcioTool::EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IEDMConverter>(this);
}

EDM4hep2LcioTool::~EDM4hep2LcioTool() { ; }

StatusCode EDM4hep2LcioTool::initialize() {
  return GaudiTool::initialize();
}

StatusCode EDM4hep2LcioTool::finalize() {
  return GaudiTool::finalize();
}


// Add EDM4hep to LCIO converted tracks to vector
void EDM4hep2LcioTool::convertLCIOTracks(
  std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& tracks_vec,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event)
{
  DataHandle<edm4hep::TrackCollection> tracks_handle {
    e4h_coll_name, Gaudi::DataHandle::Reader, this};
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
    tracks_vec.emplace_back(
      std::make_pair(lcio_tr, edm_tr)
    );

    // Add to lcio tracks collection
    tracks->addElement(lcio_tr);
  }

  // Add all tracks to event
  lcio_event->addCollection(tracks, lcio_coll_name);
}


// Add EDM4hep to LCIO converted clusters to vector
void EDM4hep2LcioTool::convertLCIOClusters(
  std::vector<std::pair<lcio::ClusterImpl*, edm4hep::Cluster>>& cluster_vec,
  const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& particleIDs_vec,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event)
{
  DataHandle<edm4hep::ClusterCollection> cluster_handle {
    e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto cluster_coll = cluster_handle.get();

  auto* clusters = new lcio::LCCollectionVec(lcio::LCIO::CLUSTER);

  // Loop over EDM4hep clusters converting them to lcio clusters
  for (auto i = 0; i < cluster_coll->size(); ++i) {

    const edm4hep::Cluster edm_cluster = (*cluster_coll)[i];

    auto* lcio_cluster = new lcio::ClusterImpl();

    std::bitset<32> type_bits = edm_cluster.getType();
    for (int j=0; j<32; j++) {
      lcio_cluster->setTypeBit(j, (type_bits[j] == 0) ? false : true );
    }
    lcio_cluster->setEnergy(edm_cluster.getEnergy());
    lcio_cluster->setEnergyError(edm_cluster.getEnergyError());

    std::array<float, 3> edm_cluster_pos = {
      edm_cluster.getPosition().x, edm_cluster.getPosition().y, edm_cluster.getPosition().z};
    lcio_cluster->setPosition(edm_cluster_pos.data());

    lcio_cluster->setPositionError(edm_cluster.getPositionError().data());
    lcio_cluster->setITheta(edm_cluster.getITheta());
    lcio_cluster->setIPhi(edm_cluster.getPhi());
    // // lcio_cluster->setDirectionError(const EVENT::FloatVec &errdir);
    std::array<float, 3> edm_cluster_dir_err= {
      edm_cluster.getPosition().x, edm_cluster.getPosition().y, edm_cluster.getPosition().z};
    lcio_cluster->setDirectionError(edm_cluster_dir_err.data());

    EVENT::FloatVec shape_vec {};
    for (auto& param : edm_cluster.getShapeParameters()) {
      shape_vec.push_back(param);
    }
    lcio_cluster->setShape(shape_vec);

    // Link associated ParticleID
    for (auto& edm_particleID : edm_cluster.getParticleIDs()) {
      if (edm_particleID.isAvailable()) {
        for (auto& particleID : particleIDs_vec) {
          if (particleID.second == edm_particleID) {
            lcio_cluster->addParticleID(particleID.first);
          }
        }
      }
    }

    // TODO
    // lcio_cluster->addHit(EVENT::CalorimeterHit* hit  , float contribution) ;

    // Save intermediate cluster ref
    cluster_vec.emplace_back(
      std::make_pair(lcio_cluster, edm_cluster)
    );

    // Add to lcio tracks collection
    clusters->addElement(lcio_cluster);

  }

  // Link associated Clusters after converting all clusters
  for (auto i = 0; i < cluster_coll->size(); ++i) {
    const edm4hep::Cluster edm_cluster = (*cluster_coll)[i];

    for (auto& edm_linked_cluster : edm_cluster.getClusters()) {
      if (edm_linked_cluster.isAvailable()) {
        for (auto& cluster : cluster_vec) {
          if (cluster.second == edm_linked_cluster) {
            cluster.first->addCluster(cluster.first);
          }
        }
      }
    }
  }

  // Add all tracks to event
  lcio_event->addCollection(clusters, lcio_coll_name);

}


// Add EDM4hep to LCIO converted vertex to vector
void EDM4hep2LcioTool::convertLCIOVertices(
  std::vector<std::pair<lcio::VertexImpl*, edm4hep::Vertex>>& vertex_vec,
  const std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& recoparticles_vec,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event)
{
  DataHandle<edm4hep::VertexCollection> vertex_handle {
    e4h_coll_name, Gaudi::DataHandle::Reader, this};
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
    lcio_vertex->setPosition( edm_vertex.getPosition()[0], edm_vertex.getPosition()[1], edm_vertex.getPosition()[2] );
    lcio_vertex->setCovMatrix( edm_vertex.getCovMatrix().data() );

    // Associated particle to the vertex
    edm4hep::ConstReconstructedParticle vertex_rp = edm_vertex.getAssociatedParticle();
    if (vertex_rp.isAvailable()) {
      for (auto& rp : recoparticles_vec) {
        if (rp.second == vertex_rp) {
          lcio_vertex->setAssociatedParticle(rp.first);
          break;
        }
      }
    }

    for (int i=0; i < edm_vertex.parameters_size(); ++i) {
      lcio_vertex->addParameter(edm_vertex.getParameters(i));
    }

    // Save intermediate vertex ref
    vertex_vec.emplace_back(
      std::make_pair(lcio_vertex, edm_vertex)
    );

    // Add to lcio tracks collection
    vertices->addElement(lcio_vertex);
  }

  // Add all tracks to event
  lcio_event->addCollection(vertices, lcio_coll_name);
}




void EDM4hep2LcioTool::convertLCIOParticleIDs(
  std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& particleIDs_vec,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event)
{
  // ParticleIDs handle
  DataHandle<edm4hep::ParticleIDCollection> pIDs_handle {
    e4h_coll_name, Gaudi::DataHandle::Reader, this};
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

    particleIDs_vec.emplace_back(
      std::make_pair(lcio_pID, edm_pid)
    );

    // Add to lcio particleIDs collection
    particleIDs->addElement(lcio_pID);
  }

  // Add all particles to event
  lcio_event->addCollection(particleIDs, lcio_coll_name);
}


void EDM4hep2LcioTool::convertLCIOReconstructedParticles(
  std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& recoparticles_vec,
  const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& particleIDs_vec,
  const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& tracks_vec,
  const std::vector<std::pair<lcio::VertexImpl*, edm4hep::Vertex>>& vertex_vec,
  const std::vector<std::pair<lcio::ClusterImpl*, edm4hep::Cluster>>& clusters_vec,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event)
{
  // ReconstructedParticles handle
  DataHandle<edm4hep::ReconstructedParticleCollection> recos_handle {
    e4h_coll_name, Gaudi::DataHandle::Reader, this};
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

      // Link associated ParticleID
      edm4hep::ConstParticleID pIDUsed = edm_rp.getParticleIDUsed();
      if (pIDUsed.isAvailable()) {
        for (auto& particleID : particleIDs_vec) {
          if (particleID.second == pIDUsed) {
            lcio_recp->setParticleIDUsed(particleID.first);
            break;
          }
        }
      }

      // Link associated Vertex
      edm4hep::ConstVertex vertex = edm_rp.getStartVertex();
      if (vertex.isAvailable()) {
        for (auto& lcio_vertex : vertex_vec) {
          if (lcio_vertex.second == vertex) {
            lcio_recp->setStartVertex(lcio_vertex.first);
            break;
          }
        }
      }

      // Link associated Tracks
      for (int j=0; j < edm_rp.tracks_size(); ++j) {
        edm4hep::ConstTrack edm_rp_tr = edm_rp.getTracks(j);
        for (auto& lcio_track : tracks_vec) {
          if (lcio_track.second == edm_rp_tr) {
            lcio_recp->addTrack(lcio_track.first);
            break;
          }
        }
      }

      // Link associated Clusters
      for (auto& edm_cluster : edm_rp.getClusters()) {
        for (auto& cluster_pair : clusters_vec) {
          if (cluster_pair.second == edm_cluster) {
            lcio_recp->addCluster(cluster_pair.first);
            break;
          }
        }
      }
    }

    // Add converted LCIO RecoParticle ptr, and original EDM4hep RecoParticle
    recoparticles_vec.push_back(
      std::make_pair(lcio_recp, edm_rp)
    );

    // Add to reconstructed particles collection
    recops->addElement(lcio_recp);
  }

  // Link associated recopartilces after converting all recoparticles
  for (auto i = 0; i < recos_coll->size(); ++i) {
    const edm4hep::ReconstructedParticle edm_rp = (*recos_coll)[i];

    for (auto& edm_linked_rp : edm_rp.getParticles()) {
      if (edm_linked_rp.isAvailable()) {
        for (auto& rp : recoparticles_vec) {
          if (rp.second == edm_linked_rp) {
            rp.first->addParticle(rp.first);
          }
        }
      }
    }
  }

  // Add all reconstructed particles to event
  lcio_event->addCollection(recops, lcio_coll_name);
}


void EDM4hep2LcioTool::FillMissingCollections(
  CollectionsPairVectors& collection_pairs)
{

  // Fill missing ReconstructedParticle collections
  for (auto& rp_pair : collection_pairs.recoparticles) {

    // Link ParticleID
    if (rp_pair.first->getParticleIDUsed() == nullptr) {
      if (rp_pair.second.getParticleIDUsed().isAvailable()) {
        for (auto& particleID : collection_pairs.particleIDs) {
          if (particleID.second == rp_pair.second.getParticleIDUsed()) {
            rp_pair.first->setParticleIDUsed(particleID.first);
          }
        }
      }
    }

    // Link Vertex
    if (rp_pair.first->getStartVertex() == nullptr) {
      if (rp_pair.second.getStartVertex().isAvailable()) {
        for (auto& vertex : collection_pairs.vertices) {
          if (vertex.second == rp_pair.second.getStartVertex()) {
            rp_pair.first->setStartVertex(vertex.first);
          }
        }
      }
    }

    // Link Tracks
    if (rp_pair.first->getTracks().size() != rp_pair.second.tracks_size()) {
      assert(rp_pair.first->getTracks().size() == 0);
      for (int i=0; i < rp_pair.second.tracks_size(); ++i) {
        edm4hep::ConstTrack edm_rp_tr = rp_pair.second.getTracks(i);
        for (auto& lcio_track : collection_pairs.tracks) {
          if (lcio_track.second == edm_rp_tr) {
            rp_pair.first->addTrack(lcio_track.first);
            break;
          }
        }
      }
    }

    // Link Clusters
    if (rp_pair.first->getClusters().size() != rp_pair.second.clusters_size()) {
      assert(rp_pair.first->getClusters().size() == 0);
      for (auto& edm_rp_cluster : rp_pair.second.getClusters()) { // linked
        for (auto& lcio_cluster : collection_pairs.clusters) { // converted pairs
          if (lcio_cluster.second == edm_rp_cluster) {
            rp_pair.first->addCluster(lcio_cluster.first);
            break;
          }
        }
      }
    }

  } // reconstructed particles

  // Fill missing Vertices collections
  for (auto& vertex_pair : collection_pairs.vertices) {

    // Link Reconstructed Particles
    if (vertex_pair.first->getAssociatedParticle() == nullptr) {
      if (vertex_pair.second.getAssociatedParticle().isAvailable()) {
        for (auto& rp_pair : collection_pairs.recoparticles) {
          if (rp_pair.second == vertex_pair.second.getAssociatedParticle()) {
            vertex_pair.first->setAssociatedParticle(rp_pair.first);
          }
        }
      }
    }

  } // vertices

  // Fill missing Cluster collections
  for (auto& cluster_pair : collection_pairs.clusters) {

    // Link ParticleIDs
    if (cluster_pair.first->getParticleIDs().size() != cluster_pair.second.particleIDs_size()) {
      assert(cluster_pair.first->getParticleIDs().size() == 0);
      for (int i=0; i < cluster_pair.second.particleIDs_size(); ++i) {
        edm4hep::ConstParticleID edm_cluster_pid = cluster_pair.second.getParticleIDs(i);
        for (auto& lcio_pid : collection_pairs.particleIDs) {
          if (lcio_pid.second == edm_cluster_pid) {
            cluster_pair.first->addParticleID(lcio_pid.first);
            break;
          }
        }
      }
    }

  } // clusters

}


void EDM4hep2LcioTool::convertAdd(
  const std::string& type,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event,
  CollectionsPairVectors& collection_pairs)
{

  // Types are edm4hep::<Name>Collection
  if (type == "Track") {
    convertLCIOTracks(
      collection_pairs.tracks,
      e4h_coll_name,
      lcio_coll_name,
      lcio_event);
  } else
  if (type == "Cluster") {
    convertLCIOClusters(
      collection_pairs.clusters,
      collection_pairs.particleIDs,
      e4h_coll_name,
      lcio_coll_name,
      lcio_event);
  } else
  if (type == "ParticleID") {
    convertLCIOParticleIDs(
      collection_pairs.particleIDs,
      e4h_coll_name,
      lcio_coll_name,
      lcio_event);
  } else
  if (type == "Vertex") {
    convertLCIOVertices(
      collection_pairs.vertices,
      collection_pairs.recoparticles,
      e4h_coll_name,
      lcio_coll_name,
      lcio_event);
  } else
  if (type == "ReconstructedParticle") {
    convertLCIOReconstructedParticles(
      collection_pairs.recoparticles,
      collection_pairs.particleIDs,
      collection_pairs.tracks,
      collection_pairs.vertices,
      collection_pairs.clusters,
      e4h_coll_name,
      lcio_coll_name,
      lcio_event);
  } else {
    error() << "Error trying to convert requested " << type << " with name " << e4h_coll_name << endmsg;
    error() << "List of supported types: Track, Cluster, Vertex, ParticleID, ReconstructedParticle." << endmsg;
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

  CollectionsPairVectors collection_pairs {};

  for (int i = 0; i < m_edm2lcio_params.size(); i=i+3) {
    if (! collectionExist(m_edm2lcio_params[i+2], lcio_event)) {
      convertAdd(
        m_edm2lcio_params[i],
        m_edm2lcio_params[i+1],
        m_edm2lcio_params[i+2],
        lcio_event,
        collection_pairs);
    } else {
      debug() << " Collection " << m_edm2lcio_params[i+2] << " already in place, skipping conversion. " << endmsg;
    }
  }

  FillMissingCollections(
    collection_pairs);

  return StatusCode::SUCCESS;
}
