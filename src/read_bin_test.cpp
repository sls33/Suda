#include <iostream>
#include <fstream>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <vector>




int main() {
    std::ifstream file("/home/zhangqizhi.zqz/mpc_project/python/SVHN_X.bin", std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::cout << data.size() << " ";
    
    
    return 0;
}