#pragma once

#include "Mesh/Sections/Section.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{

			class I : public Section
			{
			public:
				//friends
				friend class Section;

			protected:
				//constructors
				I(void);

				//destructor
				virtual ~I(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//type
				virtual sections::type type(void) const override;
				
				//data
				double web_height(void) const;
				double web_height(double);
				double web_thickness(void) const;
				double web_thickness(double);
				double flange_width(void) const;
				double flange_width(double);
				double flange_thickness(void) const;
				double flange_thickness(double);

				unsigned mesh_fw(void) const;
				unsigned mesh_fw(unsigned);
				unsigned mesh_ft(void) const;
				unsigned mesh_ft(unsigned);
				unsigned mesh_wh(void) const;
				unsigned mesh_wh(unsigned);
				unsigned mesh_wt(void) const;
				unsigned mesh_wt(unsigned);

				//geometry
				virtual double area(void) const override;
				virtual double width(void) const override;
				virtual double height(void) const override;
				virtual double inercia_y(void) const override;
				virtual double inercia_z(void) const override;
				virtual double inercia_w(void) const override;
				virtual double centroid_y(void) const override;
				virtual double centroid_z(void) const override;
				virtual double plastic_modulus_y(void) const override;
				virtual double plastic_modulus_z(void) const override;
				virtual double shear_coefficient_y(double) const override;
				virtual double shear_coefficient_z(double) const override;

				//fibers
				virtual std::vector<points::Fiber> discretize(void) const override;
				
				//draw
				virtual void draw(void) const override;

			protected:
				//attributes
				double m_web_height;
				double m_web_thickness;
				double m_flange_width;
				double m_flange_thickness;
				
				unsigned m_mesh_fw;
				unsigned m_mesh_ft;
				unsigned m_mesh_wh;
				unsigned m_mesh_wt;
			};
		}
	}
}
