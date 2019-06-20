#pragma once

#include "Mesh/Joints/Joint.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			class Spherical : public Joint
			{
			public:
				//friends
				friend class Joint;

			protected:
				//constructors
				Spherical(void);

				//destructor
				virtual ~Spherical(void);
				
				//serialization
				virtual void load(FILE*);
				virtual void save(FILE*) const;

			public:
				//data
				double damping(void) const;
				double damping(double);
				double stiffness(void) const;
				double stiffness(double);
				
				//types
				virtual unsigned states(void) const override;
				virtual joints::type type(void) const override;

				//dofs
				virtual std::vector<unsigned> dofs(void) const override;

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
				double m_damping;
				double m_stiffness;
			};
		}
	}
}
