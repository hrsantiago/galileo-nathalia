#include <string.h>

#include "Topology/Topology.h"
#include "Topology/Points/Point.h"
#include "Topology/Curves/Curve.h"
#include "Topology/Surfaces/Surface.h"

namespace fea
{
	namespace topology
	{
		//constructors
		Topology::Topology(void)
		{
			Curve::m_topology = this;
			Point::m_topology = this;
		}

		//destructor
		Topology::~Topology(void)
		{
			//delete Curves
			for(unsigned i = 0; i < m_curves.size(); i++)
			{
				delete m_curves[i];
			}
			//delete points
			for(unsigned i = 0; i < m_points.size(); i++)
			{
				delete m_points[i];
			}
		}

		//serialization
		void Topology::load(FILE*)
		{
			return;
		}
		void Topology::save(FILE*) const
		{
			return;
		}

		//models
		models::Model* Topology::model(void) const
		{
			return m_model;
		}

		//data
		Point* Topology::point(unsigned index) const
		{
			return m_points[index];
		}
		Curve* Topology::curve(unsigned index) const
		{
			return m_curves[index];
		}
		Surface* Topology::surface(unsigned index) const
		{
			return m_surfaces[index];
		}

		//size
		unsigned Topology::points(void)
		{
			return (unsigned) m_points.size();
		}
		unsigned Topology::curves(void)
		{
			return (unsigned) m_curves.size();
		}
		unsigned Topology::surfaces(void)
		{
			return (unsigned) m_surfaces.size();
		}

		//add
		Point* Topology::add_point(const double* p)
		{
			Point* point = new Point;
			memcpy(point->m_coordinates, p, 3 * sizeof(double));
			m_points.push_back(point);
			return point;
		}
		Point* Topology::add_point(double x, double y, double z)
		{
			Point* point = new Point;
			point->m_coordinates[0] = x;
			point->m_coordinates[1] = y;
			point->m_coordinates[2] = z;
			m_points.push_back(point);
			return point;
		}
		Curve* Topology::add_curve(void)
		{
			Curve* curve = new Curve;
			m_curves.push_back(curve);
			return curve;
		}
		Surface* Topology::add_surface(void)
		{
			Surface* surface = new Surface;
			m_surfaces.push_back(surface);
			return surface;
		}

		//remove
		void Topology::remove_point(unsigned index)
		{
			delete m_points[index];
			m_points.erase(m_points.begin() + index);
		}
		void Topology::remove_curve(unsigned index)
		{
			delete m_curves[index];
			m_curves.erase(m_curves.begin() + index);
		}
		void Topology::remove_surface(unsigned index)
		{
			delete m_surfaces[index];
			m_surfaces.erase(m_surfaces.begin() + index);
		}

		//attributes
		models::Model* Topology::m_model = nullptr;
	}
}
