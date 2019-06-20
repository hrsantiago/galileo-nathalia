//mat
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Assembler/Assembler.h"
#include "Analysis/Strategies/Minimal_Norm.h"

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			//constructors
			Minimal_Norm::Minimal_Norm(void)
			{
				return;
			}

			//destructor
			Minimal_Norm::~Minimal_Norm(void)
			{
				return;
			}

			//type
			strategies::type Minimal_Norm::type(void) const
			{
				return strategies::type::minimal_norm;
			}

			//analysis
			double Minimal_Norm::predictor(void) const
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
			double Minimal_Norm::corrector(void) const
			{
				//analysis
				const solvers::Solver* solver = m_analysis->solver();
				const unsigned nu = m_analysis->assembler()->dof_unknow();
				//data
				const double* ddur = solver->state_corrector_residue();
				const double* ddut = solver->state_corrector_tangent();
				//return
				const double r = mat::dot(ddut, ddur, nu);
				const double t = mat::dot(ddut, ddut, nu);
				return -r / t;
			}
		}
	}
}
