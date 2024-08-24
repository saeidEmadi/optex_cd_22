#include "optex_cd_22.h"

optex_cd_22::optex_cd_22(QObject *parent)
    : QObject{parent}
    , m_serialPort(nullptr)
    , m_value(0)
    , m_errorMessage(tr(""))
{
}

optex_cd_22::~optex_cd_22()
{
    delete m_serialPort;
}

void optex_cd_22::initial(const QString &portName, qint32 baudRate,
    QSerialPort::DataBits dataLength, QSerialPort::StopBits stopLength, QSerialPort::Parity parityCheck, QString *error)
{
    if (m_serialPort != nullptr)
    {
        m_serialPort->close();
        delete m_serialPort;
    }

    m_serialPort = new QSerialPort(this);

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(dataLength);
    m_serialPort->setStopBits(stopLength);
    m_serialPort->setParity(parityCheck);

    if (!m_serialPort->open(QIODevice::ReadWrite))
    {
        if (error != nullptr)
        {
            *error = QString("cant read write Serial Port :: portname");
        }
    }
}

void optex_cd_22::write(int16_t STX, int16_t command, int16_t data1, int16_t data2, int16_t ETX)
{
    QByteArray data = packData(STX, command, data1, data2, ETX);

    m_serialPort->write(data);
    m_serialPort->flush();
}

void optex_cd_22::write(int16_t STX, int16_t command, int16_t data1, int16_t data2, int16_t ETX, int16_t BCC)
{
    QByteArray data = packData(STX, command, data1, data2, ETX, BCC);

    m_serialPort->write(data);
    m_serialPort->flush();
}

double optex_cd_22::value(QString *error)
{
    this->write(0x02, 0x43, 0xB0, 0x01, 0x03);

    if (m_serialPort->waitForReadyRead())
    {
        this->receiveData();
    }

    if (m_errorMessage != "")
    {
        if (error != nullptr)
        {
            *error = m_errorMessage;
        }

        return -1;
    }

    return m_value;
}
void optex_cd_22::close()
{
    m_serialPort->close();
}

void optex_cd_22::receiveData()
{
    QByteArray response = m_serialPort->readAll();
    this->dataInterpretation(response);
}

int optex_cd_22::xorCalculate(int16_t STX, int16_t command, int16_t data1, int16_t data2, int16_t ETX)
{
    return (STX ^ command ^ data1 ^ data2 ^ ETX) - 1;
}

const QByteArray optex_cd_22::packData(int16_t STX, int16_t command, int16_t data1, int16_t data2, int16_t ETX, int16_t BCC)
{
    if (BCC == -1)
    {
        BCC = xorCalculate(STX, command, data1, data2, ETX);
    }

    QByteArray data;
    data.append(STX);
    data.append(command);
    data.append(data1);
    data.append(data2);
    data.append(ETX);
    data.append(BCC);

    return data;
}

void optex_cd_22::dataInterpretation(QByteArray data)
{
    int ACK = data.at(1);

    int RESPONSE1 = data.at(2);

    if (ACK == 6)
    {
        calculateValue(data.at(2), data.at(3));
    }
    else if (ACK == 21)
    {
        if (RESPONSE1 == 2)
        {
            m_errorMessage = QString("Address is invalid");
        }
        else if (RESPONSE1 == 4)
        {
            m_errorMessage = QString("BCC value is invalid");
        }
        else if (RESPONSE1 == 5)
        {
            m_errorMessage = QString("Invalid command is issued except \"C\", \"W\", \"R\"");
        }
        else if (RESPONSE1 == 6)
        {
            m_errorMessage = QString("Setting value is invalid (out of specifications)");
        }
        else if (RESPONSE1 == 7)
        {
            m_errorMessage = QString("Setting value is invalid (out of range)");
        }
        else
        {
            m_errorMessage = QString("Error code :: %1").arg(RESPONSE1);
        }
    }
    else
    {
        m_errorMessage = QString("Invalid Command");
    }
}

void optex_cd_22::calculateValue(quint8 data1, quint8 data2)
{
    double result = 0;

    if (data1 > 0xef)
    {
        data1 = 0xff - data1;
        data2 = 0xff - data2;
        result = (data1 * 256) + data2;
        result = -1 * result - 1;
    }
    else
    {
        result = (data1 * 256) + data2;
    }

    result = (result / measurementValueScaling) + centerOfMeasurementRange;

    m_value = result;
}
