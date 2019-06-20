#pragma once

#include "Analysis/Strategies/Strategy.h"

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			//declarations
			enum class type : unsigned;
			
			class Orthogonal_Residue : public Strategy
			{
			public:
				//constructors
				Orthogonal_Residue(void);

				//destructor
				virtual ~Orthogonal_Residue(void) override;

				//type
				virtual strategies::type type(void) const override;

				//analysis
				virtual double predictor(void) const override;
				virtual double corrector(void) const override;
			};
		}
	}
}
