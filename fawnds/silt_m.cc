/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#include "silt_m.h"
#include "configuration.h"
#include "print.h"
#include "debug.h"
#include "fawnds_factory.h"

#include "intrange.h"
#include "meanshift.h"

#include <cstdio>
#include <cerrno>
#include <sstream>

namespace fawn {

    SiltM::SiltM(){}

    FawnDS* SiltM::createSubKeyValueStore(size_t key_len, size_t size)
    {
        // Add to the Configuration file the size of the store

        Configuration* sub_kv_config = new Configuration();
        char buf[1024];

        // Writing what the key-len will be
        if (sub_kv_config->CreateNodeAndAppend("key-len", ".") != 0)
            assert(false);
        snprintf(buf, sizeof(buf), "%zu", key_len);
        if (sub_kv_config->SetStringValue("key-len", buf) != 0)
            assert(false);

        // Writing that this is a temp file
        if (sub_kv_config->CreateNodeAndAppend("temp-file", ".") != 0)
            assert(false);
        if (sub_kv_config->SetStringValue("temp-file", "/tmp") != 0)
            assert(false);

        FawnDS* sub_kv = FawnDS_Factory::New(sub_kv_config);
        if (!sub_kv) {
            assert(false);
            delete sub_kv_config;
            return NULL;
        }
        if (sub_kv->Create() != OK) {
            assert(false);
            delete sub_kv;
            return NULL;
        }

        return sub_kv;
    }

    

    FawnDS_Return SiltM::Create(vector<vector<double>>& points, int kernel_bandwidth)
    {
        // Clusters a list of keys
        MeanShift *meanShift = new MeanShift();
        vector<Cluster> clusters = meanShift->cluster(points, kernel_bandwidth);

        // Traverse the list of clusters. 
        for(int i = 0; i < clusters.size(); i++) {
            // Get the largest value in that cluster
            int max_key_len = clusters[i].maxX;
            int min_key_len = clusters[i].minX;
            // Create a store based on that value

            FawnDS* store = createSubKeyValueStore(max_key_len, sizeof(clusters[i]) * 2);

            PopulateStore(store, min_key_len, max_key_len);
        }
    }

    void SiltM::PopulateStore(FawnDS* dataStore, size_t start, size_t end)
    {
        for (size_t i = start; i <= end; i++){
            dataStoreMap[i] = dataStore;
        }
    }

    FawnDS_Return SiltM::Status(const FawnDS_StatusType& type, Value& status) const
    {
        for (auto it = dataStoreMap.begin(); it != dataStoreMap.end(); ++it) {
            // For each data store get the status
        }
    }

    FawnDS_Return SiltM::Put(const ConstValue& key, const ConstValue& data)
    {
        size_t key_size = key.size();
        FawnDS* kv_store = dataStoreMap.at(key_size);
        FawnDS_Return put_status = kv_store->Put(key, data);
    }


    FawnDS_Return SiltM::Get(const ConstValue& key, Value& data, size_t offset, size_t len) const
    {
        size_t key_size = key.size();
        FawnDS* kv_store = dataStoreMap.at(key_size);
        FawnDS_Return get_status = kv_store->Get(key, data);

        return get_status;
    }
} // namespace fawn
