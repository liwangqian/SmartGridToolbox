#ifndef SINGLE_PHASE_TRANSFORMER_PARSER
#define SINGLE_PHASE_TRANSFORMER_PARSER

#include <SmartGridToolbox/Parser.h>

namespace SmartGridToolbox
{
   class SinglePhaseTransformerParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "single_phase_transformer";
         }

      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
         virtual void postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // SINGLE_PHASE_TRANSFORMER_PARSER