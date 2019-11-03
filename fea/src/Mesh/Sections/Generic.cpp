//fea
#include "Mesh/Sections/Types.h"
#include "Mesh/Sections/Generic.h"
#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Generic::Generic(void) :
			m_area(1.00e-2),
			m_width(1.00e-1), m_height(1.00e-1),
			m_inertia_x(1.42e-5), m_inertia_y(8.33e-6), m_inertia_z(8.33e-6),
			m_centroid_y(5.00e-2), m_centroid_z(5.00e-2),
			m_plastic_modulus_y(2.50e-4), m_plastic_modulus_z(2.50e-4),
			m_shear_coefficient_y(8.33e-1), m_shear_coefficient_z(8.33e-1),
			m_mesh_w(1), m_mesh_h(20)
			{
				return;
			}

			//destructor
			Generic::~Generic(void)
			{
				return;
			}

			//serialization
			void Generic::load(FILE* file)
			{
				Section::load(file);
				fscanf(file, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d ",
							&m_area,
							&m_width,
							&m_height,
							&m_inertia_x,
							&m_inertia_y,
							&m_inertia_z,
							&m_centroid_y,
							&m_centroid_z,
							&m_plastic_modulus_y,
							&m_plastic_modulus_z,
							&m_shear_coefficient_y,
							&m_shear_coefficient_z,
							&m_mesh_w,
							&m_mesh_h);
			}
			void Generic::save(FILE* file) const
			{
				Section::save(file);
				fprintf(file, "%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e %+.6e %+.6e %+.6e %+.6e %+.6e %+.6e %02d %02d ",
							 m_area,
							 m_width,
							 m_height,
							 m_inertia_x,
							 m_inertia_y,
							 m_inertia_z,
							 m_centroid_y,
							 m_centroid_z,
							 m_plastic_modulus_y,
							 m_plastic_modulus_z,
							 m_shear_coefficient_y,
							 m_shear_coefficient_z,
							 m_mesh_w,
							 m_mesh_h);
			}

			//type
			sections::type Generic::type(void) const
			{
				return sections::type::generic;
			}

			//data
			double Generic::area(double A)
			{
				return m_area = A;
			}
			double Generic::width(double w)
			{
				return m_width = w;
			}
			double Generic::height(double h)
			{
				return m_height = h;
			}
			double Generic::centroid_y(double cy)
			{
				return m_centroid_y = cy;
			}
			double Generic::centroid_z(double cz)
			{
				return m_centroid_z = cz;
			}
			double Generic::inertia_x(double Ix)
			{
				return m_inertia_x = Ix;
			}
			double Generic::inertia_y(double Iy)
			{
				return m_inertia_y = Iy;
			}
			double Generic::inertia_z(double Iz)
			{
				return m_inertia_z = Iz;
			}
			double Generic::plastic_modulus_y(double Wy)
			{
				return m_plastic_modulus_y = Wy;
			}
			double Generic::plastic_modulus_z(double Wz)
			{
				return m_plastic_modulus_z = Wz;
			}
			double Generic::shear_coefficient_y(double ky)
			{
				return m_shear_coefficient_y = ky;
			}
			double Generic::shear_coefficient_z(double kz)
			{
				return m_shear_coefficient_z = kz;
			}
			
			unsigned Generic::mesh_w(unsigned mesh)
			{
				return m_mesh_w = mesh;
			}
			unsigned Generic::mesh_h(unsigned mesh)
			{
				return m_mesh_h = mesh;
			}

			//geometry
			double Generic::area(void) const
			{
				return m_area;
			}
			double Generic::width(void) const
			{
				return m_width;
			}
			double Generic::height(void) const
			{
				return m_height;
			}
			double Generic::inertia_x(void) const
			{
				return m_inertia_x;
			}
			double Generic::inertia_y(void) const
			{
				return m_inertia_y;
			}
			double Generic::inertia_z(void) const
			{
				return m_inertia_z;
			}
			double Generic::inertia_w(void) const
			{
				return m_inertia_y + m_inertia_z - m_inertia_x;
			}
			double Generic::centroid_z(void) const
			{
				return m_centroid_y;
			}
			double Generic::centroid_y(void) const
			{
				return m_centroid_z;
			}
			double Generic::plastic_modulus_y(void) const
			{
				return m_plastic_modulus_y;
			}
			double Generic::plastic_modulus_z(void) const
			{
				return m_plastic_modulus_z;
			}
			double Generic::shear_coefficient_y(double) const
			{
				return m_shear_coefficient_y;
			}
			double Generic::shear_coefficient_z(double) const
			{
				return m_shear_coefficient_z;
			}

			//fibers
			std::vector<points::Fiber> Generic::discretize(void) const
			{
				const unsigned nw = m_mesh_w;
				const unsigned nh = m_mesh_h;
				const double zc = m_width / 2;
				const double yc = m_height / 2;
				const double dw = m_width / nw;
				const double dh = m_height / nh;
				std::vector<points::Fiber> fibers(nw * nh);
				for(unsigned i = 0; i < nh; i++)
				{
					for(unsigned j = 0; j < nw; j++)
					{
						fibers[i * nw + j].m_area = dw * dh;
						fibers[i * nw + j].m_position_z = dw * j + dw / 2 - zc;
						fibers[i * nw + j].m_position_y = dh * i + dh / 2 - yc;
					}
				}
				return fibers;
			}
			
			//draw
			void Generic::draw(void) const
			{
				
			}
		}
	}
}
