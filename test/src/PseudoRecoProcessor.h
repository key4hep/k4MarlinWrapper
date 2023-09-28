#include "marlin/Processor.h"

#include <string>

class PseudoRecoProcessor : public marlin::Processor {
public:
  PseudoRecoProcessor();

  marlin::Processor* newProcessor() override { return new PseudoRecoProcessor; }

  /** process the event - In this case simply create one ReconstructedParticle
   * per input MCParticle (copying the kinematics over)
   */
  void processEvent(LCEvent* evt) override;

private:
  std::string m_mcCollName{};
  std::string m_recoCollName{};
};
