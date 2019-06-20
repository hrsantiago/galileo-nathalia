//std
#include <cmath>

//mat
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Assembler/Assembler.h"
#include "Analysis/Strategies/Arc_Length_Linear.h"

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			//constructors
			Arc_Length_Linear::Arc_Length_Linear(void)
			{
				return;
			}

			//destructor
			Arc_Length_Linear::~Arc_Length_Linear(void)
			{
				return;
			}

			//type
			strategies::type Arc_Length_Linear::type(void) const
			{
				return strategies::type::arc_length_linear;
			}

			//analysis
			double Arc_Length_Linear::predictor(void) const
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
			double Arc_Length_Linear::corrector(void) const
			{
				//analysis
				const solvers::Solver* solver = m_analysis->solver();
				const unsigned nu = m_analysis->assembler()->dof_unknow();
				//data
				const double* du = solver->state_increment();
				const double* ddur = solver->state_corrector_residue();
				const double* ddut = solver->state_corrector_tangent();
				//return
				const double t = mat::dot(du, ddut, nu);
				const double r = mat::dot(du, ddur, nu);
				return -r / t;
			}
		}
	}
}
