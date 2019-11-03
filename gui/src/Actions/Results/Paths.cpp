//qt
#include <QStandardItemModel>

//gui
#include "Actions/Results/Paths.h"

//ui
#include "ui_Paths.h"

namespace gui
{
	namespace results
	{
		//constructors
		Paths::Paths(std::vector<unsigned>& paths, unsigned nodes, QWidget* parent) : QDialog(parent), m_ui(new Ui::Paths), m_nodes(nodes), m_paths(paths)
		{
			//set ui
			m_ui->setupUi(this);
			//set edit
			m_ui->edit->setValidator(new QIntValidator(0, m_nodes - 1));
			m_ui->edit->setText(QString::asprintf("%05d", m_paths.empty() || m_paths.back() + 1 == m_nodes ? 0 : m_paths.back() + 1));
			//set table
			m_ui->table->setColumnCount(1);
			m_ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("Node"));
			m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
			//update
			update_table();
			m_ui->table->setCurrentCell(m_ui->table->rowCount() - 1, 0);
			//connect
			QObject::connect(m_ui->button_add, SIGNAL(clicked(bool)), this, SLOT(slot_add(void)));
			QObject::connect(m_ui->button_clear, SIGNAL(clicked(bool)), this, SLOT(slot_clear(void)));
			QObject::connect(m_ui->button_remove, SIGNAL(clicked(bool)), this, SLOT(slot_remove(void)));
		}
		
		//destructor
		Paths::~Paths(void)
		{
			delete m_ui;
		}
		
		//slots
		void Paths::slot_add(void)
		{
			const unsigned p = m_ui->edit->text().toUInt();
			if(std::find(m_paths.begin(), m_paths.end(), p) == m_paths.end())
			{
				//plots
				m_paths.push_back(p);
				std::sort(m_paths.begin(), m_paths.end());
				//edit
				m_ui->edit->setText(QString::asprintf("%05d", p + 1 == m_nodes ? 0 : p + 1));
				//table
				update_table();
				m_ui->table->selectRow(m_ui->table->rowCount() - 1);
			}
		}
		void Paths::slot_clear(void)
		{
			//plots
			m_paths.clear();
			//table
			update_table();
			//edit
			m_ui->edit->setText(QString::asprintf("%05d", 0));
		}
		void Paths::slot_remove(void)
		{
			//list
			QModelIndexList sr = m_ui->table->selectionModel()->selectedRows(0);
			//remove
			for(unsigned i = 0; i < (unsigned) sr.size(); i++)
			{
				const unsigned j = sr.size() - i - 1;
				m_paths.erase(m_paths.begin() + sr[j].row());
			}
			//table
			update_table();
			m_ui->table->setCurrentCell(m_ui->table->rowCount() - 1, 0);
			//edit
			m_ui->edit->setText(QString::asprintf("%05d", m_paths.empty() || m_paths.back() + 1 == m_nodes ? 0 : m_paths.back() + 1));
		}
		
		//update
		void Paths::update_table(void)
		{
			//set rows
			m_ui->table->setRowCount(m_paths.size());
			//set data
			for(unsigned i = 0; i < m_paths.size(); i++)
			{
				//text
				QTableWidgetItem* item = new QTableWidgetItem(QString::asprintf("%05d", m_paths[i]));
				//properties
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() ^ Qt::ItemIsEditable);
				//add
				m_ui->table->setItem(i, 0, item);
			}
		}
	}
}
