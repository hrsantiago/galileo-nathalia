//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Watch_Dof.h"
#include "Analysis/Strategies/Control_State.h"

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			//constructors
			Control_State::Control_State(void)
			{
				return;
			}

			//destructor
			Control_State::~Control_State(void)
			{
				return;
			}

			//type
			strategies::type Control_State::type(void) const
			{
				return strategies::type::control_state;
			}

			//analysis
			double Control_State::predictor(void) const
			{
				//analysis
				const solvers::Solver* solver = m_analysis->solver();
				//data
				const double* du = solver->state_increment();
				const double* dut = solver->state_predictor();
				//watch dof
				const unsigned j = m_analysis->solver()->watch_dof()->m_dof_index;
				//return
				return du[j] / dut[j];
			}
			double Control_State::corrector(void) const
			{
				//analysis
				const solvers::Solver* solver = m_analysis->solver();
				//data
				const double* ddur = solver->state_corrector_residue();
				const double* ddut = solver->state_corrector_tangent();
				//watch dof
				const unsigned j = m_analysis->solver()->watch_dof()->m_dof_index;
				//return
				return -ddur[j] / ddut[j];
			}
		}
	}
}
