//std
#include <cmath>
#include <cstring>
#include <GL/gl.h>
#include <algorithm>

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Sections/Sections.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Section::Section(void) : m_label("Section")
			{
				return;
			}

			//destructor
			Section::~Section(void)
			{
				return;
			}

			//serialization
			void Section::load(FILE* file)
			{
				unsigned n_rebars;
				fscanf(file, "%s %d", m_label, &n_rebars);
				m_rebars.resize(n_rebars);
				for(Rebar& rebar : m_rebars)
				{
					rebar.load(file);
				}
			}
			void Section::save(FILE* file) const
			{
				fprintf(file, "%s %02d ", m_label, m_rebars.size());
				for(const Rebar& rebar : m_rebars)
				{
					rebar.save(file);
				}
			}

			//create
			Section* Section::create(Section*& section, const Section* base)
			{
				return Section::create(section, base->type(), base);
			}
			Section* Section::create(Section*& section, sections::type type, const Section* base)
			{
				//delete
				delete section;
				//create
				switch(type)
				{
					case sections::type::generic:
						return section = base ? new Generic(*(Generic*) base) : new Generic;
					case sections::type::rectangle:
						return section = base ? new Rectangle(*(Rectangle*) base) : new Rectangle;
					case sections::type::circle:
						return section = base ? new Circle(*(Circle*) base) : new Circle;
					case sections::type::box:
						return section = base ? new Box(*(Box*) base) : new Box;
					case sections::type::ring:
						return section = base ? new Ring(*(Ring*) base) : new Ring;
					case sections::type::T:
						return section = base ? new T(*(T*) base) : new T;
					case sections::type::I:
						return section = base ? new I(*(I*) base) : new I;
					default:
						return section = nullptr;
				}
			}
			
			//name
			const char* Section::name(void) const
			{
				return name(type());
			}
			const char* Section::name(sections::type type)
			{
				switch(type)
				{
					case sections::type::generic:
						return "Generic";
					case sections::type::rectangle:
						return "Rectangle";
					case sections::type::circle:
						return "Circle";
					case sections::type::box:
						return "Box";
					case sections::type::ring:
						return "Ring";
					case sections::type::T:
						return "T";
					case sections::type::I:
						return "I";
					default:
						return "";
				}
			}
			
			//data
			Mesh* Section::mesh(void)
			{
				return m_mesh;
			}
			
			Rebar& Section::rebar(unsigned index)
			{
				return m_rebars[index];
			}
			
			const char* Section::label(void) const
			{
				return m_label;
			}
			const char* Section::label(const char* label)
			{
				return (const char*) strcpy(m_label, label);
			}
			
			//add
			unsigned Section::rebars(void) const
			{
				return m_rebars.size();
			}
			void Section::remove_rebar(unsigned index)
			{
				m_rebars.erase(m_rebars.begin() + index);
			}
			Rebar& Section::add_rebar(double d, double z, double y)
			{
				m_rebars.push_back(Rebar(d, z, y));
				return m_rebars.back();
			}

			//index
			unsigned Section::index(void) const
			{
				return distance(m_mesh->m_sections.begin(), find(m_mesh->m_sections.begin(), m_mesh->m_sections.end(), this));
			}

			//analysis
			bool Section::check(void) const
			{
				return true;
			}

			//geometry
			double Section::warping(double, double) const
			{
				return 0;
			}
			double Section::warping_y(double, double) const
			{
				return 0;
			}
			double Section::warping_z(double, double) const
			{
				return 0;	
			}
			double Section::inercia_x(void) const
			{
				const double Iy = inercia_y();
				const double Iz = inercia_z();
				const double Iw = inercia_w();
				return Iy + Iz - Iw;
			}
			double Section::elastic_modulus_z(void) const
			{
				const double h = height();
				const double y = centroid_y();
				const double Iz = inercia_z();
				const double d = fmax(y, h - y);
				return Iz / d;
			}
			double Section::elastic_modulus_y(void) const
			{
				const double w = width();
				const double z = centroid_z();
				const double Iy = inercia_y();
				const double d = fmax(z, w - z);
				return Iy / d;
			}
			double Section::radius_of_gyration_z(void) const
			{
				const double A = area();
				const double Iz = inercia_z();
				return sqrt(Iz / A);
			}
			double Section::radius_of_gyration_y(void) const
			{
				const double A = area();
				const double Iy = inercia_y();
				return sqrt(Iy / A);
			}
			
			//draw
			void Section::draw_rebars(void) const
			{
				for(const Rebar& rebar : m_rebars)
				{
					rebar.draw();
				}
			}

			//static attributes
			Mesh* Section::m_mesh = nullptr;
		}
	}
}
