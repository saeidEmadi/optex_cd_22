#ifndef OPTEX_CD_22_H
#define OPTEX_CD_22_H

#include <QObject>

#include <QSerialPort>

#define centerOfMeasurementRange    35.0
#define measurementValueScaling     100.0

class optex_cd_22 : public QObject
{
    Q_OBJECT
public:
    explicit optex_cd_22(QObject *parent = nullptr);
    ~optex_cd_22();

public:
    void initial(const QString &portName, qint32 baudRate, QSerialPort::DataBits dataLength,
        QSerialPort::StopBits stopLength, QSerialPort::Parity parityCheck, QString *error = nullptr);

    void write(int16_t STX, int16_t command, int16_t data1, int16_t data2, int16_t ETX);
    void write(int16_t STX, int16_t command, int16_t data1, int16_t data2, int16_t ETX, int16_t BCC);

    double value(QString *error = nullptr);

    void close();

private:

    void receiveData();

    int xorCalculate(int16_t STX, int16_t command, int16_t data1, int16_t data2, int16_t ETX);

    const QByteArray packData(int16_t STX, int16_t command, int16_t data1, int16_t data2, int16_t ETX, int16_t BCC = -1);

    void dataInterpretation(QByteArray data);

    void calculateValue(quint8 data1, quint8 data2);

private:
    QSerialPort *m_serialPort;
    double m_value;
    QString m_errorMessage;
};

#endif // OPTEX_CD_22_H
