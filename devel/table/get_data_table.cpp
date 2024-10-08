// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-24 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_data_table dev}

C++: Get the Data Table Information
###################################

Syntax
******

| ``get_data_table`` (
| |tab| *db* , *n_covariate* , *age_min* , *age_max* , *time_min* , *time_max* ,
| |tab| *data_table* , *data_cov_value*
| )

Purpose
*******
To read the :ref:`data_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

n_covariate
***********
This argument has prototype

   ``size_t`` *n_covariate*

and is the size of the :ref:`covariate_table<get_covariate_table-name>` .

age_min
*******
This argument has prototype

   ``double`` *age_min*

and is the minimum value in the :ref:`age_table-name` .
If an :ref:`data_table@age_lower` value is less than
*age_min* , an error is reported.

age_max
*******
This argument has prototype

   ``double`` *age_max*

and is the maximum value in the :ref:`age_table-name` .
If an :ref:`data_table@age_upper` value is greater than
*age_max* , an error is reported.
The condition

   *age_lower* <= *age_upper*

is also checked.

time_min
********
This argument has prototype

   ``double`` *time_min*

and is the minimum value in the :ref:`time_table-name` .
If an :ref:`data_table@time_lower` value is less than
*time_min* , an error is reported.

time_max
********
This argument has prototype

   ``double`` *time_max*

and is the maximum value in the :ref:`time_table-name` .
If an :ref:`data_table@time_upper` value is greater than
*time_max* , an error is reported.
The condition

   *time_lower* <= *time_upper*

is also checked.

data_table
**********
This argument has prototype

   ``CppAD::vector<data_struct>&`` *data_table*

On input its size is zero and upon return it has one element for
each row in the data table.
For each :ref:`data_table@data_id` ,

   *data_table* [ *data_id* ]

is the ``data_struct`` information for the corresponding data.

data_struct
===========
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``int``
     - ``integrand_id``
     - The :ref:`data_table@integrand_id` for this measurement
   * - ``int``
     - ``density_id``
     - The :ref:`data_table@density_id` for this measurement
   * - ``int``
     - ``node_id``
     - The :ref:`data_table@node_id` for this measurement
   * - ``int``
     - ``subgroup_id``
     - The :ref:`data_table@subgroup_id` for this measurement
   * - ``int``
     - ``weight_id``
     - The :ref:`data_table@weight_id` for this measurement
   * - ``int``
     - ``hold_out``
     - The :ref:`data_table@hold_out` for this measurement
   * - ``double``
     - ``meas_value``
     - The :ref:`data_table@meas_value` for this measurement
   * - ``double``
     - ``meas_std``
     - The :ref:`data_table@meas_std` for this measurement
   * - ``double``
     - ``eta``
     - The :ref:`data_table@eta` for this measurement
   * - ``double``
     - ``nu``
     - The :ref:`data_table@nu` for this measurement
   * - ``int``
     - ``sample_size``
     - The :ref:`data_table@sample_size` for this measurement
   * - ``double``
     - ``age_lower``
     - The :ref:`data_table@age_lower` for this measurement
   * - ``double``
     - ``age_upper``
     - The :ref:`data_table@age_upper` for this measurement
   * - ``double``
     - ``time_lower``
     - The :ref:`data_table@time_lower` for this measurement
   * - ``double``
     - ``time_upper``
     - The :ref:`data_table@time_upper` for this measurement

data_cov_value
**************
This argument has prototype

   ``CppAD::vector<double>&`` *data_cov_value*

On input its size is zero.
Upon return, its size is the number of data points times
the number of covariates.
For each
:ref:`covariate_table@covariate_id` and *data_id* pair

   ``data_cov_value`` [ *data_id* * *n_covariate* + *covariate_id* ]

is the corresponding covariate value.
{xrst_toc_hidden
   example/devel/table/get_data_table_xam.cpp
}
Example
*******
The file :ref:`get_data_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_data_table}
-----------------------------------------------------------------------------
*/
# include <cmath>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void get_data_table(
   sqlite3*                           db                ,
   const CppAD::vector<density_enum>& density_table     ,
   size_t                             n_covariate       ,
   double                             age_min           ,
   double                             age_max           ,
   double                             time_min          ,
   double                             time_max          ,
   CppAD::vector<data_struct>&        data_table        ,
   CppAD::vector<double>&             data_cov_value    )
{  using std::string;

   string table_name  = "data";
   string column_name;
   size_t n_data      = check_table_id(db, table_name);
   //
   // data_table
   {  // read data table

      column_name = "integrand_id";
      CppAD::vector<int>    integrand_id;
      get_table_column(db, table_name, column_name, integrand_id);
      assert( n_data == integrand_id.size() );

      column_name        = "density_id";
      CppAD::vector<int>    density_id;
      get_table_column(db, table_name, column_name, density_id);
      assert( n_data == density_id.size() );

      column_name        = "hold_out";
      CppAD::vector<int>    hold_out;
      get_table_column(db, table_name, column_name, hold_out);
      assert( n_data == hold_out.size() );

      column_name        = "node_id";
      CppAD::vector<int>    node_id;
      get_table_column(db, table_name, column_name, node_id);
      assert( n_data == node_id.size() );

      column_name        = "weight_id";
      CppAD::vector<int>    weight_id;
      get_table_column(db, table_name, column_name, weight_id);
      assert( n_data == weight_id.size() );

      column_name           =  "meas_value";
      CppAD::vector<double>     meas_value;
      get_table_column(db, table_name, column_name, meas_value);
      assert( n_data == meas_value.size() );

      column_name           =  "meas_std";
      CppAD::vector<double>     meas_std;
      get_table_column(db, table_name, column_name, meas_std);
      assert( n_data == meas_std.size() );

      column_name           =  "eta";
      CppAD::vector<double>     eta;
      get_table_column(db, table_name, column_name, eta);
      assert( n_data == eta.size() );

      column_name           =  "nu";
      CppAD::vector<double>     nu;
      get_table_column(db, table_name, column_name, nu);
      assert( n_data == nu.size() );

      column_name           =  "sample_size";
      CppAD::vector<int>     sample_size;
      get_table_column(db, table_name, column_name, sample_size);
      assert( n_data == sample_size.size() );

      column_name           =  "age_lower";
      CppAD::vector<double>     age_lower;
      get_table_column(db, table_name, column_name, age_lower);
      assert( n_data == age_lower.size() );

      column_name           =  "age_upper";
      CppAD::vector<double>     age_upper;
      get_table_column(db, table_name, column_name, age_upper);
      assert( n_data == age_upper.size() );

      column_name           =  "time_lower";
      CppAD::vector<double>     time_lower;
      get_table_column(db, table_name, column_name, time_lower);
      assert( n_data == time_lower.size() );

      column_name           =  "time_upper";
      CppAD::vector<double>     time_upper;
      get_table_column(db, table_name, column_name, time_upper);
      assert( n_data == time_upper.size() );

      column_name           = "subgroup_id";
      CppAD::vector<int>       subgroup_id;
      get_table_column(db, table_name, column_name, subgroup_id);
      assert( n_data == subgroup_id.size() );

      // set data_table
      assert( data_table.size() == 0 );
      data_table.resize(n_data);
      for(size_t i = 0; i < n_data; i++)
      {
         data_table[i].integrand_id  = integrand_id[i];
         data_table[i].density_id    = density_id[i];
         data_table[i].node_id       = node_id[i];
         data_table[i].subgroup_id   = subgroup_id[i];
         data_table[i].weight_id     = weight_id[i];
         data_table[i].hold_out      = hold_out[i];
         data_table[i].meas_value    = meas_value[i];
         data_table[i].meas_std      = meas_std[i];
         data_table[i].eta           = eta[i];
         data_table[i].nu            = nu[i];
         data_table[i].sample_size   = sample_size[i];
         data_table[i].age_lower     = age_lower[i];
         data_table[i].age_upper     = age_upper[i];
         data_table[i].time_lower    = time_lower[i];
         data_table[i].time_upper    = time_upper[i];
      }
   }

   // now get the covariates
   assert( data_cov_value.size() == 0 );
   data_cov_value.resize(n_data * n_covariate );
   for(size_t j = 0; j < n_covariate; j++)
   {  std::stringstream ss;
      ss << "x_" << j;
      column_name = ss.str();
      CppAD::vector<double> x_j;
      get_table_column(db, table_name, column_name, x_j);
      for(size_t i = 0; i < n_data; i++)
         data_cov_value[ i * n_covariate + j ] = x_j[i];
   }

   // check for error conditions
   // (primary key conditions checked by calling routine)
   string msg;
   for(size_t data_id = 0; data_id < n_data; data_id++)
   {
      // density
      density_enum density = density_table[ data_table[data_id].density_id ];
      //
      // meas_std
      double meas_std = data_table[data_id].meas_std;
      //
      // ------------------------------------------------------------
      int subgroup_id = data_table[data_id].subgroup_id;
      if( subgroup_id == DISMOD_AT_NULL_INT )
      {  msg = "subgroup_id is null";
         error_exit(msg, table_name, data_id);
      }
      // -------------------------------------------------------------
      int hold_out = data_table[data_id].hold_out;
      if( hold_out != 0 && hold_out != 1 )
      {  msg = "hold_out is not equal to zero or one";
         error_exit(msg, table_name, data_id);
      }
      // -------------------------------------------------------------
      double age_lower  = data_table[data_id].age_lower;
      double age_upper  = data_table[data_id].age_upper;
      if( age_lower < age_min )
      {  msg = "age_lower is less than minimum age in age table";
         error_exit(msg, table_name, data_id);
      }
      if( age_max < age_upper )
      {  msg = "age_upper is greater than maximum age in age table";
         error_exit(msg, table_name, data_id);
      }
      if( age_upper < age_lower )
      {  msg = "age_lower is greater than age_upper";
         error_exit(msg, table_name, data_id);
      }
      // ------------------------------------------------------------
      double time_lower = data_table[data_id].time_lower;
      double time_upper = data_table[data_id].time_upper;
      if( time_lower < time_min )
      {  msg = "time_lower is less than minimum time in time table";
         error_exit(msg, table_name, data_id);
      }
      if( time_max < time_upper )
      {  msg = "time_upper is greater than maximum time in time table";
         error_exit(msg, table_name, data_id);
      }
      if( time_upper < time_lower )
      {  msg = "time_lower is greater than time_upper";
         error_exit(msg, table_name, data_id);
      }
      if( density == uniform_enum )
      {  msg = "density_id corresponds to the uniform distribution";
         error_exit(msg, table_name, data_id);
      }
      if( density != binomial_enum && meas_std <= 0.0 )
      {  msg = "meas_std is not positive and density is not binomial";
         error_exit(msg, table_name, data_id);
      }
      double meas_value = data_table[data_id].meas_value;
      if( std::isnan( meas_value ) )
      {  msg = "meas_value is null";
         error_exit(msg, table_name, data_id);
      }
      //
      double eta       = data_table[data_id].eta;
      bool eta_null    = std::isnan(eta);
      if( log_density( density ) && eta_null )
      {  msg = "density is a log density and eta is null";
         error_exit(msg, table_name, data_id);
      }
      if( log_density( density ) && eta < 0.0 )
      {  msg = "eta is less than zero";
         error_exit(msg, table_name, data_id);
      }
      //
      double nu        = data_table[data_id].nu;
      bool nu_null     = std::isnan(nu);
      bool students    = density == students_enum;
      students        |= density == log_students_enum;
      if( students && nu_null )
      {  msg = "density is students or log_students and nu is null";
         error_exit(msg, table_name, data_id);
      }
      if( students && nu <= 2.0 )
      {  msg = "density is students or log_students and nu <= 2.0";
         error_exit(msg, table_name, data_id);
      }
      //
      int sample_size       = data_table[data_id].sample_size;
      bool sample_size_null = sample_size == DISMOD_AT_NULL_INT;
      bool binomial         = density == binomial_enum;
      if( binomial && sample_size_null )
      {  msg = "densityy is binomial and sample_size is null";
         error_exit(msg, table_name, data_id);
      }
      if( binomial && sample_size <= 0 )
      {  msg = "densityy is binomial and sample_size is <= 0";
         error_exit(msg, table_name, data_id);
      }
      if( binomial && ! std::isnan(meas_std) )
      {  msg = "densityy is binomial and meas_std is not null";
         error_exit(msg, table_name, data_id);
      }
   }
   return;
}

} // END DISMOD_AT_NAMESPACE
