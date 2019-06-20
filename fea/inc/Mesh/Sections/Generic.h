#pragma once

#include "Mesh/Sections/Section.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{

			class Generic : public Section
			{
			public:
				//friends
				friend class Section;

			protected:
				//constructors
				Generic(void);

				//destructor
				virtual ~Generic(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//type
				virtual sections::type type(void) const override;
				
				//data
				double area(double);
				double width(double);
				double height(double);
				double centroid_y(double);
				double centroid_z(double);
				double inercia_x(double);
				double inercia_y(double);
				double inercia_z(double);
				double plastic_modulus_y(double);
				double plastic_modulus_z(double);
				double shear_coefficient_y(double);
				double shear_coefficient_z(double);

				unsigned mesh_w(unsigned);
				unsigned mesh_h(unsigned);

				//geometry
				virtual double area(void) const override;
				virtual double width(void) const override;
				virtual double height(void) const override;
				virtual double inercia_x(void) const override;
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
				double m_area;
				double m_width;
				double m_height;
				double m_inercia_x;
				double m_inercia_y;
				double m_inercia_z;
				double m_centroid_y;
				double m_centroid_z;
				double m_plastic_modulus_y;
				double m_plastic_modulus_z;
				double m_shear_coefficient_y;
				double m_shear_coefficient_z;

				unsigned m_mesh_w;
				unsigned m_mesh_h;
			};
		}
	}
}
