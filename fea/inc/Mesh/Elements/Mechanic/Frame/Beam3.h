#pragma once

#include "Mesh/Elements/Mechanic/Frame/Frame.h"

namespace fea
{
	//declarations
	namespace boundary
	{
		namespace loads
		{
			class Axial;
			class Shear;
			class Torsion;
			class Bending;
		}
	}

	namespace mesh
	{
		namespace elements
		{
			class Beam3 : public Frame
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Beam3(void);

				//destructor
				virtual ~Beam3(void) override;
				
				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				static bool shear(void);
				static bool shear(bool);
				static bool mixed(void);
				static bool mixed(bool);
				
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
				virtual void local_fiber(const double*);
				virtual void local_mixed(const double*);
				virtual void local_elastic(const double*);
				
				virtual double* inertial_force(double*) const override;
				virtual double* internal_force(double*) const override;
				virtual double* reference_force(double*, const boundary::loads::Element*) const override;
				
				virtual double* load_axial(double*, const boundary::loads::Axial*) const;
				virtual double* load_shear(double*, const boundary::loads::Shear*) const;
				virtual double* load_torsion(double*, const boundary::loads::Torsion*) const;
				virtual double* load_bending(double*, const boundary::loads::Bending*) const;
				
				virtual double* inertia(double*) const override;
				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;
				
				//attributes
				static bool m_shear;
				static bool m_mixed;

				double m_orientation[3];
				
				double m_f[6];
				double m_k[36];
			};
		}
	}
}
