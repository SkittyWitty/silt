#include "intrange.h"

namespace fawn {

    IntRange::IntRange(int start, int end) : start_(start), end_(end) {}

    bool IntRange::operator<(const IntRange& other) const 
    {
        return start_ < other.start_ || (start_ == other.start_ && end_ < other.end_);
    }

    bool IntRange::operator<=(const IntRange& other) const {
        return *this < other || *this == other;
    }

    bool IntRange::operator==(const IntRange& other) const {
        return start_ == other.start_ && end_ == other.end_;
    }

    bool IntRange::operator!=(const IntRange& other) const {
        return !(*this == other);
    }

    bool IntRange::operator>(const IntRange& other) const {
        return other < *this;
    }

    bool IntRange::operator>=(const IntRange& other) const {
        return other <= *this;
    }
}
        