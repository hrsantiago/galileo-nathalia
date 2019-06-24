#pragma once

//std
#include <vector>
#include <string>

#include <QMenu>

namespace gui
{
	namespace util
	{
		class Recent 
		{ 
		public:
			//constructors
			Recent(void);
				
			//destructor
			virtual ~Recent(void);
			
			//serialization
			void save(void);
			void load(void);
			
			void update(QMenu*);
			
			void add(std::string);
			void remove(unsigned);

		protected:
			//attributes
			const static unsigned m_n = 15;
			std::vector<std::string> m_list;
		};
	}
}
