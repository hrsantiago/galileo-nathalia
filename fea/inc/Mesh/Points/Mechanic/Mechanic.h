#pragma once

#include "Mesh/Points/Point.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			//declarations
			enum class state : unsigned;
			
			class Mechanic : public Point
			{
			public:
				//constructors
				Mechanic(void);

				//destructor
				virtual ~Mechanic(void) override;

				//type
				virtual points::type type(void) const override;

				//analysis
				virtual void prepare(unsigned);
				virtual void prepare(const elements::Element*) override;
				
				virtual void update(void) override;
				virtual void restore(void) override;

				//attributes
				state m_state_old;
				state m_state_new;
				
				double m_hardening_old;
				double m_hardening_new;
				
				unsigned m_stress_types;
				
				double* m_plastic_strain_old;
				double* m_plastic_strain_new;
			};
		}
	}
}
