#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QButtonGroup>
#include "cscale.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void RefreshUI();
	void InitUI();
private slots:
	void on_comboBox_key_currentIndexChanged(int index);

	void on_comboBox_tonality_currentIndexChanged(int index);

	void on_comboBox_turning_currentIndexChanged(int index);

	void on_comboBox_chord_currentIndexChanged(int index);

	void on_radioButton_sharp_clicked();

	void on_radioButton_flat_clicked();

	void on_comboBox_chordlevel_currentIndexChanged(int index);

	void on_pushButton_convertkey_clicked();

	void on_checkBox_stateChanged(int arg1);

	void on_pushButton_converttonal_clicked();

private:
	Ui::MainWindow *ui;
	CScale* m_Scale;
	QButtonGroup* m_ButtonGroup;
};

#endif // MAINWINDOW_H
