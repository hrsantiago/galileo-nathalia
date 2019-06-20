#pragma once

#include <cstdio>

namespace fea
{
	//declarations
	namespace mesh
	{
		namespace nodes
		{
			enum class dof : unsigned;
		}
	}
	
	//struct
	namespace analysis
	{
		namespace solvers
		{
			struct Watch_Dof
			{
				//constructors
				Watch_Dof(void);

				//destructor
				virtual ~Watch_Dof(void);
				
				//serialization
				void load(FILE*);
				void save(FILE*) const;
				
				//attributes
				unsigned m_node;
				unsigned m_dof_index;
				mesh::nodes::dof m_dof;
			};
		}
	}
}
