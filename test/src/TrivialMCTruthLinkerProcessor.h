#include "marlin/Processor.h"

#include <string>

class TrivialMCTruthLinkerProcessor : public marlin::Processor {
public:
  TrivialMCTruthLinkerProcessor();

  marlin::Processor* newProcessor() override { return new TrivialMCTruthLinkerProcessor; }

  /** process the event - In this case simply link MCparticle[i] with
   * ReconstructedParticle[i]
   */
  void processEvent(LCEvent* evt) override;

private:
  std::string m_mcCollName{};
  std::string m_recoCollName{};
  std::string m_relCollName{};
};
