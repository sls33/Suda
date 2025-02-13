#include "include/netio.h"
#include "include/utils.h"
#ifdef NETIOTESTWITHEC


void test()
{
    std::vector<uint64_t> data={0,1,2,3,5};
    std::string addres = "localhost:8011";
    auto futurePA = std::async(send_vector<uint64_t>, std::ref(data), addres);
    auto futurePB = std::async(recv_vector<uint64_t>, addres, data.size());


    auto result = futurePB.get();

    for (size_t i=0; i<result.size(); i++)
    {
        std::cout<<result[i];
    }
    
    std::cout<<std::endl;

}


void test_auto()
{
    std::vector<uint64_t> data={0,1,2,3,5};
    std::string addres = "localhost:8011";
        auto futurePA = std::async(send_vector_auto<uint64_t>, std::ref(data), addres);
    auto futurePB = std::async(recv_vector_auto<uint64_t>, addres);

    auto result = futurePB.get();

    for (size_t i=0; i<result.size(); i++)
    {
        std::cout<<result[i];
    }
    
    std::cout<<std::endl;

}




// void test_auto2()
// {
//     std::vector<uint64_t> data={0,1,2,3,5};
//     std::string addres = "localhost:8011";
//      std::vector<uint64_t> result;
//         auto futurePA = std::async(send_vector_auto<uint64_t>, std::ref(data), addres);
//     auto futurePB = std::async(recv_vector_auto<uint64_t>, std::ref(result), addres);

//     // auto result = futurePB.get();

//     for (size_t i=0; i<result.size(); i++)
//     {
//         std::cout<<result[i];
//     }
    
//     std::cout<<std::endl;

// }


void test_ECPoint()
{
    ECPoint ec_point, result;
     std::string addres = "localhost:8011";
        auto futurePA = std::async(send<ECPoint>, std::ref(ec_point), addres);
    // auto futurePA = std::async(static_cast<void(*)(ECPoint &, std::string&)>(send<ECPoint>), std::ref(ec_point), addres);
    auto futurePB = std::async(recv<ECPoint>, std::ref(result), addres);
    // auto futurePA = std::async(static_cast<void(*)(ECPoint &, std::string&)>(recv<ECPoint>), addres);

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    // auto result = futurePB.get();
    std::cout<<"len(result.P)="<<std::size(result.P)<<std::endl;
    for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
    {
        std::cout<<(result.P[i]==ec_point.P[i]);
    }
    
    std::cout<<std::endl;

}


void test_DoubleECPoint()
{
    DoubleECPoint dec_point, result;
     std::string addres = "localhost:8011";
        auto futurePA = std::async(send<DoubleECPoint>, std::ref(dec_point), addres);
    auto futurePB = std::async(recv<DoubleECPoint>, std::ref(result), addres);

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    // auto result = futurePB.get();
    // std::cout<<"len(result.P)="<<std::size(result.P)<<std::endl;
    for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
    {
        std::cout<<(result.R.P[i]==dec_point.R.P[i]);
    }
        for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
    {
        std::cout<<(result.Q.P[i]==dec_point.Q.P[i]);
    }
    
    std::cout<<std::endl;

}





void test_DoubleECPointPlus1bit()
{
    DoubleECPointPlus1bit dec_point_p1, result;
     std::string addres = "localhost:8011";
        auto futurePA = std::async(send<DoubleECPointPlus1bit>, std::ref(dec_point_p1), addres);
    auto futurePB = std::async(recv<DoubleECPointPlus1bit>, std::ref(result), addres);

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    // auto result = futurePB.get();
    // std::cout<<"len(result.P)="<<std::size(result.P)<<std::endl;
    for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
    {
        std::cout<<(result.R.P[i]==dec_point_p1.R.P[i]);
    }
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
    {
        std::cout<<(result.Q.P[i]==dec_point_p1.Q.P[i]);
    }
               std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
    {
        std::cout<<(result.b==dec_point_p1.b);
    }
    

    std::cout<<std::endl;

}



void test_ECPSIQuery()
{
     ECPSIQuery ecpsi_query(10);
     std::string addres = "localhost:8011";
        auto futurePA = std::async(send_ECPSIQuery, std::ref(ecpsi_query), addres);
    auto futurePB = std::async(recv_ECPSIQuery, addres);

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    auto result = futurePB.get();
    std::cout<<"result.size()="<<result.size()<<std::endl;
    std::cout<<"result==ecpsi_query: "<<(result==ecpsi_query)<<std::endl;

    std::cout<<std::endl;
}



void test_ECPSIResponce()
{
    ECPSIResponce ecpsi_response({10,10,10});
     std::string addres = "localhost:8011";
        auto futurePA = std::async(send_ECPSIResponce, std::ref(ecpsi_response), addres);
    auto futurePB = std::async(recv_ECPSIResponce, addres);

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    auto result = futurePB.get();
    std::cout<<"result.size()="<<result.sizes()[0]<<std::endl;
    std::cout<<"result==ecpsi_response: "<<(result==ecpsi_response)<<std::endl;

    std::cout<<std::endl;
}




void test_string()
{
    string x="qwerty";
     std::string addres = "localhost:8011";
        auto futurePA = std::async(send<string>, std::ref(x), addres);
    auto futurePB = std::async(recv_auto<string>, addres);

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    auto result = futurePB.get();
    std::cout<<"result="<<result<<std::endl;
    // for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
    // {
    //     std::cout<<(result.P[i]==ec_point.P[i]);
    // }
    
    std::cout<<std::endl;
}


// void test_vector_string()
// {
//     string x="qwerty";
//      std::string addres = "localhost:8011";
//         auto futurePA = std::async(send<string>, std::ref(x), addres);
//     auto futurePB = std::async(recv_auto<string>, addres);

//     std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
//     auto result = futurePB.get();
//     std::cout<<"result="<<result<<std::endl;
//     // for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
//     // {
//     //     std::cout<<(result.P[i]==ec_point.P[i]);
//     // }
    
//     std::cout<<std::endl;
// }



void test_PK()
{
    PK keys;
    keys.bps_from_str("qwerty");
     std::string addres = "localhost:8011";
        auto futurePA = std::async(send_PK, std::ref(keys), addres);
    auto futurePB = std::async(recv_PK, addres);

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    auto result = futurePB.get();

    std::cout<<"result==keys: "<<(result==keys)<<std::endl;

    std::cout<<std::endl;
}




// void test_socket()
// {
//     std::vector<uint64_t> data={0,1,2,3,5};
//     std::string addres = "localhost:8011";
//     auto socket = coproto::asioConnect(addres, true);
//     auto futurePA = std::async(send_vector_socket<uint64_t>, std::ref(data), socket);
//     auto futurePB = std::async(recv_vector_socket<uint64_t>, socket, data.size());

//     auto result = futurePB.get();

//     for (size_t i=0; i<result.size(); i++)
//     {
//         std::cout<<result[i];
//     }
    
//     std::cout<<std::endl;

// }



void test_speed()
{
    std::vector<uint64_t> data(1<<28);
    std::string addres = "localhost:8011";

    Timer timer;
     //-----------Server initialize-----------------------
    timer.reset();

    auto futurePA = std::async(send_vector<uint64_t>, std::ref(data), addres);
    auto futurePB = std::async(recv_vector<uint64_t>, addres, data.size());
    



    auto result = futurePB.get();
    auto comm_time = timer.elapsed();
    std::cout <<" comm_time:" << comm_time << " ms " << std::endl;

    // for (size_t i=0; i<result.size(); i++)
    // {
    //     std::cout<<result[i];
    // }
    
    std::cout<<std::endl;

}



void test_speed(int party_id)
{
    std::vector<uint64_t> data(1<<27);
    for (int i=0; i<data.size(); i++)
    {
        data[i]=0;
    }
    int round=2;
    std::string addres = "localhost:8011";

    Timer timer;
    
     //-----------Server initialize-----------------------
    timer.reset();

    // auto futurePA = std::async(send_vector<uint64_t>, std::ref(data), addres);
    // auto futurePB = std::async(recv_vector<uint64_t>, addres, data.size());
    
    for (size_t i=0; i<round; i++)
    {
        // std::cout<<"i="<<i<<std::endl;
        if (party_id==0)
        {
            send_vector<uint64_t>(data, addres);
        }
        else
        {
            auto result = recv_vector<uint64_t>(addres, data.size());
        } 

    }



    // auto result = futurePB.get();
    auto comm_time = timer.elapsed();
    std::cout <<" comm_time:" << comm_time << " ms " << std::endl;

    // for (size_t i=0; i<result.size(); i++)
    // {
    //     std::cout<<result[i];
    // }
    
    std::cout<<std::endl;

}
#endif

void test_send_receive(int party_id)
{
    string address = "localhost:9876";
    int x, y;
    if (party_id==0)
    {   x=123;
        auto socket = coproto::asioConnect(address, true);
        send_socket<int>(x, socket);
        recv_socket<int>(y, socket);
        std::cout<<"x="<<x<<std::endl;
        std::cout<<"y="<<y<<std::endl;
    }
    else
    {   y=456;
        auto socket = coproto::asioConnect(address, false);
        send_socket<int>(y, socket);
        recv_socket<int>(x, socket);
        std::cout<<"x="<<x<<std::endl;
        std::cout<<"y="<<y<<std::endl;
    }

}


int main(int argc, char **argv) {


    // test();
    // test_auto();
    // test_ECPoint();
    // test_DoubleECPoint();
    // test_DoubleECPointPlus1bit();
    // test_ECPSIQuery();
    // test_ECPSIResponce();
    // test_string();
    

    // test_socket();

    // test_PK();
    // test_speed();


    if (argc >= 2) {
        int party_id = atoi(argv[1]);
        // test_speed(party_id);
        test_send_receive(party_id);
    }


}
