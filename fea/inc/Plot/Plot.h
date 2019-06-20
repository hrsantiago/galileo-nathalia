#pragma once

#include <cstdio>

namespace fea
{
	namespace plot
	{
		class What;
		class View;
		class Sizes;
		class Colors;
		class Gradient;
		
		class Plot
		{
		public:
			//constructors
			Plot(void);
			
			//destructor
			virtual ~Plot(void);

			//serialization
			void load(FILE*);
			void save(FILE*) const;
		
			//data
			What* what(void);
			View* view(void);
			Sizes* sizes(void);
			Colors* colors(void);
			Gradient* gradient(void);
			
			//info
			void info(void) const;
			
			//config
			void reset(void);
		
		protected:
			//attributes
			What* m_what;
			View* m_view;
			Sizes* m_sizes;
			Colors* m_colors;
			Gradient* m_gradient;
		};
	}
}
