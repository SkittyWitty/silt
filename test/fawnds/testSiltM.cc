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

vector<vector<double>> load_points(const char *filename) {
    vector<vector<double> > points;
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
                vector<double> point;
                point.push_back(x);
                point.push_back(y);
                points.push_back(point);
                break;
            }
            ++y_str;
        }
    }
    fclose(fp);
    return points;
}

void print_points(vector<vector<double>> points){
    for(int i=0; i<points.size(); i++){
        for(int dim = 0; dim<points[i].size(); dim++) {
            printf("%f ", points[i][dim]);
        }
        printf("\n");
    }
}

static void generate_random_kv(kv_array_type& out_arr, size_t key_len, size_t data_len, size_t size, bool clear = false, unsigned int seed = 0)
{
	size_t kv_len = key_len + data_len;
    
    if (clear == true){
        out_arr.clear();
    }

    srand(seed);
    char* buf = new char[kv_len * size];
    for (size_t i = 0; i < kv_len * size; i++)
        buf[i] = rand() & 0xff;

    for (size_t i = 0; i < size; i++) {
		kv_pair kv;
		kv.key = fawn::RefValue(buf + i * kv_len, key_len);
		kv.data = fawn::RefValue(buf + i * kv_len + key_len, data_len);
        out_arr.push_back(kv);
	}
}

static void generate_mixed_sized_kv(kv_array_type& out_arr, vector<vector<double>>& points, size_t data_len){
    // Consists of points x,y
    // x = the key-lengths
    // y = the number of keys
    out_arr.clear();

    for (int i = 0; i < points.size(); i++){
        vector<double>& cur_point = points.at(i);
        size_t key_len = cur_point.at(0);
        size_t size = cur_point.at(1);
        generate_random_kv(out_arr, key_len, data_len, size);
    }
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
	static std::string conf_file = "/home/min-slice/silt/test/fawnds/testConfigs/testSiltM.xml";
    static std::string points_file = "/home/min-slice/silt/test/fawnds/testMultiKeyPoints/basic.csv";

    class FawnDS_SiltM_Test : public testing::Test
    {
    protected:
        // Code here will be called immediately after the constructor (right before
        // each test).
        virtual void SetUp() {
			Configuration* config = new Configuration(conf_file);
            std::string underlying_kv_store_ = config->GetStringValue("child::underlying-kv-store").c_str();
            std::cout << "Underlying Key-Value Store Config:" << underlying_kv_store_ << std::endl;
            
            data_len_ = atoi(config->GetStringValue("child::data-len").c_str());
            assert(data_len_ != 0);

            // Initialize the key-value store
            siltm_ = new SiltM();
            siltm_->SetConfig(config);

            // The key_lens that will be referenced to create stores
            vector<size_t> key_lens {1,2,3,5,20,30,40};
			siltm_->Create(key_lens);
            key_len_sizes_ = siltm_->GetKeyLengths();

			ret_data_.resize(0);
        }

        // Code in the TearDown() method will be called immediately after each test
        // (right before the destructor).
        virtual void TearDown() {
            delete siltm_;
        }

        // Objects declared here can be used by all tests in the test case for HashDB.

        vector<size_t> key_len_sizes_; //ordered
        size_t data_len_;
        size_t size_;

        kv_array_type arr_;

        SiltM* siltm_;

		Value ret_data_;
    };

    TEST_F(FawnDS_SiltM_Test, TestSimpleInsertRetrieve1) {
        generate_random_kv(arr_, key_len_sizes_[0], data_len_, 1, true);
        EXPECT_EQ(OK, siltm_->Put(arr_[0].key, arr_[0].data));

        EXPECT_EQ(OK, siltm_->Get(arr_[0].key, ret_data_));
        EXPECT_EQ(data_len_, ret_data_.size());
        EXPECT_EQ(0, memcmp(arr_[0].data.data(), ret_data_.data(), data_len_));

		free_kv(arr_);
    }

    TEST_F(FawnDS_SiltM_Test, TestSimpleInsertRetrieve2) {
		generate_random_kv(arr_, key_len_sizes_[0], data_len_, 1, true);

        EXPECT_EQ(OK, siltm_->Put(arr_[0].key, arr_[0].data));
        EXPECT_EQ(OK, siltm_->Get(arr_[0].key, ret_data_));
        EXPECT_EQ(data_len_, ret_data_.size());
        EXPECT_EQ(0, memcmp(arr_[0].data.data(), ret_data_.data(), data_len_));

		generate_random_kv(arr_, key_len_sizes_[1], data_len_, 1, true);
        EXPECT_EQ(OK, siltm_->Put(arr_[0].key, arr_[0].data));
        EXPECT_EQ(OK, siltm_->Get(arr_[0].key, ret_data_));
        EXPECT_EQ(data_len_, ret_data_.size());
        EXPECT_EQ(0, memcmp(arr_[0].data.data(), ret_data_.data(), data_len_));

		free_kv(arr_);
    }

}  // namespace fawn

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

