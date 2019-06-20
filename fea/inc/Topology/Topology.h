#pragma once

#include <cstdio>
#include <vector>

namespace fea
{
	//declarations
	namespace models
	{
		class Model;
	}
	namespace topology
	{
		class Point;
		class Curve;
		class Surface;
	}

	//class
	namespace topology
	{
		class Topology
		{
			//friends
			friend class models::Model;

		protected:
			//constructor
			Topology(void);

			//destructor
			virtual ~Topology(void);

			//serialization
			void load(FILE*);
			void save(FILE*) const;

		public:
			//model
			models::Model* model(void) const;

			//data
			Point* point(unsigned) const;
			Curve* curve(unsigned) const;
			Surface* surface(unsigned) const;
			
			//size
			unsigned points(void);
			unsigned curves(void);
			unsigned surfaces(void);

			//add 
			Point* add_point(const double*);
			Point* add_point(double, double, double);
			Curve* add_curve(void);
			Surface* add_surface(void);

			//remove
			void remove_point(unsigned);
			void remove_curve(unsigned);
			void remove_surface(unsigned);

		public:
			//attributes
			static models::Model* m_model;

			std::vector<Point*> m_points;
			std::vector<Curve*> m_curves;
			std::vector<Surface*> m_surfaces;			
		};
	}
}
