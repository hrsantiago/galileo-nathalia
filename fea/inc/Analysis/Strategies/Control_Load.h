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
			
			class Control_Load : public Strategy
			{
			public:
				//constructors
				Control_Load(void);

				//destructor
				virtual ~Control_Load(void);

				//type
				virtual strategies::type type(void) const override;

				//analysis
				virtual double predictor(void) const override;
				virtual double corrector(void) const override;
			};
		}
	}
}
