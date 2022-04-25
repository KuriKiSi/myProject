#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cscale.h"
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
    delete m_Scale;
}

void MainWindow::InitUI()
{
    m_Scale = new CScale(TONE_C, TONAL_NATURAL_MAJOR, TURNING_STANDARD);

	/* Table View */
	QTableView *tableView = ui->tableView;
	ui->tableView->resize(TBL_WIDTH, TBL_HEIGHT);
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);	//自适应所有列
	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	/* Tonality */
	QStringList tonalitylist;
	for (int i = 0; i < TONAL_CNT; i++) {
		tonalitylist.append(m_Scale->GetTonalityName(i));
	}
	ui->comboBox_tonality->addItems(tonalitylist);
	ui->comboBox_curtonal->addItems(tonalitylist);
	ui->comboBox_desttonal->addItems(tonalitylist);

	/* Key */
	QStringList keylist;
	for (int i = 0; i < TONE_CNT; i++) {
		QString name = m_Scale->GetToneName(i);
		keylist.append(name);
	}
	ui->comboBox_key->addItems(keylist);
	ui->comboBox_curkey->addItems(keylist);
	ui->comboBox_destkey->addItems(keylist);

	/* Turning */
	QStringList turninglist;
	for (int i = 0; i < TURNING_CNT; i++) {
		turninglist.append(m_Scale->GetTurningName(i));
	}
	ui->comboBox_turning->addItems(turninglist);

	/* Chord */
	QStringList chordlist;
	for (int i = 0; i < CHORD_CNT; i++) {
		chordlist.append(m_Scale->GetChordName(i));
	}
	ui->comboBox_chord->addItems(chordlist);

	QStringList levellist;
	for (int i = 0; i < INNER_TONE_CNT; i++) {
		levellist.append(m_Scale->GetRomaName(i));
	}
	ui->comboBox_chordlevel->addItems(levellist);

	/* Accidental */
	m_ButtonGroup = new QButtonGroup(this);
	m_ButtonGroup->addButton(ui->radioButton_sharp, MARK_SHARP);
	m_ButtonGroup->addButton(ui->radioButton_flat, MARK_FLAT);
	ui->radioButton_sharp->setChecked(true);

	/* Main Window */
	this->setMaximumWidth(930);
	this->setMaximumHeight(750);
	this->setStyleSheet("#MainWindow{border-image: url(:/new/prefix1/resources/guitar2.jpg);color: rgb(170, 85, 255);}");
}

void MainWindow::RefreshUI()
{
	QStringList rowLabels;
	QTableView *tableView = ui->tableView;
	QStandardItemModel* model = new QStandardItemModel();
    model->setHorizontalHeaderLabels({"6", "5", "4", "3", "2", "1"});   //表头
    tableView->verticalScrollBar();     //竖直滚动条
	for (int i = 0; i < 20; i++) {		//品
		for (int j = 0; j < 6; j++) {	//弦
			int tone = m_Scale->GetToneByPos(i, j);
			QString itemName;
			int level = m_Scale->GetLevelByTone(tone);
			if (level != -1) {
				itemName = QString("%1(%2)").arg(m_Scale->GetToneName(tone)).arg(level);
			}
			else {
				itemName = QString("%1").arg(m_Scale->GetToneName(tone));
			}
			model->setItem(i, j, new QStandardItem(itemName));
			model->item(i, j)->setForeground(QBrush(QColor(100, 200, 255)));
			model->item(i, j)->setBackground(QBrush(QColor(210, 250, 250)));
			if (m_Scale->IsInnerTone(tone)) {
				model->item(i, j)->setFont(QFont("Times", 10, QFont::Black));
				model->item(i, j)->setForeground(QBrush(QColor(255, 80, 190)));
			}
			if (m_Scale->IsChordTone(tone)) {
				model->item(i, j)->setBackground(QBrush(QColor(255, 192, 203)));
			}
		}
		ui->tableView->setRowHeight(i, 45);
		rowLabels.append(QString("%1\n").arg(i));
	}
	model->setVerticalHeaderLabels(rowLabels);
	tableView->setModel(model);
	tableView->show();

	for (int i = 0; i < ui->comboBox_key->count(); i++) {
		ui->comboBox_key->setItemText(i, m_Scale->GetKeyName(i));
		ui->comboBox_curkey->setItemText(i, m_Scale->GetKeyName(i));
		ui->comboBox_destkey->setItemText(i, m_Scale->GetKeyName(i));
	}

	QString scaleText = m_Scale->GetTitle();
	scaleText += "\n\nTone: ";
	for (int i = 0; i < 8; i++) {
		int index = m_Scale->GetToneByLevel(i);
		QString name = m_Scale->GetToneName(index);
		scaleText += name + " ";
	}
	ui->label_scaleinfo->clear();
	ui->label_scaleinfo->setStyleSheet("color: rgb(170, 85, 255);");
	ui->label_scaleinfo->setText(scaleText);

	QString chordText = "";
	if (m_Scale->GetCurChord() != CHORD_NONE) {
		chordText = "Chord tone:\n-> ";
		for (int i = m_Scale->GetCurKey(); i < 12+m_Scale->GetCurKey(); i++) {
			int tone = i%12;
			if (m_Scale->IsChordTone(tone)) {
				chordText += m_Scale->GetToneName(tone) + " ";
			}
		}
		chordText += "<-";
	}
	ui->label_chordinfo->clear();
	ui->label_chordinfo->setText(chordText);
}


void MainWindow::on_comboBox_key_currentIndexChanged(int key)
{
	if (key < 0) {
		return;
	}
	m_Scale->SetKey(key);
	int tonality = m_Scale->GetCurTonality();
	m_Scale->RefreshData(key, tonality);
	RefreshUI();
}

void MainWindow::on_comboBox_tonality_currentIndexChanged(int tonality)
{
	m_Scale->SetTonality(tonality);
	int key = m_Scale->GetCurKey();
	m_Scale->RefreshData(key, tonality);
	RefreshUI();
}

void MainWindow::on_comboBox_turning_currentIndexChanged(int turning)
{
	m_Scale->SetTurning(turning);
	RefreshUI();
}


void MainWindow::on_comboBox_chord_currentIndexChanged(int chord)
{
	m_Scale->SetChord(chord);
	RefreshUI();
}

void MainWindow::on_radioButton_sharp_clicked()
{
	int id = m_ButtonGroup->checkedId();
	int add = ui->checkBox->isChecked() ? 2 : 0;
	m_Scale->SetAccidentalMark(id+add);
	RefreshUI();
}

void MainWindow::on_radioButton_flat_clicked()
{
	int id = m_ButtonGroup->checkedId();
	int add = ui->checkBox->isChecked() ? 2 : 0;
	m_Scale->SetAccidentalMark(id+add);
	RefreshUI();
}

void MainWindow::on_comboBox_chordlevel_currentIndexChanged(int level)
{
	m_Scale->SetChordLevel(level+1);
	if (m_Scale->GetCurChord() == CHORD_NONE) {
		return;
	}
	RefreshUI();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
	int add = (arg1 == 0) ? 0 : 2;
	int mark = m_Scale->GetCurMark() % 2;
	m_Scale->SetAccidentalMark(mark+add);
	RefreshUI();
}

void MainWindow::on_pushButton_convertkey_clicked()
{
	QString staff = ui->textEdit_staff->toPlainText();
	int curKey = ui->comboBox_curkey->currentIndex();
	int destKey = ui->comboBox_destkey->currentIndex();
	vector<s_ToneInfo> vToneInfo = m_Scale->AnalyStaff(staff);
	vector<s_ToneInfo> vToneTarInfo = m_Scale->Modulation(vToneInfo, curKey, destKey);
	QString text = m_Scale->ToneInfoToStr(vToneTarInfo);
	if (vToneInfo.size() > 0) {
		ui->textEdit_output->setText(text);
	}
	else {
		ui->textEdit_output->setText(QString("Err Input"));
	}
}

void MainWindow::on_pushButton_converttonal_clicked()
{
	QString staff = ui->textEdit_staff->toPlainText();
	int curTonal = ui->comboBox_curtonal->currentIndex();
	int destTonal = ui->comboBox_desttonal->currentIndex();
	vector<s_ToneInfo> vToneInfo = m_Scale->AnalyStaff(staff);
	vector<s_ToneInfo> vToneTarInfo = m_Scale->TonalityConvert(vToneInfo, curTonal, destTonal);
	QString text = m_Scale->ToneInfoToStr(vToneTarInfo);
	if (vToneInfo.size() > 0) {
		ui->textEdit_output->setText(text);
	}
	else {
		ui->textEdit_output->setText(QString("Err Input"));
	}
}
