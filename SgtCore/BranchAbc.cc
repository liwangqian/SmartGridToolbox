#include "BranchAbc.h"
#include "Bus.h"

#include <ostream>

namespace SmartGridToolbox
{
         
   BranchAbc::BranchAbc(const std::string& id, Phases phases0, Phases phases1) :
      Component(id),
      phases0_(phases0),
      phases1_(phases1),
      status_(true)
   {
      // Empty.
   }
   void BranchAbc::print(std::ostream& os) const
   {
      Component::print(os);
      IndentingOStreamBuf _(os);
      os << "phases0: " << phases0() << std::endl;
      os << "phases1: " << phases1() << std::endl;
      os << "bus0: " << bus0().id() << std::endl;
      os << "bus1: " << bus1().id() << std::endl;
   }
}