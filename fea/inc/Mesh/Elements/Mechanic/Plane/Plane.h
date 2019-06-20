#pragma once

#include "Mesh/Elements/Mechanic/Mechanic.h"

namespace fea
{
	//declarations
	namespace boundary
	{
		namespace loads
		{
			class Plane_Edge;
			class Plane_Face;
		}
	}
	
	namespace mesh
	{
		namespace elements
		{
			class Plane : public Mechanic
			{
				//friends
				friend class Element;

			protected:
				//constructors
				Plane(void);

				//destructor
				virtual ~Plane(void) override;

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

				//strain
				void strain_gradient(double*, const double*, unsigned) const;

				//formulation
				virtual double* internal_force(double*) const override;
				virtual double* inertial_force(double*) const override;
				virtual double* reference_force(double*, const boundary::loads::Element*) const override;
				
				virtual double* load_edge(double*, const boundary::loads::Plane_Edge*) const;
				virtual double* load_face(double*, const boundary::loads::Plane_Face*) const;

				virtual double* inertia(double*) const override;
				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;
			};
		}
	}
}
