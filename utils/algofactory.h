#ifndef _ALGO_FACTORY_H_
#define _ALGO_FACTORY_H_

#include "sortingalgorithm.h"
#include <string>

class AlgoFactory {
    public:
        static SortingAlgorithm* New(std::string algo_name);
};


#endif  // #ifndef _ALGO_FACTORY_H_
