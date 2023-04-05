/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#include "silt_m.h"
#include "configuration.h"
#include "print.h"
#include "file_io.h"
#include "debug.h"
#include "file_store.h"

#include "hash_table_cuckoo.h"
#include "hash_functions.h"
#include "hashutil.h"

#include <cstdio>
#include <cerrno>
#include <sstream>

namespace fawn {

    SiltM::SiltM()
    {
    }

    SiltM::~SiltM()
    {
    }

    FawnDS_Return SiltM::Create()
    {
    }

    FawnDS_Return SiltM::Open()
    {  
    }

    FawnDS_Return SiltM::ConvertTo(FawnDS* new_store) const
    {
    }

    FawnDS_Return SiltM::Flush()
    {
    }

    FawnDS_Return SiltM::Close()
    {
    }

    FawnDS_Return SiltM::Destroy()
    {
    }

    FawnDS_Return SiltM::Status(const FawnDS_StatusType& type, Value& status) const
    {
    }

    FawnDS_Return SiltM::Put(const ConstValue& key, const ConstValue& data)
    {
    }

    FawnDS_ConstIterator SiltM::Enumerate() const
    {
    }

    FawnDS_Iterator SiltM::Enumerate()
    {
    }

    FawnDS_ConstIterator SiltM::Find(const ConstValue& key) const
    {
    }

    FawnDS_Iterator SiltM::Find(const ConstValue& key)
    {
    }
} // namespace fawn
