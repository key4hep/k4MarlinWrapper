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


// Convert EDM4hep Tracks to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
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
  for (const auto& edm_tr : (*tracks_coll)) {

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


// Convert EDM4hep Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertLCIOCalorimeterHits(
  std::vector<std::pair<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>>& calo_hits_vec,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event)
{
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle {
    e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto calohit_coll = calohit_handle.get();

  auto* calohits = new lcio::LCCollectionVec(lcio::LCIO::CALORIMETERHIT);

  for (const auto& edm_calohit : (*calohit_coll)) {

    auto* lcio_calohit = new lcio::CalorimeterHitImpl();

    #warning "Splitting unsigned long long into two ints"
    unsigned long long combined_value = edm_calohit.getCellID();
    int* combined_value_int_ptr = (int*) combined_value;
    lcio_calohit->setCellID0(combined_value_int_ptr[0]);
    lcio_calohit->setCellID1(combined_value_int_ptr[1]);
    lcio_calohit->setEnergy(edm_calohit.getEnergy());
    lcio_calohit->setEnergyError(edm_calohit.getEnergyError());
    lcio_calohit->setTime(edm_calohit.getTime());
    std::array<float, 3> positions {edm_calohit.getPosition()[0], edm_calohit.getPosition()[1], edm_calohit.getPosition()[2]};
    lcio_calohit->setPosition(positions.data());
    lcio_calohit->setType(edm_calohit.getType());

    // TODO
    // lcio_calohit->setRawHit(EVENT::LCObject* rawHit );

    // Save Calorimeter Hits LCIO and EDM4hep collections
    calo_hits_vec.emplace_back(
      std::make_pair(lcio_calohit, edm_calohit)
    );

    // Add to lcio tracks collection
    calohits->addElement(lcio_calohit);
  }

  // Add all Calorimeter Hits to event
  lcio_event->addCollection(calohits, lcio_coll_name);
}



// Convert EDM4hep Clusters to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertLCIOClusters(
  std::vector<std::pair<lcio::ClusterImpl*, edm4hep::Cluster>>& cluster_vec,
  const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& particleIDs_vec,
  const std::vector<std::pair<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>>& calohits_vec,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event)
{
  DataHandle<edm4hep::ClusterCollection> cluster_handle {
    e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto cluster_coll = cluster_handle.get();

  auto* clusters = new lcio::LCCollectionVec(lcio::LCIO::CLUSTER);

  // Loop over EDM4hep clusters converting them to lcio clusters
  for (const auto& edm_cluster : (*cluster_coll)) {

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
    std::array<float, 3> edm_cluster_dir_err= {
      edm_cluster.getPosition().x, edm_cluster.getPosition().y, edm_cluster.getPosition().z};
    lcio_cluster->setDirectionError(edm_cluster_dir_err.data());

    EVENT::FloatVec shape_vec {};
    for (auto& param : edm_cluster.getShapeParameters()) {
      shape_vec.push_back(param);
    }
    lcio_cluster->setShape(shape_vec);

    // Link multiple associated ParticleID if found in converted ones
    for (auto& edm_particleID : edm_cluster.getParticleIDs()) {
      if (edm_particleID.isAvailable()) {
        bool conv_found = false;
        for (auto& particleID : particleIDs_vec) {
          if (particleID.second == edm_particleID) {
            lcio_cluster->addParticleID(particleID.first);
            conv_found = true;
            break;
          }
        }
        // If particle avilable, but not found in converted vec, add nullptr
        if (not conv_found) lcio_cluster->addParticleID(nullptr);
      }
    }

    // Link multiple associated Calorimeter Hits, and Hit Contributions
    // There must be same number of Calo Hits and Hit Contributions
    if (edm_cluster.hits_size() == edm_cluster.hitContributions_size()) {
      for (int j=0; j < edm_cluster.hits_size(); ++j) { // use index to get same hit and contrib
        if (edm_cluster.getHits(j).isAvailable()) {
          bool conv_found = false;
          for (auto& hit : calohits_vec) {
            if (hit.second == edm_cluster.getHits(j)) {
              lcio_cluster->addHit(
                hit.first, edm_cluster.getHitContributions(j));
              conv_found = true;
              break;
            }
          }
          // If hit avilable, but not found in converted vec, add nullptr
          if (not conv_found) lcio_cluster->addHit(nullptr, 0);
        }
      }
    }

    // Add LCIO and EDM4hep pair collections to vec
    cluster_vec.emplace_back(
      std::make_pair(lcio_cluster, edm_cluster)
    );

    // Add to lcio tracks collection
    clusters->addElement(lcio_cluster);

  }

  // Link associated Clusters after converting all clusters
  for (const auto& edm_cluster : (*cluster_coll)) {
    for (auto& edm_linked_cluster : edm_cluster.getClusters()) {
      if (edm_linked_cluster.isAvailable()) {
        for (auto& cluster : cluster_vec) {
          if (cluster.second == edm_linked_cluster) {
            cluster.first->addCluster(cluster.first);
            break;
          }
        }
      }
    }
  }

  // Add clusters to event
  lcio_event->addCollection(clusters, lcio_coll_name);

}


// Convert EDM4hep Vertices to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
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
  for (const auto& edm_vertex : (*vertex_coll)) {

    auto* lcio_vertex = new lcio::VertexImpl();
    lcio_vertex->setPrimary( edm_vertex.getPrimary() );
    #warning "AlgoritymType conversion from int to string"
    lcio_vertex->setAlgorithmType( std::string{edm_vertex.getAlgorithmType()} ); // TODO std::string(int)
    lcio_vertex->setChi2( edm_vertex.getChi2() );
    lcio_vertex->setProbability( edm_vertex.getProbability() );
    lcio_vertex->setPosition( edm_vertex.getPosition()[0], edm_vertex.getPosition()[1], edm_vertex.getPosition()[2] );
    lcio_vertex->setCovMatrix( edm_vertex.getCovMatrix().data() );

    for (auto& param : edm_vertex.getParameters()) {
      lcio_vertex->addParameter(param);
    }

    // Link sinlge associated Particle if found in converted ones
    edm4hep::ConstReconstructedParticle vertex_rp = edm_vertex.getAssociatedParticle();
    if (vertex_rp.isAvailable()) {
      bool conv_found = false;
      for (auto& rp : recoparticles_vec) {
        if (rp.second == vertex_rp) {
          lcio_vertex->setAssociatedParticle(rp.first);
          conv_found = true;
          break;
        }
      }
      // If recoparticle avilable, but not found in converted vec, add nullptr
      if (not conv_found) lcio_vertex->setAssociatedParticle(nullptr);
    }

    // Add LCIO and EDM4hep pair collections to vec
    vertex_vec.emplace_back(
      std::make_pair(lcio_vertex, edm_vertex)
    );

    // Add to lcio tracks collection
    vertices->addElement(lcio_vertex);
  }

  // Add all tracks to event
  lcio_event->addCollection(vertices, lcio_coll_name);
}



// Convert EDM4hep ParticleEDs to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
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

  for (const auto& edm_pid : (*pIDs_coll)) {

    auto* lcio_pID = new lcio::ParticleIDImpl;

    lcio_pID->setType(edm_pid.getType());
    lcio_pID->setPDG(edm_pid.getPDG());
    lcio_pID->setLikelihood(edm_pid.getLikelihood());
    lcio_pID->setAlgorithmType(edm_pid.getAlgorithmType());
    podio::RelationRange<float> pID_params = edm_pid.getParameters();
    for (auto& param : pID_params) {
      lcio_pID->addParameter(param);
    }

    // Add LCIO and EDM4hep pair collections to vec
    particleIDs_vec.emplace_back(
      std::make_pair(lcio_pID, edm_pid)
    );

    // Add to lcio particleIDs collection
    particleIDs->addElement(lcio_pID);
  }

  // Add all particles to event
  lcio_event->addCollection(particleIDs, lcio_coll_name);
}

// Convert EDM4hep RecoParticles to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
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

  for (const auto& edm_rp : (*recos_coll)) {

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

      // Link sinlge associated Particle if found in converted ones
      edm4hep::ConstParticleID pIDUsed = edm_rp.getParticleIDUsed();
      if (pIDUsed.isAvailable()) {
        bool conv_found = false;
        for (auto& particleID : particleIDs_vec) {
          if (particleID.second == pIDUsed) {
            lcio_recp->setParticleIDUsed(particleID.first);
            conv_found = true;
            break;
          }
        }
        // If particleID, but not found in converted vec, add nullptr
        if (not conv_found) lcio_recp->setParticleIDUsed(nullptr);
      }

      // Link sinlge associated Vertex if found in converted ones
      edm4hep::ConstVertex vertex = edm_rp.getStartVertex();
      if (vertex.isAvailable()) {
        bool conv_found = false;
        for (auto& lcio_vertex : vertex_vec) {
          if (lcio_vertex.second == vertex) {
            lcio_recp->setStartVertex(lcio_vertex.first);
            conv_found = true;
            break;
          }
        }
        // If particleID available, but not found in converted vec, add nullptr
        if (not conv_found) lcio_recp->setStartVertex(nullptr);
      }

      // Link multiple associated Tracks if found in converted ones
      for (auto& edm_rp_tr : edm_rp.getTracks()) {
        if (edm_rp_tr.isAvailable()){
          bool conv_found = false;
          for (auto& lcio_track : tracks_vec) {
            if (lcio_track.second == edm_rp_tr) {
              lcio_recp->addTrack(lcio_track.first);
              conv_found = true;
              break;
            }
          }
          // If track available, but not found in converted vec, add nullptr
          if (not conv_found) lcio_recp->addTrack(nullptr);
        }
      }

      // Link multiple associated Clusters if found in converted ones
      for (auto& edm_cluster : edm_rp.getClusters()) {
        if (edm_cluster.isAvailable()) {
          bool conv_found = false;
          for (auto& cluster_pair : clusters_vec) {
            if (cluster_pair.second == edm_cluster) {
              lcio_recp->addCluster(cluster_pair.first);
              conv_found = true;
              break;
            }
          }
          // If cluster available, but not found in converted vec, add nullptr
          if (not conv_found) lcio_recp->addCluster(nullptr);
        }
      }
    }

    // Add LCIO and EDM4hep pair collections to vec
    recoparticles_vec.push_back(
      std::make_pair(lcio_recp, edm_rp)
    );

    // Add to reconstructed particles collection
    recops->addElement(lcio_recp);
  }

  // Link associated recopartilces after converting all recoparticles
  for (const auto& edm_rp : (*recos_coll)) {
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
      for (auto& edm_rp_tr : rp_pair.second.getTracks()) {
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
      for (auto& edm_rp_cluster : rp_pair.second.getClusters()) {
        for (auto& lcio_cluster : collection_pairs.clusters) {
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

    // Link associated Calorimeter Hits, and Hit Contributions
    if (cluster_pair.first->getCalorimeterHits().size() != cluster_pair.second.hits_size()) {
      assert(cluster_pair.first->getCalorimeterHits().size() == 0);
      for (int i=0; i < cluster_pair.second.hits_size(); ++i) {
        auto edm_cluster_hit = cluster_pair.second.getHits(i);
        auto edm_cluster_contribution = cluster_pair.second.getHitContributions(i);
        for (auto& lcio_hit : collection_pairs.calohits) {
          if (lcio_hit.second == edm_cluster_hit) {
            cluster_pair.first->addHit(
              lcio_hit.first, edm_cluster_contribution);
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
  if (type == "CalorimeterHit") {
    convertLCIOCalorimeterHits(
      collection_pairs.calohits,
      e4h_coll_name,
      lcio_coll_name,
      lcio_event);
  } else
  if (type == "Cluster") {
    convertLCIOClusters(
      collection_pairs.clusters,
      collection_pairs.particleIDs,
      collection_pairs.calohits,
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
    error() << "List of supported types: Track, Cluster, CalorimeterHit, Vertex, ParticleID, ReconstructedParticle." << endmsg;
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
