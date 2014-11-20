#include <klu.h>

#include <armadillo>

bool kluSolve(const arma::SpMat<double>& a, const arma::Col<double>& b, arma::Col<double>& result)
{
   boost::numeric::ublas::compressed_matrix<double, boost::numeric::ublas::column_major> ac = a;
   ac.complete_index1_data();

   int n = b.size();
   int nnz = ac.nnz();

   int* ap = new int[n + 1];
   for (int i = 0; i <= n; ++i) ap[i] = ac.index1_data()[i];

   int* ai = new int[nnz];
   for (int i = 0; i < nnz; ++i) ai[i] = ac.index2_data()[i];

   double* ax = new double[nnz];
   for (int i = 0; i < nnz; ++i) ax[i] = ac.value_data()[i];

   double* b1 = new double[n];
   for (int i = 0; i < n; ++i) b1[i] = b(i);

   klu_symbolic *Symbolic;
   klu_numeric *Numeric;
   klu_common Common;

   klu_defaults (&Common);
   Symbolic = klu_analyze (n, ap, ai, &Common);
   Numeric = klu_factor (ap, ai, ax, Symbolic, &Common);
   bool ok = klu_solve(Symbolic, Numeric, n, 1, b1, &Common) == 1;

   if (!ok)
   {
      std::cerr << "KLU failed." << std::endl;
      std::cerr << "Status = " << Common.status << std::endl;
   }

   klu_free_symbolic (&Symbolic, &Common);
   klu_free_numeric (&Numeric, &Common);

   result = boost::numeric::arma::Col<double>(n);
   for (int i = 0; i < n; ++i)
   {
      result(i) = b1[i];
   }

   delete[] ap;
   delete[] ai;
   delete[] ax;
   delete[] b1;

   return ok;
}
