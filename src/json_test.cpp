#include "include/json.hpp"
#include "include/file_io.h"
#include <iostream>
#include <fstream> 
#include <string>
// #include <emmintrin.h>
// #include <smmintrin.h>

using json = nlohmann::json;
using namespace std;



// int main() {
//     std::ifstream file("/root/zqz/mpc_projects/data/CFI.json"); // 打开 JSON 文件
//     json json_obj;
    

//     file >> json_obj; // 将 JSON 数据解析为 nlohmann::json 对象
//     file.close(); // 关闭文件
//     size_t col_num = json_obj.size();
//     std::cout<<"json_obj.size()="<<col_num<<std::endl;
//     std::vector<std::string> server_ids;
//     std::vector<std::vector<int64_t>> data(col_num-1); 

//     vector<string> client_ids;
//     vector<bool> client_labels;

//     // 遍历 JSON 对象并打印值
//     size_t j=0;
//     for (auto& col : json_obj.items()) {
//         // std::cout << "Key: " << element.key() << ", Value: " << element.value() << std::endl;
//             string key = col.key();
//             std::cout << "Key: " << key << std::endl;  
//             if (key=="label")
//             {
//                 client_labels = col.value().get<std::vector<bool>>();
//                 std::cout<<"client_labels.size()="<<client_labels.size()<<std::endl;
//             }
//             else{
//                 std::cout<<"j="<<j<<std::endl;
//                 data[j] = col.value().get<std::vector<int64_t>>();
//                 // std::cout<<"data[j].size()="<<data[j].size()<<std::endl;
//                 j++;
//             }
//     }

//     return 0;
// }




int test_read_cols_server() {
    string file_name = "/root/zqz/mpc_projects/data/CFIX.json";
    std::vector<std::string> server_ids;
    std::vector<std::vector<int64_t>> data; 
    read_cols_server(file_name, server_ids, data);
    std::cout<<"server_ids.size()="<<server_ids.size()<<std::endl;
    std::cout<<"data.size()="<<data.size()<<std::endl;
    std::cout<<"data[0].size()="<<data[0].size()<<std::endl;
    return 0;
}




int test_read_label_client()
{
    string file_name = "/root/zqz/mpc_projects/data/CFIY4096.json";
    std::vector<string> client_ids;
     std::vector<bool> labels;
    read_label_client(file_name, client_ids, labels);
     std::cout<<"client_ids.size()="<<client_ids.size()<<std::endl;
    std::cout<<"labels.size()="<<labels.size()<<std::endl;
    return 0;
}


int main_rows() {
    std::ifstream file("/root/zqz/mpc_projects/data/CFIX_rows.json"); // 打开 JSON 文件
    json json_obj;
    

    file >> json_obj; // 将 JSON 数据解析为 nlohmann::json 对象
    file.close(); // 关闭文件

    std::vector<int64_t> server_ids=json_obj["index"].get<std::vector<int64_t>>();
    std::vector<std::vector<int64_t>> data=json_obj["data"].get<std::vector<std::vector<int64_t>>>();

        std::cout<<"server_ids.size()="<<server_ids.size()<<std::endl;
        std::cout<<"data.size()="<<data.size()<<std::endl;
        std::cout<<"data[0].size()="<<data[0].size()<<std::endl;

    return 0;
}


// int main_1row() {
//     std::ifstream file("/root/zqz/mpc_projects/data/CFIX_1row.json"); // 打开 JSON 文件
//     json json_obj;

    

//     file >> json_obj; // 将 JSON 数据解析为 nlohmann::json 对象
//     file.close(); // 关闭文件

//     std::vector<int64_t> server_ids=json_obj["index"].get<std::vector<int64_t>>();
//     std::vector<int64_t> data=json_obj["data"].get<std::vector<int64_t>>();

//         std::cout<<"server_ids.size()="<<server_ids.size()<<std::endl;
//         std::cout<<"data.size()="<<data.size()<<std::endl;
//         // std::cout<<"data[0].size()="<<data[0].size()<<std::endl;
//         std::cout<<"1.0*data.size()/server_ids.size()="<<1.0*data.size()/server_ids.size()<<std::endl;
//     return 0;
// }


int main_1row()
{
    string file_name = "/root/zqz/mpc_projects/data/CFIX_1row.json";
    std::vector<int64_t> server_ids;
    std::vector<int64_t> data;
    read_1row(file_name, server_ids, data);
    return 0;
}


int main()
{

    // main_1row();
    // test_read_cols_server();
    // test_read_label_client();
    main_1row();
    return 0;
}