//mat
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Assembler/Assembler.h"
#include "Analysis/Strategies/Orthogonal_Residue.h"

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			//constructors
			Orthogonal_Residue::Orthogonal_Residue(void)
			{
				return;
			}

			//destructor
			Orthogonal_Residue::~Orthogonal_Residue(void)
			{
				return;
			}

			//type
			strategies::type Orthogonal_Residue::type(void) const
			{
				return strategies::type::orthogonal_residue;
			}

			//analysis
			double Orthogonal_Residue::predictor(void) const
			{
				//analysis
				const solvers::Solver* solver = m_analysis->solver();
				const unsigned nu = m_analysis->assembler()->dof_unknow();
				//data
				const double* du = solver->state_increment();
				const double* dut = solver->state_predictor();
				//return
				const double d = mat::norm(du, nu);
				const double t = mat::norm(dut, nu);
				const double s = mat::dot(du, dut, nu);
				return mat::sign(s) * d / t;
			}
			double Orthogonal_Residue::corrector(void) const
			{
				//analysis
				const solvers::Solver* solver = m_analysis->solver();
				const unsigned nu = m_analysis->assembler()->dof_unknow();
				//data
				const double* r = solver->residue();
				const double* du = solver->state_increment();
				const double* Fr = solver->reference_force();
				//return
				const double e = mat::dot(du, r, nu);
				const double f = mat::dot(du, Fr, nu);
				return -e / f;
			}
		}
	}
}
