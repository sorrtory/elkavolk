#include "signal.h"
#include <iostream>

// Calculate samples for the entire signal (duration*sampleRate)
std::vector<double> Signal::getSamples() const
{
    std::vector<double> samples;
    int points = sampleRate * duration;
    samples.reserve(points);

    // Sample the signal at each point in time
    for (int n = 0; n < points; n++)
    {
        double value = getValue(static_cast<double>(n) / sampleRate);
        samples.push_back(value);
    }

    return samples;
}


std::vector<std::complex<double>> Signal::getDFT() const
{
    
    // Calculate samples for the entire signal
    std::vector<double> samples = getSamples();
    if (samples.empty())
    {
        std::cerr << "No samples available for DFT calculation." << std::endl;
        return {};
    }
    
    // Calculate the Discrete Fourier Transform (DFT)
    std::vector<std::complex<double>> DFT;
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
    return DFT;
}