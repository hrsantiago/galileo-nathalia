#pragma once

#include "Mesh/Joints/Joint.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			class Rigid : public Joint
			{
				//friends
				friend class Joint;

			protected:
				//constructors
				Rigid(void);

				//destructor
				virtual ~Rigid(void) override;

			public:
				//types
				virtual unsigned states(void) const override;
				virtual joints::type type(void) const override;
				
				//dofs
				virtual std::vector<unsigned> dofs(void) const override;

			protected:
				//analysis
				virtual void apply(void) override;
				
				virtual void configure(void) override;

				//formulation
				virtual double* internal_force(double*) const override;
				
				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;
				
				//results
				virtual void plot(double, const double*, const double** = nullptr) const override;
			};
		}
	}
}
