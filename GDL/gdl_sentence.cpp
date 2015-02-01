#include "gdl_sentence.h"


QSet<PConstant> GDL_Sentence::getDependentConstants(){
    return dependentConstants;
}

QSet<PConstant> GDL_Sentence::getDependentConstantsNegative(){
    return dependentConstantsNegative;
}

