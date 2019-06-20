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
		}
	}
	
	namespace mesh
	{
		namespace elements
		{
			class Bar : public Frame
			{
			public:
				//friends
				friend class Element;

			protected:
				//contructors
				Bar(void);

				//destructors
				virtual ~Bar(void) override;
				
				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//types
				virtual unsigned cells(void) const override;
				virtual unsigned loads(void) const override;
				virtual unsigned states(void) const override;
				virtual unsigned stresses(void) const override;
				virtual unsigned dofs(unsigned) const override;
				virtual elements::type type(void) const override;

				//data
				bool cable(void) const;
				bool cable(bool);
				
				static bool strain(void);
				static bool strain(bool);
				
				double residual_stress(void) const;
				double residual_stress(double);

			protected:
				//analysis
				virtual void apply(void) override;
				
				virtual void record(void) override;

				//formulation
				virtual double* internal_force(double*) const override;
				virtual double* inertial_force(double*) const override;

				virtual double* reference_force(double*, const boundary::loads::Element*) const override;
				
				virtual double* load_axial(double*, const boundary::loads::Axial*) const;

				virtual double* inertia(double*) const override;
				virtual double* damping(double*) const override;
				virtual double* stiffness(double*) const override;

				//attributes
				bool m_cable;
				static bool m_strain;
				
				double m_f;
				double m_k;
				double m_sr;
			};
		}
	}
}
