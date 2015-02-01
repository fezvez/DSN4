#include "gdl.h"

QString GDL::toString() const{
    return name;
}

GDL::~GDL(){

}

QMap<QString, GDL::GDL_TYPE> GDL::mapString2GDLType = QMap<QString, GDL::GDL_TYPE>();
QMap<GDL::GDL_TYPE, QString> GDL::mapGDLType2String = QMap<GDL::GDL_TYPE, QString>();

QMap<QString, GDL::GDL_QUALIFIER> GDL::mapString2GDLQualifier = QMap<QString, GDL::GDL_QUALIFIER>();
QMap<GDL::GDL_QUALIFIER, QString> GDL::mapGDLQualifier2String = QMap<GDL::GDL_QUALIFIER, QString>();

bool GDL::useSkolemNames = false;
