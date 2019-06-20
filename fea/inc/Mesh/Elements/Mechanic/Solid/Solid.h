#pragma once

#include "Mesh/Elements/Mechanic/Mechanic.h"

namespace fea
{
	//declarations
	namespace boundary
	{
		namespace loads
		{
			class Solid_Edge;
			class Solid_Face;
			class Solid_Body;
		}
	}
	
	namespace mesh
	{
		namespace elements
		{
			class Solid : public Mechanic
			{
				//friends
				friend class Element;

			protected:
				//constructors
				Solid(void);

				//destructor
				virtual ~Solid(void) override;

			public:
				//types
				virtual unsigned cells(void) const override;
				virtual unsigned loads(void) const override;
				virtual unsigned states(void) const override;
				virtual unsigned stresses(void) const override;
				virtual unsigned dofs(unsigned) const override;
				virtual elements::type type(void) const override;

			protected:
				//analysis
				virtual void apply(void) override;

				//formulation
				virtual double* internal_force(double*) const override;
				virtual double* inertial_force(double*) const override;

				virtual double* reference_force(double*, const boundary::loads::Element*) const override;
				virtual double* edge_force(double*, const boundary::loads::Solid_Edge*) const;
				virtual double* face_force(double*, const boundary::loads::Solid_Face*) const;
				virtual double* body_force(double*, const boundary::loads::Solid_Body*) const;

				virtual double* inertia(double*) const override;
				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;
			};
		}
	}
}
