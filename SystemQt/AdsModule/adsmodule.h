#ifndef ADSMODULE_H
#define ADSMODULE_H

#include <QtCore/qglobal.h>

#if defined(ADSMODULE_LIBRARY)
#  define ADSMODULE_EXPORT Q_DECL_EXPORT
#else
#  define ADSMODULE_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>

class AdsModulePrivate;
class ADSMODULE_EXPORT AdsModule : public QObject
{
    Q_OBJECT
public:
    enum MessageDataType
    {
        Header_I    =  0, // 0-2     header of the first frame data
        Length_I    =  3, // 3       length of the first frame data
        LeadWireI   =  4, // 4-7
        LeadWireII  =  8, // 8-11
        LeadWireV1  = 12, // 12-15
        LeadWireV2  = 16, // 16-19
        LeadWireV3  = 20, // 20-23
        LeadWireV4  = 24, // 24-27
        LeadWireV5  = 28, // 28-31
        LeadWireV6  = 32, // 32-35
        Checksums_I = 36, // 36      checksums of the first frame data
        AddCheck_I  = 37, // 37      additional verification of the first frame data
        Header_II   = 38, // 38-40   header of the second frame data
        Length_II   = 41, // 41      length of the second frame data
        LeadWireIII = 42, // 42-45
        LeadWireAVR = 46, // 46-49
        LeadWireAVL = 50, // 50-53
        LeadWireAVF = 54, // 54-57
        DetSign     = 58, // 58-59   Lead wire detachment sign
        IncTestData = 60, // 60      incremental test data
        Checksums_II= 61, // 61      checksums of the second frame data
        AddCheck_II = 62  // 62      additional verification of the second frame data
    };
    Q_ENUM(MessageDataType)
    enum MessageError
    {
        FrameHeaderError,
        ChecksumsError,
        AddCheckError
    };
    Q_ENUM(MessageError)
public:
    AdsModule(QObject *parent = nullptr);
public:
    bool open(const QString &portname);
    void close();
    QStringList portNames();
Q_SIGNALS:
    void newdata(AdsModule::MessageDataType type, double voltage);
    void invalid(AdsModule::MessageError error, QByteArray msg);
    void connectFailed();
private:
    AdsModulePrivate *d_ptr;
};

#endif // ADSMODULE_H
