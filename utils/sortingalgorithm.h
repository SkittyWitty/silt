#ifndef _SORTINGALGO_H_
#define _SORTINGALGO_H_

#include <vector>

struct StoreRange {
    size_t start;
    size_t end;
};

class SortingAlgorithm {
public:
    SortingAlgorithm();
    virtual ~SortingAlgorithm() {};
    virtual void CalculateStores(std::vector<size_t>& values);
    const std::vector<StoreRange>& GetStoreRanges();

protected:
    void SetStoreRanges(std::vector<StoreRange> newStoreRanges);

    std::vector<StoreRange> storeRanges;
};

#endif // #ifndef _SORTINGALGO_H_
