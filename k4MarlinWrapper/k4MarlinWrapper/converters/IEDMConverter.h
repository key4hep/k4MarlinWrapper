#ifndef K4MARLINWRAPPER_IEDMCONVERTER_H
#define K4MARLINWRAPPER_IEDMCONVERTER_H

#include <GaudiKernel/IAlgTool.h>

// EDM4hep
#include <edm4hep/EventHeaderCollection.h>
#include <edm4hep/ParticleIDCollection.h>
#include <edm4hep/ReconstructedParticle.h>
#include <edm4hep/ReconstructedParticleCollection.h>
#include <edm4hep/ReconstructedParticleData.h>
#include <edm4hep/Track.h>
#include <edm4hep/TrackCollection.h>
#include <edm4hep/TrackerHit.h>
#include <edm4hep/TrackerHitCollection.h>
#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/CalorimeterHit.h>
#include <edm4hep/CalorimeterHitCollection.h>
#include <edm4hep/RawCalorimeterHit.h>
#include <edm4hep/RawCalorimeterHitCollection.h>
#include <edm4hep/SimCalorimeterHit.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/CaloHitContribution.h>
#include <edm4hep/CaloHitContributionCollection.h>
#include <edm4hep/TPCHit.h>
#include <edm4hep/TPCHitCollection.h>
#include <edm4hep/Cluster.h>
#include <edm4hep/ClusterCollection.h>
#include <edm4hep/Vertex.h>
#include <edm4hep/VertexCollection.h>
#include <edm4hep/MCParticle.h>
#include <edm4hep/MCParticleCollection.h>

// LCIO
#include <lcio.h>
#include <IMPL/LCEventImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/ReconstructedParticleImpl.h>
#include <IMPL/TrackImpl.h>
#include <IMPL/TrackStateImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <IMPL/RawCalorimeterHitImpl.h>
#include <IMPL/SimCalorimeterHitImpl.h>
#include <IMPL/TPCHitImpl.h>
#include <IMPL/TrackerHitImpl.h>
#include <IMPL/SimTrackerHitImpl.h>
#include <IMPL/ClusterImpl.h>
#include <IMPL/VertexImpl.h>
#include <IMPL/ParticleIDImpl.h>
#include <IMPL/MCParticleImpl.h>
#include <LCIOSTLTypes.h>


class IEDMConverter : virtual public IAlgTool {
public:

  DeclareInterfaceID( IEDMConverter, 1, 0 );

  virtual StatusCode convertCollections(
    lcio::LCEventImpl* lcio_event) = 0;
};

#endif