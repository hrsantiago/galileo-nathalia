#include "Boundary/Loads/Load.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Load::Load(void) : m_value(1), m_function([](double t) { return 1; })
			{
				return;
			}

			//destructor
			Load::~Load(void)
			{
				return;
			}

			//serialization
			void Load::load(FILE* file)
			{
				fscanf(file, "%lf", &m_value);
			}
			void Load::save(FILE* file) const
			{
				fprintf(file, "%+.6e ", m_value);
			}

			//data
			double Load::value(void) const
			{
				return m_value;
			}
			double Load::value(double value)
			{
				return m_value = value;
			}
			std::function<double(double)> Load::function(void) const
			{
				return m_function;
			}
			std::function<double(double)> Load::function(std::function<double(double)> function)
			{
				return m_function = function;
			}

			//static attributes
			Boundary* Load::m_boundary = nullptr;
		}
	}
}
