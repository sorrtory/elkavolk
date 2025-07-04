#ifndef SIGNAL_H
#define SIGNAL_H
#include <vector>
#include <cmath>
#include <complex>
#include <QVariant>
#include <QString>

#include <QIODevice>

// Overtone structure representing a harmonic signal
// https://ru.wikipedia.org/wiki/%D0%93%D0%B0%D1%80%D0%BC%D0%BE%D0%BD%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%B8%D0%B9_%D1%81%D0%B8%D0%B3%D0%BD%D0%B0%D0%BB
struct overtone
{
    QString name;     // Name of the overtone
    double amplitude; // Amplitude (0.0 to 1.0)
    double frequency; // Frequency in Hz
    double phase;     // Phase in radians

    // Harmonic signal value at a given time
    double getValue(double time) const
    {
        return amplitude * std::cos(2 * M_PI * frequency * time + phase);
    }

    overtone(const QVariant &var)
    {
        QVariantMap obj = var.toMap();
        name = obj["name"].toString();
        amplitude = obj["amplitude"].toDouble();
        frequency = obj["frequency"].toDouble();
        phase = obj["phase"].toDouble();
    }

    overtone(QString name, double amplitude, double frequency, double phase)
        : name(name), amplitude(amplitude), frequency(frequency), phase(phase) {};
};

// Signal class representing a collection of overtones
struct Signal
{
    Signal(QString name, int sampleRate, double duration, const std::vector<overtone> &overtones)
        : name(name), duration(duration), sampleRate(sampleRate), overtones(overtones) {};
    Signal(const QVariant &other)
    {
        QVariantMap obj = other.toMap();
        name = obj["name"].toString();
        duration = obj["duration"].toDouble();
        sampleRate = obj["sampleRate"].toInt();
        QVariantList arr = obj["overtones"].toList();
        for (const QVariant &val : arr)
            overtones.emplace_back(val);
    }
    ~Signal() = default;

    // Return samples of the signal by sampleRate and duration
    std::vector<double> getSamples() const;

    // Calculate the Discrete Fourier Transform (DFT) coefficients
    // by sampling the signal over its duration
    std::vector<std::complex<double>> getDFT() const;

    // List of overtones making up the signal
    std::vector<overtone> overtones;

    double duration; // Duration in seconds
    int sampleRate;  // Sample rate in Hz
    QString name;    // Name of the signal

    // Calculate the value of the signal at a given time
    const double getValue(double time) const
    {
        double value = 0.0;
        for (const auto &ot : overtones)
        {
            value += ot.getValue(time);
        }
        return value;
    }
};

#endif // SIGNAL_H