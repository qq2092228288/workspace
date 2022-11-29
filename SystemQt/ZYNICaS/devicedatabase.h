#ifndef DEVICEDATABASE_H
#define DEVICEDATABASE_H

#include <QObject>

class DeviceDatabase : public QObject
{
    Q_OBJECT
public:
    explicit DeviceDatabase(QObject *parent = nullptr);

signals:

};

#endif // DEVICEDATABASE_H
