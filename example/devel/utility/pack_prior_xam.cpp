// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-25 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin pack_prior_xam.cpp dev}

C++ pack_prior: Example and Test
################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end pack_prior_xam.cpp}
*/
// BEGIN C++
# include <limits>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/null_int.hpp>

# define DISMOD_AT_PRIOR_DENSITY_XAM_TRACE 0

bool pack_prior_xam(void)
{  bool   ok = true;
   using CppAD::vector;
   using std::cout;
   using std::endl;
   double inf = std::numeric_limits<double>::infinity();
   double nan = std::numeric_limits<double>::quiet_NaN();


   // --------------------------------------------------------------------
   // age_table
   size_t n_age_table = 10;
   vector<double> age_table(n_age_table);
   for(size_t i = 0; i < n_age_table; i++)
      age_table[i] = 100 * double(i) / double(n_age_table - 1);
   //
   // time_table
   size_t n_time_table = 5;
   vector<double> time_table(n_time_table);
   for(size_t i = 0; i < n_time_table; i++)
      time_table[i] = (2015 - 1975) * double(i) / double(n_time_table - 1);
   // ----------------------- prior table ---------------------------------
   size_t n_prior_table = 6;
   vector<dismod_at::prior_struct> prior_table(n_prior_table);
   // prior_id = 0
   prior_table[0].density_id = int( dismod_at::uniform_enum );
   prior_table[0].lower      = 0.0;
   prior_table[0].mean       = 0.0;
   prior_table[0].upper      = 0.0;
   // prior_id = 1
   prior_table[1].density_id = int( dismod_at::uniform_enum );
   prior_table[1].lower      = 1.0;
   prior_table[1].mean       = 1.0;
   prior_table[1].upper      = 1.0;
   // size_t prior_id_none = 2;
   prior_table[2].density_id = int( dismod_at::uniform_enum );
   prior_table[2].lower      = -inf;
   prior_table[2].mean       = 0.0;
   prior_table[2].upper      = +inf;
   size_t prior_id_gaussian = 3;
   prior_table[3].density_id = int( dismod_at::gaussian_enum );
   prior_table[3].lower      = - 1.0;
   prior_table[3].mean       = 0.0;
   prior_table[3].upper      = 1.0;
   prior_table[3].std        = 0.5;
   size_t prior_id_laplace = 4;
   prior_table[4].density_id = int( dismod_at::laplace_enum );
   prior_table[4].lower      = -1.0;
   prior_table[4].mean       = 0.0;
   prior_table[4].upper      = 1.0;
   prior_table[4].std        = 0.5;
   size_t prior_id_log_gaussian = 5;
   prior_table[5].density_id = int( dismod_at::log_gaussian_enum );
   prior_table[5].lower      = 0.01;
   prior_table[5].mean       = 0.1;
   prior_table[5].upper      = 1.0;
   prior_table[5].std        = 0.5;
   prior_table[5].eta        = 1e-3;
   // -------------------------------------------------------------------
   // smoothing information
   vector<size_t> age_id, time_id;
   vector<size_t> value_prior_id, dage_prior_id, dtime_prior_id;
   vector<double> const_value;
   size_t mulstd_value, mulstd_dage, mulstd_dtime;
   bool all_const_value = false;
   size_t n_age, n_time, n_grid;

   vector<dismod_at::smooth_info> s_info_vec(2);
   // ------------------ first smoothing ------------------------------------
   // age_id
   n_age = 3;
   age_id.resize(n_age);
   age_id[0] = 0;
   age_id[1] = n_age_table / 2;
   age_id[2] = n_age_table - 1;
   // time_id
   n_time = 2;
   time_id.resize(n_time);
   time_id[0] = 0;
   time_id[1] = n_time_table - 1;
   // scalar prior_id
   mulstd_value = 0;
   mulstd_dage  = 1;
   mulstd_dtime = 2;
   // grid prior ids
   n_grid = n_age * n_time;
   value_prior_id.resize(n_grid);
   dage_prior_id.resize(n_grid);
   dtime_prior_id.resize(n_grid);
   const_value.resize(n_grid);
   for(size_t i = 0; i < n_age; i++)
   {  for(size_t j = 0; j < n_time; j++)
      {  value_prior_id[ i * n_time + j ] = prior_id_gaussian;
         dage_prior_id[ i * n_time + j ]  = prior_id_laplace;
         dtime_prior_id[ i * n_time + j ] = prior_id_log_gaussian;
         const_value[ i * n_time + j ]    = nan;
      }
   }
   //
   // s_info
   size_t smooth_id_3_by_2 = 0;
   s_info_vec[0] = dismod_at::smooth_info(
      age_table, time_table, age_id, time_id,
      value_prior_id, dage_prior_id, dtime_prior_id, const_value,
      mulstd_value, mulstd_dage, mulstd_dtime, all_const_value
   );
   // ------------------ second smoothing -----------------------------------
   // age_id
   n_age = 1;
   age_id.resize(n_age);
   age_id[0] = n_age_table / 2;
   // time_id
   n_time = 2;
   time_id.resize(n_time);
   time_id[0] = 0;
   time_id[1] = n_time_table - 1;
   // scalar prior_id
   mulstd_value = 0;
   mulstd_dage  = 1;
   mulstd_dtime = 2;
   // grid prior ids
   n_grid = n_age * n_time;
   value_prior_id.resize(n_grid);
   dage_prior_id.resize(n_grid);
   dtime_prior_id.resize(n_grid);
   const_value.resize(n_grid);
   for(size_t i = 0; i < n_age; i++)
   {  for(size_t j = 0; j < n_time; j++)
      {  value_prior_id[ i * n_time + j ] = prior_id_gaussian;
         dage_prior_id[ i * n_time + j ]  = prior_id_laplace;
         dtime_prior_id[ i * n_time + j ] = prior_id_log_gaussian;
         const_value[ i * n_time + j ]    = nan;
      }
   }
   //
   // s_info
   size_t smooth_id_1_by_2 = 1;
   s_info_vec[1] = dismod_at::smooth_info(
      age_table, time_table, age_id, time_id,
      value_prior_id, dage_prior_id, dtime_prior_id, const_value,
      mulstd_value, mulstd_dage, mulstd_dtime, all_const_value
   );
   // ----------------------- pack_object --------------------------------
   //
   // subgroup_table
   size_t n_subgroup = 1;
   vector<dismod_at::subgroup_struct> subgroup_table(n_subgroup);
   subgroup_table[0].subgroup_name = "world";
   subgroup_table[0].group_id      = 0;
   subgroup_table[0].group_name    = "world";
   //
   // smooth_table
   vector<dismod_at::smooth_struct> smooth_table(s_info_vec.size());
   for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
   {  smooth_table[smooth_id].n_age  =  int( s_info_vec[smooth_id].age_size() );
      smooth_table[smooth_id].n_time =  int( s_info_vec[smooth_id].time_size() );
   }
   //
   // mulcov_table
   vector<dismod_at::mulcov_struct> mulcov_table(0);
   //
   // rate_table
   vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
   for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
   {  if( rate_id == dismod_at::pini_enum )
      {  // smoothing must have only one age
         rate_table[rate_id].parent_smooth_id =  int( smooth_id_1_by_2 );
         rate_table[rate_id].child_smooth_id  =  int( smooth_id_1_by_2 );
         rate_table[rate_id].child_nslist_id  =  int( DISMOD_AT_NULL_INT );
      }
      else
      {  rate_table[rate_id].parent_smooth_id =  int( smooth_id_3_by_2 );
         rate_table[rate_id].child_smooth_id  =  int( smooth_id_3_by_2 );
         rate_table[rate_id].child_nslist_id  =  int( DISMOD_AT_NULL_INT );
      }
   }
   // pack_object
   // values in child_id2node_id do not matter because child_nslist_id is null
   size_t n_integrand = 0;
   size_t n_child     = 1;
   vector<size_t> child_id2node_id(n_child);
   vector<dismod_at::nslist_pair_struct> nslist_pair(0);
   dismod_at::pack_info pack_object(
      n_integrand,
      child_id2node_id,
      subgroup_table,
      smooth_table,
      mulcov_table,
      rate_table,
      nslist_pair
   );
   // ----------------------- value_prior -------------------------------
   double bound_random = inf;
   vector<size_t> n_child_data_in_fit(n_child);
   for(size_t child = 0; child < n_child; ++child)
      n_child_data_in_fit[child] = 1;
   dismod_at::pack_prior var2prior(
      bound_random, n_child_data_in_fit, prior_table, pack_object, s_info_vec
   );
   dismod_at::pack_info::subvec_info info;
   //
   // check mulstd
   for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
   {  for(size_t k = 0; k < 3; k++)
      {  size_t offset  = pack_object.mulstd_offset(smooth_id, k);
         assert( offset != DISMOD_AT_NULL_SIZE_T );
         ok &= var2prior.value_prior_id(offset) == k;
      }
   }
   //
   // check rates
   for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
   {  for(size_t child_id = 0; child_id <= n_child; child_id++)
      {  info = pack_object.node_rate_value_info(rate_id, child_id);
         dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
         n_age  = s_info.age_size();
         n_time = s_info.time_size();
         for(size_t i = 0; i < n_age; i++)
         {  for(size_t j = 0; j < n_time; j++)
            {  size_t index    = info.offset + i * n_time + j;
               size_t prior_id = var2prior.value_prior_id(index);
               ok &= s_info.value_prior_id(i, j) == prior_id;
            }
         }
      }
   }
   return ok;
}
// END C++
