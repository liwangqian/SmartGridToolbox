#include "PowerFlow.h"

#include <iostream>
#include <sstream>

namespace SmartGridToolbox
{
   static const int nPhases = 9;
   static Phase allPhases[nPhases + 1] = {
      Phase::BAL, 
      Phase::A, 
      Phase::B, 
      Phase::C, 
      Phase::G, 
      Phase::N, 
      Phase::SP, 
      Phase::SM, 
      Phase::SN,
      Phase::BAD
   };

   const char * busType2Str(BusType type)
   {
      switch (type)
      {
         case BusType::SL: return "SL"; break;
         case BusType::PQ: return "PQ"; break;
         case BusType::PV: return "PV"; break;
         case BusType::BAD: return "UNDEFINED"; break;
      }
   }

   BusType str2BusType(const std::string & str)
   {
      static BusType busTypes[] = {BusType::SL, BusType::PQ, BusType::PV, BusType::BAD};
      BusType result = BusType::BAD;
      for (BusType * test = &busTypes[0]; *test != BusType::BAD; ++test)
      {
         if (str == busType2Str(*test))
         {
            result = *test; 
         }
      }
      return result;
   }

   const char * phase2Str(Phase phase)
   {
      switch (phase)
      {
         case Phase::BAL: return "BAL"; break;
         case Phase::A: return "A"; break;
         case Phase::B: return "B"; break;
         case Phase::C: return "C"; break;
         case Phase::G: return "G"; break;
         case Phase::N: return "N"; break;
         case Phase::SP: return "SP"; break;
         case Phase::SM: return "SM"; break;
         case Phase::SN: return "SN"; break;
         case Phase::BAD: return "BAD"; break;
      }
   }

   Phase str2Phase(const std::string & str)
   {
      static Phase phases[] = {Phase::BAL, Phase::A, Phase::B, Phase::C, Phase::G, Phase::N, Phase::SP, 
                               Phase::SM, Phase:: SN, Phase::BAD};
      Phase result = Phase::BAD;
      for (Phase * test = &phases[0]; *test != Phase::BAD; ++test)
      {
         if (str == phase2Str(*test))
         {
            result = *test; 
         }
      }
      return result;
   }

   const char * phaseDescr(Phase phase)
   {
      switch (phase)
      {
         case Phase::BAL: return "balanced/1-phase"; break;
         case Phase::A: return "3-phase A"; break;
         case Phase::B: return "3-phase B"; break;
         case Phase::C: return "3-phase C"; break;
         case Phase::G: return "ground"; break;
         case Phase::N: return "neutral"; break;
         case Phase::SP: return "split-phase +ve"; break;
         case Phase::SM: return "split-phase -ve"; break;
         case Phase::SN: return "split-phase neutral"; break;
         case Phase::BAD: return "UNDEFINED"; break;
      }
   }   

   Phases & Phases::operator&=(const Phases & other)
   {
      mask_ &= other;
      rebuild();
      return *this;
   }

   Phases & Phases::operator|=(const Phases & other)
   {
      mask_ |= other;
      rebuild();
      return *this;
   }

   std::string Phases::toStr() const
   {
      std::ostringstream ss;
      ss << phaseVec_[0];
      for (int i = 1; i < phaseVec_.size(); ++i)
      {
         ss << "|" << phaseVec_[i];
      }
      return ss.str();
   }

   void Phases::rebuild()
   {
      phaseVec_ = PhaseVec();
      phaseVec_.reserve(nPhases);
      idxMap_ = IdxMap();
      for (unsigned int i = 0, j = 0; allPhases[i] != Phase::BAD; ++i)
      {
         if (hasPhase(allPhases[i]))
         {
            phaseVec_.push_back(allPhases[i]);
            idxMap_[allPhases[i]] = j++;
         }
      }
      phaseVec_.shrink_to_fit();
   }

   // Balanced/1-phase simple line with a single admittance.
   const ublas::matrix<Complex> YLine1P(const Complex & y)
   {
      ublas::matrix<Complex> Y(2, 2, czero);
      Y(0, 0) = y;
      Y(1, 1) = y;
      Y(0, 1) = -y;
      Y(1, 0) = -y;
      return Y;
   }

   // No cross terms, just nPhase lines with single admittances.
   const ublas::matrix<Complex> YSimpleLine(const ublas::vector<Complex> & y)
   {
      int nPhase = y.size();
      int nTerm = 2 * nPhase; 
      ublas::matrix<Complex> Y(nTerm, nTerm, czero);
      for (int i = 0; i < nPhase; ++i)
      {
         Y(i, i) = y(i);
         Y(i + nPhase, i + nPhase) = y(i);
         Y(i, i + nPhase) = -y(i);
         Y(i + nPhase, i) = -y(i);
      }
      return Y;
   }
   
   ublas::matrix<Complex> YOverheadLine(ublas::vector<double> r, ublas::matrix<double> DMat, double L,
                                        double freq, double rho)
   {
      const double mile = 1609.344; // meters
      const double foot = 0.3048; // meters

      int n = r.size();
      assert(DMat.size1() == n);
      assert(DMat.size2() == n);

		double freqCoeffReal = 0.00158836 * freq / mile;
		double freqCoeffImag = 0.00202237 * freq / mile;
		double freqAdditiveTerm = 0.5 * log(rho / freq) + 7.6786;

      ublas::matrix<Complex> z(n, n, czero);
      for (int i = 0; i < n; ++i)
      {
         z(i, i) = {r(i) + freqCoeffReal, freqCoeffImag * (log(foot / (DMat(i, i))) + freqAdditiveTerm)};
         for (int k = i + 1; k < n; ++k)
         {
				z(i, k) = {freqCoeffReal, freqCoeffImag * (log(foot / (DMat(i, k)) + freqAdditiveTerm))};
				z(k, i) = z(i, k);
         }
      }
      z *= L;

      // TODO: eliminate the neutral phase, as per calculation of b_mat in gridLAB-D overhead_line.cpp.
      // Very easy, but interface needs consideration. Equation is:
		// b_mat[0][0] = (z_aa - z_an * z_an * z_nn_inv) * miles;
      // b_mat[0][1] = (z_ab - z_an * z_bn * z_nn_inv) * miles;
      // etc.
      
      std::cout << "z = " << row(z, 0) << std::endl;
      std::cout << "z = " << row(z, 1) << std::endl;
      std::cout << "z = " << row(z, 2) << std::endl;
      
      ublas::matrix<Complex> y(n, n); bool ok = invertMatrix(z, y); assert(ok);
      
      std::cout << "y = " << row(y, 0) << std::endl;
      std::cout << "y = " << row(y, 1) << std::endl;
      std::cout << "y = " << row(y, 2) << std::endl;
      
      ublas::matrix<Complex> Y(2 * n, 2 * n, czero);
      for (int i = 0; i < n; ++i)
      {
         Y(i, i + n) = -y(i, i); 
         Y(i + n, i) = -y(i, i); 

         for (int k = i + 1; k < n; ++k)
         {
            // Diagonal terms in node admittance matrix.
            Y(i, i)         += y(i, k);
            Y(k, k)         += y(k, i);
            Y(i + n, i + n) += y(i, k);
            Y(k + n, k + n) += y(k, i);

            Y(i, k + n)      = -y(i, k);
            Y(i + n, k)      = -y(i, k);
            Y(k, i + n)      = -y(k, i);
            Y(k + n, i)      = -y(k, i);
         }
      }
      std::cout << "Y = " << std::setw(20) << std::left <<  row(Y, 0) << std::endl;
      std::cout << "Y = " << std::setw(20) << std::left <<  row(Y, 1) << std::endl;
      std::cout << "Y = " << std::setw(20) << std::left <<  row(Y, 2) << std::endl;
      std::cout << "Y = " << std::setw(20) << std::left <<  row(Y, 3) << std::endl;
      std::cout << "Y = " << std::setw(20) << std::left <<  row(Y, 4) << std::endl;
      std::cout << "Y = " << std::setw(20) << std::left <<  row(Y, 5) << std::endl;

      return Y;
   }
}
