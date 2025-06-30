#ifndef SINEWAVEGENERATOR_H
#define SINEWAVEGENERATOR_H

#include <QIODevice>
#include <QByteArray>
#include <QtMath>

#include "signal.h"

class SineWaveGenerator : public QIODevice
{
    Q_OBJECT
public:
    SineWaveGenerator(QObject *parent = nullptr)
        : QIODevice(parent), m_pos(0) {}

    void start(Signal &signal)
    {
        int sampleCount = signal.sampleRate * signal.duration;
        m_data.resize(sampleCount * 2); // 16-bit mono = 2 bytes per sample

        for (int i = 0; i < sampleCount; ++i)
        {
            qint16 sample = static_cast<qint16>(32767.0 *
                signal.getValue(i / static_cast<double>(signal.sampleRate)));
            m_data[2 * i] = static_cast<char>(sample & 0xFF);
            m_data[2 * i + 1] = static_cast<char>((sample >> 8) & 0xFF);
        }

        m_pos = 0;
        open(QIODevice::ReadOnly);
    }

    void stop()
    {
        close();
    }

protected:
    qint64 readData(char *data, qint64 maxlen) override
    {
        if (m_pos >= m_data.size())
            return 0;

        qint64 len = qMin(maxlen, m_data.size() - m_pos);
        memcpy(data, m_data.constData() + m_pos, len);
        m_pos += len;
        return len;
    }

    qint64 writeData(const char *, qint64) override
    {
        return 0; // Read-only
    }

    qint64 bytesAvailable() const override
    {
        return m_data.size() - m_pos + QIODevice::bytesAvailable();
    }

private:
    QByteArray m_data;
    qint64 m_pos;
};

#endif // SINEWAVEGENERATOR_H