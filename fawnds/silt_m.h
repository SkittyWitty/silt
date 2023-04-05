
#ifndef _SILT_M_H_
#define _SILT_M_H_

#include "print.h"
#include "debug.h"
#include <cassert>
#include <string>

#include "basic_types.h"
#include "hash_table_cuckoo.h"
#include "fawnds.h"

namespace fawn {

    class HashTableCuckoo;
    class Configuration;
    // configuration
    //   <type>: "silt_m"
    //   <id>: the ID of the file store
    //   <file>: the file name prefix to store the hash table for startup 
    //   <hash-table-size>: the number of entries that the hash table is expected to hold
    //   <use-offset>: 1 (default): use an explicit offset field of 4 bytes
    //                 0: do not use offsets; a location in the hash table becomes an offset

    class SiltM : public FawnDS {
    public:
        SiltM();
        virtual ~SiltM();

        virtual FawnDS_Return Create();
        virtual FawnDS_Return Open();

        virtual FawnDS_Return ConvertTo(FawnDS* new_store) const;

        virtual FawnDS_Return Flush();
        virtual FawnDS_Return Close();

        virtual FawnDS_Return Destroy();

        virtual FawnDS_Return Status(const FawnDS_StatusType& type, Value& status) const;

        virtual FawnDS_Return Put(const ConstValue& key, const ConstValue& data);
        virtual FawnDS_Return Append(Value& key, const ConstValue& data);

        // ~Functions that are not supported~
        // virtual FawnDS_Return Delete(const ConstValue& key);

        virtual FawnDS_Return Contains(const ConstValue& key) const;
        virtual FawnDS_Return Length(const ConstValue& key, size_t& len) const;
        virtual FawnDS_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;

        virtual FawnDS_ConstIterator Enumerate() const;
        virtual FawnDS_Iterator Enumerate();

        virtual FawnDS_ConstIterator Find(const ConstValue& key) const;
        virtual FawnDS_Iterator Find(const ConstValue& key);

    private:
        HashTableCuckoo silt;
    };

} // namespace fawn

#endif  // #ifndef _SILT_M_H_