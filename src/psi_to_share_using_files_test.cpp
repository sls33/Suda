#include "include/psi_to_share.h"

using namespace std;
using namespace seal;




int main(int argc, char *argv[])
{
    bool modswitch=true;
    int party_id = std::stoi(argv[1]);
    string file_name = argv[2];
    string out_file = argv[3];
    psi_to_share_2party_socketopt_fromfile(modswitch, file_name, party_id, out_file);
    return 0;
    
}