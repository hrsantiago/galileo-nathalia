#pragma once

#include "Mesh/Elements/Mechanic/Frame/Frame.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			class BeamT : public Frame
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				BeamT(void);

				//destructor
				virtual ~BeamT(void) override;
				
				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				const double* orientation(void) const;
				const double* orientation(const double*);
				const double* orientation(double, double, double);

				//types
				virtual unsigned cells(void) const override;
				virtual unsigned loads(void) const override;
				virtual unsigned states(void) const override;
				virtual unsigned stresses(void) const override;
				virtual unsigned dofs(unsigned) const override;
				virtual elements::type type(void) const override;

			protected:
				//analysis
				virtual bool symmetric(void) const override;

				virtual void apply(void) override;
				
				virtual bool check(void) const override;
				virtual void prepare(void) override;

				virtual void record(void) override;

				//formulation
				virtual double* inertial_force(double*) const override;
				virtual double* internal_force(double*) const override;
				virtual double* reference_force(double*, const boundary::loads::Element*) const override;
				
				virtual double* inertia(double*) const override;
				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;
				
				//attributes
				double m_f[6];
				double m_k[36];
				double m_orientation[3];
			};
		}
	}
}
