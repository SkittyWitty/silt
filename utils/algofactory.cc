/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include "algofactory.h"
#include "meanshift.h"
#include "median.h"
#include "kmeans.h"

ClusteringAlgorithm* AlgoFactory::New(std::string algoName)
{
    ClusteringAlgorithm* sortAlgo = NULL;

    if (algoName == "median")
    {
         sortAlgo = new Median();
    }
    else if (algoName == "kmeans")
    {
         sortAlgo = new Kmeans();
    }
    else 
    {
        return nullptr;
    }

    return sortAlgo;
}


