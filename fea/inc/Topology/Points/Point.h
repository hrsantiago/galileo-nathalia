#pragma once

namespace fea
{
	namespace topology
	{
		//declarations
		class Topology;

		//class

		class Point
		{
			//friends
			friend class Topology;

		protected:
			//constructors
			Point(void);

			//destructor
			~Point(void);

			//data
			const double* coordinates(void) const;
			const double* coordinates(const double*);
			const double* coordinates(double, double, double);

			//index
			unsigned index(void) const;

		protected:
			//attributes
			static Topology* m_topology;

			double m_coordinates[3];
		};
	}
}
