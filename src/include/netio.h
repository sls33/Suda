
#ifndef NETIO
#define NETIO
#include <coproto/Common/macoro.h>
#include <coproto/Socket/AsioSocket.h>
// #include <cryptoTools/Common/Defines.h>
// #include <cryptoTools/Common/config.h>
// #include <cryptoTools/Crypto/PRNG.h>
// #include <cryptoTools/Network/Channel.h>
// #include <cryptoTools/Network/IOService.h>
// #include <cryptoTools/Network/Session.h>
#include "utils.h"

// using namespace coproto;


//   ---------- send to receive from address -----------


template <typename T>
void send_vector(std::vector<T> &data, std::string  address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = osuCrypto::cp::asioConnect(context.address, false);

    auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    auto p = socket.send(data);
    coproto::sync_wait(p);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


template <typename T>
std::vector<T> recv_vector(std::string address, size_t size)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    std::vector<uint64_t> data(size);
    auto p = socket.recv(data);
    coproto::sync_wait(p);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return data;
}




template <typename T>
void send_vector_auto(std::vector<T> &data, std::string  address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = osuCrypto::cp::asioConnect(context.address, false);

    auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    auto p = socket.send(data.size());
    coproto::sync_wait(p);
    auto q = socket.send(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


template <typename T>
std::vector<T> recv_vector_auto(std::string address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    size_t size;
    auto p = socket.recv(size);
    coproto::sync_wait(p);
    std::vector<T> data(size);
    auto q = socket.recv(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return data;
}







template <typename T>
void send(T & data, std::string  address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = osuCrypto::cp::asioConnect(context.address, false);

    auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    auto p = socket.send(data);
    coproto::sync_wait(p);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}



// template <typename T>
// void send(T & data, coproto::AsioSocket&  socket)
// {

//     auto p = socket.send(data);
//     coproto::sync_wait(p);
//     coproto::sync_wait(socket.flush());
//     socket.close();
//     // context.totalReceive += socket.bytesReceived();
//     // context.totalSend += socket.bytesSent();
// }

// template <typename T>
// T recv(std::string address)
// {
//     const auto wait_start_time = std::chrono::system_clock::now();

//     auto socket = coproto::asioConnect(address, true);

//     const auto wait_end_time = std::chrono::system_clock::now();

//     // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
//     // context.timings.wait += wait_duration_time.count();

//     T data;
//     auto q = socket.recv(data);
//     coproto::sync_wait(q);
//     coproto::sync_wait(socket.flush());
//     socket.close();
//     return data;
// }


template <typename T>
void recv(T & data, std::string address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    auto q = socket.recv(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    socket.close();
}


template <typename T>
T recv_auto(std::string address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    T data;
    auto q =  socket.recvResize(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    socket.close();
    return data;
}
     





void send_ECPSIQuery(ECPSIQuery & data, std::string  address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    auto s = socket.send(data.size());
    coproto::sync_wait(s);
    auto p = socket.send(data.EIDs);
    coproto::sync_wait(p);
        auto q = socket.send(data.Elabels);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


ECPSIQuery recv_ECPSIQuery(std::string address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    size_t size;
    auto s=socket.recv(size);
        coproto::sync_wait(s);
    ECPSIQuery data(size);
    auto p = socket.recv(data.EIDs);
    coproto::sync_wait(p);

    auto q = socket.recv(data.Elabels);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    socket.close();
    return data;
}




void send_ECPSIResponce(ECPSIResponce & data, std::string  address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    auto s = socket.send(data.sizes());
    coproto::sync_wait(s);
    auto p = socket.send(data.ShortEIDs);
    coproto::sync_wait(p);
    auto q = socket.send(data.EEIDs);
    coproto::sync_wait(q);
    auto r = socket.send(data.R1Q1s);
    coproto::sync_wait(r);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


ECPSIResponce recv_ECPSIResponce(std::string address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    std::vector<size_t> sizes(3);
    auto s=socket.recv(sizes);
        coproto::sync_wait(s);
    ECPSIResponce data(sizes);
    auto p = socket.recv(data.ShortEIDs);
    coproto::sync_wait(p);

    auto q = socket.recv(data.EEIDs);
    coproto::sync_wait(q);

        auto r = socket.recv(data.R1Q1s);
    coproto::sync_wait(r);
    coproto::sync_wait(socket.flush());
    socket.close();
    return data;
}




void send_PK(const PK & data, std::string  address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    auto p = socket.send(data.psi_pk);
    coproto::sync_wait(p);

    string bps_pk_str = data.bps_to_str();

    auto q = socket.send(bps_pk_str);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


PK recv_PK(std::string address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    PK keys;
    auto p = socket.recv(keys.psi_pk);
    coproto::sync_wait(p);
// std::cout<<__FILE__<<":"<<__LINE__<<endl;
    string bps_pk_str;
    auto q = socket.recvResize(bps_pk_str);
    coproto::sync_wait(q);
    // std::cout<<__FILE__<<":"<<__LINE__<<endl;
    coproto::sync_wait(socket.flush());
    socket.close();
// std::cout<<__FILE__<<":"<<__LINE__<<endl;
    keys.bps_from_str(bps_pk_str);
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return keys;
}





void send_BatchPirToShareQueryStream(BatchPirToShareQueryStream & data, std::string  address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();


    auto p = socket.send(data.cipher_x_powers_stream.str());
    coproto::sync_wait(p);
    auto q = socket.send(data.row_keepers_stream.str());
    coproto::sync_wait(q);
    auto r = socket.send(data.col_keepers_stream.str());
    coproto::sync_wait(r);
    coproto::sync_wait(socket.flush());
    socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}




BatchPirToShareQueryStream recv_BatchPirToShareQueryStream(std::string address)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();


    string cipher_x_powers_str, row_keepers_str, col_keepers_str;
    auto p = socket.recvResize(cipher_x_powers_str);
    coproto::sync_wait(p);

    auto q = socket.recvResize(row_keepers_str);
    coproto::sync_wait(q);

    auto r = socket.recvResize(col_keepers_str);
    coproto::sync_wait(r);

    coproto::sync_wait(socket.flush());
    socket.close();

    BatchPirToShareQueryStream bps_query;
    bps_query.cipher_x_powers_stream << cipher_x_powers_str;
    bps_query.row_keepers_stream << row_keepers_str;
    bps_query.col_keepers_stream << col_keepers_str;
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return bps_query;
}



void send_stringstream(stringstream & data, std::string  address)
{
    string data_str = data.str();
    send<string>(data_str, address);
}

stringstream recv_stringstream(std::string  address)
{
    string r_str = recv_auto<string>(address);
    stringstream r;
    r << r_str;
    return r;
}


//----- send to receive from socket ------------------






// template <typename T>
// void send(T & data, coproto::Socket & socket)
// {
//     auto p = socket.send(data);
//     coproto::sync_wait(p);
//     coproto::sync_wait(socket.flush());
//     socket.close();

// }

// template <typename T>
// void recv(T & data, coproto::Socket & socket)
// {
//     auto q = socket.recv(data);
//     coproto::sync_wait(q);
//     coproto::sync_wait(socket.flush());
//     socket.close();
// }









template <typename T>
void send_vector_socket(std::vector<T> &data, coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = osuCrypto::cp::asioConnect(context.address, false);

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    auto p = socket.send(data);
    coproto::sync_wait(p);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


template <typename T>
std::vector<T> recv_vector_socket(coproto::Socket & socket, size_t size)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    std::vector<uint64_t> data(size);
    auto p = socket.recv(data);
    coproto::sync_wait(p);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return data;
}



template <typename T>
void send_vector_auto_socket(std::vector<T> &data, coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = osuCrypto::cp::asioConnect(context.address, false);

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    auto p = socket.send(data.size());
    coproto::sync_wait(p);
    auto q = socket.send(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


template <typename T>
std::vector<T> recv_vector_auto_socket(coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    size_t size;
    auto p = socket.recv(size);
    coproto::sync_wait(p);
    std::vector<T> data(size);
    auto q = socket.recv(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return data;
}







template <typename T>
void send_socket(T & data, coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = osuCrypto::cp::asioConnect(context.address, false);

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    auto p = socket.send(data);
    coproto::sync_wait(p);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}

template <typename T>
T recv_socket(coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    T data;
    auto q = socket.recv(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    // socket.close();
    return data;
}



template <typename T>
void recv_socket(T& data, coproto::Socket & socket)
{
    auto q = socket.recv(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // return data;
}


template <typename T>
T recv_auto_socket(coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    T data;
    auto q =  socket.recvResize(data);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    // socket.close();
    return data;
}
     





void send_ECPSIQuery_socket(ECPSIQuery & data, coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    auto s = socket.send(data.size());
    coproto::sync_wait(s);
    auto p = socket.send(data.EIDs);
    coproto::sync_wait(p);
        auto q = socket.send(data.Elabels);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


ECPSIQuery recv_ECPSIQuery_socket(coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    size_t size;
    auto s=socket.recv(size);
        coproto::sync_wait(s);
    ECPSIQuery data(size);
    auto p = socket.recv(data.EIDs);
    coproto::sync_wait(p);

    auto q = socket.recv(data.Elabels);
    coproto::sync_wait(q);
    coproto::sync_wait(socket.flush());
    // socket.close();
    return data;
}






void send_ECPaillierPSIQuery_socket(ECPaillierPSIQuery & data, coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    auto s = socket.send(data.size());
    coproto::sync_wait(s);
    auto p = socket.send(data.EIDs);
    coproto::sync_wait(p);

    for (size_t i=0; i<data.Elabels.size(); i++)
    {
        // std::cout<<__FILE__<<":"<<__LINE__<<":"<<std::endl;
        // std::cout<<"i="<<i<<std::endl;
        auto q = socket.send(data.Elabels[i]);
        coproto::sync_wait(q);
    }
    // send_vector_auto_socket<string>(data.Elabels, socket);
//  std::cout<<__FILE__<<":"<<__LINE__<<":"<<std::endl;
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}



ECPaillierPSIQuery recv_ECPaillierPSIQuery_socket(coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    size_t size;
    auto s=socket.recv(size);
        coproto::sync_wait(s);
    ECPaillierPSIQuery data(size);
    auto p = socket.recv(data.EIDs);
    coproto::sync_wait(p);

    for (size_t i=0; i<size; i++)
    {
        // std::cout<<__FILE__<<":"<<__LINE__<<":"<<std::endl;
        // std::cout<<"i="<<i<<std::endl;
        auto q = socket.recvResize(data.Elabels[i]);
        coproto::sync_wait(q);
    }
//  std::cout<<__FILE__<<":"<<__LINE__<<":"<<std::endl;
    coproto::sync_wait(socket.flush());
    // socket.close();
    return data;
}






void send_ECPSIResponce_socket(ECPSIResponce & data, coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    auto s = socket.send(data.sizes());
    coproto::sync_wait(s);
    auto p = socket.send(data.ShortEIDs);
    coproto::sync_wait(p);
    auto q = socket.send(data.EEIDs);
    coproto::sync_wait(q);
    auto r = socket.send(data.R1Q1s);
    coproto::sync_wait(r);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


ECPSIResponce recv_ECPSIResponce_socket(coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    std::vector<size_t> sizes(3);
    auto s=socket.recv(sizes);
        coproto::sync_wait(s);
    ECPSIResponce data(sizes);
    auto p = socket.recv(data.ShortEIDs);
    coproto::sync_wait(p);

    auto q = socket.recv(data.EEIDs);
    coproto::sync_wait(q);

        auto r = socket.recv(data.R1Q1s);
    coproto::sync_wait(r);
    coproto::sync_wait(socket.flush());
    // socket.close();
    return data;
}




void send_ECPaillierPSIResponce_socket(ECPaillierPSIResponce & data, coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    auto s = socket.send(data.sizes());
    coproto::sync_wait(s);
    auto p = socket.send(data.ShortEIDs);
    coproto::sync_wait(p);
    auto q = socket.send(data.EEIDs);
    coproto::sync_wait(q);
    for (size_t i=0; i<data.Elabels.size(); i++)
    {
            //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
            //  std::cout<<"i="<<i<<std::endl;
    auto r = socket.send(data.Elabels[i]);
    // std::cout<<__FILE__<<":"<<__LINE__<<endl;
    coproto::sync_wait(r);
    }

    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}




ECPaillierPSIResponce recv_ECPaillierPSIResponce_socket(coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    std::vector<size_t> sizes(3);
    auto s=socket.recv(sizes);
        coproto::sync_wait(s);
    ECPaillierPSIResponce data(sizes);
    auto p = socket.recv(data.ShortEIDs);
    coproto::sync_wait(p);

    auto q = socket.recv(data.EEIDs);
    coproto::sync_wait(q);
    for (size_t i=0; i<data.Elabels.size(); i++)
    {    
                    //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
            //  std::cout<<"i="<<i<<std::endl;
            auto r = socket.recvResize(data.Elabels[i]);
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;
    coproto::sync_wait(r);
    }
    coproto::sync_wait(socket.flush());
    // socket.close();
    return data;
}




void send_PK_socket(const PK & data, coproto::Socket & socket, string stage="all")
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    if (stage=="psi" || stage=="all")
    {
        auto p = socket.send(data.psi_pk);
        coproto::sync_wait(p);
    }
    if (stage=="bps" || stage=="all")
    {
        string bps_pk_str = data.bps_to_str();
        auto q = socket.send(bps_pk_str);
        coproto::sync_wait(q);
        coproto::sync_wait(socket.flush());
    }
    

    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}


PK recv_PK_socket(coproto::Socket & socket, string stage="all")
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    PK keys;
    if (stage=="psi" || stage=="all")
    {
        auto p = socket.recv(keys.psi_pk);
        coproto::sync_wait(p);
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
    if (stage=="bps" || stage=="all")
    {
        string bps_pk_str;
        auto q = socket.recvResize(bps_pk_str);
        coproto::sync_wait(q);
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        coproto::sync_wait(socket.flush());
        // socket.close();
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
         keys.bps_from_str(bps_pk_str);
    }

    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return keys;
}





void send_PKPaillier_socket(const PKPaillier & data, coproto::Socket & socket, string stage="all")
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();
    if (stage=="psi" || stage=="all")
    {
       
        string psi_pk_str = data.psi_to_str();
         auto p = socket.send(psi_pk_str);
        coproto::sync_wait(p);
        coproto::sync_wait(socket.flush());
    }
    if (stage=="bps" || stage=="all")
    {
        string bps_pk_str = data.bps_to_str();
        auto q = socket.send(bps_pk_str);
        coproto::sync_wait(q);
        coproto::sync_wait(socket.flush());
    }
    

    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}



PKPaillier recv_PKPaillier_socket(coproto::Socket & socket, string stage="all")
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();

    PKPaillier keys;
    if (stage=="psi" || stage=="all")
    {
        // auto p = socket.recv(keys.psi_pk);
        // coproto::sync_wait(p);
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        string psi_pk_str;
        auto p = socket.recvResize(psi_pk_str);
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        coproto::sync_wait(p);
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        coproto::sync_wait(socket.flush());
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
         keys.psi_from_str(psi_pk_str);
        //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }
    if (stage=="bps" || stage=="all")
    {
        string bps_pk_str;
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        auto q = socket.recvResize(bps_pk_str);
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        coproto::sync_wait(q);
        // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        coproto::sync_wait(socket.flush());
            //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
         keys.bps_from_str(bps_pk_str);
            //   std::cout<<__FILE__<<":"<<__LINE__<<endl;
    }

    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return keys;
}






void send_BatchPirToShareQueryStream_socket(BatchPirToShareQueryStream & data, coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, false);

    const auto wait_end_time = std::chrono::system_clock::now();

    const auto wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();


    auto p = socket.send(data.cipher_x_powers_stream.str());
    coproto::sync_wait(p);
    auto q = socket.send(data.row_keepers_stream.str());
    coproto::sync_wait(q);
    auto r = socket.send(data.col_keepers_stream.str());
    coproto::sync_wait(r);
    coproto::sync_wait(socket.flush());
    // socket.close();
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
}




BatchPirToShareQueryStream recv_BatchPirToShareQueryStream_socket(coproto::Socket & socket)
{
    const auto wait_start_time = std::chrono::system_clock::now();

    // auto socket = coproto::asioConnect(address, true);

    const auto wait_end_time = std::chrono::system_clock::now();

    // const duration_millis wait_duration_time = wait_end_time - wait_start_time;
    // context.timings.wait += wait_duration_time.count();


    string cipher_x_powers_str, row_keepers_str, col_keepers_str;
    auto p = socket.recvResize(cipher_x_powers_str);
    coproto::sync_wait(p);

    auto q = socket.recvResize(row_keepers_str);
    coproto::sync_wait(q);

    auto r = socket.recvResize(col_keepers_str);
    coproto::sync_wait(r);

    coproto::sync_wait(socket.flush());
    // socket.close();

    BatchPirToShareQueryStream bps_query;
    bps_query.cipher_x_powers_stream << cipher_x_powers_str;
    bps_query.row_keepers_stream << row_keepers_str;
    bps_query.col_keepers_stream << col_keepers_str;
    // context.totalReceive += socket.bytesReceived();
    // context.totalSend += socket.bytesSent();
    return bps_query;
}



void send_stringstream_socket(stringstream & data, coproto::Socket & socket)
{
    string data_str = data.str();
    send_socket<string>(data_str, socket);
}

stringstream recv_stringstream_socket(coproto::Socket & socket)
{
    string r_str = recv_auto_socket<string>(socket);
    stringstream r;
    r << r_str;
    return r;
}






#endif