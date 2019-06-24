#include "Util/Recent.h"

namespace gui
{
	namespace util
	{
		//constructors
		Recent::Recent(void)
		{
			return;
		}
			
		//destructor
		Recent::~Recent(void)
		{
			return;
		}
		
		//serialization
		void Recent::save(void)
		{
			FILE* file = fopen(".recent", "w");
			for(const std::string& path : m_list)
			{
				fprintf(file, "%s\n", path.c_str());
			}
			fclose(file);
		}
		void Recent::load(void)
		{
			char s[200];
			m_list.clear();
			m_list.reserve(m_n);
			FILE* file = fopen(".recent", "r");
			if(file)
			{
				while(m_list.size() < m_n && feof(file))
				{
					fgets(s, 200, file);
					m_list.push_back(s);
				}
				fclose(file);
			}
		}
		
		void Recent::update(QMenu* menu)
		{
			menu->clear();
			for(const std::string& path : m_list)
			{
				menu->addAction(path.c_str());
			}
		}
		
		void Recent::add(std::string path)
		{
			for(unsigned i = 0; i < m_list.size(); i++)
			{
				if(!m_list[i].compare(path))
				{
					m_list.erase(m_list.begin() + i);
					m_list.insert(m_list.begin(), path);
				}
			}
			if(m_list.size() == m_n)
			{
				m_list.pop_back();
			}
			m_list.insert(m_list.begin(), path);
			save();
		}
		void Recent::remove(unsigned index)
		{
			m_list.erase(m_list.begin() + index);
			save();
		}
	}
}
