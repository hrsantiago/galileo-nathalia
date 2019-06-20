#pragma once

#include "Mesh/Sections/Section.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{

			class Circle : public Section
			{
			public:
				//friends
				friend class Section;

			protected:
				//constructors
				Circle(void);

				//destructor
				virtual ~Circle(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//type
				virtual sections::type type(void) const override;
				
				//data
				double diameter(void) const;
				double diameter(double);

				unsigned mesh_t(void) const;
				unsigned mesh_t(unsigned);
				unsigned mesh_r(void) const;
				unsigned mesh_r(unsigned);

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
				double m_diameter;

				unsigned m_mesh_t;
				unsigned m_mesh_r;
			};
		}
	}
}
