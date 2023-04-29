#include "median.h"

#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

void Median::CalculateStores(std::vector<size_t>& values){
    std::sort(values.begin(), values.end());  // sort the vector in ascending order

    int size = values.size();

    // Set small store
    int mid_index = floor(size / 2.0);
    int last_index = size - 1;

    small_store.start = values[0];
    small_store.end = values[mid_index];

    big_store.start = values[mid_index+1];
    big_store.end = values[last_index];

    vector<StoreRange> ranges;
    ranges.push_back(small_store);
    ranges.push_back(big_store);

    SetStoreRanges(ranges);
}