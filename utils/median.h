#ifndef _MEDIAN_H_
#define _MEDIAN_H_

#include <vector>
#include "clusteringalgorithm.h"

class Median : public ClusteringAlgorithm {
public:
    void CalculateStores(std::vector<size_t>& values);

private:
    StoreRange small_store;
    StoreRange big_store;
};

#endif // #ifndef _MEDIAN_H_
