//std
#include <limits>

//suitesparse
#include <umfpack.h>

//boost
#include <boost/filesystem/operations.hpp>

//mat
#include "misc/util.h"
#include "linear/dense.h"
#include "linear/sparse.h"

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Elements/Element.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Initials/Initial.h"
#include "Boundary/Supports/Support.h"
#include "Boundary/Loads/Nodes/Node.h"
#include "Boundary/Loads/Elements/Element.h"
#include "Boundary/Dependencies/Dependency.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Time.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace analysis
	{
		//constructor
		Assembler::Assembler(void) : 
			m_row_map(nullptr), m_col_map(nullptr), m_row_triplet(nullptr), m_col_triplet(nullptr),
			m_qe(0), m_ue(nullptr), m_ve(nullptr), m_ae(nullptr), m_fe(nullptr), m_ke(nullptr), m_de(nullptr),
			m_dof_know(0), m_dof_local(0), m_dof_total(0), m_dof_unknow(0), m_dof_triplet(0), m_dof_dependent(0)
		{
			return;
		}

		//destructor
		Assembler::~Assembler(void)
		{
			delete[] m_ue;
			delete[] m_ve;
			delete[] m_ae;
			delete[] m_fe;
			delete[] m_ke;
			delete[] m_de;
			delete[] m_row_map;
			delete[] m_col_map;
			delete[] m_row_triplet;
			delete[] m_col_triplet;
		}

		//dof
		void Assembler::map_dof(void)
		{
			//count dof
			for(const mesh::joints::Joint* joint : m_analysis->m_model->m_mesh->m_joints)
			{
				count_dof(joint->m_dof);
			}
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				count_dof(element->m_dof);
			}
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				count_dof(support->m_dof);
			}
			//map dof
			m_row_map = new int[m_dof_triplet];
			m_col_map = new int[m_dof_unknow + 1];
			m_row_triplet = new int[m_dof_triplet];
			m_col_triplet = new int[m_dof_triplet];
			memset(m_col_map, 0, (m_dof_unknow + 1) * sizeof(int));
			for(const mesh::joints::Joint* joint : m_analysis->m_model->m_mesh->m_joints)
			{
				for(unsigned i : joint->m_dof)
				{
					for(unsigned j : joint->m_dof)
					{
						add_dof(i, j);
					}
				}
			}
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				for(unsigned i : element->m_dof)
				{
					for(unsigned j : element->m_dof)
					{
						add_dof(i, j);
					}
				}
			}
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				add_dof(support->m_dof, support->m_dof);
			}
			//sparse format
			const unsigned nu = m_dof_unknow;
			const unsigned nz = m_dof_triplet;
			const unsigned st = umfpack_di_triplet_to_col(nu, nu, nz, m_row_triplet, m_col_triplet, nullptr, m_col_map, m_row_map, nullptr, nullptr);
		}
		void Assembler::add_dof(unsigned i, unsigned j)
		{
			int& k = m_col_map[m_dof_unknow];
			if(i < m_dof_unknow && j < m_dof_unknow)
			{
				m_row_triplet[k] = i;
				m_col_triplet[k++] = j;
			}
			else if(i < m_dof_unknow && j < m_dof_unknow + m_dof_dependent)
			{
				for(unsigned p : m_analysis->m_model->m_boundary->m_dependencies[j - m_dof_unknow]->m_masters_dof_index)
				{
					add_dof(i, p);
				}
			}
			else if(i < m_dof_unknow + m_dof_dependent && j < m_dof_unknow)
			{
				for(unsigned p : m_analysis->m_model->m_boundary->m_dependencies[i - m_dof_unknow]->m_masters_dof_index)
				{
					add_dof(p, j);
				}
			}
			else if(i < m_dof_unknow + m_dof_dependent && j < m_dof_unknow + m_dof_dependent)
			{
				for(unsigned p : m_analysis->m_model->m_boundary->m_dependencies[i - m_dof_unknow]->m_masters_dof_index)
				{
					for(unsigned q : m_analysis->m_model->m_boundary->m_dependencies[j - m_dof_unknow]->m_masters_dof_index)
					{
						add_dof(p, q);
					}
				}
			}
		}
		
		void Assembler::sort_dof(void)
		{
			//know dof
			std::vector<unsigned> dd, kd;
			for(const boundary::Dependency* dependency : m_analysis->m_model->m_boundary->m_dependencies)
			{
				const mesh::nodes::dof d = dependency->m_slave_dof;
				const mesh::nodes::Node* n = dependency->slave_node();
				const char p = mat::bit_find(n->m_dof_types, (unsigned) d);
				dd.push_back(n->m_dof[p]);
			}
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				if(support->m_fix)
				{
					const mesh::nodes::dof d = support->m_dof_type;
					const mesh::nodes::Node* n = support->node();
					const char p = mat::bit_find(n->m_dof_types, (unsigned) d);
					kd.push_back(n->m_dof[p]);
				}
			}
			//number of dof
			const unsigned nk = m_dof_know = (unsigned) kd.size();
			const unsigned nd = m_dof_dependent = (unsigned) dd.size();
			const unsigned nu = m_dof_unknow = m_dof_total - nk - nd;
			//sort dof
			unsigned d = 0, k = 0;
			for(mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				for(unsigned& dof : node->m_dof)
				{
					std::vector<unsigned>::iterator pd = std::find(dd.begin(), dd.end(), dof);
					std::vector<unsigned>::iterator pk = std::find(kd.begin(), kd.end(), dof);
					if(pd != dd.end() ? ++d : false)
					{
						dof = nu + (unsigned) std::distance(dd.begin(), pd);
					}
					else if(pk != kd.end() ? ++k : false)
					{
						dof = nu + nd + (unsigned) std::distance(kd.begin(), pk);
					}
					else
					{
						dof -= d + k;
					}
				}
			}
		}
		void Assembler::save_dof(void)
		{
			//save dof types
			std::string buffer;
			char formatter[200];
			for(const mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				sprintf(formatter, "%03d ", node->m_dof_types);
				buffer += formatter;
				for(unsigned dof : node->m_dof)
				{
					sprintf(formatter, "%05d ", dof);
					buffer += formatter;
				}
				buffer += "\n";
			}
			//write in file
			boost::filesystem::create_directories(m_analysis->m_model->folder());
			boost::filesystem::create_directories(m_analysis->m_model->folder() + "/Assembler/");
			FILE* file = fopen((m_analysis->m_model->folder() + "/Assembler/Dof.txt").c_str(), "w");
			fprintf(file, buffer.c_str());
			fclose(file);
		}
		void Assembler::apply_dof(void)
		{
			m_analysis->m_model->m_mesh->apply_dof();
			m_analysis->m_model->m_boundary->apply_dof();
		}
		void Assembler::count_dof(unsigned i)
		{
			unsigned p = 0;
			if(i < m_dof_unknow)
			{
				p++;
			}
			else if(i < m_dof_unknow + m_dof_dependent)
			{
				p += m_analysis->m_model->m_boundary->m_dependencies[i - m_dof_unknow]->masters();
			}
			m_dof_triplet += p * p;
		}
		void Assembler::count_dof(const std::vector<unsigned>& list)
		{
			unsigned p = 0;
			for(unsigned i : list)
			{
				if(i < m_dof_unknow)
				{
					p++;
				}
				else if(i < m_dof_unknow + m_dof_dependent)
				{
					p += m_analysis->m_model->m_boundary->m_dependencies[i - m_dof_unknow]->masters();
				}
			}
			m_dof_triplet += p * p;
		}

		//memory
		void Assembler::allocate(void)
		{
			//dofs
			map_dof();
			const unsigned nk = m_dof_know;
			const unsigned nu = m_dof_unknow;
			//solver
			solvers::Solver* solver = m_analysis->m_solver;
			//sets
			const unsigned ss = solver->state_set();
			const unsigned sf = solver->force_set();
			const unsigned st = solver->tangent_set();
			//state
			if(ss & (unsigned) solvers::state::u)
			{
				solver->m_u = new double[nu];
				solver->m_du = new double[nu];
				solver->m_dut = new double[nu];
				solver->m_ddu = new double[nu];
				solver->m_ddur = new double[nu];
				solver->m_ddut = new double[nu];
				memset(solver->m_du, 0, nu * sizeof(double));
				memset(solver->m_ddu, 0, nu * sizeof(double));
			}
			if(ss & (unsigned) solvers::state::v)
			{
				solver->m_v = new double[nu];
				solver->m_dv = new double[nu];
			}
			if(ss & (unsigned) solvers::state::a)
			{
				solver->m_a = new double[nu];
				solver->m_da = new double[nu];
			}
			//force
			if(sf & (unsigned) solvers::force::r)
			{
				solver->m_r = new double[nu];
			}
			if(sf & (unsigned) solvers::force::R)
			{
				solver->m_R = new double[nk];
			}
			if(sf & (unsigned) solvers::force::k)
			{
				solver->m_k = new double[nu];
			}
			if(sf & (unsigned) solvers::force::Fi)
			{
				solver->m_Fiu = new double[nu];
				solver->m_Fik = new double[nk];
			}
			if(sf & (unsigned) solvers::force::Fn)
			{
				solver->m_Fnu = new double[nu];
				solver->m_Fnk = new double[nk];
			}
			if(sf & (unsigned) solvers::force::Fr)
			{
				solver->m_Fru = new double[nu];
				solver->m_Frk = new double[nk];
			}
			if(sf & (unsigned) solvers::force::Fd)
			{
				solver->m_Fdu = new double[nu];
				solver->m_Fdk = new double[nk];
			}
			if(sf & (unsigned) solvers::force::Fe)
			{
				solver->m_Feu = new double[nu];
				solver->m_Fek = new double[nk];
			}
			//tangent
			if(st & (unsigned) solvers::tangent::f)
			{
				solver->m_f = new double[m_col_map[nu]];
			}
			if(st & (unsigned) solvers::tangent::K)
			{
				solver->m_K = new double[m_col_map[nu]];
			}
			if(st & (unsigned) solvers::tangent::C)
			{
				solver->m_C = new double[m_col_map[nu]];
			}
			if(st & (unsigned) solvers::tangent::M)
			{
				solver->m_M = new double[m_col_map[nu]];
			}
			//local
			unsigned& nd = m_dof_local = 0;
			for(const mesh::joints::Joint* joint : m_analysis->m_model->m_mesh->m_joints)
			{
				nd = std::max(nd, (unsigned) joint->m_dof.size());
			}
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				nd = std::max(nd, (unsigned) element->m_dof.size());
			}
			for(const boundary::Dependency* dependency : m_analysis->m_model->m_boundary->m_dependencies)
			{
				nd = std::max(nd, (unsigned) dependency->m_masters_dof.size());
			}
			m_ue = new double[nd];
			m_ve = new double[nd];
			m_ae = new double[nd];
			m_fe = new double[nd];
			m_de = new unsigned[nd];
			m_ke = new double[nd * nd];
		}

		//analysis
		bool Assembler::check(void) const
		{
			//print
			printf("-------------------------------------------------------------------------------\n");
			printf("Checking model:\n");
			//check mesh
			printf("\tMesh:\t\t");
			if(m_analysis->m_model->m_mesh->check())
			{
				printf("succeded\n");
			}
			else
			{
				printf("failed\n");
				return false;
			}
			//check analysis
			printf("\tSolver:\t\t");
			if(m_analysis->check())
			{
				printf("succeded\n");
			}
			else
			{
				printf("failed\n");
				return false;
			}
			//check boundary
			printf("\tBoundary:\t");
			if(m_analysis->m_model->m_boundary->check())
			{
				printf("succeded\n");
			}
			else
			{
				printf("failed\n");
				return false;
			}
			printf("-------------------------------------------------------------------------------\n");
			//return
			return true;
		}
		bool Assembler::symmetric(void) const
		{
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				if(!element->symmetric())
				{
					return false;
				}
			}
			return true;
		}
		void Assembler::prepare(void)
		{
			//prepare
			m_analysis->m_model->m_mesh->prepare();
			m_analysis->m_model->m_boundary->prepare();
			m_analysis->prepare();
			//memory
			allocate();
			//dof report
			printf("-------------------------------------------------------------------------------\n");
			printf("Degrees of freedom:\n");
			printf("\tKnow:\t\t %05d\n", m_dof_know);
			printf("\tUnknow:\t\t %05d\n", m_dof_unknow);
			printf("\tDependent:\t %05d\n", m_dof_dependent);
			printf("\tTotal:\t\t %05d\n", m_dof_know + m_dof_unknow + m_dof_dependent);
			printf("-------------------------------------------------------------------------------\n");
		}
		void Assembler::finish(void) const
		{
			//state set
			const std::string state_name[] = {"State", "Velocity", "Acceleration"};
			const unsigned solver_set = m_analysis->m_solver->state_set();
			const unsigned state_set[] = {(unsigned) solvers::state::u, (unsigned) solvers::state::v, (unsigned) solvers::state::a};
			//create folders
			const std::string path = m_analysis->m_model->m_path + std::string("/") + m_analysis->m_model->m_name;
			boost::filesystem::create_directories(path);
			for(unsigned i = 0; i < 3; i++)
			{
				if(solver_set & state_set[i])
				{
					boost::filesystem::remove_all(path + "/" + state_name[i]);
					boost::filesystem::create_directories(path + "/" + state_name[i]);
				}
			}
			boost::filesystem::remove_all(path + "/Solver");
			boost::filesystem::remove_all(path + "/Joints");
			boost::filesystem::remove_all(path + "/Supports");
			boost::filesystem::remove_all(path + "/Elements");
			boost::filesystem::create_directories(path + "/Solver");
			boost::filesystem::create_directories(path + "/Joints");
			boost::filesystem::create_directories(path + "/Supports");
			boost::filesystem::create_directories(path + "/Elements");
			//finish
			m_analysis->finish();
			m_analysis->m_model->m_mesh->finish();
			m_analysis->m_model->m_boundary->finish();
		}
		
		void Assembler::record(void) const
		{
			m_analysis->m_solver->record();
			m_analysis->m_model->m_mesh->record();
			m_analysis->m_model->m_boundary->record();
		}
		void Assembler::update(void) const
		{
			m_analysis->m_model->m_mesh->update();
		}
		void Assembler::restore(void) const
		{
			m_analysis->m_model->m_mesh->restore();
		}

		//dof
		unsigned Assembler::dof_know(void) const
		{
			return m_dof_know;
		}
		unsigned Assembler::dof_local(void) const
		{
			return m_dof_local;
		}
		unsigned Assembler::dof_total(void) const
		{
			return m_dof_total;
		}
		unsigned Assembler::dof_unknow(void) const
		{
			return m_dof_unknow;
		}
		unsigned Assembler::dof_triplet(void) const
		{
			return m_dof_triplet;
		}
		unsigned Assembler::dof_nonzero(void) const
		{
			return m_col_map[m_dof_unknow];
		}
		unsigned Assembler::dof_dependent(void) const
		{
			return m_dof_dependent;
		}

		//map
		const int* Assembler::row_map(void) const
		{
			return m_row_map;
		}
		const int* Assembler::col_map(void) const
		{
			return m_col_map;
		}
		const int* Assembler::row_triplet(void) const
		{
			return m_row_triplet;
		}
		const int* Assembler::col_triplet(void) const
		{
			return m_col_triplet;
		}

		//assembly
		void Assembler::assembly_state(void) const
		{
			double* u = m_analysis->m_solver->m_u;
			for(const mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				for(const unsigned& dof : node->m_dof)
				{
					if(dof < m_dof_unknow)
					{
						u[dof] = node->m_state_new[&dof - &node->m_dof[0]];
					}
				}
			}
		}
		void Assembler::assembly_velocity(void) const
		{
			double* v = m_analysis->m_solver->m_v;
			for(const mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				for(const unsigned& dof : node->m_dof)
				{
					if(dof < m_dof_unknow)
					{
						v[dof] = node->m_velocity_new[&dof - &node->m_dof[0]];
					}
				}
			}
		}
		void Assembler::assembly_acceleration(void) const
		{
			double* a = m_analysis->m_solver->m_a;
			for(const mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				for(const unsigned& dof : node->m_dof)
				{
					if(dof < m_dof_unknow)
					{
						a[dof] = node->m_acceleration_new[&dof - &node->m_dof[0]];
					}
				}
			}
		}
		
		void Assembler::assembly_inertial_force(void) const
		{
			//solver
			double* Fnu = m_analysis->m_solver->m_Fnu;
			double* Fnk = m_analysis->m_solver->m_Fnk;
			//clear vectors
			memset(Fnk, 0, m_dof_know * sizeof(double));
			memset(Fnu, 0, m_dof_unknow * sizeof(double));
			//assembly
			assembly_inertial_force_elements(Fnu, Fnk);
			assembly_inertial_force_supports(Fnu, Fnk);
		}
		void Assembler::assembly_internal_force(void) const
		{
			//solver
			double* Fiu = m_analysis->m_solver->m_Fiu;
			double* Fik = m_analysis->m_solver->m_Fik;
			//clear vectors
			memset(Fik, 0, m_dof_know * sizeof(double));
			memset(Fiu, 0, m_dof_unknow * sizeof(double));
			//assembly
			assembly_internal_force_joints(Fiu, Fik);
			assembly_internal_force_elements(Fiu, Fik);
			assembly_internal_force_supports(Fiu, Fik);
		}
		
		void Assembler::assembly_dead_force(void) const
		{
			//solver
			double* Fdu = m_analysis->m_solver->m_Fdu;
			double* Fdk = m_analysis->m_solver->m_Fdk;
			//clear vectors
			memset(Fdk, 0, m_dof_know * sizeof(double));
			memset(Fdu, 0, m_dof_unknow * sizeof(double));
			//assembly
			if(!m_analysis->m_model->m_boundary->m_load_cases.empty() && m_analysis->m_solver->m_dead)
			{
				assembly_dead_force_nodes(Fdu, Fdk);
				assembly_dead_force_elements(Fdu, Fdk);
			}
		}
		void Assembler::assembly_external_force(void) const
		{
			//solver
			double* Feu = m_analysis->m_solver->m_Feu;
			double* Fek = m_analysis->m_solver->m_Fek;
			const double t = m_analysis->m_solver->time();
			//clear
			memset(Fek, 0, m_dof_know * sizeof(double));
			memset(Feu, 0, m_dof_unknow * sizeof(double));
			//assembly
			if(!m_analysis->m_model->m_boundary->m_load_cases.empty())
			{
				assembly_external_force_nodes(Feu, Fek, t);
				assembly_external_force_elements(Feu, Fek, t);
			}
		}
		void Assembler::assembly_external_force(double t) const
		{
			//solver
			double* Feu = m_analysis->m_solver->m_Feu;
			double* Fek = m_analysis->m_solver->m_Fek;
			//clear
			memset(Fek, 0, m_dof_know * sizeof(double));
			memset(Feu, 0, m_dof_unknow * sizeof(double));
			//assembly
			if(!m_analysis->m_model->m_boundary->m_load_cases.empty())
			{
				assembly_external_force_nodes(Feu, Fek, t);
				assembly_external_force_elements(Feu, Fek, t);
			}
		}
		void Assembler::assembly_reference_force(void) const
		{
			//solver
			double* Fru = m_analysis->m_solver->m_Fru;
			double* Frk = m_analysis->m_solver->m_Frk;
			//clear vectors
			memset(Frk, 0, m_dof_know * sizeof(double));
			memset(Fru, 0, m_dof_unknow * sizeof(double));
			//assembly
			if(!m_analysis->m_model->m_boundary->m_load_cases.empty())
			{
				assembly_reference_force_nodes(Fru, Frk);
				assembly_reference_force_elements(Fru, Frk);
			}
		}
		
		void Assembler::assembly_inertia(void) const
		{
			//solver
			double* M = m_analysis->m_solver->m_M;
			//clear matrix
			memset(M, 0, m_col_map[m_dof_unknow] * sizeof(double));
			//assembly
			assembly_inertia_elements(M);
			assembly_inertia_supports(M);
		}
		void Assembler::assembly_damping(void) const
		{
			//solver
			solvers::Time* solver = dynamic_cast<solvers::Time*> (m_analysis->m_solver);
			//matrices
			double* C = solver->m_C;
			const double* M = solver->m_M;
			const double* K = solver->m_K;
			//clear matrix
			memset(C, 0, m_col_map[m_dof_unknow] * sizeof(double));
			//formulation damping
			if(solver->damping() & (unsigned) solvers::damping::formulation)
			{
				//assembly
				assembly_damping_elements(C);
				assembly_damping_supports(C);
			}
			//Rayleigh damping
			if(solver->damping() & (unsigned) solvers::damping::rayleigh)
			{
				const double a = solver->rayleigh().alpha;
				const double b = solver->rayleigh().beta;
				for(unsigned i = 0; i < m_col_map[m_dof_unknow]; i++)
				{
					C[i] += a * M[i] + b * K[i];
				}
			}
		}
		void Assembler::assembly_stiffness(void) const
		{
			//solver
			double* K = m_analysis->m_solver->m_K;
			//clear matrix
			memset(K, 0, m_col_map[m_dof_unknow] * sizeof(double));
			//assembly
			assembly_stiffness_joints(K);
			assembly_stiffness_elements(K);
			assembly_stiffness_supports(K);
		}

		//apply
		void Assembler::apply_state(void) const
		{
			for(mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				node->apply_state();
			}
		}
		void Assembler::apply_velocity(void) const
		{
			for(mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				node->apply_velocity();
			}
		}
		void Assembler::apply_acceleration(void) const
		{
			for(mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				node->apply_acceleration();
			}
		}
		
		void Assembler::apply_initials(void) const
		{
			for(const boundary::Initial* initial : m_analysis->m_model->m_boundary->m_initials)
			{
				initial->apply();
			}
		}
		void Assembler::apply_supports(void) const
		{
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				support->apply_values();
			}
			for(unsigned i : boundary::Support::m_update_nodes)
			{
				m_analysis->m_model->m_mesh->m_nodes[i]->update_rotation();
			}
		}
		void Assembler::apply_dependencies(void) const
		{
			for(const boundary::Dependency* dependency : m_analysis->m_model->m_boundary->m_dependencies)
			{
				dependency->apply(m_ue, m_ve, m_ae);
			}
			for(unsigned i : boundary::Dependency::m_update_nodes)
			{
				m_analysis->m_model->m_mesh->m_nodes[i]->update_rotation();
			}
		}
		void Assembler::apply_configurations(void) const
		{
			for(mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				element->apply();
			}
			for(mesh::joints::Joint* joint : m_analysis->m_model->m_mesh->m_joints)
			{
				joint->apply();
			}
		}
		
		void Assembler::apply_reactions(void) const
		{
			for(boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				support->apply_reaction();
			}
		}

		//increment
		void Assembler::increment_state(void) const
		{
			for(mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				node->increment_state();
			}
		}
		void Assembler::increment_velocity(void) const
		{
			for(mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				node->increment_velocity();
			}
		}
		void Assembler::increment_acceleration(void) const
		{
			for(mesh::nodes::Node* node : m_analysis->m_model->m_mesh->m_nodes)
			{
				node->increment_acceleration();
			}
		}

		//assembly
		void Assembler::assembly_number(double v, unsigned d, double* Fu, double* Fk) const
		{
			//dofs
			const unsigned nu = m_dof_unknow;
			const unsigned nd = m_dof_dependent;
			//assembly
			if(d < nu)
			{
				Fu[d] += v;
			}
			else if(d >= nu + nd)
			{
				Fk[d - nu - nd] += v;
			}
			else
			{
				assembly_dependency(v, m_analysis->m_model->m_boundary->m_dependencies[d - nu], Fu, Fk);
			}
		}
		void Assembler::assembly_number(double v, unsigned pi, unsigned pj, double* K) const
		{
			//dofs
			const unsigned nu = m_dof_unknow;
			const unsigned nd = m_dof_dependent;
			//dependencies
			const boundary::Dependency* di = pi < nu || pi >= nu + nd ? nullptr : m_analysis->m_model->m_boundary->m_dependencies[pi - nu];
			const boundary::Dependency* dj = pj < nu || pj >= nu + nd ? nullptr : m_analysis->m_model->m_boundary->m_dependencies[pj - nu];
			//assembly
			if(pi < nu && pj < nu)
			{
				mat::index(K, m_row_map, m_col_map, pi, pj) += v;
			}
			else if(pi < nu && pj < nu + nd)
			{
				assembly_dependency(v, pi, dj, K);
			}
			else if(pi < nu + nd && pj < nu)
			{
				assembly_dependency(v, di, pj, K);
			}
			else if(pi < nu + nd && pj < nu + nd)
			{
				assembly_dependency(v, di, dj, K);
			}
		}
		void Assembler::assembly_vector(const double* f, const std::vector<unsigned>& d, double* Fu, double* Fk) const
		{
			for(unsigned i = 0; i < d.size(); i++)
			{
				assembly_number(f[i], d[i], Fu, Fk);
			}
		}
		void Assembler::assembly_matrix(const double* k, const std::vector<unsigned>& d, double* K) const
		{
			const unsigned ne = d.size();
			for(unsigned i = 0; i < ne; i++)
			{
				for(unsigned j = 0; j < ne; j++)
				{
					assembly_number(k[i + ne * j], d[i], d[j], K);
				}
			}
		}
		
		void Assembler::assembly_dependency(double v, const boundary::Dependency* di, double* Fu, double* Fk) const
		{
			//dofs
			const unsigned nu = m_dof_unknow;
			const unsigned nd = m_dof_dependent;
			//assembly
			di->master_state(m_ue);
			for(unsigned i = 0; i < di->masters(); i++)
			{
				const double g = di->m_gradient(m_ue, i);
				const unsigned p = di->m_masters_dof_index[i];
				if(p < nu)
				{
					Fu[p] += g * v;
				}
				else if(p < nu + nd)
				{
					assembly_dependency(g * v, m_analysis->m_model->m_boundary->m_dependencies[p - nu], Fu, Fk);
				}
				else
				{
					Fk[p - nu - nd] += g * v;
				}
			}
		}
		void Assembler::assembly_dependency(double v, unsigned pi, const boundary::Dependency* dj, double* K) const
		{
			//dofs
			const unsigned nu = m_dof_unknow;
			const unsigned nd = m_dof_dependent;
			//assembly
			dj->master_state(m_ue);
			for(unsigned j = 0; j < dj->masters(); j++)
			{
				const double gj = dj->m_gradient(m_ue, j);
				const unsigned pj = dj->m_masters_dof_index[j];
				if(pj < nu)
				{
					mat::index(K, m_row_map, m_col_map, pi, pj) += gj * v;
				}
				else if(pj < nu + nd)
				{
					assembly_dependency(gj * v, pi, m_analysis->m_model->m_boundary->m_dependencies[pj - nu], K);
				}
			}
		}
		void Assembler::assembly_dependency(double v, const boundary::Dependency* di, unsigned pj, double* K) const
		{
			//dofs
			const unsigned nu = m_dof_unknow;
			const unsigned nd = m_dof_dependent;
			//assembly
			di->master_state(m_ue);
			for(unsigned i = 0; i < di->masters(); i++)
			{
				const double gi = di->m_gradient(m_ue, i);
				const unsigned pi = di->m_masters_dof_index[i];
				if(pi < nu)
				{
					mat::index(K, m_row_map, m_col_map, pi, pj) += gi * v;
				}
				else if(pi < nu + nd)
				{
					assembly_dependency(gi * v, m_analysis->m_model->m_boundary->m_dependencies[pi - nu], pj, K);
				}
			}
		}
		void Assembler::assembly_dependency(double v, const boundary::Dependency* di, const boundary::Dependency* dj, double* K) const
		{
			//dofs
			const unsigned nu = m_dof_unknow;
			const unsigned nd = m_dof_dependent;
			//dependencies
			const std::vector<boundary::Dependency*>& dl = m_analysis->m_model->m_boundary->m_dependencies;
			//assembly
			for(unsigned i = 0; i < di->masters(); i++)
			{
				di->master_state(m_ue);
				const double gi = di->m_gradient(m_ue, i);
				const unsigned pi = di->m_masters_dof_index[i];
				dj->master_state(m_ue);
				for(unsigned j = 0; j < dj->masters(); j++)
				{
					const double gj = dj->m_gradient(m_ue, j);
					const unsigned pj = dj->m_masters_dof_index[j];
					if(pi < nu && pj < nu)
					{
						mat::index(K, m_row_map, m_col_map, pi, pj) += gi * gj * v;
					}
					else if(pi < nu && pj < nu + nd)
					{
						assembly_dependency(v, pi, dl[pj - nu], K);
					}
					else if(pi < nu + nd && pj < nu)
					{
						assembly_dependency(v, dl[pi - nu], pj, K);
					}
					else if(pi < nu + nd && pj < nu + nd)
					{
						assembly_dependency(v, dl[pi - nu], dl[pj - nu], K);
					}
				}
			}
		}
		
		void Assembler::assembly_dead_force_nodes(double* Fu, double* Fk) const
		{
			//load case
			const boundary::Load_Case* load_case = m_analysis->m_model->m_boundary->m_load_cases[m_analysis->m_solver->m_dead_case];
			//node loads loop
			for(const boundary::loads::Node* load : load_case->m_loads_nodes)
			{
				//load
				const double p = load->m_value;
				const unsigned d = load->m_dof;
				//assembly
				assembly_number(p, d, Fu, Fk);
			}
		}
		void Assembler::assembly_dead_force_elements(double* Fu, double* Fk) const
		{
			//time
			const double t = m_analysis->m_solver->time();
			//load case
			const boundary::Load_Case* load_case = m_analysis->m_model->m_boundary->m_load_cases[m_analysis->m_solver->m_dead_case];
			//loads loop
			for(const boundary::loads::Element* load : load_case->m_loads_elements)
			{
				for(unsigned e : load->m_elements)
				{
					const mesh::elements::Element* element = m_analysis->m_model->m_mesh->m_elements[e];
					element->reference_force(m_fe, load);
					const std::vector<unsigned>& d = element->m_dof;
					assembly_vector(m_fe, d, Fu, Fk);
				}
			}
		}
		
		void Assembler::assembly_inertial_force_elements(double* Fu, double* Fk) const
		{
			//time
			const double t = m_analysis->m_solver->time();
			//elements loop
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				//formulation
				element->inertial_force(m_fe);
				const std::vector<unsigned>& d = element->m_dof;
				//assembly
				assembly_vector(m_fe, d, Fu, Fk);
			}
		}
		void Assembler::assembly_inertial_force_supports(double* Fu, double* Fk) const
		{
			//time
			const double t = m_analysis->m_solver->time();
			//solver
			const unsigned ss = m_analysis->m_solver->state_set();
			//supports loop
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				if(!support->m_fix)
				{
					const double m = support->m_inertia;
					const double a = ss & (unsigned) solvers::state::a ? support->acceleration(t) : 0;
					const unsigned d = support->m_dof;
					assembly_number(m * a, d, Fu, Fk);
				}
			}
		}
		
		void Assembler::assembly_internal_force_joints(double* Fu, double* Fk) const
		{
			//joints loop
			for(const mesh::joints::Joint* joint : m_analysis->m_model->m_mesh->m_joints)
			{
				//formulation
				joint->internal_force(m_fe);
				const std::vector<unsigned>& d = joint->m_dof;
				//assembly
				assembly_vector(m_fe, d, Fu, Fk);
			}
		}
		void Assembler::assembly_internal_force_elements(double* Fu, double* Fk) const
		{
			//elements loop
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				//formulation
				element->internal_force(m_fe);
				const std::vector<unsigned>& d = element->m_dof;
				//assembly
				assembly_vector(m_fe, d, Fu, Fk);
			}
		}
		void Assembler::assembly_internal_force_supports(double* Fu, double* Fk) const
		{
			//time
			const double t = m_analysis->m_solver->time();
			//solver
			const unsigned ss = m_analysis->m_solver->state_set();
			//supports loop
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				if(!support->m_fix)
				{
					const double c = support->m_damping;
					const double k = support->m_stiffness;
					const double u = ss & (unsigned) solvers::state::u ? support->state(t) : 0;
					const double v = ss & (unsigned) solvers::state::v ? support->velocity(t) : 0;
					const unsigned d = support->m_dof;
					assembly_number(k * u + c * v, d, Fu, Fk);
				}
			}
		}
		
		void Assembler::assembly_external_force_nodes(double* Fu, double* Fk, double t) const
		{
			//load case
			const boundary::Load_Case* load_case = m_analysis->m_model->m_boundary->m_load_cases[m_analysis->m_solver->m_load_case];
			//loads loop
			for(const boundary::loads::Node* load : load_case->m_loads_nodes)
			{
				//load
				const double p = load->m_value;
				const double f = load->m_function(t);
				const unsigned d = load->m_dof;
				//assembly
				assembly_number(f * p, d, Fu, Fk);
			}
		}
		void Assembler::assembly_external_force_elements(double* Fu, double* Fk, double t) const
		{
			//load case
			const boundary::Load_Case* load_case = m_analysis->m_model->m_boundary->m_load_cases[m_analysis->m_solver->m_load_case];
			//loads loop
			for(const boundary::loads::Element* load : load_case->m_loads_elements)
			{
				for(const unsigned e : load->m_elements)
				{
					//element
					const mesh::elements::Element* element = m_analysis->m_model->m_mesh->m_elements[e];
					//formulation
					const double p = load->m_function(t);
					element->reference_force(m_fe, load);
					const std::vector<unsigned>& d = element->m_dof;
					mat::mul(m_fe, p, d.size());
					//assembly
					assembly_vector(m_fe, d, Fu, Fk);
				}
			}
		}
		
		void Assembler::assembly_reference_force_nodes(double* Fu, double* Fk) const
		{
			//load case
			const boundary::Load_Case* load_case = m_analysis->m_model->m_boundary->m_load_cases[m_analysis->m_solver->m_load_case];
			//node loads loop
			for(const boundary::loads::Node* load : load_case->m_loads_nodes)
			{
				//load
				const double p = load->m_value;
				const unsigned d = load->m_dof;
				//assembly
				assembly_number(p, d, Fu, Fk);
			}
		}
		void Assembler::assembly_reference_force_elements(double* Fu, double* Fk) const
		{
			//time
			const double t = m_analysis->m_solver->time();
			//load case
			const boundary::Load_Case* load_case = m_analysis->m_model->m_boundary->m_load_cases[m_analysis->m_solver->m_load_case];
			//loads loop
			for(const boundary::loads::Element* load : load_case->m_loads_elements)
			{
				for(unsigned e : load->m_elements)
				{
					const mesh::elements::Element* element = m_analysis->m_model->m_mesh->m_elements[e];
					element->reference_force(m_fe, load);
					const std::vector<unsigned>& d = element->m_dof;
					assembly_vector(m_fe, d, Fu, Fk);
				}
			}
		}
		
		void Assembler::assembly_inertia_elements(double* M) const
		{
			//time
			const double t = m_analysis->m_solver->time();
			//elements loop
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				//formulation
				element->inertia(m_ke);
				const std::vector<unsigned>& d = element->m_dof;
				//assembly
				assembly_matrix(m_ke, d, M);
			}
		}
		void Assembler::assembly_inertia_supports(double* M) const
		{
			//supports loop
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				if(!support->m_fix)
				{
					assembly_number(support->m_inertia, support->m_dof, support->m_dof, M);
				}
			}
		}
		
		void Assembler::assembly_damping_elements(double* C) const
		{
			//time
			const double t = m_analysis->m_solver->time();
			//elements loop
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				//formulation
				element->damping(m_ke);
				const std::vector<unsigned>& d = element->m_dof;
				//assembly
				assembly_matrix(m_ke, d, C);
			}
		}
		void Assembler::assembly_damping_supports(double* C) const
		{
			//supports loop
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				if(!support->m_fix)
				{
					assembly_number(support->m_damping, support->m_dof, support->m_dof, C);
				}
			}
		}
		
		void Assembler::assembly_stiffness_joints(double* K) const
		{
			//joints loop
			for(const mesh::joints::Joint* joint : m_analysis->m_model->m_mesh->m_joints)
			{
				//formulation
				joint->stiffness(m_ke);
				const std::vector<unsigned>& d = joint->m_dof;
				//assembly
				assembly_matrix(m_ke, d, K);
			}
		}
		void Assembler::assembly_stiffness_elements(double* K) const
		{
			//time
			const double t = m_analysis->m_solver->time();
			//elements loop
			for(const mesh::elements::Element* element : m_analysis->m_model->m_mesh->m_elements)
			{
				//formulation
				element->stiffness(m_ke);
				const std::vector<unsigned>& d = element->m_dof;
				//assembly
				assembly_matrix(m_ke, d, K);
			}
		}
		void Assembler::assembly_stiffness_supports(double* K) const
		{
			//supports loop
			for(const boundary::Support* support : m_analysis->m_model->m_boundary->m_supports)
			{
				if(!support->m_fix)
				{
					assembly_number(support->m_stiffness, support->m_dof, support->m_dof, K);
				}
			}
		}

		//static attributes
		Analysis* Assembler::m_analysis = nullptr;
	}
}

