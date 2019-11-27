//std
#include <cmath>

//mat
#include "misc/util.h"
#include "linear/dense.h"
#include "linear/sparse.h"
#include "linear/linear.h"

//fea
#include "Model/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Eigen.h"
#include "Analysis/Assembler/Assembler.h"


//lapack
extern "C"
{
	void dsyev_(const char*, const char*, unsigned*, double*, unsigned*, double*, double*, unsigned*, int*);
	void dsygv_(unsigned*, const char*, const char*, unsigned*, double*, unsigned*, double*, unsigned*, double*, double*, unsigned*, int*);
	void dsyevx_(const char*, const char*, const char*, unsigned*, double*, unsigned*, double*, double*, unsigned*, unsigned*, double*, unsigned*, double*, 
		double*, unsigned*, double*, int*, unsigned*, unsigned*, int*);
	void dsygvx_(unsigned*, const char*, const char*, const char*, unsigned*, double*, unsigned*, double*, unsigned*, double*, double*, unsigned*, unsigned*,
		double*, unsigned*, double*, double*, unsigned*, double*, int*, unsigned*, unsigned*, int*);
}

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//constructors
			Eigen::Eigen(void) : m_spectre(spectre::min), m_scale(1)
			{
				return;
			}

			//destructor
			Eigen::~Eigen(void)
			{
				return;
			}

			//serialization
			void Eigen::load(FILE* file)
			{
				Solver::load(file);
				fscanf(file, "%d %lf", &m_spectre, &m_scale);
			}
			void Eigen::save(FILE* file) const
			{
				Solver::save(file);
				fprintf(file, "%d %+.2e ", m_spectre, m_scale);
			}

			//data
			double Eigen::scale(void) const
			{
				return m_scale;
			}
			double Eigen::scale(double scale)
			{
				return m_scale = scale;
			}
			
			solvers::spectre Eigen::spectre(void) const
			{
				return m_spectre;
			}
			solvers::spectre Eigen::spectre(solvers::spectre spectre)
			{
				return m_spectre = spectre;
			}
			
			unsigned Eigen::modes(void) const
			{
				return m_spectre == spectre::full ? m_analysis->assembler()->dof_unknow() : 1;
			}
			
			//eigen
			bool Eigen::eigen_std(void) const
			{
				//data
				const int* c = m_analysis->assembler()->col_map();
				const int* r = m_analysis->assembler()->row_map();
				unsigned n = m_analysis->assembler()->dof_unknow();
				//solve
				switch(m_spectre)
				{
					case spectre::max:
					{
						int status;
						int m = 8 * n;
						unsigned p = 1;
						double a = 0, w[m];
						unsigned il = n, iu = n;
						unsigned  q[5 * n], u[n];
						mat::convert(m_k, m_K, r, c, n);
						dsyevx_("N", "I", "L", &n, m_k, &n, nullptr, nullptr, &il, &iu, &a, &p, m_e, nullptr, &n, w, &m, q, u, &status);
						return status == 0;
					}
					case spectre::min:
					{
						int status;
						int m = 8 * n;
						unsigned p = 1;
						double a = 0, w[m];
						unsigned il = 1, iu = 1;
						unsigned  q[5 * n], u[n];
						mat::convert(m_k, m_K, r, c, n);
						dsyevx_("N", "I", "L", &n, m_k, &n, nullptr, nullptr, &il, &iu, &a, &p, m_e, nullptr, &n, w, &m, q, u, &status);
						return status == 0;
					}
					case spectre::full:
					{
						int status;
						double w[3 * n - 1];
						unsigned m = 3 * n - 1;
						mat::convert(m_k, m_K, r, c, n);
						dsyev_("V", "L", &n, m_k, &n, m_e, w, &m, &status);
						return status == 0;
					}
					default:
						return false;
				}
			}
			bool Eigen::eigen_gen(void) const
			{
				//data
				const int* c = m_analysis->assembler()->col_map();
				const int* r = m_analysis->assembler()->row_map();
				unsigned n = m_analysis->assembler()->dof_unknow();
				//solve
				switch(m_spectre)
				{
					case spectre::max:
					{
						int status;
						int m = 8 * n;
						double a = 0, w[m];
						unsigned t = 1, p = 1;
						unsigned il = n, iu = n;
						unsigned  q[5 * n], u[n];
						mat::convert(m_k, m_K, r, c, n);
						mat::convert(m_m, m_M, r, c, n);
						dsygvx_(&t, "N", "I", "L", &n, m_k, &n, m_m, &n, nullptr, nullptr, &il, &iu, &a, &p, m_e, nullptr, &n, w, &m, q, u, &status);
						return status == 0;
					}
					case spectre::min:
					{
						int status;
						int m = 8 * n;
						double a = 0, w[m];
						unsigned t = 1, p = 1;
						unsigned il = 1, iu = 1;
						unsigned  q[5 * n], u[n];
						mat::convert(m_k, m_K, r, c, n);
						mat::convert(m_m, m_M, r, c, n);
						dsygvx_(&t, "N", "I", "L", &n, m_k, &n, m_m, &n, nullptr, nullptr, &il, &iu, &a, &p, m_e, nullptr, &n, w, &m, q, u, &status);
						return status == 0;
					}
					case spectre::full:
					{
						int status;
						unsigned type = 1;
						double w[3 * n - 1];
						unsigned m = 3 * n - 1;
						mat::convert(m_k, m_K, r, c, n);
						mat::convert(m_m, m_M, r, c, n);
						dsygv_(&type, "V", "L", &n, m_k, &n, m_m, &n, m_e, w, &m, &status);
						return status == 0;
					}
					default:
						return false;
				}
			}
		}
	}
}
