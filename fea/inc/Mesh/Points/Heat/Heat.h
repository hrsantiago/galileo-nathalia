#pragma once

#include "Mesh/Points/Point.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			class Heat : public Point
			{
			public:
				//constructors
				Heat(void);

				//destructor
				~Heat(void);

				//type
				virtual points::type type(void) const override;

				//analysis
				virtual void prepare(const elements::Element*) override;
				
				virtual void update(void) override;
				virtual void restore(void) override;
			};
		}
	}
}
