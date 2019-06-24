//qt
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>

//boost
#include <boost/filesystem.hpp>

//fea
#include "Model/Model.h"

#include "Plot/Plot.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Joints/States.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Element.h"

#include "Boundary/Boundary.h"
#include "Boundary/Supports/Support.h"

#include "Analysis/Analysis.h"

//gui
#include "Util/Recent.h"

#include "Galileo/Galileo.h"

#include "Actions/Model/Model.h"

#include "Actions/Mesh/Nodes/Nodes.h"
#include "Actions/Mesh/Cells/Cells.h"
#include "Actions/Mesh/Sections/Sections.h"
#include "Actions/Mesh/Elements/ElementsMesh.h"
#include "Actions/Mesh/Materials/Materials.h"

#include "Actions/Results/Nodal.h"
#include "Actions/Results/Elements.h"
#include "Actions/Results/Deformed.h"
#include "Actions/Results/Equilibrium_Path.h"

#include "Actions/Help/About.h"
#include "Actions/Help/Index.h"

//ui
#include "ui_Galileo.h"

namespace gui
{
	//constructors
	Galileo::Galileo(QWidget* parent) : QMainWindow(parent), 
		m_ui(new Ui::Galileo), m_plot(new fea::plot::Plot), m_model(new fea::models::Model), m_recent(new gui::util::Recent)
	{
		//ui
		m_ui->setupUi(this);
		m_ui->canvas->update_model(m_model);
		m_ui->menubar->setNativeMenuBar(false);
		//data
		plot(true);
		m_recent->load();
		m_recent->update(m_ui->menu_model_recent);
		//maximize
		showMaximized();
	}

	//destructor
	Galileo::~Galileo(void)
	{
		//save
		plot(false);
		//delete
		delete m_ui;
		delete m_plot;
		delete m_model;
		delete m_recent;
	}
		
	//slots
	bool Galileo::slot_saved(void)
	{
		if(!m_model->saved())
		{
			const QString question = "Would you like to save the current model?";
			const QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
			const QMessageBox::StandardButton reply = QMessageBox::warning(nullptr, "Save Model", question, buttons);
			if(reply == QMessageBox::StandardButton::Yes)
			{
				on_action_model_save_triggered();
			}
			else if(reply == QMessageBox::StandardButton::Cancel)
			{
				return false;
			}
		}
		return true;
	}
	void Galileo::slot_recent(void)
	{
		
	}
	
	void Galileo::on_action_model_new_triggered(void)
	{
		//check
		if(!slot_saved())
		{
			return;
		}
		//create
		delete m_model;
		m_model = new fea::models::Model();
		//update
		m_model->plot(m_plot);
		m_ui->canvas->m_model = m_model;
		m_ui->canvas->m_bound.update(m_model);
	}
	void Galileo::on_action_model_save_triggered(void)
	{
		//path
		char folder[200];
		current(folder, true);
		const unsigned options = (unsigned) QFileDialog::ReadOnly | (unsigned) QFileDialog::ShowDirsOnly;
		std::string path = QFileDialog::getExistingDirectory(nullptr, "Save Model", folder).toStdString();
		if(path.empty())
		{
			return;
		}
		//save dialog
		m_model->path(path);
		if(!m_model->save())
		{
			QMessageBox::critical(nullptr, "Error", "Unable to save model!", QMessageBox::Ok);
		}
		else
		{
			m_recent->add(m_model->folder());
			m_recent->update(m_ui->menu_model_recent);
		}
	}
	void Galileo::on_action_model_open_triggered(void)
	{
		//check
		if(!slot_saved())
		{
			return;
		}
		//get model
		char path[200];
		current(path, true);
		const unsigned options = (unsigned) QFileDialog::ReadOnly | (unsigned) QFileDialog::ShowDirsOnly;
		strcpy(path, QFileDialog::getExistingDirectory(nullptr, "Open model", path, QFileDialog::Options(options)).toLocal8Bit().data());
		if(path[0] == '\0')
		{
			return;
		}
		//load model
		delete m_model;
		m_model = new fea::models::Model();
		m_model->plot(m_plot);
		if(!m_model->load(path))
		{
			QMessageBox::critical(nullptr, "Error", "Unable to load model!", QMessageBox::Ok);
		}
		//update
		current(path, false);
		m_ui->canvas->m_model = m_model;
		m_ui->canvas->m_bound.update(m_model);
		setWindowTitle("Galileo - " + QString(m_model->name().c_str()));
	}
	void Galileo::on_action_model_plot_triggered(void)
	{
		//create dialog
		model::Model(m_model, nullptr).exec();
        //update title
		setWindowTitle("Galileo - " + QString(m_model->name().c_str()));
	}
	void Galileo::on_action_model_print_triggered(void)
	{
		//data
		const QString path = m_model->path().c_str();
		const QString name = m_model->name().c_str();
		const QString file = path + "/" + name + "/Model.png";
		//print
		m_ui->canvas->grabFramebuffer().save(file);
		QMessageBox::information(nullptr, "Model", "Model image saved!", QMessageBox::Ok);
	}
	void Galileo::on_action_model_close_triggered(void)
	{
		//check
		if(!slot_saved())
		{
			return;
		}
		//close
		QApplication::quit();
	}
	
	void Galileo::on_action_mesh_nodes_triggered(void)
	{
		mesh::nodes::Nodes(m_model->mesh(), m_ui->canvas, nullptr).exec();
	}
	void Galileo::on_action_mesh_cells_triggered(void)
	{
		mesh::cells::Cells(m_model->mesh(), nullptr).exec();
	}
	void Galileo::on_action_mesh_sections_triggered(void)
	{
		mesh::sections::Sections(m_model->mesh(), nullptr).exec();
	}
	void Galileo::on_action_mesh_elements_triggered(void)
	{
		mesh::elements::Elements(m_model->mesh(), nullptr).exec();
	}
	void Galileo::on_action_mesh_materials_triggered(void)
	{
		mesh::materials::Materials(m_model->mesh(), nullptr).exec();
	}
	
	void Galileo::on_action_results_nodes_triggered(void)
	{
		if(!m_model->analysis()->analysed())
		{
			QMessageBox::warning(nullptr, "Analysis", "The model must be analysed first!", QMessageBox::Ok);
			return;
		}
		results::Nodal(m_model, this).exec();
	}
	void Galileo::on_action_results_elements_triggered(void)
	{
		if(!m_model->analysis()->analysed())
		{
			QMessageBox::warning(nullptr, "Analysis", "The model must be analysed first!", QMessageBox::Ok);
			return;
		}
		results::Elements(m_model, this).exec();
	}
	void Galileo::on_action_results_deformed_triggered(void)
	{
		if(!m_model->analysis()->analysed())
		{
			QMessageBox::warning(nullptr, "Analysis", "The model must be analysed first!", QMessageBox::Ok);
			return;
		}
		results::Deformed(m_model, this).exec();
	}
	void Galileo::on_action_results_equilibrium_path_triggered(void)
	{
		if(!m_model->analysis()->analysed())
		{
			QMessageBox::warning(nullptr, "Analysis", "The model must be analysed first!", QMessageBox::Ok);
			return;
		}
		const std::string path = m_model->path() + "/" + m_model->name() + "/Solver/";
		if(!boost::filesystem::exists(path + "Load.txt") && !boost::filesystem::exists(path + "Time.txt"))
		{
			QMessageBox::warning(nullptr, "Analysis", "Solver type does not allow equilibrium path!", QMessageBox::Ok);
			return;
		}
		results::Equilibrium_Path(m_model, this).exec();
	}
	
	void Galileo::on_action_help_index_triggered(void)
	{
		help::Index(nullptr).exec();
	}
	void Galileo::on_action_help_about_triggered(void)
	{
        help::About(nullptr).exec();
	}
	
	//events
	void Galileo::keyPressEvent(QKeyEvent* event) 
	{
		//key
		const unsigned key = event->key();
		//canvas
		const unsigned ck[] = {
			Qt::Key_X, Qt::Key_Y, Qt::Key_Z, Qt::Key_I, Qt::Key_F, Qt::Key_C, 
			Qt::Key_Right, Qt::Key_Left, Qt::Key_Up, Qt::Key_Down, Qt::Key_Plus, Qt::Key_Minus
		};
		if(std::find(ck, ck + 11, key) != ck + 11)
		{
			m_ui->canvas->keyPressEvent(event);
		}
		//deformed
		if(key == Qt::Key_D)
		{
			on_action_results_deformed_triggered();
		}
		//equilibrium path
		if(key == Qt::Key_E)
		{
			on_action_results_equilibrium_path_triggered();
		}
	}
	
	//saved
	void Galileo::plot(bool load)
	{
		if(load)
		{
			m_model->plot(m_plot);
			FILE* file = fopen(".plot", "r");
			if(file)
			{
				m_plot->load(file);
				fclose(file);
			}
		}
		else
		{
			FILE* file = fopen(".plot", "w");
			m_plot->save(file);
			fclose(file);
		}
	}
	void Galileo::current(char* path, bool load)
	{
		if(load)
		{
			FILE* file = fopen(".current", "r");
			if(file)
			{
				fgets(path, 200, file);
				fclose(file);
			}
			else
			{
				strcpy(path, QDir::currentPath().toLocal8Bit().data());
			}
		}
		else
		{
			*strrchr(path, '/') = '\0';
			FILE* file = fopen(".current", "w");
			fprintf(file, "%s", path);
			fclose(file);
		}
	}
}
