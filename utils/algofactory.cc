/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include "algofactory.h"
#include "meanshift.h"
#include "median.h"

ClusteringAlgorithm* AlgoFactory::New(std::string algoName)
{
    ClusteringAlgorithm* sortAlgo = NULL;

    if (algoName == "Median")
         sortAlgo = new Median();
    else
        return nullptr;

    return sortAlgo;
}


