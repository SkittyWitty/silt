#include "basic_types.h"
#include "fawnds.h"

namespace fawn {
    class IntRange {
        public:
            IntRange(int start, int end);
            
            bool operator<  (const IntRange& other) const;
            
            bool operator<= (const IntRange& other) const;
            
            bool operator== (const IntRange& other) const;
            
            bool operator!= (const IntRange& other) const;
            
            bool operator>  (const IntRange& other) const;
            
            bool operator>= (const IntRange& other) const;
            
        private:
            int start_;
            int end_;
    };
}
