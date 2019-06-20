#pragma once

#include "Mesh/Joints/Joint.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			class Spring : public Joint
			{
				//friends
				friend class Joint;

			protected:
				//constructors
				Spring(void);

				//destructor
				~Spring(void);

			public:
				//types
				virtual unsigned states(void) const override;
				virtual joints::type type(void) const override;
				
				//dofs
				virtual std::vector<unsigned> dofs(void) const override;

				//data
				double damping(void) const;
				double damping(double);
				double stiffness(void) const;
				double stiffness(double);
				const double* axis(void) const;
				const double* axis(const double*);
				const double* axis(double, double);
				const double* axis(double, double, double);

			protected:
				//analysis
				virtual void apply(void) override;
				
				//formulation
				virtual double* internal_force(double*) const override;

				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;
				
				//results
				virtual void plot(double, const double*, const double** = nullptr) const override;

				//attributes
				double m_axis[3];
				double m_damping;
				double m_stiffness;
			};
		}
	}
}

