#ifndef ZIP_TO_GROUND_DOT_H
#define ZIP_TO_GROUND_DOT_H

#include "Parser.h"
#include "ZipToGroundBase.h"

namespace SmartGridToolbox
{
   class ZipToGroundParser : public ComponentParser
   {
      public:
         static constexpr const char * getComponentName()
         {
            return "zip_to_ground";
         }
      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class ZipToGround : public ZipToGroundBase
   {
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return pos_infin;
         }
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override {};
         virtual void updateState(ptime t0, ptime t1) override {};
      /// @}
      
      /// @name My public member functions.
      /// @{
      public:
         ZipToGround(const std::string & name) : ZipToGroundBase(name), Y_(0.0), I_(0.0), S_(0.0) {}

         virtual Phases getPhases() const override {return phases_;}
         virtual void setPhases(Phases phases) {phases_ = phases;}

         virtual const UblasVector<Complex> & getY() const override {return Y_;}
         virtual void setY(const UblasVector<Complex> & Y) {Y_ = Y;}

         virtual const UblasVector<Complex> & getI() const override {return I_;} // Injection.
         virtual void setI(const UblasVector<Complex> & I) {I_ = I;} // Injection.

         virtual const UblasVector<Complex> & getS() const override {return S_;} // Injection.
         virtual void setS(const UblasVector<Complex> & S) {S_ = S;} // Injection.
      /// @}
      
      /// @name My private member variables.
      /// @{
         Phases phases_;               ///< My phases on parent bus.
         UblasVector<Complex> Y_;      ///< Constant admittance component.
         UblasVector<Complex> I_;      ///< Constant current injection component.
         UblasVector<Complex> S_;      ///< Constant power injection component.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_DOT_H