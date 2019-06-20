#pragma once

namespace equations
{
	namespace tent
	{
		namespace dynamic
		{
			namespace vertical
			{
				void orbits(void);
				void initial(void);
				void numeric(void);
				void analytic(void);
				void frequency(void);
				void load_primary(void);
				void load_initial(void);
				void load_secondary(void);
			}
			namespace horizontal
			{
				void frequency(void);
				void load_primary(void);
				void load_secondary(void);
			}
		}
	}
}
