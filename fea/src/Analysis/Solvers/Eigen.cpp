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

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//constructors
			Eigen::Eigen(void) : m_spectre(spectre::min), m_scale(1), m_modes(1), m_tolerance(1e-5)
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
				fscanf(file, "%d %lf %d %lf", &m_spectre, &m_scale, &m_modes, &m_tolerance);
			}
			void Eigen::save(FILE* file) const
			{
				Solver::save(file);
				fprintf(file, "%d %+.2e %02d %+.2e ", m_spectre, m_scale, m_modes, m_tolerance);
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
			
			double Eigen::tolerance(void) const
			{
				return m_tolerance;
			}
			double Eigen::tolerance(double tolerance)
			{
				return m_tolerance = tolerance;
			}
			
			unsigned Eigen::modes(void) const
			{
				switch(m_spectre)
				{
					case spectre::min:
						return 1;
					case spectre::max:
						return 1;
					case spectre::part:
						return m_modes;
					case spectre::full:
						return m_analysis->assembler()->dof_unknow();
					default:
						return 0;
				}
			}
			unsigned Eigen::modes(unsigned modes)
			{
				return m_modes = modes;
			}
			
			solvers::spectre Eigen::spectre(void) const
			{
				return m_spectre;
			}
			solvers::spectre Eigen::spectre(solvers::spectre spectre)
			{
				return m_spectre = spectre;
			}
			
			//eigen
			void Eigen::sort(void) const
			{
				//dofs
				const unsigned m = modes();
				const unsigned n = m_analysis->assembler()->dof_unknow();
				//sort
				for(unsigned i = 0; i < m; i++)
				{
					unsigned p = i;
					for(unsigned j = i + 1; j < m; j++)
					{
						if(m_k[j] < m_k[p])
						{
							p = j;
						}
					}
					if(p != i)
					{
						mat::swap(m_k[i], m_k[p]);
						mat::swap(m_f + n * i, m_f + n * p, n);
					}
				}
			}
			bool Eigen::eigen_std(void) const
			{
				//data
				const int* c = m_analysis->assembler()->col_map();
				const int* r = m_analysis->assembler()->row_map();
				const unsigned n = m_analysis->assembler()->dof_unknow();
				//solve
				switch(m_spectre)
				{
					case spectre::max:
						return mat::eigen(m_k[0], m_f, m_K, r, c, n, true);
					case spectre::min:
						return mat::eigen(m_k[0], m_f, m_K, r, c, n, false);
					case spectre::full:
					{
						//decompose
						double* A = new double[n * n];
						mat::convert(A, m_K, r, c, n);
						const bool test = mat::eigen(A, m_k, m_f, n, m_tolerance);
						//sort
						if(test)
						{
							sort();
						}
						//delete
						delete[] A;
						//return
						return test;
					}
					case spectre::part:
						return false;
					default:
						return false;
				}
			}
			bool Eigen::eigen_gen(void) const
			{
				return false;
			}
		}
	}
}
