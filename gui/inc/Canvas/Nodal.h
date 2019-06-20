#pragma once

#include "Canvas/Model.h"

namespace gui
{
	//forward declarations
	namespace results
	{
		class Nodal;
	}
	
	namespace canvas
	{
		class Nodal : public Model
		{
			Q_OBJECT
		
			//friends
			friend class results::Nodal;
		
			public:
				//construtors
				Nodal(QWidget* = nullptr);
		
				//destructor
				virtual ~Nodal(void);

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
				unsigned m_dofs;
				unsigned m_steps;
				
				unsigned* m_step[2];
				unsigned* m_node[2];
				double* m_limits[2];
				double* m_parameter;
				double*** m_results;
				double*** m_positions;
				
				std::vector<unsigned> m_paths;
				std::vector<unsigned> m_plots;
		};
	}
}
