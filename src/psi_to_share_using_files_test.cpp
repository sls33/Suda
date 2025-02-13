#include "include/psi_to_share.h"

using namespace std;
using namespace seal;

int main(int argc, char *argv[])
{
    bool modswitch = true;
    int party_id = std::stoi(argv[1]);
    string file_name = argv[2];
    string out_file = argv[3];
    std::ostream *out = &std::cout;
    std::fstream fs;
    if (argv[4][0] != '\0') {
        fs.open(argv[4], std::ios::out | std::ios::app);
        if (fs.is_open()) {
            out = &fs; // 指向文件流
        } else {
            std::cerr << "Failed to open file: " << argv[4] << std::endl;
            return 1;
        }
    }
    psi_to_share_2party_socketopt_fromfile(modswitch, file_name, party_id, out_file, out);
    return 0;
}