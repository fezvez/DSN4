#ifndef GDL_TOOLS_H
#define GDL_TOOLS_H

#include "gdl_constant.h"

class GDL_Tools{
public:
    static QString printMembers(QSet<PConstant> set){
        QString membersString;
        for(PConstant constant : set){
            membersString = membersString + constant->toString() + " ";
        }
        return membersString;
    }
};
#endif // GDL_TOOLS_H
