#include "include/batch_pir_to_share.h"
#include "include/utils.h"

using namespace std;
using namespace seal;


void batch_pir_to_share_test(bool modswitch)
{

    bool isHost=1;
    bool isGuest=1;
    size_t feature_num=10;
    size_t host_log_n_data=20;
    size_t batch_size = 4096;
    int host_n_data = 1<<host_log_n_data;
    std::vector<int64_t> data(host_n_data), new_ids(batch_size);
    const int64_t prime_num = 167772161UL;
    for (int i = 0; i < host_n_data; i++) {
        data[i] = random() % prime_num;
    }
    std::cout<<"data="<<std::endl;
    for (int i=0; i<4; i++)
    {
        std::cout<<data[i]<<std::endl;
    }
    std::cout<<"...."<<std::endl;
    for (int i=batch_size-4; i<batch_size; i++)
    {
        std::cout<<data[i]<<std::endl;
    }

    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=i;
    }
    vector<vector<int64_t>>  result0, result1;
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        Timer timer;
     //-----------Server initialize-----------------------
    timer.reset();
    batch_pir_to_share_from_nttform_modswitch(isHost, isGuest, data, new_ids, result0, result1, feature_num, host_log_n_data, batch_size, modswitch);
         auto run_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
  std::cout <<" run_time:" << run_time << " ms " << std::endl;
    std::cout<<"result0[0].size()"<<result0[0].size()<<std::endl;

    // vector<int64_t> result = poly_guest->reveal_poly_value(poly_share0, poly_share1, new_ids);
   
 //--------------------host & guest for test--------------


    for (int feature_id=0; feature_id<1; feature_id++)
    {
        std::cout<<"result="<<std::endl;
        for (int i=0; i<4; i++)
        {
            std::cout<<(result0[feature_id][i]+result1[feature_id][i])%prime_num<<std::endl;
        }
        std::cout<<"....."<<std::endl;
        for (int i=batch_size-4; i<batch_size; i++)
        {
            std::cout<<(result0[feature_id][i]+result1[feature_id][i])%prime_num<<std::endl;
        }
    }
    
}





//---------------------------------------------------------------------------

void batch_pir_to_share_test1(bool mode_switch, size_t batch_size)
{
    //-----------数据准备--------- --------------
    size_t feature_num=4; //20000*8/27;
    size_t host_log_n_data=20;
    // size_t batch_size = 4096;
    int host_n_data = 1<<host_log_n_data;
    std::vector<std::vector<int64_t>> data(4); 
    vector<int64_t> new_ids(batch_size);
    // const int64_t prime_num = 167772161UL;
    for (size_t feature_id=0; feature_id<data.size(); feature_id++)
    {
        data[feature_id].resize(host_n_data);
        for (int i = 0; i < host_n_data; i++) {
            data[feature_id][i] =  random() % 1024;
        }
    }

    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=(17*i)%host_n_data;
    }
    Timer timer;
     //-----------Server initialize-----------------------
    timer.reset();
     auto bp_server = BatchPirToShareServer(data, batch_size, feature_num);
     auto server_init_time = timer.elapsed();
             std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client initialize and send pk----------------------
     timer.reset();
     auto bp_client = BatchPirToShareClient(host_log_n_data, feature_num, batch_size);
     auto keys=bp_client.save_keys();
     auto client_init_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //----------Server receive pk-----------------
     bp_server.load_keys(keys);
     //-----------Client generate query ------------
     timer.reset();
    //  BatchPirToShareQuery query = bp_client.gen_query(new_ids);
     BatchPirToShareQueryStream query = bp_client.gen_query_stream(new_ids);
    auto query_time = timer.elapsed();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Server generate respose-------------
    timer.reset();
     std::stringstream response = bp_server.gen_response(query, mode_switch);
    auto response_time = timer.elapsed();
         auto result0 = bp_server.extract_answer();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client exact answer
        timer.reset();
     auto result1 = bp_client.extract_answer(response);
    auto extract_time = timer.elapsed();


  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
  std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
  std::cout <<" sclient_init_time:" << client_init_time << " ms " << std::endl;
  std::cout << "Gen query time: " << query_time << " ms " << std::endl;
  std::cout << "Gen response time: " << response_time << " ms " << std::endl;
  std::cout << "Extract answer time: " << extract_time << " ms "
            << std::endl;

  std::cout << "Query size: " << query.byte_size() / 1024.0 << " KBytes"
            << std::endl;

  std::cout << "Response size: " << response.str().size() / 1024.0 << " KBytes"
            << std::endl;

     bool correctness =test_feature_correctness(data, new_ids, result0, result1, bp_client.get_prime_num());
     std::cout<<"correctness="<<correctness<<std::endl;

}






int main()
{
    bool modswitch=true;
    // batch_pir_to_share_test(modswitch);
    // batch_pir_to_share_test1(modswitch, 4096);
    batch_pir_to_share_test1(modswitch, 1024);
}