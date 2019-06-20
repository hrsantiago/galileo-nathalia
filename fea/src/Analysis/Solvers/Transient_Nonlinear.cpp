//fea
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Transient_Nonlinear.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//constructors
			Transient_Nonlinear::Transient_Nonlinear(void)
			{
				return;
			}

			//destructor
			Transient_Nonlinear::~Transient_Nonlinear(void)
			{
				return;
			}

			//serialization
			void Transient_Nonlinear::load(FILE* file)
			{
				Solver::load(file);
				Time::load(file);
				Nonlinear::load(file);
			}
			void Transient_Nonlinear::save(FILE* file) const
			{
				Solver::save(file);
				Time::save(file);
				Nonlinear::save(file);
			}

			//type
			solvers::type Transient_Nonlinear::type(void) const
			{
				return solvers::type::transient_nonlinear;
			}

			//sets
			unsigned Transient_Nonlinear::state_set(void) const
			{
				return 0;
			}
			unsigned Transient_Nonlinear::force_set(void) const
			{
				return 0;
			}
			unsigned Transient_Nonlinear::tangent_set(void) const
			{
				return 0;
			}

			//analysis
			void Transient_Nonlinear::prepare(void)
			{
				return;
			}
			
			void Transient_Nonlinear::run(void)
			{
				return;
			}
			
			void Transient_Nonlinear::record(void)
			{
				return;
			}
			void Transient_Nonlinear::finish(void) const
			{
				return;
			}
		}
	}
}
