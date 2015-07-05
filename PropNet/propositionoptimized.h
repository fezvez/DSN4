#ifndef PROPOSITIONOPTIMIZED_H
#define PROPOSITIONOPTIMIZED_H

#include <vector>

class PropositionOptimized
{
public:
    PropositionOptimized(std::vector<std::vector<uint32_t> > v);
    ~PropositionOptimized();

//    std::vector<uint32_t> & getInputs();
    std::vector<std::vector<uint32_t> > & getAndOrInputs();
    bool getValue();
    bool isAlreadyComputed();
    bool computeValue(std::vector<PropositionOptimized> & allPropositions);

    void setValue(bool b);
    void setAlreadyComputed(bool b);


    std::vector<std::vector<uint32_t> > andOrInputs;
//    std::vector<uint32_t> allInputs;
    bool value;
    bool alreadyComputed;
};

#endif // PROPOSITIONOPTIMIZED_H
