#include "clusteringalgorithm.h"

ClusteringAlgorithm::ClusteringAlgorithm(): storeRanges(NULL){}

void ClusteringAlgorithm::CalculateStores(std::vector<size_t> &values)
{
}

const std::vector<StoreRange> &ClusteringAlgorithm::GetStoreRanges()
{
    return storeRanges;
}

void ClusteringAlgorithm::SetStoreRanges(std::vector<StoreRange> newStoreRanges){
    storeRanges = newStoreRanges;
}
