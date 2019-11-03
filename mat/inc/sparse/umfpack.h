//mat
#include "sparse/solver.h"

namespace mat
{
	namespace sparse
	{
		class umfpack : solver
		{
			//constructors
			umfpack(void);
			
			//destructor
			virtual ~umfpack(void);
			
			//solve
			virtual void clear(void) override;
			virtual bool solve(void) override;
			virtual bool decompose(void) override;
			virtual bool substitute(void) override;
		};
	}
}
