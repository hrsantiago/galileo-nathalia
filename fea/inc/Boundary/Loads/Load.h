#pragma once

#include <functional>

namespace fea
{
	//declarations
	namespace boundary
	{
		class Boundary;
		class Load_Case;
	}

	namespace boundary
	{
		namespace loads
		{
			class Load
			{
				//friends
				friend class boundary::Load_Case;

			protected:
				//constructors
				Load(void);

				//destructor
				virtual ~Load(void);

				//serialization
				virtual void load(FILE*);
				virtual void save(FILE*) const;

			public:
				//data
				double value(void) const;
				double value(double);
				std::function<double(double)> function(void) const;
				std::function<double(double)> function(std::function<double(double)>);

			protected:
				//analysis
				virtual bool check(void) const = 0;
				
				//results
				virtual void plot(double, const double*, const double** = nullptr) const = 0;

				//attributes
				static Boundary* m_boundary;

				double m_value;
				std::function<double(double)> m_function;
			};
		}
	}
}
