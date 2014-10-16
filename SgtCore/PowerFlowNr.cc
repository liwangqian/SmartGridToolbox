#include <algorithm>
#include <ostream>
#include <sstream>
#include "PowerFlowNr.h"
#include "SparseSolver.h"
#include "Stopwatch.h"

namespace SmartGridToolbox
{
   namespace
   {
      // For some reason, even ublas::axpy_prod is much slower than this!
      ublas::vector<double> myProd(const ublas::compressed_matrix<double>& A, const ublas::vector<double>& x)
      {
         ublas::vector<double> result(A.size1(), 0.0);
         for (auto it1 = A.begin1(); it1 != A.end1(); ++it1)
         {
            for (auto it2 = it1.begin(); it2 != it1.end(); ++it2)
            {
               int i = it2.index1();
               int k = it2.index2();
               result(i) += A(i, k) * x(k);
            }
         }
         return result;
      }

      void initJcBlock(
            const ublas::matrix_range<const ublas::compressed_matrix<double>>& G,
            const ublas::matrix_range<const ublas::compressed_matrix<double>>& B,
            ublas::compressed_matrix<double>& Jrr,
            ublas::compressed_matrix<double>& Jri,
            ublas::compressed_matrix<double>& Jir,
            ublas::compressed_matrix<double>& Jii)
      {
         Jrr = -G;
         Jri =  B;
         Jir = -B;
         Jii = -G;
      }
   }

   Jacobian::Jacobian(int nPq, int nPv)
   {
      for (int i = 0; i < 2; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k].resize(nPq, nPq, false);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k].resize(nPq, nPv, false);
         }
      }
      for (int i = 2; i < 4; ++i)
      {
         for (int k = 0; k < 2; ++k)
         {
            blocks_[i][k].resize(nPv, nPq, false);
         }
         for (int k = 2; k < 5; ++k)
         {
            blocks_[i][k].resize(nPv, nPv, false);
         }
      }
   }

   void PowerFlowNr::validate()
   {
      Y_.resize(mod_->nNode(), mod_->nNode(), false);

      // Branch admittances:
      for (const std::unique_ptr<PfBranch>& branch : mod_->branches())
      {
         auto it0 = mod_->busses().find(branch->ids_[0]);
         if (it0 == mod_->busses().end())
         {
            Log().fatal() << "BranchComp " << branch->ids_[0] << " " << branch->ids_[1]
               << " contains a non-existent bus " << branch->ids_[0] << std::endl;
         }
         auto it1 = mod_->busses().find(branch->ids_[1]);
         if (it1 == mod_->busses().end())
         {
            Log().fatal() << "BranchComp " << branch->ids_[0] << " " << branch->ids_[1]
               << " contains a non-existent bus " << branch->ids_[1] << std::endl;
         }
         const PfBus* busses[] = {it0->second.get(), it1->second.get()};
         int nTerm = 2 * branch->nPhase_;

         // There is one link per distinct pair of bus/phase pairs.
         for (int i = 0; i < nTerm; ++i)
         {
            int busIdxI = i / branch->nPhase_; // 0 or 1
            int branchPhaseIdxI = i % branch->nPhase_; // 0 to nPhase of branch.
            const PfBus* busI = busses[busIdxI];
            int busPhaseIdxI = busI->phases_.phaseIndex(branch->phases_[busIdxI][branchPhaseIdxI]);
            const PfNode* nodeI = busI->nodes_[busPhaseIdxI].get();
            int idxNodeI = nodeI->idx_;

            // Only count each diagonal element in branch->Y_ once!
            Y_(idxNodeI, idxNodeI) += branch->Y_(i, i);

            for (int k = i + 1; k < nTerm; ++k)
            {
               int busIdxK = k / branch->nPhase_; // 0 or 1
               int branchPhaseIdxK = k % branch->nPhase_; // 0 to nPhase of branch.
               const PfBus* busK = busses[busIdxK];
               int busPhaseIdxK = busK->phases_.phaseIndex(branch->phases_[busIdxK][branchPhaseIdxK]);
               const PfNode* nodeK = busK->nodes_[busPhaseIdxK].get();
               int idxNodeK = nodeK->idx_;

               Y_(idxNodeI, idxNodeK) += branch->Y_(i, k);
               Y_(idxNodeK, idxNodeI) += branch->Y_(k, i);
            }
         }
      } // Loop over branches.

      // Add shunt terms:
      for (int i = 0; i < mod_->nNode(); ++i)
      {
         Y_(i, i) += mod_->nodes()[i]->Ys_;
      }

      G_ = real(Y_);
      B_ = imag(Y_);
      SGT_DEBUG(Log().debug() << "Y_.nnz() = " << Y_.nnz() << std::endl);

      // Load quantities.
      Ic_.resize(mod_->nNode(), false);
      for (int i = 0; i < mod_->nNode(); ++i)
      {
         Ic_(i) = mod_->nodes()[i]->Ic_;
      }

      SGT_DEBUG(Log().debug() << "PowerFlowNr : validate complete." << std::endl);
      SGT_DEBUG(printProblem());
   }

   /// Initialize voltages:
   void PowerFlowNr::initV(ublas::vector<double>& Vr, ublas::vector<double>& Vi) const
   {
      for (int i = 0; i < mod_->nNode(); ++i)
      {
         const PfNode& node = *mod_->nodes()[i];
         Vr(i) = node.V_.real();
         Vi(i) = node.V_.imag();
      }
   }

   void PowerFlowNr::initS(ublas::vector<double>& P, ublas::vector<double>& Q) const
   {
      for (int i = 0; i < mod_->nNode(); ++i)
      {
         const PfNode& node = *mod_->nodes()[i];
         P(i) = node.S_.real();
         Q(i) = node.S_.imag();
      }
   }

   /// Set the part of J that doesn't update at each iteration.
   /** At this stage, we are treating J as if all busses were PQ. */
   void PowerFlowNr::initJc(Jacobian& Jc) const
   {
      initJcBlock(project(G_, selPqFromAll(), selPqFromAll()),
                  project(B_, selPqFromAll(), selPqFromAll()),
                  Jc.IrPqVrPq(),
                  Jc.IrPqViPq(),
                  Jc.IiPqVrPq(),
                  Jc.IiPqViPq());
      initJcBlock(project(G_, selPqFromAll(), selPvFromAll()),
                  project(B_, selPqFromAll(), selPvFromAll()),
                  Jc.IrPqVrPv(),
                  Jc.IrPqViPv(),
                  Jc.IiPqVrPv(),
                  Jc.IiPqViPv());
      initJcBlock(project(G_, selPvFromAll(), selPqFromAll()),
                  project(B_, selPvFromAll(), selPqFromAll()),
                  Jc.IrPvVrPq(),
                  Jc.IrPvViPq(),
                  Jc.IiPvVrPq(),
                  Jc.IiPvViPq());
      initJcBlock(project(G_, selPvFromAll(), selPvFromAll()),
                  project(B_, selPvFromAll(), selPvFromAll()),
                  Jc.IrPvVrPv(),
                  Jc.IrPvViPv(),
                  Jc.IiPvVrPv(),
                  Jc.IiPvViPv());
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNr::calcf(ublas::vector<double>& f,
                           const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                           const ublas::vector<double>& P, const ublas::vector<double>& Q,
                           const ublas::vector<double>& M2Pv) const
   {
      // PQ busses:
      const ublas::compressed_matrix<double> GPq = project(G_, selPqFromAll(), selAllFromAll());
      const ublas::compressed_matrix<double> BPq = project(B_, selPqFromAll(), selAllFromAll());

      const auto VrPq = project(Vr, selPqFromAll());
      const auto ViPq = project(Vi, selPqFromAll());

      const auto PPq = project(P, selPqFromAll());
      const auto QPq = project(Q, selPqFromAll());

      const auto IcrPq = project(real(Ic_), selPqFromAll());
      const auto IciPq = project(imag(Ic_), selPqFromAll());

      ublas::vector<double> M2Pq = element_prod(VrPq, VrPq) + element_prod(ViPq, ViPq);

      project(f, selIrPqFrom_f()) = element_div(element_prod(VrPq, PPq) + element_prod(ViPq, QPq), M2Pq)
                                 + IcrPq - myProd(GPq, Vr) + myProd(BPq, Vi);
      project(f, selIiPqFrom_f()) = element_div(element_prod(ViPq, PPq) - element_prod(VrPq, QPq), M2Pq)
                                 + IciPq - myProd(GPq, Vi) - myProd(BPq, Vr);

      // PV busses. Note that these differ in that M2Pv is considered a constant.
      const auto GPv = project(G_, selPvFromAll(), selAllFromAll());
      const auto BPv = project(B_, selPvFromAll(), selAllFromAll());

      const auto VrPv = project(Vr, selPvFromAll());
      const auto ViPv = project(Vi, selPvFromAll());

      const auto PPv = project(P, selPvFromAll());
      const auto QPv = project(Q, selPvFromAll());

      const auto IcrPv = project(real(Ic_), selPvFromAll());
      const auto IciPv = project(imag(Ic_), selPvFromAll());

      project(f, selIrPvFrom_f()) = element_div(element_prod(VrPv, PPv) + element_prod(ViPv, QPv), M2Pv)
                                 + IcrPv - myProd(GPv, Vr) + myProd(BPv, Vi);
      project(f, selIiPvFrom_f()) = element_div(element_prod(ViPv, PPv) - element_prod(VrPv, QPv), M2Pv)
                                 + IciPv - myProd(GPv, Vi) - myProd(BPv, Vr);
   }

   // At this stage, we are treating f as if all busses were PQ. PV busses will be taken into account later.
   void PowerFlowNr::updateJ(Jacobian& J, const Jacobian& Jc,
                             const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                             const ublas::vector<double>& P, const ublas::vector <double>& Q,
                             const ublas::vector<double>& M2Pv) const
   {
      // Elements in J that have no non-constant part will be initialized to the corresponding term in Jc at the
      // start of the calculation, and will not change. Thus, only set elements that have a non-constant part.

      // Reset PV Vi columns, since these get messed with:
      J.IrPqViPv() = Jc.IrPqViPv();
      J.IiPqViPv() = Jc.IiPqViPv();
      J.IrPvViPv() = Jc.IrPvViPv();
      J.IiPvViPv() = Jc.IiPvViPv();

      // Block diagonal:
      for (int i = 0; i < mod_->nPq(); ++i)
      {
         int iPqi = iPq(i);

         double Pvr_p_QVi = P(iPqi) * Vr(iPqi) + Q(iPqi) * Vi(iPqi);
         double Pvi_m_QVr = P(iPqi) * Vi(iPqi) - Q(iPqi) * Vr(iPqi);
         double M2 = Vr(iPqi) * Vr(iPqi) + Vi(iPqi) * Vi(iPqi);
         double M4 = M2 * M2;
         double VrdM4 = Vr(iPqi) / M4;
         double VidM4 = Vi(iPqi) / M4;
         double PdM2 = P(iPqi) / M2;
         double QdM2 = Q(iPqi) / M2;

         J.IrPqVrPq()(i, i) = Jc.IrPqVrPq()(i, i) - (2 * VrdM4 * Pvr_p_QVi) + PdM2;
         J.IrPqViPq()(i, i) = Jc.IrPqViPq()(i, i) - (2 * VidM4 * Pvr_p_QVi) + QdM2;
         J.IiPqVrPq()(i, i) = Jc.IiPqVrPq()(i, i) - (2 * VrdM4 * Pvi_m_QVr) - QdM2;
         J.IiPqViPq()(i, i) = Jc.IiPqViPq()(i, i) - (2 * VidM4 * Pvi_m_QVr) + PdM2;
      }

      // For PV busses, M^2 is constant, and therefore we can write the Jacobian more simply.
      for (int i = 0; i < mod_->nPv(); ++i)
      {
         int iPvi = iPv(i);

         J.IrPvVrPv()(i, i) = Jc.IrPvVrPv()(i, i) + P(iPvi) / M2Pv(i); // Could -> Jc if we wanted.
         J.IrPvViPv()(i, i) = Jc.IrPvViPv()(i, i) + Q(iPvi) / M2Pv(i);
         J.IiPvVrPv()(i, i) = Jc.IiPvVrPv()(i, i) - Q(iPvi) / M2Pv(i);
         J.IiPvViPv()(i, i) = Jc.IiPvViPv()(i, i) + P(iPvi) / M2Pv(i);
      }

      // Set the PV Q columns in the Jacobian. They are diagonal.
      const auto VrPv = project(Vr, selPvFromAll());
      const auto ViPv = project(Vi, selPvFromAll());
      for (int i = 0; i < mod_->nPv(); ++i)
      {
         J.IrPvQPv()(i, i) = ViPv(i) / M2Pv(i);
         J.IiPvQPv()(i, i) = -VrPv(i) / M2Pv(i);
      }
   }

   // Modify J and f to take into account PV busses.
   void PowerFlowNr::modifyForPv(Jacobian& J, ublas::vector<double>& f,
                                 const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                                 const ublas::vector<double>& M2Pv)
   {
      const auto VrPv = project(Vr, selPvFromAll());
      const auto ViPv = project(Vi, selPvFromAll());

      typedef ublas::vector_slice<ublas::vector<double>> VecSel;
      typedef ublas::matrix_column<ublas::compressed_matrix<double>> Column;
      auto mod = [](VecSel fProj, Column colViPv, const Column colVrPv, double fMult, double colViPvMult)
      {
         for (auto it = colVrPv.begin(); it != colVrPv.end(); ++it)
         {
            int idx = it.index();
            fProj(idx) += colVrPv(idx) * fMult;
            colViPv(idx) += colVrPv(idx) * colViPvMult;
         }
      };

      for (int k = 0; k < mod_->nPv(); ++k)
      {
         double fMult = (0.5 * (M2Pv(k) - VrPv(k) * VrPv(k) - ViPv(k) * ViPv(k)) / VrPv(k));
         double colViPvMult = -ViPv(k) / VrPv(k);

         mod(project(f, selIrPqFrom_f()), column(J.IrPqViPv(), k), column(J.IrPqVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIiPqFrom_f()), column(J.IiPqViPv(), k), column(J.IiPqVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIrPvFrom_f()), column(J.IrPvViPv(), k), column(J.IrPvVrPv(), k), fMult, colViPvMult);
         mod(project(f, selIiPvFrom_f()), column(J.IiPvViPv(), k), column(J.IiPvVrPv(), k), fMult, colViPvMult);
      }
   }

   void PowerFlowNr::calcJMatrix(ublas::compressed_matrix<double>& JMat, const Jacobian& J) const
   {
      Array<int, 4> ibInd = {{0, 1, 2, 3}};
      Array<int, 4> kbInd = {{0, 1, 3, 4}}; // Skip VrPv, since it doesn't appear as a variable.
      Array<ublas::slice, 4> sl1Vec = {{selIrPqFrom_f(), selIiPqFrom_f(), selIrPvFrom_f(), selIiPvFrom_f()}};
      Array<ublas::slice, 4> sl2Vec = {{selVrPqFrom_x(), selViPqFrom_x(), selViPvFrom_x(), selQPvFrom_x()}};

      JMat = ublas::compressed_matrix<double>(nVar(), nVar());

      // Loop over all blocks in J.
      for (int ib = 0; ib < 4; ++ib)
      {
         ublas::slice sl1 = sl1Vec[ib];
         for (int kb = 0; kb < 4; ++kb)
         {
            ublas::slice sl2 = sl2Vec[kb];
            const ublas::compressed_matrix<double>& block = J.blocks_[ibInd[ib]][kbInd[kb]];

            // Loop over all non-zero elements in the block.
            for (auto it1 = block.begin1(); it1 != block.end1(); ++it1)
            {
               for (auto it2 = it1.begin(); it2 != it1.end(); ++it2)
               {
                  // Get the indices into the block.
                  int iBlock = it2.index1();
                  int kBlock = it2.index2();

                  // Get the indices into JMat.
                  int iRes = sl1(iBlock);
                  int kRes = sl2(kBlock);

                  // Assign the element.
                  JMat(iRes, kRes) = block(iBlock, kBlock);
               }
            }
         }
      }
   }

   bool PowerFlowNr::solve()
   {
      SGT_DEBUG(Log().debug() << "PowerFlowNr : solve." << std::endl; LogIndent _;);

      Stopwatch stopwatch;
      Stopwatch stopwatchTot;

      double durationInitSetup = 0;
      double durationCalcf = 0;
      double durationUpdateJ = 0;
      double durationModifyForPv = 0;
      double durationConstructJMat = 0;
      double durationSolve = 0;
      double durationUpdateIter = 0;
      double durationTot = 0;

      stopwatchTot.reset(); stopwatchTot.start();

      // Do the initial setup.
      stopwatch.reset(); stopwatch.start();

      const double tol = 1e-8;
      const int maxiter = 20;

      ublas::vector<double> Vr(mod_->nNode());
      ublas::vector<double> Vi(mod_->nNode());
      initV(Vr, Vi);

      ublas::vector<double> M2Pv = element_prod(project(Vr, selPvFromAll()), project(Vr, selPvFromAll()))
                                 + element_prod(project(Vi, selPvFromAll()), project(Vi, selPvFromAll()));

      ublas::vector<double> P(mod_->nNode());
      ublas::vector<double> Q(mod_->nNode());
      initS(P, Q);

      Jacobian Jc(mod_->nPq(), mod_->nPv()); ///< The part of J that doesn't update at each iteration.
      initJc(Jc);

      ublas::vector<double> f(nVar()); ///< Current mismatch function.

      Jacobian J = Jc; ///< Jacobian, d f_i/d x_i.

      bool wasSuccessful = false;
      double err = 0;
      int niter;

      stopwatch.stop(); durationInitSetup = stopwatch.seconds();

      for (niter = 0; niter < maxiter; ++niter)
      {
         SGT_DEBUG(Log().debug() << "Iteration = " << niter << std::endl);

         stopwatch.reset(); stopwatch.start();
         calcf(f, Vr, Vi, P, Q, M2Pv);

         err = norm_inf(f);
         SGT_DEBUG(Log().debug() << "f  = " << std::setprecision(5) << std::setw(9) << f << std::endl);
         SGT_DEBUG(Log().debug() << "Error = " << err << std::endl);
         if (err <= tol)
         {
            SGT_DEBUG(Log().debug() << "Success at iteration " << niter << "." << std::endl);
            wasSuccessful = true;
            break;
         }
         stopwatch.stop(); durationCalcf += stopwatch.seconds();

         stopwatch.reset(); stopwatch.start();
         updateJ(J, Jc, Vr, Vi, P, Q, M2Pv);
         stopwatch.stop(); durationUpdateJ += stopwatch.seconds();

         stopwatch.reset(); stopwatch.start();
         modifyForPv(J, f, Vr, Vi, M2Pv);
         stopwatch.stop(); durationModifyForPv += stopwatch.seconds();

         // Construct the full Jacobian from J, which contains the block structure.
         stopwatch.reset(); stopwatch.start();
         ublas::compressed_matrix<double> JMat; calcJMatrix(JMat, J);
         stopwatch.stop(); durationConstructJMat += stopwatch.seconds();

         SGT_DEBUG
         (
            Log().debug() << "Before kluSolve: Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl;
            Log().debug() << "Before kluSolve: Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl;
            Log().debug() << "Before kluSolve: M^2 = " << std::setprecision(5) << std::setw(9)
                    << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl;
            Log().debug() << "Before kluSolve: P   = " << std::setprecision(5) << std::setw(9) << P << std::endl;
            Log().debug() << "Before kluSolve: Q   = " << std::setprecision(5) << std::setw(9) << Q << std::endl;
            Log().debug() << "Before kluSolve: f   = " << std::setprecision(5) << std::setw(9) << f << std::endl;
            Log().debug() << "Before kluSolve: J   = " << std::endl;
            LogIndent _;
            for (int i = 0; i < nVar(); ++i)
            {
               Log().debug() << std::setprecision(5) << std::setw(9) << row(JMat, i) << std::endl;
            }
         );

         stopwatch.reset(); stopwatch.start();
         ublas::vector<double> x;
         bool ok = kluSolve(JMat, -f, x);
         stopwatch.stop(); durationSolve += stopwatch.seconds();

         SGT_DEBUG(Log().debug() << "After kluSolve: ok = " << ok << std::endl);
         SGT_DEBUG(Log().debug() << "After kluSolve: x  = " << std::setprecision(5) << std::setw(9) << x 
               << std::endl);
         if (!ok)
         {
            Log().fatal() << "kluSolve failed." << std::endl;
         }

         stopwatch.reset(); stopwatch.start();
         // Update the current values of V from the solution:
         project(Vr, selPqFromAll()) += project(x, selVrPqFrom_x());
         project(Vi, selPqFromAll()) += project(x, selViPqFrom_x());

         // Explicitly deal with the voltage magnitude constraint by updating VrPv by hand.
         auto VrPv = project(Vr, selPvFromAll());
         auto ViPv = project(Vi, selPvFromAll());
         const auto DeltaViPv = project(x, selViPvFrom_x());
         VrPv += element_div(M2Pv - element_prod(VrPv, VrPv) - element_prod(ViPv, ViPv)
                             - 2 * element_prod(ViPv, DeltaViPv), 2 * VrPv);
         ViPv += DeltaViPv;

         // Update Q for PV busses based on the solution.
         project(Q, selPvFromAll()) += project(x, selQPvFrom_x());

         SGT_DEBUG(Log().debug() << "Updated Vr  = " << std::setprecision(5) << std::setw(9) << Vr << std::endl);
         SGT_DEBUG(Log().debug() << "Updated Vi  = " << std::setprecision(5) << std::setw(9) << Vi << std::endl);
         SGT_DEBUG(Log().debug() << "Updated M^2 = " << std::setprecision(5) << std::setw(9)
                           << (element_prod(Vr, Vr) + element_prod(Vi, Vi)) << std::endl);
         SGT_DEBUG(Log().debug() << "Updated P   = " << std::setprecision(5) << std::setw(9) << P << std::endl);
         SGT_DEBUG(Log().debug() << "Updated Q   = " << std::setprecision(5) << std::setw(9) << Q << std::endl);
         stopwatch.stop(); durationUpdateIter += stopwatch.seconds();
      }

      if (wasSuccessful)
      {
         ublas::vector<Complex> V(mod_->nNode());
         ublas::vector<Complex> S(mod_->nNode());

         for (int i = 0; i < mod_->nNode(); ++i)
         {
            V(i) = {Vr(i), Vi(i)};
            S(i) = {P(i), Q(i)};
         }

         // Set the slack power.
         auto SSl = project(S, selSlFromAll());

         auto VSl = project(V, selSlFromAll());
         auto IcSl = project(Ic_, selSlFromAll());

         auto YStar = conj(project(Y_, selSlFromAll(), selAllFromAll()));
         auto VStar = conj(V);
         auto IcStar = conj(project(Ic_, selSlFromAll()));

         SSl = element_prod(VSl, prod(YStar, VStar)) - element_prod(VSl, IcStar);

         // Update nodes and busses.
         for (int i = 0; i < mod_->nNode(); ++i)
         {
            auto node = mod_->nodes()[i];
            node->V_ = V(i);
            node->S_ = S(i);
            node->bus_->V_[node->phaseIdx_] = node->V_;
            node->bus_->S_[node->phaseIdx_] = node->S_;
         }
      }
      else
      {
         Log().fatal() << "PowerFlowNr: Newton-Raphson method failed to converge." << std::endl;
      }

      stopwatchTot.stop(); durationTot = stopwatchTot.seconds();

      Log().message() << "PowerFlowNr: successful = " << wasSuccessful << ", error = " << err
                << ", iterations = " << niter << ", total time = " << durationTot << "." << std::endl;
      SGT_DEBUG(Log().debug() << "PowerFlowNr: -----------------------   " << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: init setup time         = " << durationInitSetup << " s." << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time in calcf           = " << durationCalcf << " s." << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time in updateJ         = " << durationUpdateJ << " s." << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time in modifyForPv     = " << durationModifyForPv << " s." 
            << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time to construct JMat  = " << durationConstructJMat << " s." 
            << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: solve time              = " << durationSolve << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: time to update iter     = " << durationUpdateIter << std::endl);
      SGT_DEBUG(Log().debug() << "PowerFlowNr: -----------------------   " << std::endl);

      return wasSuccessful;
   }

}
