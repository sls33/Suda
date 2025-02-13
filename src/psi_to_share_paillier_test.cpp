#include "include/psi_to_share_paillier.h"
// #include "include/utils.h"
// #include "include/netio.h"

using namespace std;
using namespace seal;




int main(int argc, char *argv[])
{
    bool modswitch=true;


    if (argc==5)
    {
        // int party_id = std::stoi(argv[1]); // 将字符串转换为整数
        // std::cout<<"party_id="<<party_id<<std::endl;
        int host_log_n_data = std::stoi(argv[1]);
        int batch_size = std::stoi(argv[2]); 
        int feature_num = std::stoi(argv[3]);
        int party_id = std::stoi(argv[4]);
        std::cout<<"host_log_n_data="<<host_log_n_data<<std::endl;
        std::cout<<"batch_size="<<batch_size<<std::endl;
        std::cout<<"feature_num="<<feature_num<<std::endl;
        std::cout<<"party_id="<<party_id<<std::endl;
        // psi_to_share_2party(modswitch, host_log_n_data, batch_size, feature_num, party_id);
        psi_to_share_2party_socketopt(modswitch, host_log_n_data, batch_size, feature_num, party_id);
    }
    else
    {
        std::cout<<"must have argc==5"<<std::endl;
    }
    return 0;
    
}