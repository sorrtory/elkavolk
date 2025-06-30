#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QChart>
#include <QChartView>
#include <QSplineSeries>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Load signal data from JSON file
    QVariant input_signals = readJsonProperty(":/data/data.json", "signals");

    if (input_signals.canConvert<QVariantList>())
    {
        QVariantList signalList = input_signals.toList();
        for (const QVariant &signal : signalList)
        {
            Signal sig(signal);
            this->signalList.push_back(sig);
        }
    }
    else
    {
        qWarning() << "Input signals are not in the expected format.";
    }

    // Populate the signal dropdown with signal names
    for (const auto &signal : this->signalList)
    {
        ui->signal->blockSignals(true);
        ui->signal->addItem(signal.name);
        ui->signal->blockSignals(false);

    }
    // Init with the first signal
    if (!this->signalList.empty())
    {
        on_signal_currentIndexChanged(0);
    }

    // // Create a chart view for the signal widget
    // // Initialize with an empty series
    // // This will be updated when a signal is selected
    // QtCharts::QLineSeries *series = new QtCharts::QSplineSeries();
    
    // QtCharts::QChart *chart = new QtCharts::QChart();
    // chart->addSeries(series);
    // // chart->createDefaultAxes();
    // // chart->setTitle("Hello World Chart");
    // chart->setBackgroundVisible(false);
    // chart->setMargins(QMargins(0, 0, 0, 0));
    // chart->legend()->hide();

    // QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    // chartView->setRenderHint(QPainter::Antialiasing, true);
    // ui->signal_widget->setLayout(new QHBoxLayout());
    // ui->signal_widget->layout()->addWidget(chartView);

    updateSignalCharts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::getCurrentSignalIndex() const
{
    if (ui->signal->currentIndex() < 0 || ui->signal->currentIndex() >= this->signalList.size())
    {
        qWarning() << "Invalid signal index" << ui->signal->currentIndex();
        return -1; // Invalid index
    }
    return ui->signal->currentIndex();
}

int MainWindow::getCurrentOvertoneIndex() const
{
    if (ui->overtone->currentIndex() < 0 || 
        ui->overtone->currentIndex() >= this->signalList[getCurrentSignalIndex()].overtones.size())
    {
        qWarning() << "Invalid overtone index" << ui->overtone->currentIndex();
        return -1; // Invalid index
    }
    return ui->overtone->currentIndex();
}

void MainWindow::updateSignalCharts() const
{
    int signalIndex = getCurrentSignalIndex();

    const Signal &signal = this->signalList[signalIndex];
    QtCharts::QLineSeries *series = new QtCharts::QSplineSeries();

    // // Clear previous series
    for (auto *chart : ui->signal_widget->findChildren<QtCharts::QChart *>())
    {
        chart->removeAllSeries();
    }

    std::vector<double> samples = signal.getSamples();
    qDebug() << "Signal" << signal.name << "has" << samples.size() << "samples.";
    
    if (samples.empty())
    {
        qWarning() << "No samples available for signal" << signal.name;
        return; // No samples to plot
    }

    // Populate the series with samples
    for (size_t i = 0; i < samples.size(); ++i)
    {
        series->append(i, samples[i]);
    }

    // Add the new series to the chart
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setBackgroundVisible(false);
    chart->setMargins(QMargins(0, 0, 0, 0));
    // chart->createDefaultAxes();
    chart->setTitle(signal.name);
    
    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    
    ui->signal_widget->setLayout(new QHBoxLayout());
    ui->signal_widget->layout()->addWidget(chartView);
}

// Signal index change handler
void MainWindow::on_signal_currentIndexChanged(int index)
{
    if (index < 0 || index >= this->signalList.size())
    {
        qWarning() << "Invalid signal index" << index;
        return;
    }

    const Signal &signal = this->signalList[index];

    // Clear previous overtone data
    ui->overtone->blockSignals(true);
    ui->overtone->clear();

    // Populate the overtone dropdown with overtone names of this signal
    for (const auto &ot : signal.overtones)
    {
        ui->overtone->addItem(ot.name);
    }
    ui->overtone->blockSignals(false);

    on_overtone_currentIndexChanged(0);
}

// Overtone index for a signal change handler
void MainWindow::on_overtone_currentIndexChanged(int index)
{
    // Parent signal for the overtone
    const Signal &signal = this->signalList[getCurrentSignalIndex()];

    // Check new index validity
    if (index < 0 || index >= signal.overtones.size())
    {
        qWarning() << "Invalid overtone index" << index << "for signal" << signal.name;
        return;
    }

    // Set default values based on the first overtone
    if (!signal.overtones.empty())
    {
        const overtone &ot = signal.overtones[index];
        ui->amplitude->setText(QString::number(ot.amplitude));
        ui->frequency->setText(QString::number(ot.frequency));
        ui->phase->setText(QString::number(ot.phase));
    }
}

// Update QLineEdits of amplitude, frequency, and phase

void MainWindow::on_amplitude_textChanged(const QString &arg1)
{
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    overtone &overtone = signal.overtones[getCurrentOvertoneIndex()];

    double amplitude = arg1.toDouble();
    overtone.amplitude = amplitude;
}

void MainWindow::on_frequency_textChanged(const QString &arg1)
{
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    overtone &overtone = signal.overtones[getCurrentOvertoneIndex()];

    double frequency = arg1.toDouble();
    overtone.frequency = frequency;
}

void MainWindow::on_phase_textChanged(const QString &arg1)
{
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    overtone &overtone = signal.overtones[getCurrentOvertoneIndex()];

    double phase = arg1.toDouble();
    overtone.phase = phase;
}

