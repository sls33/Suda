#include "include/psi_to_share_paillier.h"
// #include "include/utils.h"
// #include "include/netio.h"

using namespace std;
using namespace seal;

int main(int argc, char *argv[])
{
    bool modswitch = true;

    if (argc == 5) {
        // int party_id = std::stoi(argv[1]); // 将字符串转换为整数
        // std::cout<<"party_id="<<party_id<<std::endl;

        int host_log_n_data = std::stoi(argv[1]);
        int batch_size = std::stoi(argv[2]);
        int feature_num = std::stoi(argv[3]);
        int party_id = std::stoi(argv[4]);

        std::ostream *out = &std::cout;
        std::fstream fs;
        if (argv[5][0] != '\0') {
            fs.open(argv[5], std::ios::out | std::ios::app);
            if (fs.is_open()) {
                out = &fs; // 指向文件流
            } else {
                std::cerr << "Failed to open file: " << argv[5] << std::endl;
                return 1;
            }
        }
        *out << "host_log_n_data=" << host_log_n_data << std::endl;
        *out << "batch_size=" << batch_size << std::endl;
        *out << "feature_num=" << feature_num << std::endl;
        *out << "party_id=" << party_id << std::endl;
        // psi_to_share_2party(modswitch, host_log_n_data, batch_size, feature_num, party_id);
        psi_to_share_2party_socketopt(
            modswitch, host_log_n_data, batch_size, feature_num, party_id,out);
    } else {
        std::cerr << "must have argc==5" << std::endl;
    }
    return 0;
}