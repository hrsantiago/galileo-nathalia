#pragma once

#include <cstdio>

namespace fea
{
	namespace plot
	{
		class What
		{
		public:
			//constructors
			What(void);
			
			//destructor
			virtual ~What(void);
			
			//serialization
			void load(FILE*);
			void save(FILE*) const;
			
			//data
			bool axes(bool);
			bool scale(bool);
			bool nodes(bool);
			bool loads(bool);
			bool joints(bool);
			bool elements(bool);
			bool supports(bool);
			
			bool axes(void) const;
			bool scale(void) const;
			bool nodes(void) const;
			bool loads(void) const;
			bool joints(void) const;
			bool elements(void) const;
			bool supports(void) const;
			
			//info
			void info(void) const;
			
			//config
			void reset(void);
			
		private:
			//attributes
			bool m_axes = true;
			bool m_scale = true;
			bool m_nodes = true;
			bool m_loads = true;
			bool m_joints = true;
			bool m_elements = true;
			bool m_supports = true;
		};
	}
}
