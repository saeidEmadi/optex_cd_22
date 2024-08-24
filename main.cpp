#include <QCoreApplication>

#include "optex_cd_22.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString error;

    optex_cd_22 *optexObj = new optex_cd_22();

    optexObj->initial("COM5", 9600, QSerialPort::DataBits::Data8, QSerialPort::StopBits::OneStop, QSerialPort::Parity::NoParity);

    qInfo() << optexObj->value(&error);

    if (error != "")
    {
        qInfo() << error;
    }

    return a.exec();
}
