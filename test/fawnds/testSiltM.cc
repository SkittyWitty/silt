#include "fawnds_factory.h"
#include "rate_limiter.h"
#include "silt_m.h"

#include <gtest/gtest.h>
#include <cstdlib>
#include <cassert>
#include <algorithm>

struct kv_pair {
	fawn::Value key;
	fawn::Value data;
};
typedef std::vector<kv_pair> kv_array_type;

std::vector<std::vector<double>> load_key_size_densities(const char *filename) {
    std::vector<std::vector<double>> KeySizeDensities;
    FILE *fp = fopen(filename, "r");
    char line[50];
    while (fgets(line, sizeof(line), fp) != NULL) {
        double x, y;
        char *x_str = line;
        char *y_str = line;
        while (*y_str != '\0') {
            if (*y_str == ',') {
                *y_str++ = 0;
                x = atof(x_str);
                y = atof(y_str);
                std::vector<double> point;
                point.push_back(x);
                point.push_back(y);
                KeySizeDensities.push_back(point);
                break;
            }
            ++y_str;
        }
    }
    fclose(fp);
    return KeySizeDensities;
}

static void generate_random_kv(kv_array_type& out_arr, size_t key_len, size_t data_len, size_t size, unsigned int seed = 0)
{
	size_t kv_len = key_len + data_len;

    srand(seed);
    char* buf = new char[kv_len * size];
    for (size_t i = 0; i < kv_len * size; i++)
        buf[i] = rand() & 0xff;
        //buf[i] = (rand() % ('Z' - 'A')) + 'A';

    out_arr.clear();
    for (size_t i = 0; i < size; i++) {
		kv_pair kv;
		kv.key = fawn::RefValue(buf + i * kv_len, key_len);
		kv.data = fawn::RefValue(buf + i * kv_len + key_len, data_len);
        out_arr.push_back(kv);
	}
}


static void generate_mixed_sized_kv(const char *filename, kv_array_type& out_arr, size_t key_len, size_t data_len, size_t size, unsigned int seed = 0){
    // Consists of points x,y
    // x = the key-lengths
    // y = the number of keys
    std::vector<std::vector<double>> KeySizeDensities;

    
}



static void free_kv(kv_array_type& arr)
{
    char* min = reinterpret_cast<char*>(-1);
    for (size_t i = 0; i < arr.size(); i++)
        if (min > arr[i].key.data())
            min = arr[i].key.data();
    delete [] min;
    arr.clear();
}

namespace fawn
{
	static std::string conf_file = "testConfigs/testSiltM.xml";

    class FawnDS_SiltM_Test : public testing::Test
    {
    protected:
        // Code here will be called immediately after the constructor (right before
        // each test).
        virtual void SetUp() {
			Configuration* config = new Configuration(conf_file);
            std::string underlying_kv_store_ = config->GetStringValue("child::underlying-kv-store").c_str();
            std::cout << "Underlying Key-Value Store Config:" << underlying_kv_store_ << std::endl;
            
            siltm_ = new SiltM();
            siltm_->SetConfig(config);
			//siltm_->Create();

			// key_len_ = atoi(config->GetStringValue("child::key-len").c_str());
			// data_len_ = atoi(config->GetStringValue("child::data-len").c_str());
			// size_ = atoi(config->GetStringValue("child::size").c_str());
			// assert(key_len_ != 0);
			// assert(data_len_ != 0);
			// assert(size_ != 0);

            // fawnds_ = FawnDS_Factory::New(config);
			// fawnds_->Create();

			// ret_data_.resize(0);

            std::vector<int> numbers = {1, 5, 3, 7, 2};

            // Find the largest number


        }

        // Code in the TearDown() method will be called immediately after each test
        // (right before the destructor).
        virtual void TearDown() {
            delete siltm_;
        }

        // Objects declared here can be used by all tests in the test case for HashDB.

        size_t key_len_;
        size_t data_len_;
        size_t size_;

        kv_array_type arr_;

        SiltM* siltm_;

		Value ret_data_;
    };

    TEST_F(FawnDS_SiltM_Test, Meh) {
		// generate_random_kv(arr_, key_len_, data_len_, 1);

        // EXPECT_EQ(OK, fawnds_->Put(arr_[0].key, arr_[0].data));

        // EXPECT_EQ(OK, fawnds_->Get(arr_[0].key, ret_data_));
        // EXPECT_EQ(data_len_, ret_data_.size());
        // EXPECT_EQ(0, memcmp(arr_[0].data.data(), ret_data_.data(), data_len_));

		// free_kv(arr_);
    }

    TEST_F(FawnDS_SiltM_Test, TestSimpleInsertRetrieve1) {
		// generate_random_kv(arr_, key_len_, data_len_, 1);

        // EXPECT_EQ(OK, fawnds_->Put(arr_[0].key, arr_[0].data));

        // EXPECT_EQ(OK, fawnds_->Get(arr_[0].key, ret_data_));
        // EXPECT_EQ(data_len_, ret_data_.size());
        // EXPECT_EQ(0, memcmp(arr_[0].data.data(), ret_data_.data(), data_len_));

		// free_kv(arr_);
    }
}  // namespace fawn

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

