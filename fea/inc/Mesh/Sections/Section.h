#pragma once

//std
#include <vector>
#include <cstdio>

//fea
#include "Mesh/Sections/Rebar.h"

namespace fea
{
	//forward declarations
	namespace mesh
	{
		class Mesh;
		namespace points
		{
			class Fiber;
		}
		namespace sections
		{
			enum class type : unsigned;
		}
	}

	namespace mesh
	{
		namespace sections
		{
			class Section
			{
			public:
				//friends
				friend class mesh::Mesh;

			protected:
				//constructors
				Section(void);

				//destructor
				virtual ~Section(void);

				//serialization
				virtual void load(FILE*);
				virtual void save(FILE*) const;

				//create
				static Section* create(Section*&, const Section*);
				static Section* create(Section*&, sections::type, const Section* = nullptr);

			public:
				//type
				virtual sections::type type(void) const = 0;
				
				//name
				virtual const char* name(void) const;
				static const char* name(sections::type);
				
				//data
				static Mesh* mesh(void);
				
				virtual Rebar& rebar(unsigned);
				
				virtual const char* label(void) const;
				virtual const char* label(const char*);
				
				//rebars
				virtual unsigned rebars(void) const;
				virtual void remove_rebar(unsigned);
				virtual Rebar& add_rebar(double, double, double);

				//index
				unsigned index(void) const;

				//analysis
				virtual bool check(void) const;

				//geometry
				virtual double area(void) const = 0;
				virtual double width(void) const = 0;
				virtual double height(void) const = 0;
				virtual double warping(double, double) const;
				virtual double warping_y(double, double) const;
				virtual double warping_z(double, double) const;
				virtual double inercia_y(void) const = 0;
				virtual double inercia_z(void) const = 0;
				virtual double inercia_x(void) const;
				virtual double inercia_w(void) const = 0;
				virtual double centroid_y(void) const = 0;
				virtual double centroid_z(void) const = 0;
				virtual double elastic_modulus_y(void) const;
				virtual double elastic_modulus_z(void) const;
				virtual double plastic_modulus_y(void) const = 0;
				virtual double plastic_modulus_z(void) const = 0;
				virtual double shear_coefficient_y(double) const = 0;
				virtual double shear_coefficient_z(double) const = 0;
				virtual double radius_of_gyration_y(void) const;
				virtual double radius_of_gyration_z(void) const;

				//fibers
				virtual std::vector<points::Fiber> discretize(void) const = 0;
				
				//draw
				virtual void draw(void) const = 0; 
				virtual void draw_rebars(void) const;

			protected:
				//attributes
				static Mesh* m_mesh;

				char m_label[200];
				std::vector<Rebar> m_rebars;
			};
		}
	}
}
