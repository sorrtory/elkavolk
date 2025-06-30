#include "mainwindow.h"
#include "./ui_mainwindow.h"



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

    // Init with the first signal if available
    if (!this->signalList.empty())
    {
        on_signal_currentIndexChanged(0);
        updateCharts();
    }
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

// ---------- Chart plotting
void MainWindow::updateSignalCharts() const
{
    int signalIndex = getCurrentSignalIndex();

    const Signal &signal = this->signalList[signalIndex];
    QtCharts::QLineSeries *series = new QtCharts::QSplineSeries();

    // Clear previous series
    for (auto *chart : ui->signal_widget->findChildren<QtCharts::QChart *>())
    {
        chart->removeAllSeries();
    }

    std::vector<double> samples = signal.getSamples();
    
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
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    chart->setTheme(QtCharts::QChart::ChartThemeDark);
    ui->signal_chartLbl->setText(signal.name);
    // chart->setTitle(signal.name);
    
    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    
    // Add the chartView to the signal_widget through its layout
    if (!(ui->signal_widget->layout()))
    {
        ui->signal_widget->setLayout(new QHBoxLayout());
        ui->signal_widget->layout()->addWidget(chartView);
    }
    else
    {
        // Remove all existing widgets from the layout before adding
        QLayout *layout = ui->signal_widget->layout();
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr)
        {
            if (child->widget())
                child->widget()->deleteLater();
            delete child;
        }
        layout->addWidget(chartView);
    }
    
}

void MainWindow::updateDFTCharts() const {
    int signalIndex = getCurrentSignalIndex();
    const Signal &signal = this->signalList[signalIndex];

    // Clear previous DFT charts
    for (auto *chart : ui->dft_widget->findChildren<QtCharts::QChart *>())
    {
        chart->removeAllSeries();
    }

    // Get the DFT coefficients
    std::vector<std::complex<double>> dftCoefficients = signal.getDFT();

    if (dftCoefficients.empty())
    {
        qWarning() << "No DFT coefficients available for signal" << signal.name;
        return; // No coefficients to plot
    }

    QtCharts::QLineSeries *series = new QtCharts::QSplineSeries();

    // Populate the series with DFT coefficients
    for (size_t i = 0; i < dftCoefficients.size(); ++i)
    {
        series->append(i, std::abs(dftCoefficients[i]));
    }

    // Create a new chart and add the series
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setBackgroundVisible(false);
    chart->setMargins(QMargins(0, 0, 0, 0));

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);

    // Add the chartView to the dft_widget through its layout
    if (!(ui->dft_widget->layout()))
    {
        ui->dft_widget->setLayout(new QHBoxLayout());
        // ui->dft_widget->setFixedSize(QSize(400, 300));
        ui->dft_widget->layout()->addWidget(chartView);
    }
    else
    {
        // Remove all existing widgets from the layout before adding
        QLayout *layout = ui->dft_widget->layout();
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr)
        {
            if (child->widget())
                child->widget()->deleteLater();
            delete child;
        }
        layout->addWidget(chartView);
    }

}

void MainWindow::updateCharts() const
{
    updateSignalCharts();
    updateDFTCharts();
}

void MainWindow::on_graphBtn_clicked(){
    if (this->signalList.empty())
    {
        qWarning() << "No signals available to plot.";
        return; // No signals to plot
    }
    updateCharts();
}

void MainWindow::on_playBtn_clicked()
{
    int signalIndex = getCurrentSignalIndex();
    if (signalIndex < 0 || signalIndex >= this->signalList.size())
    {
        qWarning() << "Invalid signal index" << signalIndex;
        return;
    }

    // Clean up previous playback if any
    if (audio) {
        audio->stop();
        delete audio;
        audio = nullptr;
    }
    if (generator) {
        generator->stop();
        delete generator;
        generator = nullptr;
    }

    // Check if any audio output device is available
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if (devices.isEmpty()) {
        qWarning() << "No audio output device available. Cannot play audio.";
        return;
    }

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    if (!QAudioDeviceInfo::defaultOutputDevice().isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend";
        return;
    }

    generator = new SineWaveGenerator(this);
    generator->start(44100, 440, 1000); // 440 Hz for 1 second

    audio = new QAudioOutput(format, this);
    audio->start(generator);
}

// ---------- Signal management

void MainWindow::clearSignalProperties() const
{
    WITH_NO_SIGNALS(signal_name, clear());
    WITH_NO_SIGNALS(signal_duration, clear());
    WITH_NO_SIGNALS(signal_sampleRate, clear());
    clearOvertones();
}

void MainWindow::on_signal_newBtn_clicked()
{
    // Create a new signal with default values
    Signal newSignal("New Signal", 44100, 1.0, {});

    // Add the new signal to the list
    this->signalList.push_back(newSignal);

    // Update the signal dropdown
    WITH_NO_SIGNALS(signal, addItem(newSignal.name));

    // Select the new signal
    on_signal_currentIndexChanged(this->signalList.size() - 1);
}

void MainWindow::on_signal_removeBtn_clicked()
{
    int currentIndex = ui->signal->currentIndex();
    if (currentIndex < 0 || currentIndex >= this->signalList.size())
    {
        qWarning() << "Invalid signal index" << currentIndex;
        return;
    }

    // Remove the signal from the list
    this->signalList.erase(this->signalList.begin() + currentIndex);

    // Update the signal dropdown
    ui->signal->blockSignals(true);
    ui->signal->removeItem(currentIndex);
    ui->signal->blockSignals(false);

    // Select the first signal if available
    if (!this->signalList.empty())
    {
        on_signal_currentIndexChanged(0);
    } else
    {
        clearSignalProperties();
    }
    
}


void MainWindow::on_signal_currentIndexChanged(int index)
{
    if (index < 0 || index >= this->signalList.size())
    {
        qWarning() << "Invalid signal index" << index;
        return;
    }
    // Clear previous signal properties (and overtones)
    clearSignalProperties();

    const Signal &signal = this->signalList[index];
    // Update the signal name, duration, and sample rate
    ui->signal_name->setText(signal.name);
    ui->signal_duration->setText(QString::number(signal.duration));
    ui->signal_sampleRate->setText(QString::number(signal.sampleRate));


    // Clear previous overtone data
    ui->overtone->blockSignals(true);
    ui->overtone->clear();

    // Populate the overtone dropdown with overtone names of this signal
    for (const auto &ot : signal.overtones)
    {
        ui->overtone->addItem(ot.name);
    }
    ui->overtone->blockSignals(false);


    // If there are overtones, select the first one
    if (!signal.overtones.empty())
    {
        on_overtone_currentIndexChanged(0);
    } 
    // on_overtone_currentIndexChanged(0);
}


// ---------- Hot-Update QLineEdits of signal properties
void MainWindow::on_signal_name_textChanged(const QString &arg1)
{
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    signal.name = arg1;

    // Update the signal dropdown to reflect the new name
    ui->signal->blockSignals(true);
    ui->signal->setItemText(getCurrentSignalIndex(), signal.name);
    ui->signal->blockSignals(false);
}

void MainWindow::on_signal_duration_textChanged(const QString &arg1)
{
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    double duration = arg1.toDouble();
    signal.duration = duration;
}

void MainWindow::on_signal_sampleRate_textChanged(const QString &arg1)
{
    // Update the value
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    int sampleRate = arg1.toInt();
    signal.sampleRate = sampleRate;

}



// ---------- Overtone management

void MainWindow::on_overtone_newBtn_clicked()
{
    // Get the current signal
    Signal &signal = this->signalList[getCurrentSignalIndex()];

    // Create a new overtone with default values
    overtone newOvertone("New Overtone", 1.0, 440.0, 0.0);

    // Add the new overtone to the signal's overtone list
    signal.overtones.push_back(newOvertone);

    // Update the overtone dropdown
    ui->overtone->blockSignals(true);
    ui->overtone->addItem(newOvertone.name);
    ui->overtone->blockSignals(false);

    // Select the new overtone
    on_overtone_currentIndexChanged(signal.overtones.size() - 1);
}

void MainWindow::clearOvertones() const
{
    WITH_NO_SIGNALS(overtone, clear());
    WITH_NO_SIGNALS(overtone_name, clear());
    WITH_NO_SIGNALS(overtone_amplitude, clear());
    WITH_NO_SIGNALS(overtone_frequency, clear());
    WITH_NO_SIGNALS(overtone_phase, clear());
}


void MainWindow::on_overtone_removeBtn_clicked()
{
    int currentIndex = ui->overtone->currentIndex();
    if (currentIndex < 0 || currentIndex >= this->signalList[getCurrentSignalIndex()].overtones.size())
    {
        qWarning() << "Invalid overtone index" << currentIndex;
        return;
    }

    // Remove the overtone from the signal's overtone list
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    signal.overtones.erase(signal.overtones.begin() + currentIndex);

    // Update the overtone dropdown
    ui->overtone->blockSignals(true);
    ui->overtone->removeItem(currentIndex);
    ui->overtone->blockSignals(false);

    // Select the first overtone if available
    if (!signal.overtones.empty())
    {
        on_overtone_currentIndexChanged(0);
    }
}


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
        ui->overtone_name->setText(ot.name);
        ui->overtone_amplitude->setText(QString::number(ot.amplitude));
        ui->overtone_frequency->setText(QString::number(ot.frequency));
        ui->overtone_phase->setText(QString::number(ot.phase));
    }
}



// ---------- Hot-Update QLineEdits of overtone properties

void MainWindow::on_overtone_name_textChanged(const QString &arg1)
{
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    overtone &overtone = signal.overtones[getCurrentOvertoneIndex()];

    // Update the value
    overtone.name = arg1;

    // Update the overtone dropdown to reflect the new name
    ui->overtone->blockSignals(true);
    ui->overtone->setItemText(getCurrentOvertoneIndex(), overtone.name);
    ui->overtone->blockSignals(false);
}

void MainWindow::on_overtone_amplitude_textChanged(const QString &arg1)
{
    // Update the value
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    overtone &overtone = signal.overtones[getCurrentOvertoneIndex()];

    double amplitude = arg1.toDouble();
    overtone.amplitude = amplitude;

}

void MainWindow::on_overtone_frequency_textChanged(const QString &arg1)
{
    // Update the value
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    overtone &overtone = signal.overtones[getCurrentOvertoneIndex()];

    double frequency = arg1.toDouble();
    overtone.frequency = frequency;

}

void MainWindow::on_overtone_phase_textChanged(const QString &arg1)
{
    // Update the value
    Signal &signal = this->signalList[getCurrentSignalIndex()];
    overtone &overtone = signal.overtones[getCurrentOvertoneIndex()];

    double phase = arg1.toDouble();
    overtone.phase = phase;

}

