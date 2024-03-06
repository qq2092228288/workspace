#include "icgcontrolmodule.h"
#include "icgcontrolmoduleprivate.h"

IcgControlModule::IcgControlModule(QObject *parent)
    : QObject{parent}
    , d_ptr{new IcgControlModulePrivate(this)}
{

}

QStringList IcgControlModule::availablePorts() const
{
    return d_ptr->availablePorts();
}

bool IcgControlModule::isOpen() const
{
    return d_ptr->isOpen();
}

bool IcgControlModule::open(const QString &portName)
{
    return d_ptr->open(portName);
}

void IcgControlModule::close()
{
    d_ptr->close();
}

void IcgControlModule::startMeasuring(const uchar &height, const uchar &weight, const Gender &gender,
                                      const uchar &age, const bool &pacemaker)
{
    d_ptr->startMeasuring(PatientData(height, weight, gender, age, pacemaker));
}

void IcgControlModule::stopMeasuring()
{
    d_ptr->stopMeasuring();
}

void IcgControlModule::retrieveBsa()
{
    d_ptr->retrieveBsa();
}

void IcgControlModule::retrieveVept()
{
    d_ptr->retrieveVept();
}

void IcgControlModule::setBp(const uchar &syst, const uchar &diast)
{
    d_ptr->setBp(BloodPressure(syst, diast));
}

void IcgControlModule::setExtVal(const uchar &cvp, const uchar &pawp, const uchar &hb, const uchar &spO2)
{
    d_ptr->setExtVal(ExternalValues(cvp, pawp, hb, spO2));
}

void IcgControlModule::normalMode()
{
    d_ptr->normalMode();
}

void IcgControlModule::powerDownMode()
{
    d_ptr->powerDownMode();
}

void IcgControlModule::userCommand(const QByteArray &control_data)
{
    d_ptr->userCommand(control_data);
}
