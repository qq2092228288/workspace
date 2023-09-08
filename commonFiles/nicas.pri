#jsonlib = $$PWD/nicas/*.json

nicas.files += \
    $$PWD/nicas/hypertension.json \
    $$PWD/nicas/intensive_care_unit.json \
    $$PWD/nicas/internal_medicine.json \
    $$PWD/nicas/physical_examination.json \
    $$PWD/nicas/xprinter_report.json
INSTALLS += nicas

#!equals(_PRO_FILE_PWD_, $$DESTDIR) {
#    assetcopy.files += $$nicas.files
#} else {
#    assetcopy.files = $$jsonlib
#}
assetcopy.files += $$nicas.files

assetcopy.path = $$_PRO_FILE_PWD_
COPIES += assetcopy
