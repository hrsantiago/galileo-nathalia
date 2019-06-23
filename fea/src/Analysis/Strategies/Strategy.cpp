//fea
#include "Analysis/Strategies/Types.h"
#include "Analysis/Strategies/Strategies.h"

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			//constructors
			Strategy::Strategy(void)
			{
				return;
			}

			//destructor
			Strategy::~Strategy(void)
			{
				return;
			}

			//create
			void Strategy::create(Strategy*& strategy, strategies::type type)
			{
				//delete
				delete strategy;
				//create
				switch(type)
				{
					case strategies::type::control_load:
						strategy = new Control_Load;
						break;
					case strategies::type::control_state:
						strategy = new Control_State;
						break;
					case strategies::type::arc_length_linear:
						strategy = new Arc_Length_Linear;
						break;
					case strategies::type::arc_length_spheric:
						strategy = new Arc_Length_Spheric;
						break;
					case strategies::type::arc_length_cylindric:
						strategy = new Arc_Length_Cylindric;
						break;
					case strategies::type::minimal_norm:
						strategy = new Minimal_Norm;
						break;
					case strategies::type::orthogonal_residue:
						strategy = new Orthogonal_Residue;
						break;
					default:
						strategy = nullptr;
				}
			}

			//name
			const char* Strategy::name(void) const
			{
				switch(type())
				{
					case strategies::type::control_load:
						return "control load";
					case strategies::type::control_state:
						return "control state";
					case strategies::type::arc_length_linear:
						return "arc length linear";
					case strategies::type::arc_length_spheric:
						return "arc length spheric";
					case strategies::type::arc_length_cylindric:
						return "arc length cylindric";
					case strategies::type::minimal_norm:
						return "minimal norm";
					case strategies::type::orthogonal_residue:
						return "orthogonal residue";
					default:
						return "";
				}
			}

			//static attributes
			Analysis* Strategy::m_analysis = nullptr;
		}
	}
}
