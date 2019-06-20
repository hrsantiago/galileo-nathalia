#pragma once

#include "Mesh/Points/Point.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			//declarations
			class Fiber;
			
			//class
			class Section : public Point
			{
			public:
				//constructors
				Section(void);

				//destructor
				virtual ~Section(void) override;

				//type
				virtual points::type type(void) const override;

				//analysis
				virtual void prepare(const elements::Element*) override;
				
				virtual void update(void) override;
				virtual void restore(void) override;

				//attributes
				double m_strain_old[6];
				double m_strain_new[6];
				std::vector<Fiber> m_fibers;
			};
		}
	}
}
