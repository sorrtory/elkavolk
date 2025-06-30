#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariant>

#include "signal.h"
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::vector<Signal> signalList;

    int getCurrentSignalIndex() const;
    int getCurrentOvertoneIndex() const;
    void updateSignalCharts() const;

private slots:
  void on_signal_currentIndexChanged(int index);
  void on_overtone_currentIndexChanged(int index);
//   void on_drawChartsButton_clicked();
  void on_amplitude_textChanged(const QString &arg1);
  void on_frequency_textChanged(const QString &arg1);
  void on_phase_textChanged(const QString &arg1);
  

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
