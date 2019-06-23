#pragma once

#include "Mesh/Cells/Cell.h"

namespace fea
{
	//declarations
	namespace mesh
	{
		namespace sections
		{
			class Section;
		}
	}

	namespace mesh
	{
		namespace cells
		{
			class Line : public Cell
			{
			protected:
				//constructors
				Line(void);

				//destructor
				virtual ~Line(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				sections::Section* section(void) const;
				sections::Section* section(unsigned);
				
				//index
				unsigned index_section(void) const;

				//topology
				virtual unsigned faces(void) const override;
				virtual unsigned edges(void) const override;
				virtual unsigned dimension(void) const override;
				
				virtual std::vector<unsigned> face(unsigned) const override;

				//parametrization
				virtual void edge(double*, unsigned, double) const override;
				virtual void face(double*, unsigned, double, double) const override;
				
				virtual void gradient(double*, unsigned, double) const override;
				virtual void gradient(double*, unsigned, double, double) const override;

				//geometry
				virtual double jacobian(double*, const elements::Element*, const double*) const override;
				
				//integration
				virtual unsigned points(void) const override;
				virtual double point(double*, unsigned) const override;

				//mesh
				static void refine(unsigned, unsigned);

			protected:
				//results
				virtual void plot(const elements::Element*, const double*, const double** = nullptr, const double* = nullptr, bool = false) const override;
				
				//attributes
				unsigned m_section;
			};
		}
	}
}
