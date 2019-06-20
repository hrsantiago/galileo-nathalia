#pragma once

#include "Canvas/Model.h"

namespace gui
{
	//forward declarations
	namespace results
	{
		class Elements;
	}
	
	namespace canvas
	{
		class Elements : public Model
		{
			Q_OBJECT
		
			//friends
			friend class results::Elements;
		
			public:
				//construtors
				Elements(QWidget* = nullptr);
		
				//destructor
				virtual ~Elements(void);

			protected:
				//draw
				virtual void paintGL(void) override;
				
				//data
				virtual void allocate(void);
				virtual void get_data(void);
				
				//update
				virtual void update_model(fea::models::Model*) override;
				
				//events
				virtual void mousePressEvent(QMouseEvent*) override;
			
			private:
				//attributes
				unsigned m_steps;
				unsigned m_states;
				
				double* m_limits[2];
				double* m_parameter;
				double**** m_results;
				double*** m_positions;
				
				unsigned* m_step[2];
				unsigned* m_node[2];
				unsigned* m_element[2];
				
				std::vector<unsigned> m_paths;
				std::vector<unsigned> m_plots;
		};
	}
}
