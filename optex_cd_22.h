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

    void write(uint8_t STX, uint8_t command, uint8_t data1, uint8_t data2, uint8_t ETX);
    void write(uint8_t STX, uint8_t command, uint8_t data1, uint8_t data2, uint8_t ETX, uint8_t BCC);

    double value(QString *error = nullptr);

    void close();

private:

    void receiveData();

    int xorCalculate(uint8_t STX, uint8_t command, uint8_t data1, uint8_t data2, uint8_t ETX);

    const QByteArray packData(uint8_t STX, uint8_t command, uint8_t data1, uint8_t data2, uint8_t ETX, uint8_t BCC = NULL);

    void dataInterpretation(QByteArray data);

    void calculateValue(uint8_t data1, uint8_t data2);

private:
    QSerialPort *m_serialPort;
    double m_value;
    QString m_errorMessage;
};

#endif // OPTEX_CD_22_H
