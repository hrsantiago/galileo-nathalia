#pragma once

namespace fea
{
	namespace topology
	{
		//declarations
		class Topology;

		//class
		class Surface
		{
			//friends
			friend class Topology;

		public:
			//constructors
			Surface(void);

			//destructor
			~Surface(void);

			//index
			unsigned index(void) const;

		protected:
			//attributes
			static Topology* m_topology;
		};
	}
}
