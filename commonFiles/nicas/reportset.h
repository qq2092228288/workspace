#ifndef REPORTSET_H
#define REPORTSET_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

enum Printer_Type
{
    General,    // A4纸
    Thermal     // xprinter热敏纸
};

enum Check_Mode
{
    Hypertension,           // 高血压模式
    InternalMedicine,       // 内科模式
    IntensiveCareUnit,      // 重症模式
    PhysicalExamination     // 体检模式
};

class ReportParameters
{
public:
    static QJsonArray array(Check_Mode mode)
    {
        QFile file;
        switch (mode) {
        case Check_Mode::Hypertension:
            file.setFileName(":/hypertension.json");
            break;
        case Check_Mode::InternalMedicine:
            file.setFileName(":/internal_medicine.json");
            break;
        case Check_Mode::IntensiveCareUnit:
            file.setFileName(":/intensive_care_unit.json");
            break;
        case Check_Mode::PhysicalExamination:
            file.setFileName(":/physical_examination.json");
            break;
        }
        if (!file.open(QIODevice::ReadOnly)) {
            return QJsonArray();
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        return doc.array();
    }
    static QJsonArray xprinter()
    {
        QFile file(":/xprinter_report.json");
        if (!file.open(QIODevice::ReadOnly)) {
            return QJsonArray();
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        return doc.array();
    }
    static QJsonObject find(int type)
    {
        auto arr = array(Check_Mode::IntensiveCareUnit);
        foreach (auto group, arr) {
            auto parameters = group.toObject().value("parameters").toArray();
            foreach (auto value, parameters) {
                auto parameter = value.toObject();
                if (parameter.value("type").toInt() == type) {
                    return parameter;
                }
            }
        }
        return QJsonObject();
    }
};

#endif // REPORTSET_H