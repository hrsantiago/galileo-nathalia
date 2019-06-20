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
#include "Analysis/Strategies/Arc_Length_Spheric.h"

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			//constructors
			Arc_Length_Spheric::Arc_Length_Spheric(void)
			{
				return;
			}

			//destructor
			Arc_Length_Spheric::~Arc_Length_Spheric(void)
			{
				return;
			}

			//type
			strategies::type Arc_Length_Spheric::type(void) const
			{
				return strategies::type::arc_length_spheric;
			}

			//analysis
			double Arc_Length_Spheric::predictor(void) const
			{
				//analysis
				const solvers::Solver* solver = m_analysis->solver();
				const unsigned nu = m_analysis->assembler()->dof_unknow();
				//data
				const double dl = solver->load_increment();
				const double* du = solver->state_increment();
				const double* Fr = solver->reference_force();
				const double* dut = solver->state_predictor();
				//return
				const double d = mat::dot(du, du, nu);
				const double f = mat::dot(Fr, Fr, nu);
				const double s = mat::dot(du, dut, nu);
				const double t = mat::dot(dut, dut, nu);
				return mat::sign(s) * sqrt(d + pow(dl, 2) * f) / sqrt(t + f);
			}
			double Arc_Length_Spheric::corrector(void) const
			{
				//analysis
				const solvers::Solver* solver = m_analysis->solver();
				const unsigned nu = m_analysis->assembler()->dof_unknow();
				//data
				double a = 0, b = 0, c = 0, s = 0;
				const double dl = solver->load_increment();
				const double* du = solver->state_increment();
				const double* ddur = solver->state_corrector_residue();
				const double* ddut = solver->state_corrector_tangent();
				//return
				for(unsigned i = 0; i < nu; i++)
				{
					s += ddut[i] * du[i];
					a += ddut[i] * ddut[i];
					b += ddut[i] * (ddur[i] + du[i]);
					c += ddur[i] * (ddur[i] + 2 * du[i]);
				}
				return -b / a + mat::sign(s) * sqrt(pow(b / a, 2) - c / a);
			}
		}
	}
}
