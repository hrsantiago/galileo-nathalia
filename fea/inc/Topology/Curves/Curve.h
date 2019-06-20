#pragma once

namespace fea
{
	namespace topology
	{
		//declarations
		class Topology;

		//class
		class Curve
		{
			//friends
			friend class Topology;

		public:
			//constructors
			Curve(void);

			//destructor
			~Curve(void);

			//index
			unsigned index(void) const;

		protected:
			//attributes
			static Topology* m_topology;
		};
	}
}
