#include "median.h"

#include <iostream>
#include <algorithm>

using namespace std;

void Median::CalculateStores(std::vector<size_t>& values){
    std::sort(values.begin(), values.end());  // sort the vector in ascending order

    int size = values.size();

    if (size % 2 == 0) { // if the size is even
        int mid_index_1 = size / 2 - 1;
        int mid_index_2 = size / 2;
        small_store.end = (size_t)(values[mid_index_1] + values[mid_index_2]) / 2;
    }
    else { // if the size is odd
        int mid_index = size / 2;
        small_store.end = values[mid_index];
    }

    small_store.start = values[0];
    int last_index = size - 1;
    big_store.end = values[last_index];

    vector<StoreRange> ranges;
    ranges.push_back(small_store);
    ranges.push_back(big_store);

    SetStoreRanges(ranges);
}