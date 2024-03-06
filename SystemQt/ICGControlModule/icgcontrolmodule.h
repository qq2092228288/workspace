#ifndef ICGCONTROLMODULE_H
#define ICGCONTROLMODULE_H

#include <QtCore/qglobal.h>
#include <QObject>

#if defined(ICGCONTROLMODULE_LIBRARY)
#define ICGCONTROLMODULE_EXPORT Q_DECL_EXPORT
#else
#define ICGCONTROLMODULE_EXPORT Q_DECL_IMPORT
#endif

class IcgControlModulePrivate;

class ICGCONTROLMODULE_EXPORT IcgControlModule : public QObject
{
    Q_OBJECT
public:
    IcgControlModule(QObject *parent = nullptr);
    enum Gender : char
    {
        Male = 0,
        Female = 1
    };
    enum ParameterType : char
    {
        Standard,
        Extended
    };
    Q_ENUM(ParameterType)
public:
    QStringList availablePorts() const;
    bool isOpen() const;
public Q_SLOTS:
    bool open(const QString &portName);
    void close();
    void startMeasuring(const uchar &height, const uchar &weight, const Gender &gender,
                        const uchar &age, const bool &pacemaker = false);
    void stopMeasuring();
    void retrieveBsa();
    void retrieveVept();
    void setBp(const uchar &syst, const uchar &diast);
    void setExtVal(const uchar &cvp, const uchar &pawp, const uchar &hb, const uchar &spO2);
    void normalMode();
    void powerDownMode();
    void userCommand(const QByteArray &control_data);
Q_SIGNALS:
    void ecg(const short &);
    void icg(const short &);
    void data(const ParameterType &type, const QVector<float> &vect);
    void bsa(const double &);
    void vept(const double &);
    void error(const char &ctrl, const uchar &err);
private:
    IcgControlModulePrivate *d_ptr;
};

#endif // ICGCONTROLMODULE_H
