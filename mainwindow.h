#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariant>

#include "signal.h"
#include "utils.h"
#include "generator.h"

#include <QChart>
#include <QChartView>
#include <QSplineSeries>
#include <QAudioOutput>
#include <QBuffer>
#include <QAudioDecoder>
#include <QIODevice>

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
    void updateDFTCharts() const;
    void updateCharts() const;

private slots:
  // --- Signal
  // Signal index change handler
  void on_signal_currentIndexChanged(int index);
  void on_signal_newBtn_clicked();
  void on_signal_removeBtn_clicked();

  // --- Signal properties
  void on_signal_name_textChanged(const QString &arg1);
  void on_signal_duration_textChanged(const QString &arg1);
  void on_signal_sampleRate_textChanged(const QString &arg1);


  // --- Overtone
  // Overtone index change handler
  void on_overtone_currentIndexChanged(int index);
  void on_overtone_newBtn_clicked();
  void on_overtone_removeBtn_clicked();
  

  // --- Overtone properties
  void on_overtone_name_textChanged(const QString &arg1);
  void on_overtone_amplitude_textChanged(const QString &arg1);
  void on_overtone_frequency_textChanged(const QString &arg1);
  void on_overtone_phase_textChanged(const QString &arg1);

  // --- Misc
  void on_graphBtn_clicked();
  void on_playBtn_clicked();

private:
    Ui::MainWindow *ui;
    SineWaveGenerator* generator = nullptr;
    QAudioOutput* audio = nullptr;

};
#endif // MAINWINDOW_H
