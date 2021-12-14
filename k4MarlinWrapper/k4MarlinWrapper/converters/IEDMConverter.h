#ifndef K4MARLINWRAPPER_IEDMCONVERTER_H
#define K4MARLINWRAPPER_IEDMCONVERTER_H

#include <GaudiKernel/IAlgTool.h>

// EDM4hep
#include <edm4hep/CaloHitContribution.h>
#include <edm4hep/CaloHitContributionCollection.h>
#include <edm4hep/CalorimeterHit.h>
#include <edm4hep/CalorimeterHitCollection.h>
#include <edm4hep/Cluster.h>
#include <edm4hep/ClusterCollection.h>
#include <edm4hep/EventHeaderCollection.h>
#include <edm4hep/MCParticle.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/MCRecoCaloAssociation.h>
#include <edm4hep/MCRecoCaloAssociationCollection.h>
#include <edm4hep/MCRecoCaloParticleAssociation.h>
#include <edm4hep/MCRecoCaloParticleAssociationCollection.h>
#include <edm4hep/MCRecoParticleAssociation.h>
#include <edm4hep/MCRecoParticleAssociationCollection.h>
#include <edm4hep/MCRecoTrackParticleAssociation.h>
#include <edm4hep/MCRecoTrackParticleAssociationCollection.h>
#include <edm4hep/MCRecoTrackerAssociation.h>
#include <edm4hep/MCRecoTrackerAssociationCollection.h>
#include <edm4hep/ParticleIDCollection.h>
#include <edm4hep/RawCalorimeterHit.h>
#include <edm4hep/RawCalorimeterHitCollection.h>
#include <edm4hep/RecoParticleVertexAssociation.h>
#include <edm4hep/RecoParticleVertexAssociationCollection.h>
#include <edm4hep/ReconstructedParticle.h>
#include <edm4hep/ReconstructedParticleCollection.h>
#include <edm4hep/ReconstructedParticleData.h>
#include <edm4hep/SimCalorimeterHit.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/TPCHit.h>
#include <edm4hep/TPCHitCollection.h>
#include <edm4hep/Track.h>
#include <edm4hep/TrackCollection.h>
#include <edm4hep/TrackerHit.h>
#include <edm4hep/TrackerHitCollection.h>
#include <edm4hep/TrackerHitPlane.h>
#include <edm4hep/TrackerHitPlaneCollection.h>
#include <edm4hep/Vertex.h>
#include <edm4hep/VertexCollection.h>

// LCIO
#include <IMPL/CalorimeterHitImpl.h>
#include <IMPL/ClusterImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCEventImpl.h>
#include <IMPL/LCRelationImpl.h>
#include <IMPL/MCParticleImpl.h>
#include <IMPL/ParticleIDImpl.h>
#include <IMPL/RawCalorimeterHitImpl.h>
#include <IMPL/ReconstructedParticleImpl.h>
#include <IMPL/SimCalorimeterHitImpl.h>
#include <IMPL/SimTrackerHitImpl.h>
#include <IMPL/TPCHitImpl.h>
#include <IMPL/TrackImpl.h>
#include <IMPL/TrackStateImpl.h>
#include <IMPL/TrackerHitImpl.h>
#include <IMPL/VertexImpl.h>
#include <LCIOSTLTypes.h>
#include <UTIL/CellIDEncoder.h>
#include <lcio.h>

class IEDMConverter : virtual public IAlgTool {
public:
  DeclareInterfaceID(IEDMConverter, 1, 0);

  virtual StatusCode convertCollections(lcio::LCEventImpl* lcio_event) = 0;
};

#endif
