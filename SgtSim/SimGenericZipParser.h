#ifndef SIM_GENERIC_BRANCH_PARSER_DOT_H
#define SIM_GENERIC_BRANCH_PARSER_DOT_H

#include <SgtSim/SimNetworkComponent.h>

#include <SgtCore/Parser.h>

namespace SmartGridToolbox
{
   class Simulation;

   /// @brief ParserPlugin that parses generic zip objects.
   class SimGenericZipParser : public ParserPlugin<Simulation>
   {
      public:
         virtual const char* key() override
         {
            return "sim_generic_zip";
         }

      public:
         virtual void parse(const YAML::Node& nd, Simulation& into) const override;
   };
}

#endif // SIM_GENERIC_BRANCH_PARSER_DOT_H
