#pragma once

#include "Boundary/Loads/Elements/Element.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Mechanic : public Element
			{
			protected:
				//constructors
				Mechanic(void);

				//destructors
				virtual ~Mechanic(void) override;
			};
		}
	}
}
