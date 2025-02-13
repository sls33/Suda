#pragma once
// #define POLY_TOOL_TEST_FLAG true
#include <NTL/FFT.h>
#include <NTL/lzz_pX.h>
#include <time.h>

#include <bitset>
#include <cassert>
#include <vector>


NTL_CLIENT


inline long reverse(long x, size_t bits_num) {  // NOLINT
    bitset<64> bx(x), rbx;
    for (int i = 0; i < bits_num; i++) {
        rbx[i] = bx[bits_num - 1 - i];
    }
    return rbx.to_ulong();
}

void real_ifft(size_t log_da, const zz_pX &a, const FFTPrimeInfo &info,
               zz_pX &ifft_result) {    // NOLINT
    zz_pX new_fft_result;
    size_t da = (1 << log_da);
    // std::cout << "deg(a)=" << deg(a) << std::endl;
    // std::cout << "da=" << da << std::endl;
    // assert(deg(a)==da-1);
    ifft_result.SetLength(da);
    new_fft_result.SetLength(da);
    FFTFwd((long *)new_fft_result.rep.data(), (long *)a.rep.data(), log_da, // NOLINT
           info);
    std::vector<long> ids(da);  // NOLINT
    for (int i = 0; i < da; i++) {
        ifft_result[i] = new_fft_result[reverse(i, log_da)];
    }
}

void real_ifft(size_t log_da, const std::vector<int64_t> &a,
               const FFTPrimeInfo &info,
               std::vector<int64_t> &ifft_result) {  // NOLINT
    // zz_pX new_fft_result;
    size_t da = (1 << log_da);

    if (a.size() > da) {
        throw("must have a.size()<=da");
    }
    ifft_result.resize(da);
    std::vector<int64_t> new_fft_result(da);
    FFTFwd((long *)new_fft_result.data(), (long *)a.data(), log_da, info);  // NOLINT
    for (int i = 0; i < da; i++) {
        ifft_result[i] = new_fft_result[reverse(i, log_da)];
    }
}










// void find_row_col_basis(const vec_zz_p &mat, size_t rows_num, vector<zz_pX> &row_keeper,  vector<zz_pX> & col_keeper)
// {
//     size_t mat_size = mat.length();
//     size_t cols_num = mat_size/rows_num;
//     if (rows_num*cols_num!=mat.length())
//     {
//         throw("must have mat.length()/length*length!=mat.length()");
//     }
//     row_keeper.resize(rows_num);
//     col_keeper.resize(cols_num);
//     vector<zz_pX> row_killer(rows_num), col_killer(cols_num); //, row_keeper(length), col_keeper(length);
//     vector<vec_zz_p> row_vecs(rows_num), col_vecs(cols_num);
//     std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
//     for (int i=0; i<mat_size/length; i++)
//     {
//         row_vecs[i].SetLength(mat_size/length);
//         for (int j=0; j<length; j++)
//         {
//             row_vecs[i][j]=mat[i*length+j];
//         }
//         BuildFromRoots(row_killer[i], row_vecs[i]);
//     }
//     std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
//     for (int j=0; j<length; j++)
//     {   
//         col_vecs[j].SetLength(length);
//         for (int i=0; i<length; i++)
//         {
//             col_vecs[j][i]=mat[i*length+j];
//         }
//         BuildFromRoots(col_killer[j], col_vecs[j]);
//     }
//     std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
//     zz_pX all_killer;
//     BuildFromRoots(all_killer, mat);
//         std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
//     //-----------compute row_keeper col_keeper ------------------
//     for (int i=0; i<length; i++)
//     {
//         row_keeper[i]=all_killer/row_killer[i];
//     }
//     for (int j=0; j<length; j++)
//     {
//         col_keeper[j]=all_killer/col_killer[j];
//     }
//         std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
//     //-------------compute normalized row_keeper col_keeper --------
//     for (int i=0; i<length; i++)
//     {
//         vec_zz_p row_values = eval(row_keeper[i], row_vecs[i]);
//         for (int j=0; j<length; j++)
//         {
//             row_values[j]=inv(row_values[j]);
//         }
//         row_killer[i]=interpolate(row_vecs[i], row_values);
//         row_keeper[i]*=row_killer[i];
//     }
//         std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
//     for (int j=0; j<length; j++)
//     {
//         vec_zz_p col_values = eval(col_keeper[j], col_vecs[j]);
//         for (int i=0; i<length; i++)
//         {
//             col_values[i]=inv(col_values[i]);
//         }
//         col_killer[j]=interpolate(col_vecs[j], col_values);
//         col_keeper[j]*=col_killer[j];
//     }
//         std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

//     //------------test-------------
// #ifdef POLY_TOOL_TEST_FLAG
//         for (int i=0; i<length; i++)
//         {
//             std::cout<<"i="<<i<<std::endl;
//             for (int j=0; j<length; j++)
//             {
//                 auto values = eval(row_keeper[i], row_vecs[j]);
//                 std::cout<<values<<std::endl;
//             }

//         }

//         for (int j=0; j<length; j++)
//         {
//             std::cout<<"j="<<j<<std::endl;
//             for (int i=0; i<length; i++)
//             {
//                 auto values = eval(col_keeper[j], row_vecs[i]);
//                 std::cout<<values<<std::endl;
//             }
//         }
// #endif    
    
// }







void find_row_col_basis(const vec_zz_p &mat, size_t cols_num, vector<zz_pX> &row_keeper,  vector<zz_pX> & col_keeper)
{
    size_t mat_size = mat.length();
    size_t rows_num = mat_size/cols_num;
    if (rows_num*cols_num!=mat_size)
    {
        throw("must have rows_num*cols_num!=mat_size");
    }
    row_keeper.resize(rows_num);
    col_keeper.resize(cols_num);
    vector<zz_pX> row_killer(rows_num), col_killer(cols_num); //, row_keeper(length), col_keeper(length);
    vector<vec_zz_p> row_vecs(rows_num), col_vecs(cols_num);
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    for (int i=0; i<rows_num; i++)
    {
        row_vecs[i].SetLength(cols_num);
        for (int j=0; j<cols_num; j++)
        {
            row_vecs[i][j]=mat[i*cols_num+j];
        }
        BuildFromRoots(row_killer[i], row_vecs[i]);
    }
    // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    for (int j=0; j<cols_num; j++)
    {   
        col_vecs[j].SetLength(rows_num);
        for (int i=0; i<rows_num; i++)
        {
            col_vecs[j][i]=mat[i*cols_num+j];
        }
        BuildFromRoots(col_killer[j], col_vecs[j]);
    }
    // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    zz_pX all_killer;
    BuildFromRoots(all_killer, mat);
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //-----------compute row_keeper col_keeper ------------------
    for (int i=0; i<rows_num; i++)
    {
        row_keeper[i]=all_killer/row_killer[i];
    }
    for (int j=0; j<cols_num; j++)
    {
        col_keeper[j]=all_killer/col_killer[j];
    }
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //-------------compute normalized row_keeper col_keeper --------
    for (int i=0; i<rows_num; i++)
    {
        vec_zz_p row_values = eval(row_keeper[i], row_vecs[i]);
        for (int j=0; j<cols_num; j++)
        {
            row_values[j]=inv(row_values[j]);
        }
        row_killer[i]=interpolate(row_vecs[i], row_values);
        row_keeper[i]*=row_killer[i];
    }
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    for (int j=0; j<cols_num; j++)
    {
        vec_zz_p col_values = eval(col_keeper[j], col_vecs[j]);
        for (int i=0; i<rows_num; i++)
        {
            col_values[i]=inv(col_values[i]);
        }
        col_killer[j]=interpolate(col_vecs[j], col_values);
        col_keeper[j]*=col_killer[j];
    }
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

    //------------test-------------
#ifdef POLY_TOOL_TEST_FLAG
        for (int i=0; i<rows_num; i++)
        {
            std::cout<<"i="<<i<<std::endl;
            for (int j=0; j<rows_num; j++)
            {
                auto values = eval(row_keeper[i], row_vecs[j]);
                std::cout<<values<<std::endl;
            }

        }

        for (int j=0; j<cols_num; j++)
        {
            std::cout<<"j="<<j<<std::endl;
            for (int i=0; i<cols_num; i++)
            {
                auto values = eval(col_keeper[j], col_vecs[i]);
                std::cout<<values<<std::endl;
            }
        }
#endif    
    
}























void find_row_col_basis(const vector<int64_t> &mat, size_t col_num, int64_t prime_num, vector<vector<uint64_t>> &row_keeper,  vector<vector<uint64_t>> & col_keeper)
{
// void find_row_col_basis(const vector<int64_t> &mat, size_t length, vector<vector<uint64_t>> &row_keeper,  vector<vector<uint64_t>> & col_keeper)
// {
    zz_p::init(prime_num);
    vec_zz_p ntl_mat; 
    size_t row_num = mat.size()/col_num;
    assert(mat.size()==row_num*col_num);
    ntl_mat.SetLength(mat.size());
    for (int i=0; i<mat.size(); i++)
    {
        ntl_mat[i]=mat[i];
    }
    // for (int i=0; i<mat.size(); i++)
    // {
    //     ntl_mat[i]=random()%prime_num;
    // }

    row_keeper.resize(row_num);
    col_keeper.resize(col_num);

    vector<zz_pX> ntl_row_keeper, ntl_col_keeper;




    find_row_col_basis(ntl_mat, col_num, ntl_row_keeper, ntl_col_keeper);
    // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    std::cout<<"ntl_row_keeper.size()="<<ntl_row_keeper.size()<<std::endl;
      std::cout<<"ntl_col_keeper.size()="<<ntl_col_keeper.size()<<std::endl;
    for (int i=0; i<ntl_row_keeper.size(); i++)
    {
        // std::cout<<"i="<<i<<", row_keeper[i]="<<ntl_row_keeper[i]<<std::endl;
        row_keeper[i].resize(ntl_row_keeper[i].rep.length());
        for (int j=0; j<ntl_row_keeper[i].rep.length(); j++)
        {
            row_keeper[i][j]=(uint64_t) rep(ntl_row_keeper[i][j]); 

        }
    }
    // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    for (int i=0; i<ntl_col_keeper.size(); i++)
    {
        // std::cout<<"i="<<i<<", col_keeper[i]="<<ntl_col_keeper[i]<<std::endl;
        col_keeper[i].resize(ntl_col_keeper[i].rep.length());
        for (int j=0; j<ntl_col_keeper[i].rep.length(); j++)
        {
            col_keeper[i][j]=(uint64_t) rep(ntl_col_keeper[i][j]);
        }
    }
    // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

}








class PolyComputer {
   private:
    int64_t q, w, log_da, da;
    FFTPrimeInfo info;
    zz_p xi, inv_da;
    zz_pX xi_powers;

   public:
    PolyComputer(int64_t prime_num, int64_t log_degree)
        :  // NOLINT
          q(prime_num),
          log_da(log_degree) {
        zz_p::init(q);
        // std::cout << "p=" << zz_p::modulus() << std::endl;
        if (!IsFFTPrime(q, w)) LogicError("invalid prime");
        std::cout << "w=" << w << std::endl;
        da = 1 << log_da;
        InitFFTPrimeInfo(info, q, w, -1);
        // zz_pX one(1UL, 1UL), W;  // 0+1T
        zz_pX one(da - 1, 1UL);  // T^{da-1}
        one.SetLength(da);
        xi_powers.SetLength(da);
        real_ifft(log_da, one, info, xi_powers);
        // xi = W[da-1];
        xi = xi_powers[1];
        std::cout << "xi=" << xi << std::endl;
        inv_da = inv((zz_p)da);
    }

    PolyComputer(int64_t prime_num, int64_t self_defined_w, int64_t log_degree)
        :  // NOLINT
          q(prime_num),
          w(self_defined_w),
          log_da(log_degree) {
        // if (!IsFFTPrime(q, w)) LogicError("invalid prime");
        zz_p::init(q);
        da = 1 << log_da;
        InitFFTPrimeInfo(info, q, w, -1);
        // zz_pX one(1UL, 1UL), W;  // 0+1T
        zz_pX one(da - 1, 1UL);  // T^{da-1}
        one.SetLength(da);
        xi_powers.SetLength(da);
        real_ifft(log_da, one, info, xi_powers);
        xi = xi_powers[1];
        std::cout << "xi=" << xi << std::endl;
        // std::cout<<"p="<<p<<std::endl;
        // std::cout<<"da="<<da<<std::endl;
        inv_da = inv((zz_p)da);
    }

    ~PolyComputer() = default;

    void fft_interpolate(const vector<int64_t> &data, vector<int64_t> &coeff) { // NOLINT
        real_ifft(log_da, data, info, coeff);
        //  vec_zz_p * tmp = (vec_zz_p *) coeff.data();
        //  tmp->SetLength(da);
        for (int i = 0; i < da; i++) {
            mul((zz_p &)coeff[i], (zz_p)coeff[i], inv_da);
        }
    }

    int check_fft_interpolate(const vector<int64_t> &data,
                              const vector<int64_t> &coeff) {
        zz_pX poly;
        size_t degree = coeff.size();
        poly.SetLength(degree);
        for (int i = 0; i < degree; i++) {
            poly[i] = coeff[i];
        }

        for (int64_t i = 0; i < 10; i++) {
            // zz_p xi_power;
            // power(xi_power, xi, i);
            // std::cout<<"xi_power="<<xi_power<<std::endl;
            auto y = eval(poly, xi_powers[i]);
            std::cout << "i, data[i], f(xi^i)=" << i << "," << data[i] << ","
                      << y << std::endl;
            if (y != data[i]) {
                LogicError("interpolator error!");
            }
        }
        return 0;
    }

    vector<int64_t> eval_poly_val(const vector<int64_t> &x,
                                  const vector<int64_t> &coeff, bool from_ids=true) {
        zz_pX poly;
        size_t len = coeff.size();
        poly.SetLength(len);
        for (int i = 0; i < len; i++) {
            poly[i] = coeff[i];
        }
        vector<int64_t> output(x.size());
        vec_zz_p vx_zzp;
        vx_zzp.SetLength(x.size());
        if (from_ids)
        {
            for (int64_t i = 0; i < x.size(); i++) {
            vx_zzp[i]=xi_powers[x[i]];}
        }else{
            for (int64_t i = 0; i < x.size(); i++) {
                vx_zzp[i]= zz_p(x[i]);
            }
        }
        auto y = eval(poly, vx_zzp);
        for (int64_t i = 0; i < x.size(); i++) {
            output[i] = rep(y[i]);
        }
        return output;
    }


    vector<vector<int64_t>> eval_batch_poly_val(const vector<int64_t> &x,
                                  const vector<vector<int64_t>> &coeff_mat, bool from_ids=true) {
        // zz_pX poly;
        // size_t len = coeff.size();
        // poly.SetLength(len);
        // for (int i = 0; i < len; i++) {
        //     poly[i] = coeff[i];
        // }
        size_t poly_num = coeff_mat.size();
        size_t coeff_num = coeff_mat[0].size();
        size_t points_num = x.size();
        mat_zz_p coeff_mat_zzp, xi_powers_mat_zzp, output_zzp;
        coeff_mat_zzp.SetDims(poly_num, coeff_num);
        xi_powers_mat_zzp.SetDims(coeff_num, points_num);
        output_zzp.SetDims(poly_num, points_num);

        vector<vector<int64_t>> output(poly_num);
        for (size_t i=0; i<poly_num; i++)
        {
            output[i].resize(points_num);
        }

        for (int64_t i=0; i<poly_num; i++)
        {
            for (int j=0; j<coeff_num; j++)
            {
                coeff_mat_zzp[i][j]=zz_p(coeff_mat[i][j]);
            }
        }

        if (from_ids)
        {
            for (int64_t i = 0; i <coeff_num; i++) {
                for (int64_t j=0; j<points_num; j++)
                {
                    xi_powers_mat_zzp[i][j]=xi_powers[(x[j]*i)%da];
                }
            }
        }else{
            for (int64_t i = 0; i < x.size(); i++) {
                for (int64_t j=0; j<points_num; j++)
                {
                xi_powers_mat_zzp[i][j]= power(zz_p(x[j]), i);
                }
            }
        }
        std::cout<<__LINE__<<":"<<__LINE__<<std::endl;
        // std::cout<<"coeff_mat_zzp="<<coeff_mat_zzp<<std::endl;
        // std::cout<<"xi_powers_mat_zzp="<<xi_powers_mat_zzp<<std::endl;
        mul(output_zzp, coeff_mat_zzp, xi_powers_mat_zzp);

        for (int64_t i = 0; i < poly_num; i++) {
            for (int j=0; j<points_num; j++)
            {
                 output[i][j] = rep(output_zzp[i][j]);
            }
        }
        return output;
    }




    int check_poly_value(const vector<int64_t> &new_ids,
                         const vector<int64_t> &coeff,
                         const vector<int64_t> &data) {
        zz_pX poly;
        size_t len = coeff.size();
        poly.SetLength(len);
        for (int i = 0; i < len; i++) {
            poly[i] = coeff[i];
        }

        for (int64_t i = 0; i < new_ids.size(); i++) {
            // zz_p xi_power;
            // power(xi_power, xi, i);
            // std::cout<<"xi_power="<<xi_power<<std::endl;
            auto y = eval(poly, xi_powers[new_ids[i]]);
            std::cout << "new_id, data, f(xi^new_id)=" << new_ids[i] << ","
                      << data[i] << "," << y << std::endl;
            if (y != data[i]) {
                throw std::runtime_error("interpolator error!");
            }
        }
        return 0;
    }


    void get_xi_powers(const vector<int64_t> &new_ids,
                       vector<int64_t> &xi_power_new_ids) { // NOLINT
        xi_power_new_ids.resize(new_ids.size());
        for (int i = 0; i < new_ids.size(); i++) {
            xi_power_new_ids[i] = rep(xi_powers[(new_ids[i]%da+da)%da]);
        }
    }

    void get_chara_poly(
        const vector<int64_t> &new_ids,
        zz_pX &chara_poly) {  // NOLINT
        // chara_poly = \prod _{i in new_ids} (x-\xi^i)
        vec_zz_p xi_power_new_ids;
        xi_power_new_ids.SetLength(new_ids.size());
        for (int i = 0; i < new_ids.size(); i++) {
            xi_power_new_ids[i] = xi_powers[new_ids[i]];
        }
        BuildFromRoots(chara_poly, xi_power_new_ids);
    }

    void get_x_powers_mod_chara_poly(
        const zz_pX &chara_poly, const size_t K,
        std::vector<std::vector<int64_t>> &x_powers_mod_chara_poly) {   // NOLINT
        // x ^ nk \mod chara_poly(x) for k=0, \cdots, K-1
        zz_pXModulus chara_poly_mod(chara_poly);
        x_powers_mod_chara_poly.resize(K);
        size_t degree = deg(chara_poly);
        zz_pX tmp_x_powers_mod_chara_poly;

        for (int i = 0; i < K; i++) {
            PowerXMod(tmp_x_powers_mod_chara_poly, i * degree, chara_poly_mod);

            x_powers_mod_chara_poly[i].resize(degree);
            for (size_t j = 0; j < degree; j++) {
                if (j <= deg(tmp_x_powers_mod_chara_poly)) {
                    x_powers_mod_chara_poly[i][j] =
                        rep(tmp_x_powers_mod_chara_poly[j]);
                } else {
                    x_powers_mod_chara_poly[i][j] = 0;
                }
            }
        }
    }

    void get_x_powers_mod_new_ids(
        const vector<int64_t> &new_ids, const size_t K,
        std::vector<std::vector<int64_t>> &x_powers_mod_chara_poly) {   // NOLINT
        zz_pX chara_poly;
        get_chara_poly(new_ids, chara_poly);
        // std::cout << "chara_poly=" << chara_poly << std::endl;
        get_x_powers_mod_chara_poly(chara_poly, K, x_powers_mod_chara_poly);
    }

    void get_x_powers_mod_chara_poly(
        const zz_pX &chara_poly, const size_t degree, const size_t K,
        std::vector<std::vector<int64_t>> &x_powers_mod_chara_poly) {   // NOLINT
        // x ^ nk \mod chara_poly(x) for k=0, \cdots, K-1
        zz_pXModulus chara_poly_mod(chara_poly);
        x_powers_mod_chara_poly.resize(K);
        // size_t degree = deg(chara_poly);
        zz_pX tmp_x_powers_mod_chara_poly;

        for (int i = 0; i < K; i++) {
            PowerXMod(tmp_x_powers_mod_chara_poly, i * degree, chara_poly_mod);
            // if (i<10 || i>3275)
            // {
            //    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            //    std::cout<<"i,
            //    tmp_x_powers_mod_chara_poly="<<i<<","<<tmp_x_powers_mod_chara_poly<<std::endl;
            // }

            x_powers_mod_chara_poly[i].resize(degree);
            for (size_t j = 0; j < degree; j++) {
                if (j <= deg(tmp_x_powers_mod_chara_poly)) {
                    x_powers_mod_chara_poly[i][j] =
                        rep(tmp_x_powers_mod_chara_poly[j]);
                } else {
                    x_powers_mod_chara_poly[i][j] = 0;
                }
            }
        }
    }

    void get_x_powers_mod_new_ids(
        const vector<int64_t> &new_ids, const size_t degree, const size_t K,
        std::vector<std::vector<int64_t>> &x_powers_mod_chara_poly) {   // NOLINT
        zz_pX chara_poly;
        get_chara_poly(new_ids, chara_poly);
        // std::cout << "chara_poly=" << chara_poly << std::endl;
        get_x_powers_mod_chara_poly(chara_poly, degree, K,
                                    x_powers_mod_chara_poly);
    }


    void get_chara_Xpowers_mod_new_ids(
        const vector<int64_t> &new_ids, const size_t degree, const size_t K,
        std::vector<std::vector<int64_t>> &chara_x_powers_mod_chara_poly) {   // NOLINT
        zz_pX chara_poly;
        get_chara_poly(new_ids, chara_poly);
        // std::cout << "chara_poly=" << chara_poly << std::endl;
        get_x_powers_mod_chara_poly(chara_poly, degree, K,
                                    chara_x_powers_mod_chara_poly);
        size_t chara_poly_size = chara_poly.rep.length();
        chara_x_powers_mod_chara_poly[0].resize(chara_poly_size);
        for (int i=0; i<chara_poly_size; i++)
        {
            chara_x_powers_mod_chara_poly[0][i]=rep(chara_poly[i]);
        }
        
    }
};

