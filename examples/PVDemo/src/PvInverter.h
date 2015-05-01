#ifndef PV_INVERTER_DOT_H
#define PV_INVERTER_DOT_H

#include <SgtCore/Gen.h>
#include <SgtSim/Inverter.h>
#include <SgtCore/Parser.h>
#include <SgtSim/SimParser.h>

namespace Sgt
{
    class PvInverter : public SimpleInverterAbc, public GenericGen
    {
        public:

            /// @name Static member functions:
            /// @{

        public:

            static const std::string& sComponentType()
            {
                static std::string result("pv_inverter");
                return result;
            }

            /// @}

            PvInverter(const std::string& id, std::string busId) :
                GenericGen(id, Phase::BAL),
                busId_(busId)
            {
                // Empty.
            }

            /// @name ComponentInterface virtual overridden functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            /// @}

            /// @name InverterAbc virtual overridden functions.
            /// @{

            virtual void addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source) override
            {
                InverterAbc::addDcPowerSource(source);
                source->dcPowerChanged().addAction([this]() {PChanged();}, "Update max power");
            }

            /// @}

            /// @name SimpleZipInverter specific member functions.
            /// @{

            double maxSMag() const
            {
                return maxSMag_;
            }

            void setMaxSMag(double maxSMag)
            {
                maxSMag_ = maxSMag;
            }

            /// @}

        public:

            double maxSMag_{1e9};
            double requestedQ_{0.0};
            std::string busId_;
        
        private:

            void PChanged()
            {
                double Pmax = availableP();
                setPMax(Pmax);
                setQMax(Pmax);
                setQMin(-Pmax);
            }
    };

    class PvInverterParserPlugin : public SimParserPlugin
    {
        public:
            virtual const char* key() override
            {
                return "pv_inverter";
            }

        public:
            virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // PV_INVERTER_DOT_H