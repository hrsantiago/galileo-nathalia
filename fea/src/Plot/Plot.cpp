#include "Plot/Plot.h"
#include "Plot/What.h"
#include "Plot/View.h"
#include "Plot/Sizes.h"
#include "Plot/Colors.h"
#include "Plot/Gradient.h"

namespace fea
{
	namespace plot
	{
		//constructors
		Plot::Plot(void) : m_what(new What), m_view(new View), m_sizes(new Sizes), m_colors(new Colors), m_gradient(new Gradient)
		{
			return;
		}
		
		//destructor
		Plot::~Plot(void)
		{
			delete m_what;
			delete m_view;
			delete m_sizes;
			delete m_colors;
			delete m_gradient;
		}
		
		//serialization
		void Plot::load(FILE* file)
		{
			m_what->load(file);
			m_view->load(file);
			m_sizes->load(file);
			m_colors->load(file);
			m_gradient->load(file);
		}
		void Plot::save(FILE* file) const
		{
			m_what->save(file);
			m_view->save(file);
			m_sizes->save(file);
			m_colors->save(file);
			m_gradient->save(file);
		}
		
		//data
		What* Plot::what(void)
		{
			return m_what;
		}
		View* Plot::view(void)
		{
			return m_view;
		}
		Sizes* Plot::sizes(void)
		{
			return m_sizes;
		}
		Colors* Plot::colors(void)
		{
			return m_colors;
		}
		Gradient* Plot::gradient(void)
		{
			return m_gradient;
		}
		
		//info
		void Plot::info(void) const
		{
			printf("**********************************   Plot   ***********************************\n");
			m_what->info();
			m_view->info();
			m_sizes->info();
			m_colors->info();
			m_gradient->info();
		}
		
		//config
		void Plot::reset(void)
		{
			m_what->reset();
			m_view->reset();
			m_sizes->reset();
			m_colors->reset();
			m_gradient->reset();
		}
	}
}
