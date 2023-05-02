#include "fawnds_factory.h"
#include "rate_limiter.h"
#include "silt_m.h"

#include <gtest/gtest.h>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <stdio.h>
#include <sys/time.h>

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
            vector<size_t> key_lens {8,53,6,4,5,37,45,48};
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

TEST_F(FawnDS_SiltM_Test, TestSimpleSortedInsertRetrieveSome) {
// Small key-value storage
generate_random_kv(arr_, key_len_sizes_[0], data_len_, 100);

for (size_t i = 0; i < 100; i++)
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));

for (size_t i = 0; i < 100; i++)
{
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}

// Large key-value storage
generate_random_kv(arr_, key_len_sizes_[1], data_len_, 100);

for (size_t i = 0; i < 100; i++)
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));

for (size_t i = 0; i < 100; i++)
{
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}

free_kv(arr_);
}


TEST_F(FawnDS_SiltM_Test, TestSimpleSortedInsertRetrieveSome10) {

double num_writes=0;
double num_reads=0;
double num_writes2=0;
double num_reads2=0;
struct timeval tvalBeforew, tvalAfterw;
struct timeval tvalBeforew2, tvalAfterw2;
struct timeval tvalBeforer, tvalAfterr;
struct timeval tvalBeforer2, tvalAfterr2;

fprintf(stderr, "\nTestSimpleSortedInsertRetrieveSome10\n");
// Small key-value storage
generate_random_kv(arr_, key_len_sizes_[0], data_len_, 5);

gettimeofday (&tvalBeforew, NULL);
for (size_t i = 0; i < 5; i++){
num_writes++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw, NULL);
double secw= ((tvalAfterw.tv_sec - tvalBeforew.tv_sec)*1000000L+tvalAfterw.tv_usec) - tvalBeforew.tv_usec;
fprintf(stderr,"writes=%f, sec=%f, writes/s=%f\n", num_writes, secw, (num_writes/secw)*1000000);


gettimeofday (&tvalBeforer, NULL);
for (size_t i = 0; i < 5; i++)
{
num_reads++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr, NULL);

double secr= ((tvalAfterr.tv_sec - tvalBeforer.tv_sec)*1000000L+tvalAfterr.tv_usec) - tvalBeforer.tv_usec;
fprintf(stderr,"reads=%f, sec=%f, reads/s=%f\n", num_reads, secr, (num_reads/secr)*1000000);


// Large key-value storage
generate_random_kv(arr_, key_len_sizes_[1], data_len_, 5);

gettimeofday (&tvalBeforew2, NULL);
for (size_t i = 0; i < 5; i++){
num_writes2++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw2, NULL);

int secw2= ((tvalAfterw2.tv_sec - tvalBeforew2.tv_sec)*1000000L+tvalAfterw2.tv_usec) - tvalBeforew2.tv_usec;
fprintf(stderr,"writes=%f, sec=%d, writes/s=%f\n", num_writes2, secw2, (num_writes2/secw2)*1000000);

gettimeofday (&tvalBeforer2, NULL);
for (size_t i = 0; i < 5; i++)
{
num_reads2++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr2, NULL);

int secr2= ((tvalAfterr2.tv_sec - tvalBeforer2.tv_sec)*1000000L+tvalAfterr2.tv_usec) - tvalBeforer2.tv_usec;
fprintf(stderr,"reads=%f, sec=%d, reads/s=%f\n", num_reads2, secr2, (num_reads2/secr2)*1000000);


free_kv(arr_);
}


TEST_F(FawnDS_SiltM_Test, TestSimpleSortedInsertRetrieveSome100) {

double num_writes=0;
double num_reads=0;
double num_writes2=0;
double num_reads2=0;
struct timeval tvalBeforew, tvalAfterw;
struct timeval tvalBeforew2, tvalAfterw2;
struct timeval tvalBeforer, tvalAfterr;
struct timeval tvalBeforer2, tvalAfterr2;

fprintf(stderr, "\nTestSimpleSortedInsertRetrieveSome100\n");
// Small key-value storage
generate_random_kv(arr_, key_len_sizes_[0], data_len_, 50);

gettimeofday (&tvalBeforew, NULL);
for (size_t i = 0; i < 50; i++){
num_writes++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw, NULL);
double secw= ((tvalAfterw.tv_sec - tvalBeforew.tv_sec)*1000000L+tvalAfterw.tv_usec) - tvalBeforew.tv_usec;
fprintf(stderr,"writes=%f, sec=%f, writes/s=%f\n", num_writes, secw, (num_writes/secw)*1000000);


gettimeofday (&tvalBeforer, NULL);
for (size_t i = 0; i < 50; i++)
{
num_reads++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr, NULL);

double secr= ((tvalAfterr.tv_sec - tvalBeforer.tv_sec)*1000000L+tvalAfterr.tv_usec) - tvalBeforer.tv_usec;
fprintf(stderr,"reads=%f, sec=%f, reads/s=%f\n", num_reads, secr, (num_reads/secr)*1000000);


// Large key-value storage
generate_random_kv(arr_, key_len_sizes_[1], data_len_, 50);

gettimeofday (&tvalBeforew2, NULL);
for (size_t i = 0; i < 50; i++){
num_writes2++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw2, NULL);

int secw2= ((tvalAfterw2.tv_sec - tvalBeforew2.tv_sec)*1000000L+tvalAfterw2.tv_usec) - tvalBeforew2.tv_usec;
fprintf(stderr,"writes=%f, sec=%d, writes/s=%f\n", num_writes2, secw2, (num_writes2/secw2)*1000000);

gettimeofday (&tvalBeforer2, NULL);
for (size_t i = 0; i < 50; i++)
{
num_reads2++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr2, NULL);

int secr2= ((tvalAfterr2.tv_sec - tvalBeforer2.tv_sec)*1000000L+tvalAfterr2.tv_usec) - tvalBeforer2.tv_usec;
fprintf(stderr,"reads=%f, sec=%d, reads/s=%f\n", num_reads2, secr2, (num_reads2/secr2)*1000000);


free_kv(arr_);
}


TEST_F(FawnDS_SiltM_Test, TestSimpleSortedInsertRetrieveSome1000) {

double num_writes=0;
double num_reads=0;
double num_writes2=0;
double num_reads2=0;
struct timeval tvalBeforew, tvalAfterw;
struct timeval tvalBeforew2, tvalAfterw2;
struct timeval tvalBeforer, tvalAfterr;
struct timeval tvalBeforer2, tvalAfterr2;

fprintf(stderr, "\nTestSimpleSortedInsertRetrieveSome1000\n");
// Small key-value storage
generate_random_kv(arr_, key_len_sizes_[0], data_len_, 500);

gettimeofday (&tvalBeforew, NULL);
for (size_t i = 0; i < 500; i++){
num_writes++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw, NULL);
double secw= ((tvalAfterw.tv_sec - tvalBeforew.tv_sec)*1000000L+tvalAfterw.tv_usec) - tvalBeforew.tv_usec;
fprintf(stderr,"writes=%f, sec=%f, writes/s=%f\n", num_writes, secw, (num_writes/secw)*1000000);


gettimeofday (&tvalBeforer, NULL);
for (size_t i = 0; i < 500; i++)
{
num_reads++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr, NULL);

double secr= ((tvalAfterr.tv_sec - tvalBeforer.tv_sec)*1000000L+tvalAfterr.tv_usec) - tvalBeforer.tv_usec;
fprintf(stderr,"reads=%f, sec=%f, reads/s=%f\n", num_reads, secr, (num_reads/secr)*1000000);


// Large key-value storage
generate_random_kv(arr_, key_len_sizes_[1], data_len_, 500);

gettimeofday (&tvalBeforew2, NULL);
for (size_t i = 0; i < 500; i++){
num_writes2++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw2, NULL);

int secw2= ((tvalAfterw2.tv_sec - tvalBeforew2.tv_sec)*1000000L+tvalAfterw2.tv_usec) - tvalBeforew2.tv_usec;
fprintf(stderr,"writes=%f, sec=%d, writes/s=%f\n", num_writes2, secw2, (num_writes2/secw2)*1000000);

gettimeofday (&tvalBeforer2, NULL);
for (size_t i = 0; i < 500; i++)
{
num_reads2++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr2, NULL);

int secr2= ((tvalAfterr2.tv_sec - tvalBeforer2.tv_sec)*1000000L+tvalAfterr2.tv_usec) - tvalBeforer2.tv_usec;
fprintf(stderr,"reads=%f, sec=%d, reads/s=%f\n", num_reads2, secr2, (num_reads2/secr2)*1000000);


free_kv(arr_);
}


TEST_F(FawnDS_SiltM_Test, TestSimpleSortedInsertRetrieveSome10000) {

double num_writes=0;
double num_reads=0;
double num_writes2=0;
double num_reads2=0;
struct timeval tvalBeforew, tvalAfterw;
struct timeval tvalBeforew2, tvalAfterw2;
struct timeval tvalBeforer, tvalAfterr;
struct timeval tvalBeforer2, tvalAfterr2;

fprintf(stderr, "\nTestSimpleSortedInsertRetrieveSome10000\n");
// Small key-value storage
generate_random_kv(arr_, key_len_sizes_[0], data_len_, 5000);

gettimeofday (&tvalBeforew, NULL);
for (size_t i = 0; i < 5000; i++){
num_writes++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw, NULL);
double secw= ((tvalAfterw.tv_sec - tvalBeforew.tv_sec)*1000000L+tvalAfterw.tv_usec) - tvalBeforew.tv_usec;
fprintf(stderr,"writes=%f, sec=%f, writes/s=%f\n", num_writes, secw, (num_writes/secw)*1000000);


gettimeofday (&tvalBeforer, NULL);
for (size_t i = 0; i < 5000; i++)
{
num_reads++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr, NULL);

double secr= ((tvalAfterr.tv_sec - tvalBeforer.tv_sec)*1000000L+tvalAfterr.tv_usec) - tvalBeforer.tv_usec;
fprintf(stderr,"reads=%f, sec=%f, reads/s=%f\n", num_reads, secr, (num_reads/secr)*1000000);


// Large key-value storage
generate_random_kv(arr_, key_len_sizes_[1], data_len_, 5000);

gettimeofday (&tvalBeforew2, NULL);
for (size_t i = 0; i < 5000; i++){
num_writes2++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw2, NULL);

int secw2= ((tvalAfterw2.tv_sec - tvalBeforew2.tv_sec)*1000000L+tvalAfterw2.tv_usec) - tvalBeforew2.tv_usec;
fprintf(stderr,"writes=%f, sec=%d, writes/s=%f\n", num_writes2, secw2, (num_writes2/secw2)*1000000);

gettimeofday (&tvalBeforer2, NULL);
for (size_t i = 0; i < 5000; i++)
{
num_reads2++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr2, NULL);

int secr2= ((tvalAfterr2.tv_sec - tvalBeforer2.tv_sec)*1000000L+tvalAfterr2.tv_usec) - tvalBeforer2.tv_usec;
fprintf(stderr,"reads=%f, sec=%d, reads/s=%f\n", num_reads2, secr2, (num_reads2/secr2)*1000000);


free_kv(arr_);
}


TEST_F(FawnDS_SiltM_Test, TestSimpleSortedInsertRetrieveSome100000) {

double num_writes=0;
double num_reads=0;
double num_writes2=0;
double num_reads2=0;
struct timeval tvalBeforew, tvalAfterw;
struct timeval tvalBeforew2, tvalAfterw2;
struct timeval tvalBeforer, tvalAfterr;
struct timeval tvalBeforer2, tvalAfterr2;

fprintf(stderr, "\nTestSimpleSortedInsertRetrieveSome100000\n");
// Small key-value storage
generate_random_kv(arr_, key_len_sizes_[0], data_len_, 50000);

gettimeofday (&tvalBeforew, NULL);
for (size_t i = 0; i < 50000; i++){
num_writes++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw, NULL);
double secw= ((tvalAfterw.tv_sec - tvalBeforew.tv_sec)*1000000L+tvalAfterw.tv_usec) - tvalBeforew.tv_usec;
fprintf(stderr,"writes=%f, sec=%f, writes/s=%f\n", num_writes, secw, (num_writes/secw)*1000000);


gettimeofday (&tvalBeforer, NULL);
for (size_t i = 0; i < 50000; i++)
{
num_reads++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr, NULL);

double secr= ((tvalAfterr.tv_sec - tvalBeforer.tv_sec)*1000000L+tvalAfterr.tv_usec) - tvalBeforer.tv_usec;
fprintf(stderr,"reads=%f, sec=%f, reads/s=%f\n", num_reads, secr, (num_reads/secr)*1000000);


// Large key-value storage
generate_random_kv(arr_, key_len_sizes_[1], data_len_, 50000);

gettimeofday (&tvalBeforew2, NULL);
for (size_t i = 0; i < 50000; i++){
num_writes2++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw2, NULL);

int secw2= ((tvalAfterw2.tv_sec - tvalBeforew2.tv_sec)*1000000L+tvalAfterw2.tv_usec) - tvalBeforew2.tv_usec;
fprintf(stderr,"writes=%f, sec=%d, writes/s=%f\n", num_writes2, secw2, (num_writes2/secw2)*1000000);

gettimeofday (&tvalBeforer2, NULL);
for (size_t i = 0; i < 50000; i++)
{
num_reads2++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr2, NULL);

int secr2= ((tvalAfterr2.tv_sec - tvalBeforer2.tv_sec)*1000000L+tvalAfterr2.tv_usec) - tvalBeforer2.tv_usec;
fprintf(stderr,"reads=%f, sec=%d, reads/s=%f\n", num_reads2, secr2, (num_reads2/secr2)*1000000);


free_kv(arr_);
}



TEST_F(FawnDS_SiltM_Test, TestSimpleSortedInsertRetrieveSome1000000) {

double num_writes=0;
double num_reads=0;
double num_writes2=0;
double num_reads2=0;
struct timeval tvalBeforew, tvalAfterw;
struct timeval tvalBeforew2, tvalAfterw2;
struct timeval tvalBeforer, tvalAfterr;
struct timeval tvalBeforer2, tvalAfterr2;

fprintf(stderr, "\nTestSimpleSortedInsertRetrieveSome1000000\n");
// Small key-value storage
generate_random_kv(arr_, key_len_sizes_[0], data_len_, 500000);

gettimeofday (&tvalBeforew, NULL);
for (size_t i = 0; i < 500000; i++){
num_writes++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw, NULL);
double secw= ((tvalAfterw.tv_sec - tvalBeforew.tv_sec)*1000000L+tvalAfterw.tv_usec) - tvalBeforew.tv_usec;
fprintf(stderr,"writes=%f, sec=%f, writes/s=%f\n", num_writes, secw, (num_writes/secw)*1000000);


gettimeofday (&tvalBeforer, NULL);
for (size_t i = 0; i < 500000; i++)
{
num_reads++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr, NULL);

double secr= ((tvalAfterr.tv_sec - tvalBeforer.tv_sec)*1000000L+tvalAfterr.tv_usec) - tvalBeforer.tv_usec;
fprintf(stderr,"reads=%f, sec=%f, reads/s=%f\n", num_reads, secr, (num_reads/secr)*1000000);


// Large key-value storage
generate_random_kv(arr_, key_len_sizes_[1], data_len_, 500000);

gettimeofday (&tvalBeforew2, NULL);
for (size_t i = 0; i < 500000; i++){
num_writes2++;
EXPECT_EQ(OK, siltm_->Put(arr_[i].key, arr_[i].data));
}
gettimeofday (&tvalAfterw2, NULL);

int secw2= ((tvalAfterw2.tv_sec - tvalBeforew2.tv_sec)*1000000L+tvalAfterw2.tv_usec) - tvalBeforew2.tv_usec;
fprintf(stderr,"writes=%f, sec=%d, writes/s=%f\n", num_writes2, secw2, (num_writes2/secw2)*1000000);

gettimeofday (&tvalBeforer2, NULL);
for (size_t i = 0; i < 500000; i++)
{
num_reads2++;
EXPECT_EQ(OK, siltm_->Get(arr_[i].key, ret_data_));
EXPECT_EQ(data_len_, ret_data_.size());
EXPECT_EQ(0, memcmp(arr_[i].data.data(), ret_data_.data(), data_len_));
}
gettimeofday (&tvalAfterr2, NULL);

int secr2= ((tvalAfterr2.tv_sec - tvalBeforer2.tv_sec)*1000000L+tvalAfterr2.tv_usec) - tvalBeforer2.tv_usec;
fprintf(stderr,"reads=%f, sec=%d, reads/s=%f\n", num_reads2, secr2, (num_reads2/secr2)*1000000);


free_kv(arr_);
}






}  // namespace fawn

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

