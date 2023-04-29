/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include "algofactory.h"
#include "meanshift.h"
#include "median.h"

SortingAlgorithm* AlgoFactory::New(std::string algoName)
{
    SortingAlgorithm* sortAlgo = NULL;

    if (algoName == "Median")
         sortAlgo = new Median();
    else
        return nullptr;

    return sortAlgo;
}


