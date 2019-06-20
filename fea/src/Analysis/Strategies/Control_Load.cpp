//fea
#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Strategies/Control_Load.h"

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			//constructors
			Control_Load::Control_Load(void)
			{
				return;
			}

			//destructor
			Control_Load::~Control_Load(void)
			{
				return;
			}

			//type
			strategies::type Control_Load::type(void) const
			{
				return strategies::type::control_load;
			}

			//analysis
			double Control_Load::predictor(void) const
			{
				return m_analysis->solver()->load_predictor();
			}
			double Control_Load::corrector(void) const
			{
				return 0;
			}
		}
	}
}
