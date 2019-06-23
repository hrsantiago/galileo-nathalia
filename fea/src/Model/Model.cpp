//std
#include <ctime>

//gl
#include <GL/gl.h>

//boost
#include <boost/filesystem/operations.hpp>

//fea
#include "Model/Model.h"

#include "Plot/Plot.h"
#include "Plot/Colors.h"

#include "Mesh/Mesh.h"

#include "Boundary/Boundary.h"

#include "Topology/Topology.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solvers.h"

namespace fea
{
	namespace models
	{
		//constructors
		Model::Model(std::string name, std::string path, plot::Plot* plot) :
		m_name(name),
		m_plot(plot),
		m_saved(true),
		m_mesh(new mesh::Mesh),
		m_boundary(new boundary::Boundary),
		m_topology(new topology::Topology),
		m_analysis(new analysis::Analysis)
		{
			//set model
			mesh::Mesh::m_model = this;
			boundary::Boundary::m_model = this;
			topology::Topology::m_model = this;
			analysis::Analysis::m_model = this;
			//locale
			std::setlocale(LC_ALL, "en_US.UTF-8");
			//set path
			m_path = boost::filesystem::current_path().string() + "/../models" + (path.empty() ? "" : ("/" + path));
			//print header
			Model::header();
		}

		// destructor
		Model::~Model(void)
		{
			//delete
			delete m_mesh;
			delete m_boundary;
			delete m_topology;
			delete m_analysis;
			//print quiter
			Model::quiter();
		}

		//serialization
		bool Model::load(std::string path)
		{
			//path and name
			const unsigned p = path.find_last_of("/");
			m_path = path.substr(0, p);
			m_name = path.substr(p + 1);
			//open file
			printf("******************************   Loading model   ******************************\n");
			FILE* file = fopen((m_path + "/" + m_name + "/Model.txt").c_str(), "r");
			//check
			if(!file)
			{
				printf("\tUnable to open load file!\n");
				printf("*******************************************************************************\n");
				return false;
			}
			//load
			printf("\t-> Mesh\n");
			m_mesh->load(file);
			printf("\t-> Boundary\n");
			m_boundary->load(file);
			printf("\t-> Topology\n");
			m_topology->load(file);
			printf("\t-> Analysis\n");
			m_analysis->load(file);
			//close
			printf("\tDone!\n");
			printf("*******************************************************************************\n");
			fclose(file);
			//return
			return true;
		}
		bool Model::save(std::string path)
		{
			//path
			if(!path.empty())
			{
				m_path = path;
			}
			//open
			boost::filesystem::create_directories(folder() + "/");
			FILE* file = fopen((folder() + "/Model.txt").c_str(), "w");
			printf("******************************   Saving model   *******************************\n");
			//check
			if(!file)
			{
				printf("\tUnable of open save file!\n");
				printf("*******************************************************************************\n");
				return false;
			}
			//save
			printf("\t-> Mesh\n");
			m_mesh->save(file);
			printf("\t-> Boundary\n");
			m_boundary->save(file);
			printf("\t-> Topology\n");
			m_topology->save(file);
			printf("\t-> Analysis\n");
			m_analysis->save(file);
			//close
			fclose(file);
			printf("\tDone!\n");
			printf("*******************************************************************************\n");
			//return
			return m_saved = true;
		}

		//data
		void Model::mark(void)
		{
			m_saved = false;
		}
		bool Model::saved(void) const
		{
			return m_saved;
		}
		
		std::string Model::path(void) const
		{
			return m_path;
		}
		std::string Model::name(void) const
		{
			return m_name;
		}
		std::string Model::path(std::string path)
		{
			return m_path = path;
		}
		std::string Model::name(std::string name)
		{
			return m_name = name;
		}
		std::string Model::folder(void) const
		{
			return m_path + "/" + m_name;
		}
		
		plot::Plot* Model::plot(void) const
		{
			return m_plot;
		}
		mesh::Mesh* Model::mesh(void) const
		{
			return m_mesh;
		}
		plot::Plot* Model::plot(plot::Plot* plot)
		{
			return m_plot = plot;
		}
		boundary::Boundary* Model::boundary(void) const
		{
			return m_boundary;
		}
		topology::Topology* Model::topology(void) const
		{
			return m_topology;
		}
		analysis::Analysis* Model::analysis(void) const
		{
			return m_analysis;
		}
		
		//analysis
		void Model::modified(void)
		{
			m_saved = m_analysis->m_analysed = false;
		}
		
		//results
		void Model::plot(double s, const double** p, const double* vn, const double** ve) const
		{
			//plot boundary
			m_boundary->plot(s, p);
			//plot mesh
			m_mesh->plot(s, p, vn, ve);
			//clear color
			glClearColor(
				m_plot->colors()->back()[0], 
				m_plot->colors()->back()[1], 
				m_plot->colors()->back()[2], 
				m_plot->colors()->back()[3]
			);
		}
		
		//header
		void Model::header(void) const
		{
			//time
			time_t t = time(0);
			tm* c = localtime(&t);
			//header
			printf("*********************************   GALILEO   *********************************\n");
			printf("Description:\n");
			printf("\tGALILEO: static and dynamic nonlinear finite element software\n");
			printf("Version:\n");
			printf("\t 0.01 \"Ferris Bueller's day off\"\n");
			printf("Date:\n");
			printf("\t%02d:%02d:%02d %02d/%02d/%04d\n", c->tm_hour, c->tm_min, c->tm_sec, c->tm_mday, c->tm_mon + 1, c->tm_year + 1900);
			printf("Model:\n");
			printf("\t%s\n", m_name.c_str());
			printf("Path:\n");
			printf("\t%s\n", m_path.c_str());
			printf("Author:\n");
			printf("\tMurillo Vinicius Bento Santana\n");
			printf("Contact:\n");
			printf("\tmvbentosantana@gmail.com\n");
			printf("*******************************************************************************\n");
		}
		void Model::quiter(void) const
		{
			printf("*********************************   GALILEO   *********************************\n");
			printf("\tThanks for using Galileo, see you soon! :)\n");
			printf("*******************************************************************************\n");
		}
	}
}
