
#ifndef _SILT_M_H_
#define _SILT_M_H_

#include "print.h"
#include "debug.h"
#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

#include "basic_types.h"
#include "fawnds_factory.h"

namespace fawn {

    // configuration
    //   <type>: "silt_m"
    //   <underlying_store>: The Database that will be used underlying the key-value store

    class SiltM : public FawnDS {
    public:
        //FawnDS_Return MeanshiftCreate(vector<vector<double>>& points);
        FawnDS_Return Create(vector<size_t>& key_lens);
        FawnDS_Return Put(const ConstValue& key, const ConstValue& data);
        FawnDS_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;
        FawnDS_Return Status(const FawnDS_StatusType& type, Value& status) const;

    private:
        FawnDS* createSubKeyValueStore(size_t key_len, size_t size);
        void PopulateStoreMapping(FawnDS* datastore, size_t start, size_t end);
        std::unordered_map<size_t, FawnDS*> dataStoreMap;
        //std::vector<size_t> storeSizes;
        int numberOfStores = 0;
        //SortingAlgorithm sortingAlgo_;
    };
} // namespace fawn

#endif  // #ifndef _SILT_M_H_