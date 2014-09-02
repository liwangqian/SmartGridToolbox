#include "GenParser.h"

#include "Bus.h"
#include "GenericGen.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void GenParser::parse(const YAML::Node& nd, Network& into) const
   {
      SGT_DEBUG(debug() << "GenericGen : parse." << std::endl);

      auto gen = parseGenericGen(nd);

      assertFieldPresent(nd, "bus_id");
      std::string busId = nd["bus_id"].as<std::string>();
      into.addGen(std::move(gen), busId);
   }
   
   std::unique_ptr<GenericGen> GenParser::parseGenericGen(const YAML::Node& nd) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");

      std::string id = nd["id"].as<std::string>();
      Phases phases = nd["phases"].as<Phases>();

      std::unique_ptr<GenericGen> gen(new GenericGen(id, phases));

      if (const YAML::Node& subNd = nd["S"])
      {
         gen->setS(subNd.as<ublas::vector<Complex>>());
      }
      
      if (const YAML::Node& subNd = nd["P_min"])
      {
         gen->setPMin(subNd.as<double>());
      }

      if (const YAML::Node& subNd = nd["P_max"])
      {
         gen->setPMax(subNd.as<double>());
      }
      
      if (const YAML::Node& subNd = nd["Q_min"])
      {
         gen->setQMin(subNd.as<double>());
      }

      if (const YAML::Node& subNd = nd["Q_max"])
      {
         gen->setQMax(subNd.as<double>());
      }
      
      if (const YAML::Node& subNd = nd["C0"])
      {
         gen->setC0(subNd.as<double>());
      }
      
      if (const YAML::Node& subNd = nd["C1"])
      {
         gen->setC1(subNd.as<double>());
      }
      
      if (const YAML::Node& subNd = nd["C2"])
      {
         gen->setC2(subNd.as<double>());
      }

      return gen;
   }
}
