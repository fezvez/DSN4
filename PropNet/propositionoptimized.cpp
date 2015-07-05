#include "propositionoptimized.h"

PropositionOptimized::PropositionOptimized(std::vector<std::vector<uint32_t> > v):
    andOrInputs(v)
{
    value = true;
    alreadyComputed = false;
}

PropositionOptimized::~PropositionOptimized()
{

}

//std::vector<uint32_t> & PropositionOptimized::getInputs(){
//    return allInputs;
//}

std::vector<std::vector<uint32_t> > & PropositionOptimized::getAndOrInputs(){
    return andOrInputs;
}

bool PropositionOptimized::getValue(){
    return value;
}

bool PropositionOptimized::isAlreadyComputed(){
    return alreadyComputed;
}

void PropositionOptimized::setValue(bool b){
    value = b;
}

void PropositionOptimized::setAlreadyComputed(bool b){
    alreadyComputed = b;
}

bool PropositionOptimized::computeValue(std::vector<PropositionOptimized> & allPropositions){
    if(alreadyComputed){
        return value;
    }

    alreadyComputed = true;

    if(andOrInputs.size()==0){
        return value;
    }

    bool orValue = false;
    for(uint32_t index = 0; index<andOrInputs.size(); index += 2){
        const std::vector<uint32_t> & andPropositions = andOrInputs[index];
        const std::vector<uint32_t> & andNotPropositions = andOrInputs[index+1];

        bool andValue = true;
        for(uint32_t index : andPropositions){
            if(!allPropositions[index].computeValue(allPropositions)){
                andValue = false;
                break;
            }
        }
        if(!andValue){
            continue;
        }
        for(uint32_t index : andNotPropositions){
            if(allPropositions[index].computeValue(allPropositions)){
                andValue = false;
                break;
            }
        }
        if(andValue){
            orValue = true;
            break;
        }
    }

    value = orValue;
    alreadyComputed = true;
    return orValue;
}
