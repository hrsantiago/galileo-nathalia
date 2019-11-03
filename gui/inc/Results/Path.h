#pragma once

namespace fea
{
	namespace models
	{
		class Model;
	}
}

namespace gui
{
	namespace results
	{
		class Path
		{
		public:
			//constructors
			Path(const fea::models::Model*);
			
			//destructor
			virtual ~Path(void);
		
			//data
			unsigned steps(void) const;
			
			const double* dof(void) const;
			const double* step(void) const;
			const double* solver(void) const;
			
			const double** support(void) const;
			
			const double**** node(void) const;
			const double**** joint(void) const;
			const double**** element(void) const;
			
			void model(const fea::models::Model*);
			
			//data
			bool read(void);
			void clear(void);
		
		protected:
			//data
			void setup(void);
			void allocate(void);
		
			//read
			bool read_node(void);
			bool read_joint(void);
			bool read_solver(void);
			bool read_element(void);
			bool read_support(void);
			
			//clear
			void clear_node(void);
			void clear_joint(void);
			void clear_solver(void);
			void clear_element(void);
			void clear_support(void);
			
			//allocate
			void allocate_node(void);
			void allocate_joint(void);
			void allocate_solver(void);
			void allocate_element(void);
			void allocate_support(void);
		
		private:
			//attributes
			unsigned m_steps;
			
			double* m_dof;
			double* m_step;
			double* m_solver;
			
			double** m_support;
			
			double*** m_node[3];
			
			double**** m_joint;
			double**** m_element;
			
			const fea::models::Model* m_model;
		};
	}
}
