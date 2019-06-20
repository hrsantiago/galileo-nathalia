//fea
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Transient_Linear.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//constructors
			Transient_Linear::Transient_Linear(void)
			{
				return;
			}

			//destructor
			Transient_Linear::~Transient_Linear(void)
			{
				return;
			}

			//type
			solvers::type Transient_Linear::type(void) const
			{
				return solvers::type::transient_linear;
			}

			//sets
			unsigned Transient_Linear::state_set(void) const
			{
				return (unsigned) solvers::state::u | (unsigned) solvers::state::v;
			}
			unsigned Transient_Linear::force_set(void) const
			{
				return (unsigned) solvers::force::Fe | (unsigned) solvers::force::R;
			}
			unsigned Transient_Linear::tangent_set(void) const
			{
				return (unsigned) solvers::tangent::K | (unsigned) solvers::tangent::C;
			}

			//analysis
			void Transient_Linear::prepare(void)
			{
				//base call
				Time::prepare();
			}
			
			void Transient_Linear::run(void)
			{
				return;
			}
			
			void Transient_Linear::record(void)
			{
				return;
			}
			void Transient_Linear::finish(void) const
			{
				return;
			}
		}
	}
}
