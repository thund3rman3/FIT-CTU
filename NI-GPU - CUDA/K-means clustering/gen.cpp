#include <fstream>
#include <vector>
#include <random>

int main() {
    const size_t count = 100000000;
    std::vector<float> data(count);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rnd(-10000.0f, 10000.0f);

    for(auto& val : data) 
        val = rnd(gen);

    std::ofstream out("points.bin", std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()), count * sizeof(float));
    out.close();
}

