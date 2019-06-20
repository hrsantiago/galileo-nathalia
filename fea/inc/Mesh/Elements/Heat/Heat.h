#pragma once

#include "Mesh/Elements/Element.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{

			class Heat : public Element
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Heat(void);

				//destructor
				virtual ~Heat(void) override;

			public:
				//types
				virtual unsigned cells(void) const override;
				virtual unsigned loads(void) const override;
				virtual unsigned states(void) const override;
				virtual unsigned dofs(unsigned) const override;
				virtual points::type point(void) const override;
				virtual elements::type type(void) const override;

			protected:
				//analysis
				virtual void apply(void) override;

				//formulation
				virtual double* internal_force(double*) const override;
				virtual double* inertial_force(double*) const override;

				virtual double* reference_force(double*, const boundary::loads::Element*) const override;

				virtual double* inertia(double*) const override;
				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;
			};
		}
	}
}
