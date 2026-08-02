#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <random>
#include <list>
#include <array>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <vector>

float distanceND(const float* p1, const float* p2, const size_t dimensions)
{
    float res = 0.0f;
    for (size_t i = 0; i < dimensions; i++)
    {
        const float d = static_cast<float>(p1[i]) - p2[i];
        res += d * d;
    }
    return res;
}

void calculate_centroids(
    const std::vector<float>& points_coords,
    const std::vector<size_t>& points_cluster_ids,
    const std::vector<float>& centroids,
    std::vector<float>& new_centroids,
    const size_t cnt_points,
    const size_t n,
    const size_t k)
{
    std::vector<int> counts(k, 0);

    std::fill(new_centroids.begin(), new_centroids.end(), 0.0f);

    // Sum up the coordinates of points in each cluster
    for (size_t i = 0; i < cnt_points; ++i) {
        const size_t c_id = points_cluster_ids[i];
        counts[c_id]++;
        for (size_t j = 0; j < n; ++j) {
            // Formula: index = idx * dimensions + j
            new_centroids[c_id * n + j] += points_coords[i * n + j];
        }
    }

    // For each centroid - average the sums to get the new centroids
    for (size_t i = 0; i < k; ++i) {
        if (counts[i] == 0) {
            for (size_t j = 0; j < n; ++j) {
                new_centroids[i * n + j] = centroids[i * n + j];
            }
        } else {
            for (size_t j = 0; j < n; ++j) {
                new_centroids[i * n + j] /= static_cast<float>(counts[i]);
            }
        }
    }
}

void k_means(
    size_t k,
    const size_t n,
    const size_t cnt_points,
    const std::vector<float>& points_coords, // [point i] = point coords
    std::vector<size_t>& points_cluster_ids)    // [point i] = i's cluster id
{
    if (cnt_points == 0 || k == 0 || n == 0)
        return;
    k = std::min(k, cnt_points);

    std::vector<float> centroids(k * n);

    for (size_t i = 0; i < k * n; ++i) {
        centroids[i] = points_coords[i];
    }

    bool converged = false;
    std::vector<float> new_centroids(k * n);
    int iteration = 0;

    while (!converged && iteration < 50) {
        int changed_points = 0;

        for (size_t i = 0; i < cnt_points; ++i) {
            size_t closest_idx = 0;

            // Point coord start
            const float* p_coords = &points_coords[i * n];

            float min_dist = distanceND(p_coords, &centroids[0], n);

            for (size_t j = 1; j < k; ++j) {
                float d = distanceND(p_coords, &centroids[j * n], n);
                if (d < min_dist) {
                    min_dist = d;
                    closest_idx = j;
                }
            }

            // Point moved?
            if (points_cluster_ids[i] != closest_idx) {
                points_cluster_ids[i] = closest_idx;
                changed_points++;
            }
        }
        
        if (changed_points == 0) {
            converged = true;
        } else {
            calculate_centroids(points_coords, points_cluster_ids, centroids, new_centroids, cnt_points, n, k);
            std::swap(centroids, new_centroids);
        }
        iteration++;
    }
    std::cout << "converged in " << iteration << " iterations." << std::endl;
}

void print_clusters(const std::vector<float>& points, const std::vector<size_t>& point_cluster_ids, size_t n, size_t k, size_t cnt_points) {
        for (size_t c = 0; c < k; ++c) {
        bool has_points = false;

        for (size_t i = 0; i < cnt_points; ++i) {
            if (point_cluster_ids[i] == static_cast<int>(c)) {
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
    const std::string filename = "points.bin";
    std::vector<float> points;

    std::cout << "Reading " << filename << "..." << std::endl;
    
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    if (!in.is_open()) {
        std::cerr << "Error: Failed to open file " << filename << std::endl;
        return 1;
    }

    const std::streamsize size = in.tellg(); // Get number of bytes in the file
    in.seekg(0, std::ios::beg); // Move back to the beginning of the file

    points.resize(size / sizeof(float));
    in.read(reinterpret_cast<char*>(points.data()), size);
    
    std::cout << "Successfully read " << points.size() << " values." << std::endl;

    std::vector<size_t> test_points = {1000000, 10000000}; // 1 mil, 10 mil bodů
    std::vector<size_t> test_k = {10, 33,100};                // 10, 100 shluků
    std::vector<size_t> test_n = {1, 2, 8, 64};            // 1, 2, 8, 64 dimenzí

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

                std::cout << "--- Test #" << test_counter++ << " ---" << std::endl;
                std::cout << "Config: Points=" << cnt_points 
                            << ", Dim=" << n 
                            << ", K=" << k 
                            << std::endl;

                std::vector<size_t> points_cluster_ids(cnt_points);
                
                const auto start = std::chrono::high_resolution_clock::now();
                k_means(k, n, cnt_points, points, points_cluster_ids);
                const auto end = std::chrono::high_resolution_clock::now();
                const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                std::cout << "Time: " << duration.count()/1000.0 << " ms\n";
                std::cout << "---------------------------\n";
                std::cout << std::endl;
                
            }
        }
    }

    std::cout << "Success\n";

    return 0;
}

