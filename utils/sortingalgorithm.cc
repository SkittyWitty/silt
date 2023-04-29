#include "sortingalgorithm.h"

SortingAlgorithm::SortingAlgorithm(): storeRanges(NULL){}

void SortingAlgorithm::CalculateStores(std::vector<size_t> &values)
{
}

const std::vector<StoreRange> &SortingAlgorithm::GetStoreRanges()
{
    return storeRanges;
}

void SortingAlgorithm::SetStoreRanges(std::vector<StoreRange> newStoreRanges){
    storeRanges = newStoreRanges;
}
