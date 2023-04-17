
#ifndef _SILT_M_H_
#define _SILT_M_H_

#include "print.h"
#include "debug.h"
#include <cassert>
#include <string>
#include <unordered_map>

#include "basic_types.h"
#include "fawnds_factory.h"

namespace fawn {

    // configuration
    //   <type>: "silt_m"
    //   <underlying_store>: The Database that will be used underlying the key-value store

    class SiltM : public FawnDS {
    public:
        SiltM();

        FawnDS_Return Create(vector<vector<double>>& points, int kernel_bandwidth);
        virtual FawnDS_Return Status(const FawnDS_StatusType& type, Value& status) const;

        virtual FawnDS_Return Put(const ConstValue& key, const ConstValue& data);
        virtual FawnDS_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;

    private:
        FawnDS* createSubKeyValueStore(size_t key_len, size_t size);
        void PopulateStore(FawnDS* datastore, size_t start, size_t end);
        std::unordered_map<size_t, FawnDS*> dataStoreMap;
    };
} // namespace fawn

#endif  // #ifndef _SILT_M_H_