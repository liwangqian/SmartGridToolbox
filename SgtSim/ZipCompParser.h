#ifndef ZIP_COMP_PARSER_DOT_H
#define ZIP_COMP_PARSER_DOT_H

#include <SgtSim/Parser.h>

namespace SmartGridToolbox
{
   class SimZipParser : public ParserPlugin
   {
      public:
         static constexpr const char* pluginKey()
         {
            return "zip_to_ground";
         }
      public:
         virtual void parse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
         virtual void postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const override;
   };
}

#endif // ZIP_COMP_PARSER_DOT_H