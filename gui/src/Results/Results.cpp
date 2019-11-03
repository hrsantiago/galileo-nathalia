//gui
#include "Results/Path.h"
#include "Results/Canvas.h"
#include "Results/Results.h"

namespace gui
{
	namespace results
	{
		//constructors
		Results::Results(const fea::models::Model* model) : m_path(new Path(model)), m_canvas(new Canvas(model, m_path)), m_status(false)
		{
			return;
		}

		//destructor
		Results::~Results(void)
		{
			delete m_path;
			delete m_canvas;
		}
		
		//data
		bool Results::status(void) const
		{
			return m_status;
		}
		const Path* Results::path(void) const
		{
			return m_path;
		}
		const Canvas* Results::canvas(void) const
		{
			return m_canvas;
		}
		
		//model
		void Results::model(const fea::models::Model* model)
		{
			m_status = false;
			m_path->model(model);
			m_canvas->model(model);
		}
		
		//data
		bool Results::read(void)
		{
			if(!m_status)
			{
				//path
				if(!m_path->read())
				{
					return false;
				}
				//canvas
				m_canvas->read();
				//status
				m_status = true;
			}
			return true;
		}
		void Results::clear(void)
		{
			if(m_status)
			{
				//clear
				m_path->clear();
				m_canvas->clear();
				//status
				m_status = false;
			}
		}
	}
}
