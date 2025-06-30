#include "signal.h"
#include <iostream>
std::vector<double> Signal::getSamples() const
{
    // Calculate samples (sampleRate-sized) for the entire signal
    std::vector<double> samples;

    double timestep = duration / sampleRate;
    double time = 0.0;
    for (int n = 0; n < sampleRate; n++)
    {
        double value = getValue(time);
        samples.push_back(value);
        time += timestep;
    }
    return samples;
}


void Signal::calcDFT()
{
    // Calculate samples (sampleRate-sized) for the entire signal
    std::vector<double> samples = getSamples();

    // Calculate the Discrete Fourier Transform (DFT)
    DFT.clear();
    DFT.resize(sampleRate);

    // https://en.wikipedia.org/wiki/Discrete_Fourier_transform#Example_2
    for (size_t k = 0; k < sampleRate; k++)
    {
        std::complex<double> sum = 0.0;
        for (size_t n = 0; n < sampleRate; n++)
        {
            double angle = -2.0 * M_PI * k * n / sampleRate;
            std::complex<double> expTerm = std::exp(std::complex<double>(0, angle));
            sum += samples[n] * expTerm;
        }
        DFT[k] = sum;
    }
}