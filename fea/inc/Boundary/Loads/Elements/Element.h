#pragma once

#include <vector>

#include "Boundary/Loads/Load.h"

namespace fea
{
	//forward declarations
	namespace boundary
	{
		class Boundary;
		class Load_Case;
		namespace loads
		{
			enum class type : unsigned;
		}
	}
	namespace analysis
	{
		class Assembler;
	}

	namespace boundary
	{
		namespace loads
		{
			class Element : public Load
			{
			public:
				//friends
				friend class boundary::Boundary;
				friend class boundary::Load_Case;
				friend class analysis::Assembler;

			protected:
				//constructors
				Element(void);

				//destructor
				virtual ~Element(void);

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

				//create
				static void create(loads::Element*&, loads::type);

			public:
				//type
				virtual loads::type type(void) const = 0;

				//add
				void add_element(unsigned);

				//remove
				void remove_element(unsigned);

				//index
				unsigned index(void) const;

			protected:
				//analysis
				bool check(void) const override;
				
				//results
				virtual void plot(double, const double*, const double** = nullptr) const override;

				//attributes
				std::vector<unsigned> m_elements;
			};
		}
	}
}
