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

			class Minimal_Norm : public Strategy
			{
			public:
				//constructors
				Minimal_Norm(void);

				//destructor
				virtual ~Minimal_Norm(void) override;

				//type
				virtual strategies::type type(void) const override;

				//analysis
				virtual double predictor(void) const override;
				virtual double corrector(void) const override;
			};
		}
	}
}
