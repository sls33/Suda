#include "include/psi_to_share.h"
// #include "include/utils.h"
// #include "include/netio.h"

using namespace std;
using namespace seal;



void psi_to_share_test(bool mode_switch, size_t batch_size)
{
    //-----------数据准备--------- --------------
    size_t feature_num=4;
    size_t host_log_n_data=20;
    int host_n_data = 1<<host_log_n_data;
    std::vector<string> server_ids(host_n_data);
    std::vector<std::vector<int64_t>> data(4); 
    
    vector<string> client_ids(batch_size);
        int start=0;
    vector<bool> client_labels(batch_size);
    // const int64_t prime_num = 167772161UL;
    for (size_t i=0; i<host_n_data; i++)
    {
        server_ids[i]=std::to_string(i);
    }
    for (size_t feature_id=0; feature_id<data.size(); feature_id++)
    {
        data[feature_id].resize(host_n_data);
        for (int i = 0; i < host_n_data; i++) {
            data[feature_id][i] = random() % 100;
        }
    }


    for (int i=0; i<batch_size; i++)
    {
        // client_ids[i]=std::to_string(i);  
       client_ids[i]=std::to_string(i-start);  // 使得交集非client侧全集
        client_labels[i] = ((i/8)%2) | ((i/4)%2) &  ((i/2)%2) |  (i%2);
    }
    Timer timer;
     //-----------Server initialize-----------------------
    timer.reset();
     auto pss_server = PSItoShareServer(server_ids, data, batch_size, feature_num);
     auto server_init_time = timer.elapsed();
             std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client initialize and send pk----------------------
     timer.reset();
    auto pss_client = PSItoShareClient(host_log_n_data, feature_num, batch_size);
     PK keys=pss_client.save_keys();
     auto client_init_time = timer.elapsed();
     //----------Server receive pk-----------------
     pss_server.load_keys(keys);
      std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //---------------stage 1  get new_ids
        //-----------Client generate query ------------
    timer.reset();
     ECPSIQuery psi_query = pss_client.generate_psi_query(client_ids, client_labels);
         auto psi_query_time = timer.elapsed();
          std::cout<<__FILE__<<":"<<__LINE__<<endl;
        //------------server generate response
        timer.reset();
     ECPSIResponce psi_response = pss_server.generate_psi_response(psi_query);
     auto psi_response_time = timer.elapsed();
      std::cout<<__FILE__<<":"<<__LINE__<<endl;
        //------------client generate response 
        timer.reset();
    std::vector<int32_t> intersection_lastids = pss_client.generate_psi_response(psi_response);
     auto psi_client_response_time = timer.elapsed();
      std::cout<<__FILE__<<":"<<__LINE__<<endl;
    // -----------server return answer
       timer.reset();
    std::vector<bool>  label_shares0 =  pss_server.return_psi_answer(intersection_lastids);  
    auto psi_server_return_answer_time = timer.elapsed();
     std::cout<<__FILE__<<":"<<__LINE__<<endl;
    //-----------client rerurn answer
     timer.reset();
        std::vector<Int32AndBool> new_ids_and_label_shares1 = pss_client.return_psi_answer();    
        std::vector<int64_t> new_ids(new_ids_and_label_shares1.size());
        std::vector<bool> label_shares1(new_ids_and_label_shares1.size());
         std::cout<<__FILE__<<":"<<__LINE__<<endl;
        for (size_t i=0; i<new_ids_and_label_shares1.size(); i++)
        {
            new_ids[i] = new_ids_and_label_shares1[i].a;
            label_shares1[i] = new_ids_and_label_shares1[i].b;
        }
          auto psi_client_return_answer_time = timer.elapsed();
 std::cout<<__FILE__<<":"<<__LINE__<<endl;
//  std::cout<<"new_ids="<<std::endl;
//     print(new_ids);
     //----------------Stage 2  batch pir to share



     //-----------Client generate query ------------
     timer.reset();
     //-------------pad new_ids to batch_size
      size_t intersection_size = new_ids.size();
      for (size_t i=intersection_size; i<batch_size; i++)
      {
         new_ids.push_back(host_n_data - i);
         
      }
        //-----
     BatchPirToShareQuery query = pss_client.gen_BPS_query(new_ids);
    auto query_time = timer.elapsed();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Server generate respose-------------
    timer.reset();
     std::stringstream response = pss_server.gen_BPS_response(query, mode_switch);
    auto response_time = timer.elapsed();
         auto result0 = pss_server.extract_BPS_answer();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
     //-----------Client exact answer
        timer.reset();
     auto result1 = pss_client.extract_BPS_answer(response);
    auto extract_time = timer.elapsed();


  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
  std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
  std::cout <<" client_init_time:" << client_init_time << " ms " << std::endl;
     std::cout <<"get_newid: Gen psi_query time:" << psi_query_time << " ms " << std::endl;

      std::cout <<"get_newid: Gen psi_response time:" << psi_response_time << " ms " << std::endl;
       std::cout <<"get_newid: Gen psi_client_response time:" << psi_client_response_time << " ms " << std::endl;
        // std::cout <<"get_newid: psi_server_return_answer time:" << psi_server_return_answer_time << " ms " << std::endl;
        // std::cout <<"get_newid: psi_client_return_answer time:" << psi_client_return_answer_time << " ms " << std::endl;

  std::cout << "batch pir to share: Gen query time: " << query_time << " ms " << std::endl;
  std::cout << "batch pir to share: Gen response time: " << response_time << " ms " << std::endl;
  std::cout << "batch pir to share: Extract answer time: " << extract_time << " ms "
            << std::endl;


    auto server_online_time =  psi_response_time + response_time;
    auto client_online_time = psi_query_time + psi_client_response_time + query_time + extract_time;
    std::cout<<"server_online_time="<<server_online_time << " ms " << std::endl;
    std::cout<<"client_online_time="<<client_online_time << " ms " << std::endl;

  std::cout << "get_newid query size: "<< psi_query.byte_size() / 1024.0 << " KBytes"
            << std::endl;
  std::cout << "get_newid response size: "<<psi_response.byte_size() / 1024.0 << " KBytes"
            << std::endl;

  std::cout<< "get new_id client response size: "<< intersection_lastids.size() * sizeof(int32_t) / 1024.0 << " KBytes"
            << std::endl;

  std::cout << "batch pir to share Query size: " << query.str().size() / 1024.0 << " KBytes"
            << std::endl;

  std::cout << "batch pir to share Response size: " << response.str().size() / 1024.0 << " KBytes"
            << std::endl;

    std::cout<< "comm. from client to server: "<< (psi_query.byte_size()+intersection_lastids.size() * sizeof(int32_t)+ query.str().size()) / 1024.0 << " KBytes"
            << std::endl;

    std::cout<<"comm. from server to client: "<< (psi_response.byte_size() + response.str().size())/ 1024.0 << " KBytes"
            << std::endl;

     bool feature_correctness =test_feature_correctness(data, new_ids, result0, result1, pss_client.get_prime_num(), intersection_size);
     std::cout<<"feature_correctness="<<feature_correctness<<std::endl;
    bool label_correctness = test_label_correctness(client_labels, start, label_shares0, label_shares1);
    std::cout<<"label_correctness="<<label_correctness<<std::endl;
}








void psi_to_share_test(bool mode_switch, size_t batch_size, size_t player)
{
    //-----------数据准备--------- --------------
    size_t feature_num=4;
    size_t host_log_n_data=20;
    int host_n_data = 1<<host_log_n_data;
    size_t player_server=1;
    size_t player_client=1;
            std::vector<string> server_ids;
        std::vector<std::vector<int64_t>> data; 
        shared_ptr<PSItoShareServer> pss_server = nullptr;
    shared_ptr<PSItoShareClient> pss_client = nullptr;
    PK keys;
    ECPSIQuery psi_query;
    vector<string> client_ids;
    vector<bool> client_labels;
    ECPSIResponce psi_response;
    std::vector<int32_t> intersection_lastids;
    std::vector<bool>  label_shares0;
    std::vector<Int32AndBool> new_ids_and_label_shares1;
    std::vector<int64_t> new_ids;
    std::vector<bool> label_shares1;
     BatchPirToShareQuery query;
     std::stringstream response;
     double server_init_time, client_init_time, psi_query_time, psi_response_time, 
     psi_client_response_time, psi_server_return_answer_time, response_time,
     psi_client_return_answer_time, query_time, extract_time;
     std::vector<std::vector<int64_t>> result0, result1;
    size_t intersection_size;
            int start=100;

    if (player==player_server)
    {

        server_ids.resize(host_n_data);
        data.resize(4);

        // const int64_t prime_num = 167772161UL;
        for (size_t i=0; i<host_n_data; i++)
        {
            server_ids[i]=std::to_string(i);
        }
        for (size_t feature_id=0; feature_id<data.size(); feature_id++)
        {
            data[feature_id].resize(host_n_data);
            for (int i = 0; i < host_n_data; i++) {
                data[feature_id][i] = random() % 100;
            }
        }
    }
    
    if (player==player_client)
    {
        client_ids.resize(batch_size);
        client_labels.resize(batch_size);
        for (int i=0; i<batch_size; i++)
        {
            // client_ids[i]=std::to_string(i);  
            client_ids[i]=std::to_string(i-start);  // 使得交集非client侧全集
            client_labels[i] = ((i/8)%2) | ((i/4)%2) &  ((i/2)%2) |  (i%2);
        }
    }
    
    Timer timer;
     //-----------Server initialize-----------------------
     if (player==player_server)
     {
        timer.reset();
        // auto pss_server = PSItoShareServer(server_ids, data, batch_size, feature_num);
        pss_server = make_shared<PSItoShareServer>(server_ids, data, batch_size, feature_num);
        server_init_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
     }

    //-----------Client initialize and send pk----------------------
    if (player==player_client)
    {
        timer.reset();
        // auto pss_client = PSItoShareClient(host_log_n_data, feature_num, batch_size);
        pss_client =  make_shared<PSItoShareClient>(host_log_n_data, feature_num, batch_size);
        // PK keys=pss_client.save_keys();
        keys=pss_client->save_keys();
        client_init_time = timer.elapsed();
    }

     //----------Server receive pk-----------------
    if (player==player_server)
    {
        pss_server->load_keys(keys);
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
     
     //---------------stage 1  get new_ids
        //-----------Client generate query ------------
    if (player==player_client){
        timer.reset();
        // ECPSIQuery psi_query = pss_client->generate_psi_query(client_ids, client_labels);
        psi_query = pss_client->generate_psi_query(client_ids, client_labels);
        psi_query_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------server generate response
    if (player==player_server){
        timer.reset();
        psi_response = pss_server->generate_psi_response(psi_query);
        psi_response_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------client generate response 
    if (player==player_client){
        timer.reset();
        intersection_lastids = pss_client->generate_psi_response(psi_response);
        psi_client_response_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
    // -----------server return answer
    if (player==player_server){
        timer.reset();
        label_shares0 =  pss_server->return_psi_answer(intersection_lastids);  
        psi_server_return_answer_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
    //-----------client rerurn answer
    if (player==player_client){
        timer.reset();
        new_ids_and_label_shares1 = pss_client->return_psi_answer();    
        // std::vector<int64_t> new_ids(new_ids_and_label_shares1.size());
        // std::vector<bool> label_shares1(new_ids_and_label_shares1.size());
        new_ids.resize(new_ids_and_label_shares1.size());
        label_shares1.resize(new_ids_and_label_shares1.size());
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        for (size_t i=0; i<new_ids_and_label_shares1.size(); i++)
        {
            new_ids[i] = new_ids_and_label_shares1[i].a;
            label_shares1[i] = new_ids_and_label_shares1[i].b;
        }
        psi_client_return_answer_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
     //----------------Stage 2  batch pir to share



     //-----------Client generate query ------------
    if (player==player_client){
        timer.reset();
        //-------------pad new_ids to batch_size
        intersection_size = new_ids.size();
        for (size_t i=intersection_size; i<batch_size; i++)
        {
            new_ids.push_back(host_n_data - i);
            
        }
        query = pss_client->gen_BPS_query(new_ids);
        query_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //-----------Server generate respose-------------
         if (player==player_server){
    timer.reset();
     response = pss_server->gen_BPS_response(query, mode_switch);
    response_time = timer.elapsed();
         result0 = pss_server->extract_BPS_answer();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
         }

     //-----------Client exact answer
         if (player==player_client){
        timer.reset();
     result1 = pss_client->extract_BPS_answer(response);
    extract_time = timer.elapsed();
        }




  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
           if (player==player_server){
  std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
        std::cout <<"get_newid: Gen psi_response time:" << psi_response_time << " ms " << std::endl;
          std::cout << "batch pir to share: Gen response time: " << response_time << " ms " << std::endl;
              auto server_online_time =  psi_response_time + response_time;
    
    std::cout<<"server_online_time="<<server_online_time << " ms " << std::endl;

      std::cout << "get_newid response size: "<<psi_response.byte_size() / 1024.0 << " KBytes"
            << std::endl;
      std::cout << "batch pir to share Response size: " << response.str().size() / 1024.0 << " KBytes"
            << std::endl;
    std::cout<<"comm. from server to client: "<< (psi_response.byte_size() + response.str().size())/ 1024.0 << " KBytes"
            << std::endl;
           }

               if (player==player_client){
  std::cout <<" sclient_init_time:" << client_init_time << " ms " << std::endl;
     std::cout <<"get_newid: Gen psi_query time:" << psi_query_time << " ms " << std::endl;
       std::cout <<"get_newid: Gen psi_client_response time:" << psi_client_response_time << " ms " << std::endl;
         std::cout << "batch pir to share: Gen query time: " << query_time << " ms " << std::endl;
           std::cout << "batch pir to share: Extract answer time: " << extract_time << " ms "
            << std::endl;
        auto client_online_time = psi_query_time + psi_client_response_time + query_time + extract_time;
        std::cout<<"client_online_time="<<client_online_time << " ms " << std::endl;
    

  std::cout << "get_newid query size: "<< psi_query.byte_size() / 1024.0 << " KBytes"
            << std::endl;
      std::cout<< "get new_id client response size: "<< intersection_lastids.size() * sizeof(int32_t) / 1024.0 << " KBytes"
            << std::endl;
    
  std::cout << "batch pir to share Query size: " << query.str().size() / 1024.0 << " KBytes"
            << std::endl;
    std::cout<< "comm. from client to server: "<< (psi_query.byte_size()+intersection_lastids.size() * sizeof(int32_t)+ query.str().size()) / 1024.0 << " KBytes"
            << std::endl;
               }
     bool feature_correctness =test_feature_correctness(data, new_ids, result0, result1, pss_client->get_prime_num(), intersection_size);
     std::cout<<"feature_correctness="<<feature_correctness<<std::endl;
    bool label_correctness = test_label_correctness(client_labels, start, label_shares0, label_shares1);
    std::cout<<"label_correctness="<<label_correctness<<std::endl;
}





int main(int argc, char *argv[])
{
    bool modswitch=true;
    // batch_pir_to_share_test(modswitch);
    // psi_to_share_test(modswitch, 4096);
    // psi_to_share_test(modswitch, 1024);
    // psi_to_share_test(modswitch, 1024, 1);
    // psi_to_share_comm_test(modswitch, 1024, 1);


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
    else if (argc==6)
    {
        int host_log_n_data = std::stoi(argv[1]);
        int batch_size = std::stoi(argv[2]); 
        int feature_num = std::stoi(argv[3]);
        int party_id = std::stoi(argv[4]);
        float inter_ratio = std::stof(argv[5]);
        std::cout<<"host_log_n_data="<<host_log_n_data<<std::endl;
        std::cout<<"batch_size="<<batch_size<<std::endl;
        std::cout<<"feature_num="<<feature_num<<std::endl;
        std::cout<<"party_id="<<party_id<<std::endl;
        std::cout<<"intersection ratio="<<inter_ratio<<std::endl;
        // psi_to_share_2party(modswitch, host_log_n_data, batch_size, feature_num, party_id);
        psi_to_share_2party_socketopt(modswitch, host_log_n_data, batch_size, feature_num, party_id, inter_ratio);
    }
    else
    {
        // psi_to_share_test(modswitch, 1024, 1);
        psi_to_share_test(modswitch, 1024);
    }
    return 0;
    
}