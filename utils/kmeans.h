#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <vector>
#include "clusteringalgorithm.h"

class Kmeans : public ClusteringAlgorithm {
public:
    void CalculateStores(std::vector<size_t>& values);

private:
    void Cluster(const std::vector<size_t>& input_vector, std::vector<size_t>& cluster_1, std::vector<size_t>& cluster_2);

    StoreRange small_store;
    StoreRange big_store;
};

#endif // #ifndef _KMEANS_H_
