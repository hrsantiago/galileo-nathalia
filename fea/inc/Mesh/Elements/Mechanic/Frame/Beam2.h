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
			class Bending;
		}
	}
	
	namespace mesh
	{
		namespace elements
		{
			class Beam2 : public Frame
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Beam2(void);

				//destructor
				virtual ~Beam2(void) override;

			public:
				//data
				static bool shear(void);
				static bool shear(bool);
				static bool mixed(void);
				static bool mixed(bool);
				
				double residual_force(void) const;
				double residual_force(double);
				double residual_moment(bool) const;
				double residual_moment(bool, double);

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

				virtual void record(void) override;
				virtual void update(void) override;
				virtual void restore(void) override;

				//formulation
				virtual double* inertial_force(double*) const override;
				virtual double* internal_force(double*) const override;
				virtual double* reference_force(double*, const boundary::loads::Element*) const override;
				
				virtual double* load_axial(double*, const boundary::loads::Axial*) const;
				virtual double* load_shear(double*, const boundary::loads::Shear*) const;
				virtual double* load_bending(double*, const boundary::loads::Bending*) const;

				virtual double* inertia(double*) const override;
				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;
				
				virtual void local_mixed(const double*, double, double, double);
				virtual void local_fiber(const double*, double, double, double, double, double);
				virtual void local_elastic(const double*, double, double, double, double, double);

				//attributes
				static bool m_shear;
				static bool m_mixed;

				double m_nr;
				double m_mr;
				double m_ma;
				
				double m_f[3];
				double m_k[9];
				
				double m_tr_old;
				double m_tr_new;
			};
		}
	}
}
