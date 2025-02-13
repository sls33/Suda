// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include "seal/seal.h"
#include "seal/util/polyarithsmallmod.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using namespace seal;

// void example_bfv_basics();

// void example_encoders();

// void example_levels();

// void example_bgv_basics();

// void example_ckks_basics();

// void example_rotation();

// void example_serialization();

// void example_performance_test();

/*
Helper function: Prints the name of the example in a fancy banner.
*/
inline void print_example_banner(std::string title)
{
    if (!title.empty())
    {
        std::size_t title_length = title.length();
        std::size_t banner_length = title_length + 2 * 10;
        std::string banner_top = "+" + std::string(banner_length - 2, '-') + "+";
        std::string banner_middle = "|" + std::string(9, ' ') + title + std::string(9, ' ') + "|";

        std::cout << std::endl << banner_top << std::endl << banner_middle << std::endl << banner_top << std::endl;
    }
}

/*
Helper function: Prints the parameters in a SEALContext.
*/
inline void print_parameters(const seal::SEALContext &context)
{
    auto &context_data = *context.key_context_data();

    /*
    Which scheme are we using?
    */
    std::string scheme_name;
    switch (context_data.parms().scheme())
    {
    case seal::scheme_type::bfv:
        scheme_name = "BFV";
        break;
    case seal::scheme_type::ckks:
        scheme_name = "CKKS";
        break;
    case seal::scheme_type::bgv:
        scheme_name = "BGV";
        break;
    default:
        throw std::invalid_argument("unsupported scheme");
    }
    std::cout << "/" << std::endl;
    std::cout << "| Encryption parameters :" << std::endl;
    std::cout << "|   scheme: " << scheme_name << std::endl;
    std::cout << "|   poly_modulus_degree: " << context_data.parms().poly_modulus_degree() << std::endl;

    /*
    Print the size of the true (product) coefficient modulus.
    */
    std::cout << "|   coeff_modulus size: ";
    std::cout << context_data.total_coeff_modulus_bit_count() << " (";
    auto coeff_modulus = context_data.parms().coeff_modulus();
    std::size_t coeff_modulus_size = coeff_modulus.size();
    for (std::size_t i = 0; i < coeff_modulus_size - 1; i++)
    {
        std::cout << coeff_modulus[i].bit_count() << " + ";
    }
    std::cout << coeff_modulus.back().bit_count();
    std::cout << ") bits" << std::endl;

    /*
    For the BFV scheme print the plain_modulus parameter.
    */
    if (context_data.parms().scheme() == seal::scheme_type::bfv)
    {
        std::cout << "|   plain_modulus: " << context_data.parms().plain_modulus().value() << std::endl;
    }

    std::cout << "\\" << std::endl;
}

/*
Helper function: Prints the `parms_id' to std::ostream.
*/
inline std::ostream &operator<<(std::ostream &stream, seal::parms_id_type parms_id)
{
    /*
    Save the formatting information for std::cout.
    */
    std::ios old_fmt(nullptr);
    old_fmt.copyfmt(std::cout);

    stream << std::hex << std::setfill('0') << std::setw(16) << parms_id[0] << " " << std::setw(16) << parms_id[1]
           << " " << std::setw(16) << parms_id[2] << " " << std::setw(16) << parms_id[3] << " ";

    /*
    Restore the old std::cout formatting.
    */
    std::cout.copyfmt(old_fmt);

    return stream;
}

/*
Helper function: Prints a vector of floating-point values.
*/
template <typename T>
inline void print_vector(std::vector<T> vec, std::size_t print_size = 4, int prec = 3)
{
    /*
    Save the formatting information for std::cout.
    */
    std::ios old_fmt(nullptr);
    old_fmt.copyfmt(std::cout);

    std::size_t slot_count = vec.size();

    std::cout << std::fixed << std::setprecision(prec);
    std::cout << std::endl;
    if (slot_count <= 2 * print_size)
    {
        std::cout << "    [";
        for (std::size_t i = 0; i < slot_count; i++)
        {
            std::cout << " " << vec[i] << ((i != slot_count - 1) ? "," : " ]\n");
        }
    }
    else
    {
        vec.resize(std::max(vec.size(), 2 * print_size));
        std::cout << "    [";
        for (std::size_t i = 0; i < print_size; i++)
        {
            std::cout << " " << vec[i] << ",";
        }
        if (vec.size() > 2 * print_size)
        {
            std::cout << " ...,";
        }
        for (std::size_t i = slot_count - print_size; i < slot_count; i++)
        {
            std::cout << " " << vec[i] << ((i != slot_count - 1) ? "," : " ]\n");
        }
    }
    std::cout << std::endl;

    /*
    Restore the old std::cout formatting.
    */
    std::cout.copyfmt(old_fmt);
}

/*
Helper function: Prints a matrix of values.
*/
template <typename T>
inline void print_matrix(std::vector<T> matrix, std::size_t row_size)
{
    /*
    We're not going to print every column of the matrix (there are 2048). Instead
    print this many slots from beginning and end of the matrix.
    */
    std::size_t print_size = 5;

    std::cout << std::endl;
    std::cout << "    [";
    for (std::size_t i = 0; i < print_size; i++)
    {
        std::cout << std::setw(3) << std::right << matrix[i] << ",";
    }
    std::cout << std::setw(3) << " ...,";
    for (std::size_t i = row_size - print_size; i < row_size; i++)
    {
        std::cout << std::setw(3) << matrix[i] << ((i != row_size - 1) ? "," : " ]\n");
    }
    std::cout << "    [";
    for (std::size_t i = row_size; i < row_size + print_size; i++)
    {
        std::cout << std::setw(3) << matrix[i] << ",";
    }
    std::cout << std::setw(3) << " ...,";
    for (std::size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
    {
        std::cout << std::setw(3) << matrix[i] << ((i != 2 * row_size - 1) ? "," : " ]\n");
    }
    std::cout << std::endl;
}

/*
Helper function: Print line number.
*/
inline void print_line(int line_number)
{
    std::cout << "Line " << std::setw(3) << line_number << " --> ";
}

/*
Helper function: Convert a value into a hexadecimal string, e.g., uint64_t(17) --> "11".
*/
inline std::string uint64_to_hex_string(std::uint64_t value)
{
    return seal::util::uint_to_hex_string(&value, std::size_t(1));
}


int multiply_plain_normal_simple(SEALContext context, Ciphertext &encrypted, const Plaintext &plain, MemoryPoolHandle pool)
{
    // Extract encryption parameters.
    auto &context_data = *context.get_context_data(encrypted.parms_id());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();

    size_t encrypted_size = encrypted.size();
    
        util::negacyclic_multiply_poly_mono_coeffmod(
            encrypted, encrypted_size, plain[0], 0, coeff_modulus, encrypted, std::move(pool));
    

        return 0;
}

int multiply_plain_normal_simple(SEALContext context, const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &encrypted1,  MemoryPoolHandle pool)
{
    // Extract encryption parameters.
    auto &context_data = *context.get_context_data(encrypted.parms_id());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();

    size_t encrypted_size = encrypted.size();
    
        util::negacyclic_multiply_poly_mono_coeffmod(
            encrypted, encrypted_size, plain[0], 0, coeff_modulus, encrypted1, std::move(pool));

        return 0;
}

int multiply_plain_normal_simple(SEALContext context, const Ciphertext &encrypted, const uint64_t plain, Ciphertext &encrypted1,  MemoryPoolHandle pool)
{
    // Extract encryption parameters.
    auto &context_data = *context.get_context_data(encrypted.parms_id());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();

    size_t encrypted_size = encrypted.size();
    
        util::negacyclic_multiply_poly_mono_coeffmod(
            encrypted, encrypted_size, plain, 0, coeff_modulus, encrypted1, std::move(pool));

        return 0;
}

int multiply_plain_normal_simple(const std::vector<Modulus> &coeff_modulus, const Ciphertext &encrypted, const uint64_t plain, Ciphertext &encrypted1,  MemoryPoolHandle pool)
{
    // Extract encryption parameters.
    // auto &context_data = *context.get_context_data(encrypted.parms_id());
    // auto &parms = context_data.parms();
    // auto &coeff_modulus = parms.coeff_modulus();

    size_t encrypted_size = encrypted.size();
    
        util::negacyclic_multiply_poly_mono_coeffmod(
            encrypted, encrypted_size, plain, 0, coeff_modulus, encrypted1, std::move(pool));

        return 0;
}

int multiply_plain_normal_simple(const std::vector<Modulus> &coeff_modulus, const Ciphertext &encrypted, const size_t encrypted_size, const uint64_t plain, Ciphertext &encrypted1,  const MemoryPoolHandle& pool)
{   
        util::negacyclic_multiply_poly_mono_coeffmod(
            encrypted, encrypted_size, plain, 0, coeff_modulus, encrypted1, std::move(pool));
        return 0;
}

// int multiply_plain_normal_simple2(const std::vector<Modulus> &coeff_modulus, const Ciphertext &encrypted, const size_t encrypted_size, const uint64_t plain, Ciphertext &encrypted1)
// {

// // SEAL_ALLOCATE_GET_COEFF_ITER(encrypted1, coeff_count, pool);
//     util::multiply_poly_scalar_coeffmod(encrypted, 1, plain, coeff_modulus, encrypted1);
// }

std::stringstream save_cipher_vector(const std::vector<seal::Ciphertext> & vc)
{
    std::stringstream vc_stream;
    for (auto &c : vc) {
    c.save(vc_stream);
  }
  return vc_stream;
}

std::stringstream save_cipher_vector(const std::vector<seal::Serializable<seal::Ciphertext>>  & vc)
{
    std::stringstream vc_stream;
    for (auto &c : vc) {
    c.save(vc_stream);
  }
  return vc_stream;
}


std::vector<seal::Ciphertext> load_cipher_vector(SEALContext context,
    std::stringstream &vc_stream, size_t vector_size) {
  std::vector<seal::Ciphertext> vc(vector_size);
  for (auto &c : vc) {
    c.load(context, vc_stream);
  }
  return vc;
};





std::vector<Ciphertext> unzip(const Evaluator& evaluator, const std::vector<Ciphertext> & vcin, const GaloisKeys & galois_keys,  uint64_t plain_module, size_t poly_mod_degree, size_t start_sparsity)
{
    // default: start_sparsity=1 代表初始输入无间隔
    std::vector<Ciphertext> vcout(vcin.size()*2);
    Plaintext x_power(poly_mod_degree);
    for (int i=0; i<poly_mod_degree; i++)
    {
        x_power[i]=0;
    }
    x_power[poly_mod_degree-start_sparsity]=plain_module-1;

    for (size_t i=0; i<vcin.size(); i++)
    {
        Ciphertext c(vcin[i]);
        Ciphertext gc, c0, c1;
        // std::cout<<"1+2*poly_mod_degree/start_sparsity="<<1+2*poly_mod_degree/start_sparsity<<std::endl;
        evaluator.apply_galois(c, 1+poly_mod_degree/start_sparsity, galois_keys, gc);
        evaluator.add(c, gc, c0);
        evaluator.sub(c, gc, c1);
        evaluator.multiply_plain_inplace(c1, x_power);
        vcout[i]=c0;
        vcout[i+vcin.size()]=c1;
    }
    return vcout;
}

std::vector<Ciphertext> unzip(const Evaluator& evaluator, const Ciphertext & vcin, const GaloisKeys & galois_keys,  uint64_t plain_module, size_t poly_mod_degree, size_t start_sparsity, size_t end_sparsity)
{
    size_t sparsity = start_sparsity;
    std::vector<Ciphertext> vcout(1);
    vcout[0] = vcin;
    while (sparsity<end_sparsity)
    {
        vcout = unzip(evaluator, vcout, galois_keys, plain_module, poly_mod_degree, sparsity);
        sparsity *=2;
    }
    return vcout;
}


std::vector<Ciphertext> unzip_vec(const Evaluator& evaluator, const std::vector<Ciphertext> & vcin, const GaloisKeys & galois_keys,  uint64_t plain_module, size_t poly_mod_degree, size_t end_sparsity)
{
    std::vector<Ciphertext> vcout;
    for (Ciphertext c: vcin)
    {
        auto vcout1 = unzip(evaluator, c, galois_keys, plain_module, poly_mod_degree, 1, end_sparsity);
        vcout.insert(vcout.end(), vcout1.begin(), vcout1.end());
    }
    return vcout;
}
