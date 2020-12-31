#ifndef RLAPI_CONFIGURATIONOPTIMIZER_H
#define RLAPI_CONFIGURATIONOPTIMIZER_H

#include <rl/plan/VectorList.h>
#include <rl/mdl/Dynamic.h>

class RLAPI_ConfigurationOptimizer
{
public:
    RLAPI_ConfigurationOptimizer(){
    }

    void SetStartConfigurations(const rl::math::Vector &start) {
        StartConfig = start;
    }
    void SetEndConfigurations(const rl::plan::VectorList &end) {
        EndConfigs = end;
    }

    rl::plan::VectorList Process()
    {
        rl::plan::VectorList Result;
        rl::plan::VectorList::iterator indexLast;

        while(!EndConfigs.empty())
        {
            indexLast = findClosest(StartConfig);
            StartConfig = *indexLast;
            Result.push_back(*indexLast);
            EndConfigs.erase(indexLast);
        }
        return Result;
    }

    rl::mdl::Dynamic *theDynamic;

private:
    rl::plan::VectorList::iterator findClosest(const rl::math::Vector &thePos)
    {
        rl::plan::VectorList::iterator i = EndConfigs.begin();
        rl::plan::VectorList::iterator index;
        rl::math::Real dis = std::numeric_limits<rl::math::Real>::max();
        for( ; i!=EndConfigs.end(); ++i)
        {
            rl::math::Real tmp = theDynamic->distance(thePos, *i);
            if(tmp < dis)
            {
                dis = tmp;
                index = i;
            }
        }
        return index;
    }

    rl::math::Vector StartConfig;
    rl::plan::VectorList EndConfigs;
};

#endif // RLAPI_CONFIGURATIONOPTIMIZER_H
