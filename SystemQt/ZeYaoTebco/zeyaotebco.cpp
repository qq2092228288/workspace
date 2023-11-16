#include "zeyaotebco.h"
#include "zeyaotebcoprivate.h"

ZeYaoTebco::ZeYaoTebco()
    : d_ptr(new ZeYaoTebcoPrivate)
{

}

ZeYaoTebco::~ZeYaoTebco()
{
    if (d_ptr != nullptr) {
        delete d_ptr;
        d_ptr = nullptr;
    }
}

void ZeYaoTebco::release()
{
    if (d_ptr != nullptr) {
        delete d_ptr;
        d_ptr = nullptr;
    }
}

int ZeYaoTebco::login(std::string deviceId, std::string password)
{
    return d_ptr->login(deviceId, password);
}

bool ZeYaoTebco::openSerialPort(std::string portName)
{
    return d_ptr->openSerialPort(portName);
}

int ZeYaoTebco::start(const std::string &name, const std::string &id,
                      int gender, int age, int height, int weight)
{
    return d_ptr->start(name, id, gender, age, height, weight);
}

int ZeYaoTebco::appendBpAndPosition(int sbp, int dbp, int position)
{
    return d_ptr->appendBpAndPosition(sbp, dbp, position);
}

int ZeYaoTebco::end(int sbp, int dbp, int position)
{
    return d_ptr->end(sbp, dbp, position);
}

void ZeYaoTebco::closeSerialPort()
{
    d_ptr->closeSerialPort();
}

std::string ZeYaoTebco::readAll()
{
    return d_ptr->readAll();
}

bool ZeYaoTebco::serverIsConnected()
{
    return d_ptr->serverIsConnected();
}

bool ZeYaoTebco::serialPortIsOpen()
{
    return d_ptr->serialPortIsOpen();
}

bool ZeYaoTebco::isDetecting()
{
    return d_ptr->isDetecting();
}

int ZeYaoTebco::availableQuantity()
{
    return d_ptr->availableQuantity();
}

std::string ZeYaoTebco::availablePorts()
{
    return d_ptr->availablePorts();
}

std::string ZeYaoTebco::serialPortErrorMsg()
{
    return d_ptr->serialPortErrorMsg();
}
