//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Element.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

//gui
#include "Limit/Dof.h"
#include "Limit/State.h"

#include "Results/Path.h"
#include "Results/Canvas.h"

namespace gui
{
	namespace results
	{
		//constructors
		Canvas::Canvas(const fea::models::Model* model, const Path* path) : 
			m_path(path), m_model(model), m_dofs(0), m_steps(0), m_states(0),
			m_position(nullptr), m_element(nullptr), m_limit_state(nullptr)
		{
			m_node[0] = nullptr;
			m_node[1] = nullptr;
			m_node[2] = nullptr;
			m_limit_dof[0] = nullptr;
			m_limit_dof[1] = nullptr;
			m_limit_dof[2] = nullptr;
		}
		
		//destructor
		Canvas::~Canvas(void)
		{
			clear();
		}
		
		//data
		unsigned Canvas::dofs(void) const
		{
			return m_dofs;
		}
		unsigned Canvas::steps(void) const
		{
			return m_steps;
		}
		unsigned Canvas::states(void) const
		{
			return m_states;
		}
		
		const double**** Canvas::node(void) const
		{
			return (const double****) m_node;
		}
		const double**** Canvas::element(void) const
		{
			return (const double****) m_element;
		}
		const double*** Canvas::position(void) const
		{
			return (const double***) m_position;
		}
		
		const limit::State& Canvas::limit_state(unsigned i) const
		{
			return m_limit_state[i];
		}
		const limit::Dof& Canvas::limit_dof(unsigned i, unsigned j) const
		{
			return m_limit_dof[i][j];
		}
		
		void Canvas::model(const fea::models::Model* model)
		{
			m_model = model;
		}
		
		//data
		void Canvas::read(void)
		{
			//data
			dofs();
			states();
			m_steps = m_path->steps();
			//allocate
			allocate_node();
			allocate_element();
			allocate_position();
			//read
			read_node();
			read_element();
			read_position();
			//scale
			scale_node();
			scale_element();
		}
		void Canvas::clear(void)
		{
			//clear
			clear_node();
			clear_element();
			clear_position();
		}
		
		//setup
		void Canvas::dofs(void)
		{
			for(const fea::mesh::nodes::Node* node : m_model->mesh()->nodes())
			{
				m_dofs |= node->dof_types();
			}
		}
		void Canvas::states(void)
		{
			for(const fea::mesh::elements::Element* element : m_model->mesh()->elements())
			{
				m_states |= element->states();
			}
		}
		
		//read
		void Canvas::read_node(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nd = mat::bit_count(m_dofs);
			const unsigned nn = m_model->mesh()->nodes().size();
			//path
			const double**** path = m_path->node();
			//read
			for(unsigned i = 0; i < 3; i++)
			{
				if(m_node[i])
				{
					for(unsigned j = 0; j < ns; j++)
					{
						for(unsigned k = 0; k < nd; k++)
						{
							for(unsigned p = 0; p < nn; p++)
							{
								const unsigned dk = 1 << mat::bit_search(m_dofs, k);
								const unsigned dt = m_model->mesh()->node(p)->dof_types();
								m_node[i][j][k][p] = dk & dt ? path[i][p][mat::bit_index(dt, dk)][j] : 0;
							}
						}
					}
				}
			}
		}
		void Canvas::read_element(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nt = mat::bit_count(m_states);
			const unsigned ne = m_model->mesh()->elements().size();
			//path
			const double**** path = m_path->element();
			//read
			for(unsigned i = 0; i < ns; i++)
			{
				for(unsigned j = 0; j < nt; j++)
				{
					for(unsigned k = 0; k < ne; k++)
					{
						const unsigned sj = 1 << mat::bit_search(m_states, j);
						const unsigned st = m_model->mesh()->element(k)->states();
						for(unsigned p = 0; p < m_model->mesh()->element(k)->nodes().size(); p++)
						{
							m_element[i][j][k][p] = sj & st ? path[k][mat::bit_index(st, sj)][p][i] : 0;
						}
					}
				}
			}
		}
		void Canvas::read_position(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nt = mat::bit_count(m_dofs);
			const unsigned nn = m_model->mesh()->nodes().size();
			//dofs
			const unsigned dm = 
				(unsigned) fea::mesh::nodes::dof::translation_x |
				(unsigned) fea::mesh::nodes::dof::translation_y |
				(unsigned) fea::mesh::nodes::dof::translation_z;
			//path
			const double**** path = m_path->node();
			//read
			for(unsigned i = 0; i < ns; i++)
			{
				for(unsigned j = 0; j < nn; j++)
				{
					const double* x = m_model->mesh()->node(j)->coordinates();
					const unsigned dt = m_model->mesh()->node(j)->dof_types();
					for(unsigned k = 0; k < 3; k++)
					{
						m_position[i][j][k] = x[k] + (1 << k & dt ? path[0][j][mat::bit_index(dt, 1 << k)][i] : 0);
					}
				}
			}
		}
		
		//clear
		void Canvas::clear_node(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nd = mat::bit_count(m_dofs);
			//clear
			for(unsigned i = 0; i < 3; i++)
			{
				if(m_node[i])
				{
					for(unsigned j = 0; j < ns; j++)
					{
						for(unsigned k = 0; k < nd; k++)
						{
							delete[] m_node[i][j][k];
						}
						delete[] m_node[i][j];
					}
					delete[] m_node[i];
					m_node[i] = nullptr;
					delete[] m_limit_dof[i];
					m_limit_dof[i] = nullptr;
				}
			}
		}
		void Canvas::clear_element(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nr = mat::bit_count(m_states);
			const unsigned ne = m_model->mesh()->elements().size();
			//clear
			if(m_element)
			{
				for(unsigned i = 0; i < ns; i++)
				{
					for(unsigned j = 0; j < nr; j++)
					{
						for(unsigned k = 0; k < ne; k++)
						{
							delete[] m_element[i][j][k];
						}
						delete[] m_element[i][j];
					}
					delete[] m_element[i];
				}
				delete[] m_element;
				m_element = nullptr;
				delete[] m_limit_state;
				m_limit_state = nullptr;
			}
		}
		void Canvas::clear_position(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes().size();
			//clear
			if(m_position)
			{
				for(unsigned i = 0; i < ns; i++)
				{
					for(unsigned j = 0; j < nn; j++)
					{
						delete[] m_position[i][j];
					}
					delete[] m_position[i];
				}
				delete[] m_position;
				m_position = nullptr;
			}
		}
		
		//scale
		void Canvas::scale_node(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nd = mat::bit_count(m_dofs);
			const unsigned nn = m_model->mesh()->nodes().size();
			const unsigned ss = m_model->analysis()->solver()->state_set();
			//limits
			for(unsigned i = 0; i < 3; i++)
			{
				if(m_node[i])
				{
					for(unsigned j = 0; j < nd; j++)
					{
						//limits
						m_limit_dof[i][j].m_step[0] = 0;
						m_limit_dof[i][j].m_step[1] = 0;
						m_limit_dof[i][j].m_node[0] = 0;
						m_limit_dof[i][j].m_node[1] = 0;
						m_limit_dof[i][j].m_value[0] = m_node[i][0][j][0];
						m_limit_dof[i][j].m_value[1] = m_node[i][0][j][0];
						for(unsigned k = 0; k < ns; k++)
						{
							for(unsigned p = 0; p < nn; p++)
							{
								if(m_limit_dof[i][j].m_value[0] > m_node[i][k][j][p])
								{
									m_limit_dof[i][j].m_step[0] = k;
									m_limit_dof[i][j].m_node[0] = p;
									m_limit_dof[i][j].m_value[0] = m_node[i][k][j][p];
								}
								if(m_limit_dof[i][j].m_value[1] < m_node[i][k][j][p])
								{
									m_limit_dof[i][j].m_step[1] = k;
									m_limit_dof[i][j].m_node[1] = p;
									m_limit_dof[i][j].m_value[1] = m_node[i][k][j][p];
								}
							}
						}
					}
				}
			}
			//scale
			for(unsigned i = 0; i < 3; i++)
			{
				if(m_node[i])
				{
					for(unsigned j = 0; j < nd; j++)
					{
						//limits
						const double vi = m_limit_dof[i][j].m_value[0];
						const double vj = m_limit_dof[i][j].m_value[1];
						//scale
						for(unsigned k = 0; k < ns; k++)
						{
							for(unsigned p = 0; p < nn; p++)
							{
								m_node[i][k][j][p] = vi == vj ? 0.5 : (m_node[i][k][j][p] - vi) / (vj - vi);
							}
						}
					}
				}
			}
		}
		void Canvas::scale_element(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nt = mat::bit_count(m_states);
			const unsigned ne = m_model->mesh()->elements().size();
			//limits
			if(m_element)
			{
				for(unsigned i = 0; i < nt; i++)
				{
					//limits
					m_limit_state[i].m_step[0] = 0;
					m_limit_state[i].m_step[1] = 0;
					m_limit_state[i].m_node[0] = 0;
					m_limit_state[i].m_node[1] = 0;
					m_limit_state[i].m_element[0] = 0;
					m_limit_state[i].m_element[1] = 0;
					m_limit_state[i].m_value[0] = m_element[0][i][0][0];
					m_limit_state[i].m_value[1] = m_element[0][i][0][0];
					for(unsigned j = 0; j < ns; j++)
					{
						for(unsigned k = 0; k < ne; k++)
						{
							const unsigned nn = m_model->mesh()->element(k)->nodes().size();
							for(unsigned p = 0; p < nn; p++)
							{
								if(m_limit_state[i].m_value[0] > m_element[j][i][k][p])
								{
									m_limit_state[i].m_step[0] = j;
									m_limit_state[i].m_node[0] = p;
									m_limit_state[i].m_element[0] = k;
									m_limit_state[i].m_value[0] = m_element[j][i][k][p];
								}
								if(m_limit_state[i].m_value[1] < m_element[j][i][k][p])
								{
									m_limit_state[i].m_step[1] = j;
									m_limit_state[i].m_node[1] = p;
									m_limit_state[i].m_element[1] = k;
									m_limit_state[i].m_value[1] = m_element[j][i][k][p];
								}
							}
						}
					}
				}
			}
			//scale
			if(m_element)
			{
				for(unsigned i = 0; i < nt; i++)
				{
					//limits
					const double vi = m_limit_state[i].m_value[0];
					const double vj = m_limit_state[i].m_value[1];
					//scale
					for(unsigned j = 0; j < ns; j++)
					{
						for(unsigned k = 0; k < ne; k++)
						{
							const unsigned nn = m_model->mesh()->element(k)->nodes().size();
							for(unsigned p = 0; p < nn; p++)
							{
								m_element[j][i][k][p] = vi == vj ? 0.5 : (m_element[j][i][k][p] - vi) / (vj - vi);
							}
						}
					}
				}
			}
		}
		
		//allocate
		void Canvas::allocate_node(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nd = mat::bit_count(m_dofs);
			const unsigned nn = m_model->mesh()->nodes().size();
			const unsigned ss = m_model->analysis()->solver()->state_set();
			//allocate
			for(unsigned i = 0; i < 3; i++)
			{
				if(ss & 1 << i)
				{
					m_node[i] = new double**[ns];
					for(unsigned j = 0; j < ns; j++)
					{
						m_node[i][j] = new double*[nd];
						for(unsigned k = 0; k < nd; k++)
						{
							m_node[i][j][k] = new double[nn];
						}
					}
					m_limit_dof[i] = new limit::Dof[nd];
				}
			}
		}
		void Canvas::allocate_element(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nt = mat::bit_count(m_states);
			const unsigned ne = m_model->mesh()->elements().size();
			//allocate
			m_element = new double***[ns];
			for(unsigned i = 0; i < ns; i++)
			{
				m_element[i] = new double**[nt];
				for(unsigned j = 0; j < nt; j++)
				{
					m_element[i][j] = new double*[ne];
					for(unsigned k = 0; k < ne; k++)
					{
						m_element[i][j][k] = new double[m_model->mesh()->element(k)->nodes().size()];
					}
				}
			}
			m_limit_state = new limit::State[nt];
		}
		void Canvas::allocate_position(void)
		{
			//data
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes().size();
			//allocate
			m_position = new double**[ns];
			for(unsigned i = 0; i < ns; i++)
			{
				m_position[i] = new double*[nn];
				for(unsigned j = 0; j < nn; j++)
				{
					m_position[i][j] = new double[3];
				}
			}
		}
	}
}
