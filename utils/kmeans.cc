#include "kmeans.h"
#include <random>
#include <algorithm>

void Kmeans::CalculateStores(std::vector<size_t> &values)
{
    std::sort(values.begin(), values.end());  // sort the vector in ascending order
    std::vector<size_t> cluster_1;
    std::vector<size_t> cluster_2;

    Cluster(values, cluster_1, cluster_2);

    // Set KeyValue Store Sizes
    int last_small_index = cluster_1.size() - 1;
    small_store.start = cluster_1.at(0);
    small_store.end = cluster_1.at(last_small_index);

    int last_big_index = cluster_2.size() - 1;
    big_store.start = cluster_2.at(0);
    big_store.end = cluster_2.at(last_big_index);

    std::vector<StoreRange> ranges;
    ranges.push_back(small_store);
    ranges.push_back(big_store);

    SetStoreRanges(ranges);
}

void Kmeans::Cluster(const std::vector<size_t>& input_vector, std::vector<size_t>& cluster_1, std::vector<size_t>& cluster_2) {
    // Initialize the means of the two clusters randomly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, input_vector.size() - 1);
    size_t mean_1 = input_vector[dist(gen)];
    size_t mean_2 = input_vector[dist(gen)];

    // Repeat until convergence
    while (true) {
        // Assign each element of the input vector to the closest mean
        cluster_1.clear();
        cluster_2.clear();
        for (size_t i : input_vector) {
            if (abs(i - mean_1) < abs(i - mean_2)) {
                cluster_1.push_back(i);
            } else {
                cluster_2.push_back(i);
            }
        }

        // Update the means of the clusters
        int new_mean_1 = 0;
        int new_mean_2 = 0;
        if (!cluster_1.empty()) {
            new_mean_1 = std::accumulate(cluster_1.begin(), cluster_1.end(), 0) / cluster_1.size();
        }
        if (!cluster_2.empty()) {
            new_mean_2 = std::accumulate(cluster_2.begin(), cluster_2.end(), 0) / cluster_2.size();
        }

        // Check for convergence
        if (new_mean_1 == mean_1 && new_mean_2 == mean_2) {
            break;
        } else {
            mean_1 = new_mean_1;
            mean_2 = new_mean_2;
        }
    }
}
