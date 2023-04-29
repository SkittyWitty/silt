/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#include "silt_m.h"
#include "configuration.h"
#include "print.h"
#include "debug.h"
#include "fawnds_factory.h"

#include <cstdio>
#include <cerrno>
#include <sstream>

#include "algofactory.h"


namespace fawn {

    FawnDS* SiltM::createSubKeyValueStore(size_t key_len, size_t size, int store_id)
    {
        // Get the configuration file for the underlying stores
        std::string underlying_kv_store_ = config_->GetStringValue("child::underlying-kv-store").c_str();

        // Add to the Configuration file the size of the store
        Configuration* sub_kv_config = new Configuration(underlying_kv_store_);
        char buf[1024];

        // Writing what the key-len will be
        if (sub_kv_config->CreateNodeAndAppend("key-len", ".") != 0)
            assert(false);
        snprintf(buf, sizeof(buf), "%zu", key_len);
        if (sub_kv_config->SetStringValue("key-len", buf) != 0)
            assert(false);

        if (sub_kv_config->CreateNodeAndAppend("id", ".") != 0)
            assert(false);
        snprintf(buf, sizeof(buf), "%d", store_id);
        if (sub_kv_config->SetStringValue("id", buf) != 0)
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

    FawnDS_Return SiltM::Create(vector<size_t>& key_lens)
    {
        // Clusters a list of keys
        std::string algotype = config_->GetStringValue("child::cluster-algo");
        ClusteringAlgorithm *clusterAlgo = AlgoFactory::New(algotype);
        clusterAlgo->CalculateStores(key_lens);
        std::vector<StoreRange> storeRanges = clusterAlgo->GetStoreRanges();

        for(int i = 0; i < storeRanges.size(); i++) {
            StoreRange currentRange = storeRanges[i];
            size_t min_key_len = currentRange.start;
            size_t max_key_len = currentRange.end;

            size_t key_value_store_size = 1000000; // default value for now
            int store_id = i+1;

            // Create a store based on the max key value
            FawnDS* store = createSubKeyValueStore(max_key_len, key_value_store_size, store_id);
            storeSizes.push_back(max_key_len);

            PopulateStoreMapping(store, min_key_len, max_key_len);
        }

        delete clusterAlgo;
    }

    void SiltM::PopulateStoreMapping(FawnDS* dataStore, size_t start, size_t end)
    {
        for (size_t i = start; i <= end; i++){
            dataStoreMap[i] = dataStore;
        }
    }

    FawnDS_Return SiltM::Status(const FawnDS_StatusType& type, Value& status) const
    {
        
    }

    std::vector<size_t> SiltM::GetKeyLengths()
    {
        return std::vector<size_t>(storeSizes);
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
