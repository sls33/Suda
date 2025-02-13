#include "include/batch_pir_protocol.h"
#include "include/utils.h"
#include <stdio.h>
#include <thread>
#include "include/mem_usage.h"
#include <iostream>
#include <atomic>
#include <algorithm>


using namespace std;
using namespace seal;



//     std::atomic<bool> stop_sub_thread;
// int threadShowMemUsage(std::string describe) {
//     stop_sub_thread=false;
//     int pid = GetCurrentPid();
//     float mem_usage = 0;
//     while (!stop_sub_thread) {
//         mem_usage = std::max(mem_usage, GetMemoryUsage(pid));
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//         std::cout<<"max mem usage of "<<describe<<"="<<mem_usage<<"MB"<<std::endl;
//     }
//     std::cout<<"max mem usage of "<<describe<<"="<<mem_usage<<"MB"<<std::endl;
//     return 0;
// }



void batch_pir_poly_reduce_modswitch_test(bool modswitch)
{

    bool isHost=1;
    bool isGuest=1;
    size_t feature_num=100;
    size_t host_log_n_data=20;
    size_t batch_size = 4096;
    int host_n_data = 1<<host_log_n_data;
    std::vector<int64_t> data(host_n_data), new_ids(batch_size);
    const int64_t prime_num = 167772161UL;
    for (int i = 0; i < host_n_data; i++) {
        data[i] = random() % prime_num;
    }
    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=i;
    }
    vector<vector<int64_t>>  result;
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    // batch_pir_from_nttform_test(isHost, isGuest, result, feature_num, host_log_n_data, batch_size);
    batch_pir_from_nttform_modswitch(isHost, isGuest, data, new_ids, result, feature_num, host_log_n_data, batch_size, modswitch);
    
    std::cout<<"result.size()"<<result.size()<<std::endl;
    std::cout<<"result[0].size()"<<result[0].size()<<std::endl;

    // vector<int64_t> result = poly_guest->reveal_poly_value(poly_share0, poly_share1, new_ids);
   
    // std::cout<<"result="<<std::endl;
    // for (int i=0; i<4; i++)
    // {
    //     std::cout<<result[i]<<std::endl;
    // }
    // std::cout<<"....."<<std::endl;
    // for (int i=batch_size-4; i<batch_size; i++)
    // {
    //     std::cout<<result[i]<<std::endl;
    // }
}





void batch_pir_poly_reduce_modswitch_real_test(bool modswitch)
{

    bool isHost=1;
    bool isGuest=1;
    size_t feature_num=100;
    size_t host_log_n_data=20;
    size_t batch_size = 4096;
    int host_n_data = 1<<host_log_n_data;
    std::vector<std::vector<int64_t>> data(feature_num); 
    vector<int64_t> new_ids(batch_size);
    const int64_t prime_num = 167772161UL;
    for (size_t feature_id=0; feature_id<feature_num; feature_id++)
    {
        data[feature_id].resize(host_n_data);
        for (int i = 0; i < host_n_data; i++) {
            data[feature_id][i] = random() % prime_num;
        }
    }
    

    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=i;
    }
    vector<vector<int64_t>>  result;
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    batch_pir_from_nttform_modswitch(isHost, isGuest, data, new_ids, result, modswitch);
    
    // std::cout<<"result.size()="<<result.size()<<std::endl;
    // std::cout<<"result[0].size()="<<result[0].size()<<std::endl;

}





void batch_pir_poly_reduce_type0_real_test()
{

    bool isHost=1;
    bool isGuest=1;
    size_t feature_num=100;
    size_t host_log_n_data=20;
    size_t batch_size = 4096;
    int host_n_data = 1<<host_log_n_data;
    std::vector<std::vector<int64_t>> data(feature_num); 
    vector<int64_t> new_ids(batch_size);
    const int64_t prime_num = 167772161UL;
    for (size_t feature_id=0; feature_id<feature_num; feature_id++)
    {
        data[feature_id].resize(host_n_data);
        for (int i = 0; i < host_n_data; i++) {
            data[feature_id][i] = random() % prime_num;
        }
    }
    

    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=i;
    }
    vector<vector<int64_t>>  result;
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    batch_pir_from_nttform_type0(isHost, isGuest, data, new_ids, result);
    
    // std::cout<<"result.size()="<<result.size()<<std::endl;
    // std::cout<<"result[0].size()="<<result[0].size()<<std::endl;

}



void batch_pir_test(size_t host_log_n_data, size_t batch_size, size_t feature_num)
{
    //-----------数据准备-----------------------
    // size_t feature_num=8000/60;
    // size_t host_log_n_data=20;
    // size_t batch_size = 4096;
    int host_n_data = 1<<host_log_n_data;
    std::vector<std::vector<int64_t>> data(feature_num); 
    vector<int64_t> new_ids(batch_size);
    // const int64_t prime_num = 167772161UL;
    for (size_t feature_id=0; feature_id<data.size(); feature_id++)
    {
        data[feature_id].resize(host_n_data);
        for (int i = 0; i < host_n_data; i++) {
            data[feature_id][i] = random() % (1<<16);
        }
    }


    Timer timer;
    float mem_usg_start=0, mem_usg_end;
     //-----------Server initialize-----------------------
    timer.reset();
     auto bp_server = BatchPirServer(data, batch_size, feature_num);
     auto server_init_time = timer.elapsed();
     mem_usg_end=GetMemoryUsage();
     float server_init_mem = mem_usg_end-mem_usg_start;

         //-----------Client initialize and send pk----------------------
    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=random()%host_n_data;
    }
     timer.reset();
     mem_usg_start=mem_usg_end;
     auto bp_client = BatchPirClient(host_log_n_data, feature_num, batch_size);
     auto keys=bp_client.save_keys();
     auto client_init_time = timer.elapsed();
    mem_usg_end=GetMemoryUsage();
    float client_init_mem = mem_usg_end-mem_usg_start;
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //----------Server receive pk-----------------
     bp_server.load_keys(keys);
     //-----------Client generate query ------------
     timer.reset();
      mem_usg_start=mem_usg_end;
     std::stringstream query = bp_client.gen_query(new_ids);
    auto query_time = timer.elapsed();
    mem_usg_end=GetMemoryUsage();
    float query_mem= mem_usg_end-mem_usg_start;
    
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Server generate respose-------------
    timer.reset();
    mem_usg_start=mem_usg_end;
    bool mode_switch=true;
     std::stringstream response = bp_server.gen_response(query, mode_switch);
    auto response_time = timer.elapsed();
   mem_usg_end=GetMemoryUsage();
   float response_mem = mem_usg_end-mem_usg_start;
    
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client exact answer
        timer.reset();
           mem_usg_start=mem_usg_end;
     auto result = bp_client.extract_answer(response);
    auto extract_time = timer.elapsed();
      mem_usg_end=GetMemoryUsage();
      float extract_mem = mem_usg_end-mem_usg_start;
    

  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
  std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
  std::cout<<"Mem Usage of Server initialize="<<server_init_mem<<"MB"<<endl;

  std::cout <<" client_init_time:" << client_init_time << " ms " << std::endl;

std::cout<<"Mem Usage of Client initialize="<<client_init_mem<<"MB"<<endl;


  std::cout << "Gen query time: " << query_time << " ms " << std::endl;
std::cout<<"Mem Usage of Client generate query="<<query_mem<<"MB"<<endl;


  std::cout << "Gen response time: " << response_time << " ms " << std::endl;
  std::cout<<"Mem Usage of Gen response="<<response_mem<<"MB"<<endl;

  std::cout << "Extract answer time: " << extract_time << " ms "
            << std::endl;
std::cout<<"Mem Usage of Extract answer="<<extract_mem<<"MB"<<endl;


  std::cout << "Query size: " << query.str().size() / 1024.0 << " KBytes"
            << std::endl;

  std::cout << "Response size: " << response.str().size() / 1024.0 << " KBytes"
            << std::endl;


     bool correctness =test_correctness(data, new_ids, result);
     std::cout<<"correctness="<<correctness<<std::endl;

}





// void batch_pir_test(size_t party_id, size_t host_log_n_data, size_t batch_size, size_t feature_num)
// {
//     //-----------数据准备-----------------------
//     // size_t feature_num=8000/60;
//     // size_t host_log_n_data=20;
//     // size_t batch_size = 4096;
//     int host_n_data = 1<<host_log_n_data;
//     std::vector<std::vector<int64_t>> data(4); 

//     if (party_id==0)
//     {

//         for (size_t feature_id=0; feature_id<data.size(); feature_id++)
//         {
//             data[feature_id].resize(host_n_data);
//             for (int i = 0; i < host_n_data; i++) {
//                 data[feature_id][i] = random() % (1<<16);
//             }
//         }
//         Timer timer;
//         std::thread t;
//         //-----------Server initialize-----------------------
//         timer.reset();
//         t =   std::thread(threadShowMemUsage, "Server initialize");
//         auto bp_server = BatchPirServer(data, batch_size, feature_num);
//         auto server_init_time = timer.elapsed();
//         stop_sub_thread = true;
//         t.join();

//          //----------Server receive pk-----------------
//         bp_server.load_keys(keys);

//     //-----------Server generate respose-------------
//         timer.reset();
//         t = std::thread(threadShowMemUsage, "Server generate respose");
//         bool mode_switch=true;
//         std::stringstream response = bp_server.gen_response(query, mode_switch);
//         auto response_time = timer.elapsed();
//         stop_sub_thread = true;
//         t.join();

//          std::cout << "------------------------------------" << std::endl;
//   std::cout << "Performance: " << std::endl;
//   std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
// //   std::cout <<" client_init_time:" << client_init_time << " ms " << std::endl;
// //   std::cout << "Gen query time: " << query_time << " ms " << std::endl;
//   std::cout << "Gen response time: " << response_time << " ms " << std::endl;
// //   std::cout << "Extract answer time: " << extract_time << " ms "
//             // << std::endl;

// //   std::cout << "Query size: " << query.str().size() / 1024.0 << " KBytes"
// //             << std::endl;

//   std::cout << "Response size: " << response.str().size() / 1024.0 << " KBytes"
//             << std::endl;

//  std::cout<<"mem usage of server is"<<sizeof(bp_server)<<std::endl;
// //   std::cout<<"mem usage of client is"<<sizeof(bp_client)<<std::endl;
//     }
//     else
//     {
//         vector<int64_t> new_ids(batch_size);
//         for (int i=0; i<batch_size; i++)
//         {
//             new_ids[i]=random()%host_n_data;
//         }
//                     //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
//         //-----------Client initialize and send pk----------------------
//         timer.reset();
//         t = std::thread(threadShowMemUsage, "Client initialize");
//         auto bp_client = BatchPirClient(host_log_n_data, feature_num, batch_size);
//         auto keys=bp_client.save_keys();
//         auto client_init_time = timer.elapsed();
//         stop_sub_thread = true;
//         t.join();
//         // std::cout<<__FILE__<<":"<<__LINE__<<endl;

//      //-----------Client generate query ------------
//         timer.reset();
//         t = std::thread(threadShowMemUsage, "Client generate query");
//         std::stringstream query = bp_client.gen_query(new_ids);
//         auto query_time = timer.elapsed();
//         stop_sub_thread = true;
//         t.join();
//                 // std::cout<<__FILE__<<":"<<__LINE__<<endl;

//              // std::cout<<__FILE__<<":"<<__LINE__<<endl;
//      //-----------Client exact answer
//         timer.reset();
//             t = std::thread(threadShowMemUsage, "Client exact answer");
//         auto result = bp_client.extract_answer(response);
//         auto extract_time = timer.elapsed();
//             stop_sub_thread = true;
//         t.join();



//                  std::cout << "------------------------------------" << std::endl;
//   std::cout << "Performance: " << std::endl;
// //   std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
//   std::cout <<" client_init_time:" << client_init_time << " ms " << std::endl;
//   std::cout << "Gen query time: " << query_time << " ms " << std::endl;
// //   std::cout << "Gen response time: " << response_time << " ms " << std::endl;
//   std::cout << "Extract answer time: " << extract_time << " ms "
//             << std::endl;

//   std::cout << "Query size: " << query.str().size() / 1024.0 << " KBytes"
//             << std::endl;

// //   std::cout << "Response size: " << response.str().size() / 1024.0 << " KBytes"
//             // << std::endl;

// //  std::cout<<"mem usage of server is"<<sizeof(bp_server)<<std::endl;
//   std::cout<<"mem usage of client is"<<sizeof(bp_client)<<std::endl;
//     }

// }








void batch_pir_test_1024()
{
    //-----------数据准备-----------------------
    size_t feature_num=8000/60;
    size_t host_log_n_data=20;
    size_t batch_size = 1024;
    int host_n_data = 1<<host_log_n_data;
    std::vector<std::vector<int64_t>> data(4); 
    vector<int64_t> new_ids(batch_size);
    // const int64_t prime_num = 167772161UL;
    for (size_t feature_id=0; feature_id<data.size(); feature_id++)
    {
        data[feature_id].resize(host_n_data);
        for (int i = 0; i < host_n_data; i++) {
            data[feature_id][i] = random() % (1<<16);
        }
    }

    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=random()%host_n_data;
    }
    Timer timer;
     //-----------Server initialize-----------------------
    timer.reset();
     auto bp_server = BatchPirServer(data, batch_size, feature_num);
     auto server_init_time = timer.elapsed();
             std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client initialize and send pk----------------------
     timer.reset();
     auto bp_client = BatchPirClient(host_log_n_data, feature_num, batch_size);
     auto keys=bp_client.save_keys();
     auto client_init_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //----------Server receive pk-----------------
     bp_server.load_keys(keys);
     //-----------Client generate query ------------
     timer.reset();
     std::stringstream query = bp_client.gen_query(new_ids);
    auto query_time = timer.elapsed();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Server generate respose-------------
    timer.reset();
    bool mode_switch=true;
     std::stringstream response = bp_server.gen_response(query, mode_switch);
    auto response_time = timer.elapsed();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client exact answer
        timer.reset();
     auto result = bp_client.extract_answer(response);
    auto extract_time = timer.elapsed();

  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
  std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
  std::cout <<" sclient_init_time:" << client_init_time << " ms " << std::endl;
  std::cout << "Gen query time: " << query_time << " ms " << std::endl;
  std::cout << "Gen response time: " << response_time << " ms " << std::endl;
  std::cout << "Extract answer time: " << extract_time << " ms "
            << std::endl;

  std::cout << "Query size: " << query.str().size() / 1024.0 << " KBytes"
            << std::endl;

  std::cout << "Response size: " << response.str().size() / 1024.0 << " KBytes"
            << std::endl;


     bool correctness =test_correctness(data, new_ids, result);
     std::cout<<"correctness="<<correctness<<std::endl;

    // std::cout<<"result="<<std::endl;
    // for (int i=0; i<4; i++)
    // {
    //     std::cout<<result[0][i]<<std::endl;
    // }
    // std::cout<<"....."<<std::endl;
    // for (int i=batch_size-4; i<batch_size; i++)
    // {
    //     std::cout<<result[0][i]<<std::endl;
    // }


}



void batch_pir_test_2048()
{
    //-----------数据准备-----------------------
    size_t feature_num=4;
    size_t host_log_n_data=20;
    size_t batch_size = 2048;
    int host_n_data = 1<<host_log_n_data;
    std::vector<std::vector<int64_t>> data(4); 
    vector<int64_t> new_ids(batch_size);
    // const int64_t prime_num = 167772161UL;
    for (size_t feature_id=0; feature_id<data.size(); feature_id++)
    {
        data[feature_id].resize(host_n_data);
        for (int i = 0; i < host_n_data; i++) {
            data[feature_id][i] = random() % (1<<16);
        }
    }

    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=random()%host_n_data;
    }
    Timer timer;
     //-----------Server initialize-----------------------
    timer.reset();
     auto bp_server = BatchPirServer(data, batch_size, feature_num);
     auto server_init_time = timer.elapsed();
             std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client initialize and send pk----------------------
     timer.reset();
     auto bp_client = BatchPirClient(host_log_n_data, feature_num, batch_size);
     auto keys=bp_client.save_keys();
     auto client_init_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //----------Server receive pk-----------------
     bp_server.load_keys(keys);
     //-----------Client generate query ------------
     timer.reset();
     std::stringstream query = bp_client.gen_query(new_ids);
    auto query_time = timer.elapsed();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Server generate respose-------------
    timer.reset();
    bool mode_switch=true;
     std::stringstream response = bp_server.gen_response(query, mode_switch);
    auto response_time = timer.elapsed();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client exact answer
        timer.reset();
     auto result = bp_client.extract_answer(response);
    auto extract_time = timer.elapsed();

  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
  std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
  std::cout <<" sclient_init_time:" << client_init_time << " ms " << std::endl;
  std::cout << "Gen query time: " << query_time << " ms " << std::endl;
  std::cout << "Gen response time: " << response_time << " ms " << std::endl;
  std::cout << "Extract answer time: " << extract_time << " ms "
            << std::endl;

  std::cout << "Query size: " << query.str().size() / 1024.0 << " KBytes"
            << std::endl;

  std::cout << "Response size: " << response.str().size() / 1024.0 << " KBytes"
            << std::endl;

     bool correctness =test_correctness(data, new_ids, result);
     std::cout<<"correctness="<<correctness<<std::endl;

    // std::cout<<"result="<<std::endl;
    // for (int i=0; i<4; i++)
    // {
    //     std::cout<<result[0][i]<<std::endl;
    // }
    // std::cout<<"....."<<std::endl;
    // for (int i=batch_size-4; i<batch_size; i++)
    // {
    //     std::cout<<result[0][i]<<std::endl;
    // }


}

int main(int argc, char *argv[]) {
  if (argc < 4) {

    // bool modswitch=true;
    // batch_pir_poly_reduce_modswitch_test(modswitch);
    // batch_pir_poly_reduce_modswitch_real_test(modswitch);
    // batch_pir_poly_reduce_type0_real_test();
    // batch_pir_test();
    // batch_pir_test_1024();
    batch_pir_test_2048();
    std::cout<<"argc less than 4"<<std::endl;
  }
  else
  {
    size_t host_log_n_data = atoi(argv[1]);
    size_t batch_size = atoi(argv[2]);
    size_t payload_bytes= atoi(argv[3]);

    std::cout<<"host_log_n_data="<<host_log_n_data<<std::endl;
        std::cout<<"batch_size="<<batch_size<<std::endl;
        std::cout<<"payload_bytes="<<payload_bytes<<std::endl;
     size_t feature_num = (payload_bytes+49)/50*8;
    batch_pir_test(host_log_n_data, batch_size, feature_num);
  }


        return 0;
}
