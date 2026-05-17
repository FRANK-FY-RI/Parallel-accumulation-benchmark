#include <vector>
#include <thread>
#include <numeric>
#include <chrono>
#include <iostream>
#include <algorithm>

using Iterator = std::vector<unsigned long long>::iterator;

void accumulate_block(Iterator start, Iterator end, unsigned long long &result) {
    result = std::accumulate(start, end, 0ULL);
}

unsigned long long accumulate_parallel(Iterator start, Iterator end, unsigned long long init, unsigned int thread_num) {
    size_t length = std::distance(start, end); 
    size_t block_size = length/thread_num;
    std::vector<unsigned long long> results(thread_num);
    std::vector<std::thread> threads(thread_num-1);
    Iterator block_start = start, block_end = start;
    for(unsigned int i = 0; i<thread_num-1; i++) {
        std::advance(block_end, block_size);
        threads[i] = std::thread(accumulate_block, block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    accumulate_block(block_start, end, results[thread_num-1]);
    for(auto &it:threads) {it.join();} 
    return std::accumulate(results.begin(), results.end(), init);
}

int main() {
    std::vector<unsigned long long> v(500'000'000);
    unsigned long long a = 3453223;
    for(auto &it:v) {
        it = a;
        a <<= 23;
        a ^= 12315088;
        a >>= 20;
    }

    const int iterations = 10; 

    for(unsigned int thread = 1; thread<=std::thread::hardware_concurrency(); thread++) {
        unsigned long long time = 0;
        for(int i = 0; i<iterations; i++) {
            auto start = std::chrono::high_resolution_clock().now();
            accumulate_parallel(v.begin(), v.end(), 0ULL, thread);
            auto end = std::chrono::high_resolution_clock().now();
            time += std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count(); 
        }
        std::cout<<"Thread = "<<thread<<": "<<"Time taken: "<<time<<'\n';
    }


    return 0;

}