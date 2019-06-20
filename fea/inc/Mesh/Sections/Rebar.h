#pragma once

#include <cstdio>

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class Rebar
			{
			public:
				//constructors
				Rebar(double = 0, double = 0, double = 0);

				//destructor
				virtual ~Rebar(void);
			
				//serialization
				virtual void load(FILE*);
				virtual void save(FILE*) const;
				
				//draw
				virtual void draw(void) const;
				
				//data
				double diameter(void) const;
				double diameter(double);
				double position_z(void) const;
				double position_z(double);
				double position_y(void) const;
				double position_y(double);
				
			protected:
				//attributes
				double m_diameter;
				double m_position_z;
				double m_position_y;
			};
		}
	}
}
