#include <string>
#include <iostream>
#include <cmath>
#include <random>
#include <array>
#include <numeric>
#include <sstream>
#include <vector>
#include <fstream>
#include <source_location>
#include <cuda_runtime.h>

inline void HANDLE_ERR_CUDA(cudaError_t err, const std::source_location loc = std::source_location::current()) {
    if (err != cudaSuccess) {
        std::string msg = std::string("CUDA Error: ") + cudaGetErrorString(err) +
                          ", File: " + loc.file_name() +
                          ", Line: " + std::to_string(loc.line()) + "\n";

        throw std::runtime_error(msg);
    }
}

// Called from GPU
__device__ float distanceND_device(const float* p1, const float* p2, size_t n) {
    float res = 0.0f;
    for (size_t i = 0; i < n; i++) {
        float d = p1[i] - p2[i];
        res += d * d;
    }
    return res;
}

// Called from CPU
// for point p find closest centroid, add changes
__global__ void assign_clusters_kernel(
    const float* d_points_coords,
    size_t* d_points_cluster_ids,
    const float* d_centroids,
    int* d_changed_points,
    size_t cnt_points,
    size_t n,
    size_t k)
{
    // thread id
    size_t i = blockIdx.x * blockDim.x + threadIdx.x;

    // if more threads run, eliminate them
    if (i >= cnt_points)
        return;

    size_t closest_idx = 0;
    const float* p_coords = &d_points_coords[i * n];

    float min_dist = distanceND_device(p_coords, &d_centroids[0], n);

    for (size_t j = 1; j < k; ++j) {
        float d = distanceND_device(p_coords, &d_centroids[j * n], n);
        if (d < min_dist) {
            min_dist = d;
            closest_idx = j;
        }
    }

    if (d_points_cluster_ids[i] != closest_idx) {
        d_points_cluster_ids[i] = closest_idx;
        atomicAdd(d_changed_points, 1);
    }
}

// called for each point
// cluster i, point count update
__global__ void compute_sums_kernel(
    const float* d_points_coords,
    const size_t* d_points_cluster_ids,
    float* d_new_centroids_sums,
    int* d_cluster_counts,
    size_t cnt_points,
    size_t n)
{
    // thread id
    size_t i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= cnt_points)
        return;

    size_t c_id = d_points_cluster_ids[i];
    atomicAdd(&d_cluster_counts[c_id], 1);

    // save all coords to new_centroids
    for (size_t d = 0; d < n; ++d) {
        atomicAdd(&d_new_centroids_sums[c_id * n + d], d_points_coords[i * n + d]);
    }
}

// called for each centroid
__global__ void finalize_centroids_kernel(
    float* d_centroids,
    const float* d_new_centroids_sums,
    const int* d_cluster_counts,
    const float* d_old_centroids,
    size_t k,
    size_t n)
{
    // max k threads
    size_t j = blockIdx.x * blockDim.x + threadIdx.x;
    if (j >= k) return;

    int count = d_cluster_counts[j];

    if (count > 0) { // count avg
        for (size_t d = 0; d < n; ++d) {
            d_centroids[j * n + d] = d_new_centroids_sums[j * n + d] / (float)count;
        }
    } else { // empty volume
        for (size_t d = 0; d < n; ++d) {
            d_centroids[j * n + d] = d_old_centroids[j * n + d];
        }
    }
}

void k_means(
    size_t k,
    const size_t n,
    const size_t cnt_points,
    const std::vector<float>& points_coords,
    std::vector<size_t>& points_cluster_ids,
    int threadsPerBlock = 256)
{
    float elapsedTime;
    cudaEvent_t start, end;
    HANDLE_ERR_CUDA( cudaEventCreate( &start ) );
    HANDLE_ERR_CUDA( cudaEventCreate( &end ) );

    if (cnt_points == 0 || k == 0 || n == 0)
        return;
    k = std::min(k, cnt_points);

    // init (Host -> Device)
    size_t points_bytes = cnt_points * n * sizeof(float);
    size_t clusters_bytes = cnt_points * sizeof(size_t);
    size_t centroids_bytes = k * n * sizeof(float);

    float *d_points_coords, *d_centroids, *d_old_centroids, *d_new_centroids_sums;
    size_t *d_points_cluster_ids;
    int *d_changed_points, *d_cluster_counts;

    HANDLE_ERR_CUDA( cudaMalloc((void**)&d_points_coords, points_bytes) );
    HANDLE_ERR_CUDA( cudaMalloc((void**)&d_points_cluster_ids, clusters_bytes) );
    HANDLE_ERR_CUDA( cudaMalloc((void**)&d_centroids, centroids_bytes) );
    HANDLE_ERR_CUDA( cudaMalloc((void**)&d_changed_points, sizeof(int)) );

    HANDLE_ERR_CUDA( cudaMalloc((void**)&d_new_centroids_sums, centroids_bytes) );
    HANDLE_ERR_CUDA( cudaMalloc((void**)&d_old_centroids, centroids_bytes) );
    HANDLE_ERR_CUDA( cudaMalloc((void**)&d_cluster_counts, k * sizeof(int)) );

    // Copy to GPU
    // coords
    HANDLE_ERR_CUDA( cudaMemcpy(d_points_coords, points_coords.data(), points_bytes, cudaMemcpyHostToDevice) );
    // first k coords as centroids
    HANDLE_ERR_CUDA( cudaMemcpy(d_centroids, points_coords.data(), centroids_bytes, cudaMemcpyHostToDevice) );
    // set cluster ids to max
    HANDLE_ERR_CUDA( cudaMemset(d_points_cluster_ids, 0xFF, clusters_bytes) );

    HANDLE_ERR_CUDA( cudaEventRecord( start, 0 ) );

    // Config
    //int threadsPerBlock = 512;
    // round: (N + threads - 1) / threads
    int blocksForPoints = (cnt_points + threadsPerBlock - 1) / threadsPerBlock;
    int blocksForCentroids = (k + threadsPerBlock - 1) / threadsPerBlock;

    // Loop
    bool converged = false;
    int h_changed_points = 0;
    int iteration = 0;

    while (!converged && iteration < 50) {
        // reset changed points count
        HANDLE_ERR_CUDA( cudaMemset(d_changed_points, 0, sizeof(int)) );

        assign_clusters_kernel<<<blocksForPoints, threadsPerBlock>>>(
            d_points_coords,
            d_points_cluster_ids,
            d_centroids,
            d_changed_points,
            cnt_points, n, k
        );

        cudaDeviceSynchronize(); // wait for GPU to finish

        // copy changed points count back to host
        HANDLE_ERR_CUDA( cudaMemcpy(&h_changed_points, d_changed_points, sizeof(int), cudaMemcpyDeviceToHost) );

        if (h_changed_points == 0) {
            converged = true;
        } else {
            HANDLE_ERR_CUDA( cudaMemset(d_new_centroids_sums, 0, centroids_bytes) );
            HANDLE_ERR_CUDA( cudaMemset(d_cluster_counts, 0, k * sizeof(int)) );
            HANDLE_ERR_CUDA( cudaMemcpy(d_old_centroids, d_centroids, centroids_bytes, cudaMemcpyDeviceToDevice) );

            compute_sums_kernel<<<blocksForPoints, threadsPerBlock>>>(
                d_points_coords,
                d_points_cluster_ids,
                d_new_centroids_sums,
                d_cluster_counts, cnt_points, n
            );

            // avg k threads
            finalize_centroids_kernel<<<blocksForCentroids, threadsPerBlock>>>(
                d_centroids,
                d_new_centroids_sums,
                d_cluster_counts,
                d_old_centroids, k, n
            );
            cudaDeviceSynchronize();
        }
        iteration++;
    }

    HANDLE_ERR_CUDA( cudaEventRecord( end, 0 ) );
    HANDLE_ERR_CUDA( cudaEventSynchronize( end ) );
    HANDLE_ERR_CUDA( cudaEventElapsedTime( &elapsedTime, start, end ) );
    std::cout << "Time taken: " <<  elapsedTime << "ms" << ". Iterations: " << iteration << std::endl;

    // move (Device -> Host)
    HANDLE_ERR_CUDA( cudaMemcpy(points_cluster_ids.data(), d_points_cluster_ids, clusters_bytes, cudaMemcpyDeviceToHost) );

    // Free
    HANDLE_ERR_CUDA( cudaFree(d_points_coords) );
    HANDLE_ERR_CUDA( cudaFree(d_points_cluster_ids) );
    HANDLE_ERR_CUDA( cudaFree(d_centroids) );
    HANDLE_ERR_CUDA( cudaFree(d_changed_points) );
    HANDLE_ERR_CUDA( cudaFree(d_new_centroids_sums) );
    HANDLE_ERR_CUDA( cudaFree(d_old_centroids) );
    HANDLE_ERR_CUDA( cudaFree(d_cluster_counts) );
    HANDLE_ERR_CUDA( cudaEventDestroy( start ) );
    HANDLE_ERR_CUDA( cudaEventDestroy( end ) );
}

void print_clusters(const std::vector<float>& points, const std::vector<size_t>& points_cluster_ids, size_t n, size_t k, size_t cnt_points) {
    for (size_t c = 0; c < k; ++c) {
        bool has_points = false;

        for (size_t i = 0; i < cnt_points; ++i) {
            if (points_cluster_ids[i] == static_cast<int>(c)) {
                if (!has_points) {
                    std::cout << "Cluster " << c << " - POINTS: ";
                    has_points = true;
                }

                std::cout << "[";
                for (size_t d = 0; d < n; ++d) {
                    std::cout << points[i * n + d];
                    if (d < n - 1)
                        std::cout << ", ";
                }
                std::cout << "] ";
            }
        }

        if (has_points) {
            std::cout << "\n";
        }
    }
}

int main() {
    std::string filename = "points.bin";
    std::vector<float> points;

    std::cout << "Reading " << filename << "..." << std::endl;
    
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    if (!in.is_open()) {
        std::cerr << "Error: Failed to open file " << filename << std::endl;
        return 1;
    }

    std::streamsize size = in.tellg(); // /100 - is 1mil
    in.seekg(0, std::ios::beg); 

    points.resize(size / sizeof(float));
    in.read(reinterpret_cast<char*>(points.data()), size);
    
    std::cout << "Successfully read " << points.size() << " values." << std::endl;

    std::vector<size_t> test_points = {1000000, 10000000}; // 1 mil, 10 mil bodů
    std::vector<size_t> test_k = {10, 33,100};                // 10, 100 shluků
    std::vector<size_t> test_n = {1, 2, 8, 64};            // 1, 2, 8, 64 dimenzí
    std::vector<int> test_threads = {32, 256, 1024};       // Vlákna na blok

    int test_counter = 1;

    for (size_t cnt_points : test_points) {
        for (size_t n : test_n) {
            
            size_t required_floats = cnt_points * n;
            if (required_floats > points.size()) {
                std::cout << "[SKIP]: " << cnt_points 
                          << " points, " << n << " dimensions. (Need: " 
                          << required_floats << ", Read: " << points.size() << ")\n";
                continue; 
            }

            for (size_t k : test_k) {
                if (k > cnt_points) 
                    continue;

                for (int threadsPerBlock : test_threads) {
                    
                    std::cout << "--- Test #" << test_counter++ << " ---" << std::endl;
                    std::cout << "Config: Points=" << cnt_points 
                              << ", Dim=" << n 
                              << ", K=" << k 
                              << ", Threads=" << threadsPerBlock << std::endl;

                    std::vector<size_t> points_cluster_ids(cnt_points);
                    
                    k_means(k, n, cnt_points, points, points_cluster_ids, threadsPerBlock);
                    
                    std::cout << std::endl;
                }
            }
        }
    }
    std::cout << "Success" << std::endl;
}