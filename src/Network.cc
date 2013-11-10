#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Branch.h>
#include <SmartGridToolbox/Model.h>
#include "PowerFlowNr.h"
#include <iostream>

namespace SmartGridToolbox
{
   void NetworkParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Network : parse." << std::endl);

      assertFieldPresent(nd, "name");

      string name = state.expandName(nd["name"].as<std::string>());

      auto ndFreq = nd["freq_Hz"];
      double freq = ndFreq ? ndFreq.as<double>() : 50.0;

      Network & comp = mod.newComponent<Network>(name, freq);
   }

   void Network::updateState(Time t0, Time t1)
   {
      SGT_DEBUG(debug() << "Network : update state." << std::endl);
      applyBusSetpoints();
      solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
   }

   void Network::addBus(Bus & bus)
   {
      dependsOn(bus);
      busVec_.push_back(&bus);
      busMap_[bus.name()] = &bus;
      bus.didUpdate().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + name() + " needs update");
      needsUpdate().trigger();
   }
 
   void Network::addBranch(Branch & branch)
   {
      dependsOn(branch);
      branchVec_.push_back(&branch);
      branch.didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger Network " + name() + " needs update");
      needsUpdate().trigger();
   }

   void Network::solvePowerFlow()
   {
      SGT_DEBUG(debug() << "Network : solving power flow." << std::endl);
      SGT_DEBUG(debug() << *this);
      PowerFlowNr solver;
      solver.reset();
      for (const Bus * bus : busVec_)
      {
         solver.addBus(bus->name(), bus->type(), bus->phases(), bus->V(), bus->ys(), bus->Ic(), bus->STot());
      }
      for (const Branch * branch : branchVec_)
      {
         solver.addBranch(branch->bus0().name(), branch->bus1().name(), branch->phases0(),
                          branch->phases1(), branch->Y());
      }

      solver.validate();

      bool ok = solver.solve();

      if (ok)
      {
         for (const auto & busPair: solver.busses())
         {
            Bus * bus = findBus(busPair.second->id_);

            // Push the state back onto bus. We don't want to trigger any events.
            bus->setV(busPair.second->V_);
            bus->setSg(busPair.second->S_ - bus->Sc());
         }
      }
   }

   std::ostream & operator<<(std::ostream & os, const Network & nw)
   {
      os << "Network: " << nw.name() << std::endl;
      os << "\tBusses:" << std::endl;
      for (const Bus * bus : nw.busVec_)
      {
         ublas::vector<double> VMag(bus->V().size());
         for (int i = 0; i < bus->V().size(); ++i) VMag(i) = abs(bus->V()(i));

         os << "\t\tBus:" << std::endl;
         os << "\t\t\tName   : " << bus->name() << std::endl;
         os << "\t\t\tType   : " << bus->type() << std::endl;
         os << "\t\t\tPhases : " << bus->phases() << std::endl;
         os << "\t\t\tV      : " << bus->V() << std::endl;
         os << "\t\t\t|V|    : " << VMag << std::endl;
         os << "\t\t\tys     : " << bus->ys() << std::endl;
         os << "\t\t\tIc     : " << bus->Ic() << std::endl;
         os << "\t\t\tSc     : " << bus->Sc() << std::endl;
         os << "\t\t\tSg     : " << bus->Sg() << std::endl;
         os << "\t\t\tSTot   : " << bus->STot() << std::endl;
      }
      os << "\tBranches:" << std::endl;
      for (const Branch * branch : nw.branchVec_)
      {
         os << "\t\tBranch:" << std::endl; 
         os << "\t\t\tBus names  : " 
            << branch->bus0().name() << " " << branch->bus1().name() << std::endl;
         os << "\t\t\tBus phases : " << branch->phases0() << " " << branch->phases1() << std::endl;
         os << "\t\t\tY          :" << std::endl;
         for (int i = 0; i < branch->Y().size1(); ++i)
         {
            os << "\t\t\t\t" << std::setw(16) << row(branch->Y(), i) << std::endl;
         }
      }
      return os;
   }

   void Network::applyBusSetpoints()
   {
      for (Bus * bus : busVec_)
      {
         switch (bus->type())
         {
            case BusType::SL :
               {
                  ublas::vector<Complex> V(bus->phases().size());
                  for (int i = 0; i < bus->phases().size(); ++i)
                  {
                     V(i) = polar(bus->VMagSetpoint()(i), bus->VAngSetpoint()(i));
                  }
                  bus->setV(V);
               }
               break;
            case BusType::PQ :
               {
                  ublas::vector<Complex> Sg(bus->phases().size());
                  for (int i = 0; i < bus->phases().size(); ++i)
                  {
                     Sg(i) = {bus->PgSetpoint()(i), bus->QgSetpoint()(i)};
                  }
                  bus->setSg(Sg);
               }
               break;
            case BusType::PV :
               {
                  ublas::vector<Complex> Sg(bus->phases().size());
                  ublas::vector<Complex> V(bus->phases().size());
                  for (int i = 0; i < bus->phases().size(); ++i)
                  {
                     Sg(i) = {bus->PgSetpoint()(i), bus->Sg()(i).imag()};
                     V(i) = bus->VMagSetpoint()(i) * bus->V()(i) / abs(bus->V()(i));
                  }
                  bus->setSg(Sg);
                  bus->setV(V);
               }
               break;
            default :
               ;
         }
      }
   }
}
