#ifndef PSI_TO_SHARE
#define PSI_TO_SHARE
#include "ec_psi.h"
#include "batch_pir_to_share.h"
// #include "include/utils.h"
// #include "include/netio.h"
#include "math.h"
#include "utils.h"
#include "netio.h"
#include "file_io.h"
#include "mem_usage.h"
// #define DEBUG

bool with_comm=true;
string server_address = "localhost:4322";
string client_address = "localhost:4321";

class PSItoShareServer {
   private:
   shared_ptr<ECPlayer0> ec_player = nullptr;
   shared_ptr<BatchPirToShareServer> batch_pir_to_share_server = nullptr;
   coproto::Socket send_socket;
   coproto::Socket recv_socket;
//    string send_address = "localhost:4321";
//    string recv_address = "localhost:4322";
   string send_address = client_address;
   string recv_address = server_address;
   bool lazy;
    vector<vector<int64_t>> data;
    size_t feature_num;

public:
   PSItoShareServer(const vector<string> plaintext_ids, const vector<vector<int64_t>> & data, size_t client_size, size_t feature_num, bool lazy=false, bool use_out_mem=false): data(data), feature_num(feature_num), lazy(lazy)
   {
      ec_player = make_shared<ECPlayer0>();
      ec_player->init(plaintext_ids);
      if (!lazy)
      {
batch_pir_to_share_server = make_shared<BatchPirToShareServer>(data, client_size, feature_num, use_out_mem);
      }
        if (with_comm)
       {
        send_socket = coproto::asioConnect(send_address, false);
        recv_socket = coproto::asioConnect(recv_address, true);
       }

   }
   ~PSItoShareServer() {
    //        if (with_comm)
    //    {
    //     send_socket.close();
    //     recv_socket.close();
    //    }
    }

   int64_t get_prime_num()
   {
      return batch_pir_to_share_server->get_prime_num();
   }

coproto::Socket get_send_socket()
{
    return send_socket;
}
coproto::Socket get_recv_socket()
{
    return recv_socket;
}

     void load_keys(PK& pk, string stage="all") 
     {
         if (stage=="psi" || stage=="all")
         {
            ec_player->set_public_key(pk.psi_pk);
         }
         if (stage=="bps" || stage=="all")
         {
            batch_pir_to_share_server->load_keys(pk.bps_pk);
         }  
     }


   // step 2
    ECPSIResponce generate_psi_response(ECPSIQuery & query)
    {
      return ec_player->generate_response(query);
    }

   //step 4
   std::vector<bool> return_psi_answer(std::vector<int32_t> intersection_lastids)
   {
      return ec_player->return_answer(intersection_lastids);
   }

//-----------------------------------------------------

    void init_BPS(size_t batch_size)
    {
        batch_pir_to_share_server = make_shared<BatchPirToShareServer>(data, batch_size, feature_num);
    }
   //step 6
   stringstream gen_BPS_response(BatchPirToShareQuery & query, bool mod_switch=false)
   {
      return batch_pir_to_share_server->gen_response(query, mod_switch);
   }
   stringstream gen_BPS_response(BatchPirToShareQueryStream & query, bool mod_switch=false)
   {
      return batch_pir_to_share_server->gen_response(query, mod_switch);
   }
   //step 7
     std::vector<std::vector<int64_t>> extract_BPS_answer()
     {
         return batch_pir_to_share_server->extract_answer();
     }

    size_t get_sent_bytes(){
        return  send_socket.bytesSent()+recv_socket.bytesSent();
    }
    size_t get_recv_bytes(){
        return  recv_socket.bytesReceived()+send_socket.bytesReceived();
    }
    

};

class PSItoShareClient {
   private:
   size_t batch_size;
   shared_ptr<ECPlayer1> ec_player = nullptr;
   shared_ptr<BatchPirToShareClient> batch_pir_to_share_client = nullptr;
   coproto::Socket send_socket, recv_socket;
//    string send_address = "localhost:4322";
//    string recv_address = "localhost:4321";
      string send_address = server_address;
   string recv_address = client_address;
   size_t host_log_n_data, feature_num;
   bool lazy;
   public:

   PSItoShareClient(size_t host_log_n_data, size_t feature_num, size_t batch_size, bool lazy=false) : host_log_n_data(host_log_n_data), feature_num(feature_num), batch_size(batch_size), lazy(lazy)
   {
      ec_player = make_shared<ECPlayer1>(); 
      if (!lazy)
      {
        batch_pir_to_share_client = make_shared<BatchPirToShareClient>(host_log_n_data, feature_num, batch_size);
      }
        if (with_comm)
       {
      recv_socket = coproto::asioConnect(recv_address, true);
      send_socket = coproto::asioConnect(send_address, false);
       }
   }


    ~PSItoShareClient() {
    //     if (with_comm)
    //    {
    //     send_socket.close();
    //     recv_socket.close();
    //    }
    }

   int64_t get_prime_num()
   {
      return batch_pir_to_share_client->get_prime_num();
   }
coproto::Socket get_send_socket()
{
    return send_socket;
}
coproto::Socket get_recv_socket()
{
    return recv_socket;
}

   PK save_keys(string stage="all")
   {
    
      PK pk;
      std::cout<<__FILE__<<":"<<__LINE__<<endl;
      if (stage=="psi" || stage=="all")
      {
        ec_player->save_public_key(pk.psi_pk);
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
      }
      if (stage=="bps" || stage=="all")
      {        
        pk.bps_pk = batch_pir_to_share_client->save_keys();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
      }
      return pk;
   }


   // step 1
   ECPSIQuery generate_psi_query(vector<string> batch_ids, vector<bool> batch_labels)
   {
      return ec_player->generate_query(batch_ids, batch_labels);
   }

   // step 3
   std::vector<int32_t> generate_psi_response(ECPSIResponce response)
   {
      return ec_player->generate_response(response);
   } 

   // step 4
   std::vector<Int32AndBool> return_psi_answer()
   {
      return ec_player->return_answer();
   }


   //--------------------------------------------
   // step 5

   void init_BPS(size_t batch_size)
   {
    batch_pir_to_share_client = make_shared<BatchPirToShareClient>(host_log_n_data, feature_num, batch_size);

   }

   BatchPirToShareQuery gen_BPS_query(const vector<int64_t> & new_ids)
   {
      return batch_pir_to_share_client->gen_query(new_ids);
   }

   BatchPirToShareQueryStream gen_BPS_query_stream(const vector<int64_t> & new_ids)
   {
      return batch_pir_to_share_client->gen_query_stream(new_ids);
   }

   //step 7

   vector<vector<int64_t>> extract_BPS_answer(stringstream & reduced_poly_cipher_stream)
   {
      return batch_pir_to_share_client->extract_answer(reduced_poly_cipher_stream);
   }

    size_t get_sent_bytes(){
        return  send_socket.bytesSent();
    }
    size_t get_recv_bytes(){
        return  recv_socket.bytesReceived();
    }

};



void psi_to_share_2party(bool mode_switch, size_t host_log_n_data, size_t batch_size, size_t feature_num, size_t player)
{
    //-----------数据准备--------- --------------
    std::string addres = "localhost:4321";
    // size_t feature_num=4;
    // size_t host_log_n_data=20;
    int host_n_data = 1<<host_log_n_data;
    size_t player_server=0;
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
     BatchPirToShareQueryStream query;
    //  BatchPirToShareQuery query;
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
    
    Timer timer, timer_total;
    timer_total.reset();
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
        if (player_server!=player_client)
        {
             send_PK(keys, addres);
        }
        client_init_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //----------Server receive pk-----------------
    if (player==player_server)
    {
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {
            keys = recv_PK(addres);
        }
        pss_server->load_keys(keys);
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
     
     //---------------stage 1  get new_ids
        //-----------Client generate query ------------
    if (player==player_client){
        timer.reset();
        psi_query = pss_client->generate_psi_query(client_ids, client_labels);
        psi_query_time = timer.elapsed();
        if (player_server!=player_client)
        {
            send_ECPSIQuery(psi_query, addres);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------server generate response
    if (player==player_server){
        if (player_server!=player_client)
        {
            psi_query = recv_ECPSIQuery(addres);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
        psi_response = pss_server->generate_psi_response(psi_query);
        psi_response_time = timer.elapsed();
        if (player_server!=player_client)
        {
            send_ECPSIResponce(psi_response,  addres);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------client generate response 
    if (player==player_client){
        if (player_server!=player_client)
        {
            psi_response = recv_ECPSIResponce(addres);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
        intersection_lastids = pss_client->generate_psi_response(psi_response);
        psi_client_response_time = timer.elapsed();
        if (player_server!=player_client)
        {
            send_vector_auto<int>(intersection_lastids, addres);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
    // -----------server return answer
    if (player==player_server){
        if (player_server!=player_client)
        {
            intersection_lastids = recv_vector_auto<int>(addres);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
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
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
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
        query = pss_client->gen_BPS_query_stream(new_ids);
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        //  query = pss_client->gen_BPS_query(new_ids);
        query_time = timer.elapsed();
        if (player_server!=player_client)
        {
            send_BatchPirToShareQueryStream(query, addres);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //-----------Server generate respose-------------
    if (player==player_server){
        if (player_server!=player_client)
        {
            query = recv_BatchPirToShareQueryStream(addres);
        }
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
    timer.reset();
     response = pss_server->gen_BPS_response(query, mode_switch);
    response_time = timer.elapsed();
           std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {
            send_stringstream(response, addres);
        }
                // std::cout<<__FILE__<<":"<<__LINE__<<endl;
         result0 = pss_server->extract_BPS_answer();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
         }

     //-----------Client exact answer
         if (player==player_client){
        if (player_server!=player_client)
        {
            response = recv_stringstream(addres);
        }
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
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
    
  std::cout << "batch pir to share Query size: " << query.byte_size() / 1024.0 << " KBytes"
            << std::endl;
    std::cout<< "comm. from client to server: "<< (psi_query.byte_size()+intersection_lastids.size() * sizeof(int32_t)+ query.byte_size()) / 1024.0 << " KBytes"
            << std::endl;
               }


    if (player_server==player_client)
    {
     bool feature_correctness =test_feature_correctness(data, new_ids, result0, result1, pss_client->get_prime_num(), intersection_size);
     std::cout<<"feature_correctness="<<feature_correctness<<std::endl;
    bool label_correctness = test_label_correctness(client_labels, start, label_shares0, label_shares1);
    std::cout<<"label_correctness="<<label_correctness<<std::endl;
    }

    std::cout<<"total time ="<<timer_total.elapsed()<<std::endl;

}









void psi_to_share_2party_socketopt(bool mode_switch, size_t host_log_n_data, size_t batch_size, size_t feature_num, size_t player)
{
    //-----------数据准备--------- --------------
    // size_t feature_num=4;
    // size_t host_log_n_data=20;
    int host_n_data = 1<<host_log_n_data;
    size_t player_server=0;
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
     BatchPirToShareQueryStream query;
    //  BatchPirToShareQuery query;
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
    
    Timer timer, timer_total;
    timer_total.reset();
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
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {    auto socket = pss_client->get_send_socket();
             send_PK_socket(keys, socket);
        }
        client_init_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //----------Server receive pk-----------------
    if (player==player_server)
    {
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            keys = recv_PK_socket(socket);
        }
        pss_server->load_keys(keys);
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
     
     //---------------stage 1  get new_ids
        //-----------Client generate query ------------
    if (player==player_client){
        timer.reset();
        psi_query = pss_client->generate_psi_query(client_ids, client_labels);
        psi_query_time = timer.elapsed();
        if (player_server!=player_client)
        {   auto socket = pss_client->get_send_socket();
            send_ECPSIQuery_socket(psi_query, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------server generate response
    if (player==player_server){
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            psi_query = recv_ECPSIQuery_socket(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
        psi_response = pss_server->generate_psi_response(psi_query);
        psi_response_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_send_socket();
            send_ECPSIResponce_socket(psi_response, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------client generate response 
    if (player==player_client){
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_recv_socket();
            psi_response = recv_ECPSIResponce_socket(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
        intersection_lastids = pss_client->generate_psi_response(psi_response);
        psi_client_response_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_send_socket();
            send_vector_auto_socket<int>(intersection_lastids, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
    // -----------server return answer
    if (player==player_server){
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_recv_socket();
            intersection_lastids = recv_vector_auto_socket<int>(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
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
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
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
        query = pss_client->gen_BPS_query_stream(new_ids);
        //  query = pss_client->gen_BPS_query(new_ids);
        query_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_send_socket();
            send_BatchPirToShareQueryStream_socket(query, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //-----------Server generate respose-------------
    if (player==player_server){
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            query = recv_BatchPirToShareQueryStream_socket(socket);
        }
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
    timer.reset();
     response = pss_server->gen_BPS_response(query, mode_switch);
    response_time = timer.elapsed();
           std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_send_socket();
            send_stringstream_socket(response, socket);
        }
                // std::cout<<__FILE__<<":"<<__LINE__<<endl;
         result0 = pss_server->extract_BPS_answer();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
         }

     //-----------Client exact answer
         if (player==player_client){
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_recv_socket();
            response = recv_stringstream_socket(socket);
        }
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
     result1 = pss_client->extract_BPS_answer(response);
    extract_time = timer.elapsed();
        }




  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
           if (player==player_server){
#ifdef DEBUG
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
#endif
                // std::cout<<"sent bytes ="<<pss_server->get_sent_bytes()/1024.0<<"KB"<<std::endl;
            // std::cout<<"recv bytes ="<<pss_server->get_recv_bytes()/1024.0<<"KB"<<std::endl;
                std::cout<<"comm. from server to client: "<< (psi_response.byte_size() + response.str().size())/ 1024.0 << " KBytes"
            << std::endl;
            std::cout<<"mem usage of server: "<<GetMemoryUsage()<<"MB"<<std::endl;
                  std::cout<<"total time of server="<<timer_total.elapsed()<<std::endl;
           }

               if (player==player_client){
#ifdef DEBUG
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
    
  std::cout << "batch pir to share Query size: " << query.byte_size() / 1024.0 << " KBytes"
            << std::endl;
    std::cout<< "comm. from client to server: "<< (psi_query.byte_size()+intersection_lastids.size() * sizeof(int32_t)+ query.byte_size()) / 1024.0 << " KBytes"
            << std::endl;
#endif
    std::cout<< "comm. from client to server: "<< (psi_query.byte_size()+intersection_lastids.size() * sizeof(int32_t)+ query.byte_size()) / 1024.0 << " KBytes"
            << std::endl;
            // std::cout<<"sent bytes ="<<pss_client->get_sent_bytes()/1024.0<<"KB"<<std::endl;
            // std::cout<<"recv bytes ="<<pss_client->get_recv_bytes()/1024.0<<"KB"<<std::endl;
            std::cout<<"mem usage of client: "<<GetMemoryUsage()<<"MB"<<std::endl;
                std::cout<<"total time of client="<<timer_total.elapsed()<<std::endl;
               }


    if (player_server==player_client)
    {
     bool feature_correctness =test_feature_correctness(data, new_ids, result0, result1, pss_client->get_prime_num(), intersection_size);
     std::cout<<"feature_correctness="<<feature_correctness<<std::endl;
    bool label_correctness = test_label_correctness(client_labels, start, label_shares0, label_shares1);
    std::cout<<"label_correctness="<<label_correctness<<std::endl;
    }



}















void psi_to_share_2party_socketopt(bool mode_switch, size_t host_log_n_data, size_t batch_size, size_t feature_num, size_t player, float inter_ratio)
{
    //-----------数据准备--------- --------------
    // size_t feature_num=4;
    // size_t host_log_n_data=20;
    int host_n_data = 1<<host_log_n_data;
    size_t player_server=0;
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
     BatchPirToShareQueryStream query;
    //  BatchPirToShareQuery query;
     std::stringstream response;
     double server_init_time, client_init_time, psi_query_time, psi_response_time, 
     psi_client_response_time, psi_server_return_answer_time, response_time,
     psi_client_return_answer_time, query_time, extract_time;
     std::vector<std::vector<int64_t>> result0, result1;
    size_t intersection_size;

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
            if (1.0*i/batch_size<inter_ratio)
            {
                client_ids[i]=std::to_string(i);  // 使得交集非client侧全集
            }
            else
            {
                client_ids[i]=std::to_string(-i);
            }
            
            // client_labels[i] = ((i/8)%2) | ((i/4)%2) &  ((i/2)%2) |  (i%2);
            client_labels[i] = random()%2;
        }
    }
    
    Timer timer, timer_total;
    timer_total.reset();
     //-----------Server initialize-----------------------
     if (player==player_server)
     {
        timer.reset();
        // auto pss_server = PSItoShareServer(server_ids, data, batch_size, feature_num);
        pss_server = make_shared<PSItoShareServer>(server_ids, data, batch_size, feature_num, true);
        server_init_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
     }

    //-----------Client initialize and send pk----------------------
    if (player==player_client)
    {
        timer.reset();
        // auto pss_client = PSItoShareClient(host_log_n_data, feature_num, batch_size);
        pss_client =  make_shared<PSItoShareClient>(host_log_n_data, feature_num, batch_size, true);
        // PK keys=pss_client.save_keys();
        keys=pss_client->save_keys("psi");
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {    auto socket = pss_client->get_send_socket();
             send_PK_socket(keys, socket, "psi");
        }
        client_init_time = timer.elapsed();
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //----------Server receive pk-----------------
    if (player==player_server)
    {
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            keys = recv_PK_socket(socket, "psi");
        }
        pss_server->load_keys(keys, "psi");
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
     
     //---------------stage 1  get new_ids
        //-----------Client generate query ------------
    if (player==player_client){
        timer.reset();
        psi_query = pss_client->generate_psi_query(client_ids, client_labels);
        psi_query_time = timer.elapsed();
        if (player_server!=player_client)
        {   auto socket = pss_client->get_send_socket();
            send_ECPSIQuery_socket(psi_query, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------server generate response
    if (player==player_server){
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            psi_query = recv_ECPSIQuery_socket(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
        psi_response = pss_server->generate_psi_response(psi_query);
        psi_response_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_send_socket();
            send_ECPSIResponce_socket(psi_response, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------client generate response 
    if (player==player_client){
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_recv_socket();
            psi_response = recv_ECPSIResponce_socket(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
        intersection_lastids = pss_client->generate_psi_response(psi_response);
        psi_client_response_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_send_socket();
            send_vector_auto_socket<int>(intersection_lastids, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
    // -----------server return answer
    if (player==player_server){
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_recv_socket();
            intersection_lastids = recv_vector_auto_socket<int>(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
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
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        for (size_t i=0; i<new_ids_and_label_shares1.size(); i++)
        {
            new_ids[i] = new_ids_and_label_shares1[i].a;
            label_shares1[i] = new_ids_and_label_shares1[i].b;
        }
        psi_client_return_answer_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //----------------Stage 2  batch pir to share


    if (player==player_server)
    {
        intersection_size = label_shares0.size();
        std::cout<<"intersection_size="<<intersection_size<<std::endl;
        batch_size = 1<<static_cast<int>(ceil(log2(intersection_size)));
        pss_server->init_BPS(batch_size);
        
        auto socket = pss_server->get_recv_socket();
        keys = recv_PK_socket(socket, "bps");
        pss_server->load_keys(keys, "bps");
    }
    if (player==player_client)
    {
        intersection_size = label_shares1.size();
        std::cout<<"intersection_size="<<intersection_size<<std::endl;
        batch_size = 1<<static_cast<int>(ceil(log2(intersection_size)));
        pss_client->init_BPS(batch_size);

        keys=pss_client->save_keys("bps");
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        auto socket = pss_client->get_send_socket();
        send_PK_socket(keys, socket, "bps");
        
    }
    

     //-----------Client generate query ------------
    if (player==player_client){
        timer.reset();
        //-------------pad new_ids to batch_size
        // intersection_size = new_ids.size();
        for (size_t i=intersection_size; i<batch_size; i++)
        {
            // new_ids.push_back(host_n_data - i);
             
             std::unordered_set<int64_t> set(new_ids.begin(), new_ids.end());
             int64_t ele = random()%host_n_data;
             while (set.find(ele) != set.end())
             {
                ele = random()%host_n_data;
             }
            new_ids.push_back(ele);
        }
        query = pss_client->gen_BPS_query_stream(new_ids);
        //  query = pss_client->gen_BPS_query(new_ids);
        query_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_send_socket();
            send_BatchPirToShareQueryStream_socket(query, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //-----------Server generate respose-------------
    if (player==player_server){
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            query = recv_BatchPirToShareQueryStream_socket(socket);
        }
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
    timer.reset();
     response = pss_server->gen_BPS_response(query, mode_switch);
    response_time = timer.elapsed();
           std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_send_socket();
            send_stringstream_socket(response, socket);
        }
                // std::cout<<__FILE__<<":"<<__LINE__<<endl;
         result0 = pss_server->extract_BPS_answer();
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
         }

     //-----------Client exact answer
         if (player==player_client){
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_recv_socket();
            response = recv_stringstream_socket(socket);
        }
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
     result1 = pss_client->extract_BPS_answer(response);
    extract_time = timer.elapsed();
        }




  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
           if (player==player_server){
#ifdef DEBUG
  std::cout <<" server_init_time:" << server_init_time << " ms " << std::endl;
        std::cout <<"get_newid: Gen psi_response time:" << psi_response_time << " ms " << std::endl;
          std::cout << "batch pir to share: Gen response time: " << response_time << " ms " << std::endl;
              auto server_online_time =  psi_response_time + response_time;
    
    std::cout<<"server_online_time="<<server_online_time << " ms " << std::endl;

      std::cout << "get_newid response size: "<<psi_response.byte_size() / 1024.0 << " KBytes"
            << std::endl;
      std::cout << "batch pir to share Response size: " << response.str().size() / 1024.0 << " KBytes"
            << std::endl;
#endif
    std::cout<<"comm. from server to client: "<< (psi_response.byte_size() + response.str().size())/ 1024.0 << " KBytes"
            << std::endl;
                // std::cout<<"sent bytes ="<<pss_server->get_sent_bytes()/1024.0<<"KB"<<std::endl;
                // std::cout<<"recv bytes ="<<pss_server->get_recv_bytes()/1024.0<<"KB"<<std::endl;
                std::cout<<"mem usage of server: "<<GetMemoryUsage()<<"MB"<<std::endl;
                std::cout<<"total time of server="<<timer_total.elapsed()<<std::endl;
           }

               if (player==player_client){
#ifdef DEBUG
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
    
  std::cout << "batch pir to share Query size: " << query.byte_size() / 1024.0 << " KBytes"
            << std::endl;
#endif
    std::cout<< "comm. from client to server: "<< (psi_query.byte_size()+intersection_lastids.size() * sizeof(int32_t)+ query.byte_size()) / 1024.0 << " KBytes"
            << std::endl;
                // std::cout<<"sent bytes ="<<pss_client->get_sent_bytes()/1024.0<<"KB"<<std::endl;
                // std::cout<<"recv bytes ="<<pss_client->get_recv_bytes()/1024.0<<"KB"<<std::endl;
                std::cout<<"mem usage of client: "<<GetMemoryUsage()<<"MB"<<std::endl;
                    std::cout<<"total time of client="<<timer_total.elapsed()<<std::endl;
               }


    // if (player_server==player_client)
    // {
    //  bool feature_correctness =test_feature_correctness(data, new_ids, result0, result1, pss_client->get_prime_num(), intersection_size);
    //  std::cout<<"feature_correctness="<<feature_correctness<<std::endl;
    // bool label_correctness = test_label_correctness(client_labels, start, label_shares0, label_shares1);
    // std::cout<<"label_correctness="<<label_correctness<<std::endl;
    // }




}










void psi_to_share_2party_socketopt_fromfile(bool mode_switch, string file_name, size_t player, string out_file)
{
    //-----------数据准备--------- --------------
    size_t feature_num;
    size_t host_log_n_data, host_n_data, batch_size;
    
    size_t player_server=0;
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
     BatchPirToShareQueryStream query;
    //  BatchPirToShareQuery query;
     std::stringstream response;
     double server_init_time, client_init_time, psi_query_time, psi_response_time, 
     psi_client_response_time, psi_server_return_answer_time, response_time,
     psi_client_return_answer_time, query_time, extract_time;
     std::vector<std::vector<int64_t>> result0, result1;
    size_t intersection_size;
            // int start=100;

    if (player==player_server)
    {
        //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
        read_cols_server(file_name, server_ids, data);
        // print_vec(server_ids);
       
        feature_num = data.size();
        host_n_data = server_ids.size();
        host_log_n_data = ceil(log2(host_n_data));
         std::cout<<__FILE__<<":"<<__LINE__<<endl;
         std::cout<<"host_n_data="<<host_n_data<<std::endl;
          std::cout<<"host_log_n_data="<<host_log_n_data<<std::endl;
        server_ids.resize(1<<host_log_n_data);
        for (size_t i=host_n_data; i<(1<<host_log_n_data); i++)
        {
            server_ids[i]=std::to_string(random_uint64());
        }
        host_n_data = 1<<host_log_n_data;
//  std::cout<<__FILE__<<":"<<__LINE__<<endl;
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {
            data[feature_id].resize(host_n_data);
        }
//  std::cout<<__FILE__<<":"<<__LINE__<<endl;
        // auto send_socket = pss_server->get_send_socket();
        //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
        send<size_t>(feature_num, client_address);
         std::cout<<__FILE__<<":"<<__LINE__<<endl;
        send<size_t>(host_log_n_data, client_address);
        // auto receive_socket = pss_server->get_recv_socket();
        recv<size_t>(batch_size, server_address);
    }
    
    if (player==player_client)
    {
        //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
        read_label_client(file_name, client_ids, client_labels);
         std::cout<<__FILE__<<":"<<__LINE__<<endl;
        //  print_vec(client_ids);
        batch_size = client_ids.size();
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        // auto receive_socket = pss_client->get_recv_socket();
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        recv<size_t>(feature_num, client_address);
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        recv<size_t>(host_log_n_data, client_address);
// std::cout<<__FILE__<<":"<<__LINE__<<endl;
        // auto send_socket = pss_client->get_send_socket();
        send<size_t>(batch_size, server_address);
        host_n_data = 1<<host_log_n_data;
    }
    
    Timer timer, timer_total;
    timer_total.reset();
     //-----------Server initialize-----------------------
     if (player==player_server)
     {
        timer.reset();
        // auto pss_server = PSItoShareServer(server_ids, data, batch_size, feature_num);
        pss_server = make_shared<PSItoShareServer>(server_ids, data, batch_size, feature_num, false, true);
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
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {    auto socket = pss_client->get_send_socket();
             send_PK_socket(keys, socket);
        }
        client_init_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //----------Server receive pk-----------------
    if (player==player_server)
    {
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            keys = recv_PK_socket(socket);
        }
        pss_server->load_keys(keys);
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
     
     //---------------stage 1  get new_ids
        //-----------Client generate query ------------
    if (player==player_client){
        timer.reset();
        psi_query = pss_client->generate_psi_query(client_ids, client_labels);
        psi_query_time = timer.elapsed();
        if (player_server!=player_client)
        {   auto socket = pss_client->get_send_socket();
            send_ECPSIQuery_socket(psi_query, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------server generate response
    if (player==player_server){
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            psi_query = recv_ECPSIQuery_socket(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
        psi_response = pss_server->generate_psi_response(psi_query);
        psi_response_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_send_socket();
            send_ECPSIResponce_socket(psi_response, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
        //------------client generate response 
    if (player==player_client){
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_recv_socket();
            psi_response = recv_ECPSIResponce_socket(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
        intersection_lastids = pss_client->generate_psi_response(psi_response);
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
        std::cout<<"intersection_lastids.size()="<<intersection_lastids.size()<<std::endl;
        psi_client_response_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_send_socket();
            send_vector_auto_socket<int>(intersection_lastids, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
    // -----------server return answer
    if (player==player_server){
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_recv_socket();
            intersection_lastids = recv_vector_auto_socket<int>(socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
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
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        for (size_t i=0; i<new_ids_and_label_shares1.size(); i++)
        {
            new_ids[i] = new_ids_and_label_shares1[i].a;
            label_shares1[i] = new_ids_and_label_shares1[i].b;
        }
        psi_client_return_answer_time = timer.elapsed();
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
     //----------------Stage 2  batch pir to share

    if (player==player_server)
    {
        intersection_size = label_shares0.size();
    }
    if (player==player_client)
    {
        intersection_size = label_shares1.size();
    }
    

     //-----------Client generate query ------------
    if (player==player_client){
        timer.reset();
        //-------------pad new_ids to batch_size
        // intersection_size = new_ids.size();
        for (size_t i=intersection_size; i<batch_size; i++)
        {
            // new_ids.push_back(host_n_data - i);
            // new_ids.push_back(random_uint64()%host_n_data);
            std::unordered_set<int64_t> set(new_ids.begin(), new_ids.end());
             int64_t ele = random()%host_n_data;
             while (set.find(ele) != set.end())
             {
                ele = random()%host_n_data;
             }
            new_ids.push_back(ele);
            
        }
        query = pss_client->gen_BPS_query_stream(new_ids);
        //  query = pss_client->gen_BPS_query(new_ids);
        query_time = timer.elapsed();
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_send_socket();
            send_BatchPirToShareQueryStream_socket(query, socket);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

     //-----------Server generate respose-------------
    if (player==player_server){
        if (player_server!=player_client)
        {   auto socket = pss_server->get_recv_socket();
            query = recv_BatchPirToShareQueryStream_socket(socket);
        }
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
    timer.reset();
     response = pss_server->gen_BPS_response(query, mode_switch);
    response_time = timer.elapsed();
           std::cout<<__FILE__<<":"<<__LINE__<<endl;
        if (player_server!=player_client)
        {
            auto socket = pss_server->get_send_socket();
            send_stringstream_socket(response, socket);
        }
                // std::cout<<__FILE__<<":"<<__LINE__<<endl;
         result0 = pss_server->extract_BPS_answer();
        //  save_csv(result0, out_file);
        save_csv_transpose(result0, label_shares0, out_file, intersection_size);
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
         }

     //-----------Client exact answer
         if (player==player_client){
        if (player_server!=player_client)
        {
            auto socket = pss_client->get_recv_socket();
            response = recv_stringstream_socket(socket);
        }
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
        timer.reset();
     result1 = pss_client->extract_BPS_answer(response);
     for (size_t i=0;i<result1.size();i++)
     {
        for (size_t j=0; j<result1[i].size(); j++)
        {
            result1[i][j]=result1[i][j]-pss_client->get_prime_num();
        }
     }
    //  save_csv(result1, out_file);
     save_csv_transpose(result1, label_shares1, out_file, intersection_size);
    extract_time = timer.elapsed();
        }




  std::cout << "------------------------------------" << std::endl;
  std::cout << "Performance: " << std::endl;
           if (player==player_server){
#ifdef DEBUG
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
#endif
            std::cout<<"sent bytes ="<<pss_server->get_sent_bytes()/1024.0<<"KB"<<std::endl;
            std::cout<<"recv bytes ="<<pss_server->get_recv_bytes()/1024.0<<"KB"<<std::endl;
    std::cout<<"mem usage of server: "<<GetMemoryUsage()<<"MB"<<std::endl;
           }

               if (player==player_client){
#ifdef DEBUG
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
    
  std::cout << "batch pir to share Query size: " << query.byte_size() / 1024.0 << " KBytes"
            << std::endl;
    std::cout<< "comm. from client to server: "<< (psi_query.byte_size()+intersection_lastids.size() * sizeof(int32_t)+ query.byte_size()) / 1024.0 << " KBytes"
            << std::endl;
#endif
            std::cout<<"sent bytes ="<<pss_client->get_sent_bytes()/1024.0<<"KB"<<std::endl;
            std::cout<<"recv bytes ="<<pss_client->get_recv_bytes()/1024.0<<"KB"<<std::endl;
            std::cout<<"mem usage of client: "<<GetMemoryUsage()<<"MB"<<std::endl;
               }


    std::cout<<"total time ="<<timer_total.elapsed()<<std::endl;

}



#endif