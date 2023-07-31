#include "adsmodule.h"
#include "adsmoduleprivate.h"

AdsModule::AdsModule(QObject *parent)
    : QObject{parent},
      d_ptr{new AdsModulePrivate(this)}
{

}

bool AdsModule::open(const QString &portname)
{
    return d_ptr->open(portname);
}

void AdsModule::close()
{
    d_ptr->close();
}

QStringList AdsModule::portNames()
{
    return d_ptr->portNames();
}

