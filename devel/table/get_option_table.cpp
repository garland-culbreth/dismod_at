// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-25 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_option_table dev}
{xrst_spell
  option option
}

C++: Get the Option Table Information
#####################################

Syntax
******
*option_table* = ``get_option_table`` ( *db* )

Purpose
*******
To read the :ref:`option_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

option_struct
*************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``std::string``
     - ``option_name``
     - The :ref:`name<option_table@Table Format@option_name>`
       for this option
   * - ``std::string``
     - ``option_value``
     - The :ref:`value<option_table@Table Format@option_value>`
       for this option

option_table
************
The return value *option_table* has prototype

   ``CppAD::vector<option_struct>`` *option_table*

For *i* = 0 , ..., *n_option* ,

   *option_table* [ *i* ]

is the information for the one option.

Special Note
************
This table is special because the index *i*
is not the same as *option_id* in the :ref:`option_table-name` .
This is because all the options have default values that are used when
an option does not appear in the actual table.
{xrst_toc_hidden
   example/devel/table/get_option_table_xam.cpp
}
Example
*******
The file :ref:`get_option_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_option_table}
-----------------------------------------------------------------------------
*/



# include <cppad/utility/to_string.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_option_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/split_space.hpp>
# include <dismod_at/log_message.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<option_struct> get_option_table(sqlite3* db)
{  using std::string;
   using CppAD::to_string;
   //
   // input_table_list, input_table_list_size
   const char* input_table_list[] = {
      "age",
      "avgint",
      "covariate",
      "data",
      "density",
      "integrand",
      "mulcov",
      "rate_eff_cov",
      "node",
      "nlist_pair",
      "prior",
      "rate",
      "smooth",
      "smooth_grid",
      "subgroup",
      "time",
      "weight",
      "weight_grid"
   };
   size_t input_table_list_size = \
      sizeof(input_table_list) / sizeof(input_table_list[0]);
   //
   // for error messaging
   string msg;
   //
   // option name and its default value
   // Changes here must also be made in python/dismod_at/db2csv_command.py
   struct { const char* name; const char* value; } option_list[] = {
      // BEGIN_SORT_THIS_LINE_PLUS_1
      { "accept_after_max_steps_fixed",     "5"                  },
      { "accept_after_max_steps_random",    "5"                  },
      { "age_avg_split",                    ""                   },
      { "avgint_extra_columns",             ""                   },
      { "bound_frac_fixed",                 "1e-2"               },
      { "bound_random",                     ""                   },
      { "compress_interval",                "0 0"                },
      { "data_extra_columns",               ""                   },
      { "derivative_test_fixed",            "none"               },
      { "derivative_test_random",           "none"               },
      { "hold_out_integrand",               ""                   },
      { "limited_memory_max_history_fixed", "30"                 },
      { "max_num_iter_fixed",               "100"                },
      { "max_num_iter_random",              "100"                },
      { "meas_noise_effect",                "add_std_scale_all"  },
      { "method_random",                    "ipopt_random"       },
      { "ode_step_size",                    "10.0"               },
      { "other_database",                   ""                   },
      { "other_input_table",                ""                   },
      { "parent_node_id",                   ""                   },
      { "parent_node_name",                 ""                   },
      { "print_level_fixed",                "0"                  },
      { "print_level_random",               "0"                  },
      { "quasi_fixed",                      "true"               },
      { "random_seed",                      "0"                  },
      { "rate_case",                        "iota_pos_rho_zero"  },
      { "splitting_covariate",              ""                   },
      { "tolerance_fixed",                  "1e-8"               },
      { "tolerance_random",                 "1e-8"               },
      { "trace_init_fit_model",             "false"              },
      { "warn_on_stderr",                   "true"               },
      { "zero_sum_child_rate",              ""                   },
      { "zero_sum_mulcov_group",            ""                   },
      // END_SORT_THIS_LINE_MINUS_1
   };
   size_t n_option = sizeof( option_list ) / sizeof( option_list[0] );
   CppAD::vector<string> name_vec(n_option), value_vec(n_option);
   for(size_t i = 0; i < n_option; i++)
   {  name_vec[i]   = option_list[i].name;
      value_vec[i]  = option_list[i].value;
   }
   //
   string table_name  = "option";
   size_t n_in_table  = check_table_id(db, table_name);
   //
   // option_name
   string column_name = "option_name";
   CppAD::vector<string>  option_name;
   get_table_column(db, table_name, column_name, option_name);
   assert( n_in_table == option_name.size() );
   //
   // option_value
   column_name = "option_value";
   CppAD::vector<string>  option_value;
   get_table_column(db, table_name, column_name, option_value);
   assert( n_in_table == option_value.size() );
   //
   // values in table
   size_t  derivative_test_fixed_level = 0;
   bool    quasi_fixed                 = true;
   bool    other_database              = false;
   bool    other_input_table           = false;
   for(size_t option_id = 0; option_id < n_in_table; option_id++)
   {  // option_value_split
      CppAD::vector<string> option_value_split = split_space(
         option_value[option_id]
      );
      // minimum_meas_cv
      if( option_name[option_id] == "minimum_meas_cv" )
      {  msg  = "minimum_meas_cv is no longer a valid option name";
         msg += "\nThis was moved to integrand table on 2018-05-23.";
         error_exit(msg, table_name, option_id);
      }
      // meas_std_effect
      if( option_name[option_id] == "meas_std_effect" )
      {  msg  = "meas_std_effect was deprecated on 2019-04-07\n";
         msg += "and removed on 2019-11-26.\n";
         msg += "It should be changed to meas_noise_effect.";
         error_exit(msg, table_name, option_id);
      }
      // zero_sum_random
      if( option_name[option_id] == "zero_sum_random" )
      {  option_name[option_id] = "zero_sum_child_rate";
         msg  = "zero_sum_random was changed to zero_sum_child_rate\n";
         msg += "on 2019-11-29 (you should fix this).\n";
         msg += "For the time being, this change is automatic.";
         string msg_type  = "warning";
         log_message(db, &std::cerr, msg_type, msg, table_name, option_id);
      }
      //
      size_t match = n_option;
      for(size_t i = 0; i < n_option; i++)
         if( name_vec[i] == option_name[option_id] )
            match = i;
      if( match == n_option )
      {  msg  =  option_name[option_id];
         msg += " is not a valid option_name";
         error_exit(msg, table_name, option_id);
      }
      value_vec[match] = option_value[option_id];
      //
      // hold_out_integrand
      if( name_vec[match] == "hold_out_integrand" )
      {  const CppAD::vector<string>& integrand_list = option_value_split;
         for(size_t i = 0; i < integrand_list.size(); ++i)
         {  string name  = integrand_list[i];
            bool   found = false;
            for(size_t j = 0; j < number_integrand_enum; ++j)
            {  if( name == integrand_enum2name[j] && name != "mulcov" )
                  found = true;
            }
            if( ! found )
            {  msg = name_vec[match] + ": invalid interand_name = '";
               msg += name + "'";
               error_exit(msg, table_name, option_id);
            }
         }
      }
      // mes_noise_effect
      if( name_vec[match] == "meas_noise_effect" )
      {  bool ok = false;
         ok     |= option_value[option_id] == "add_std_scale_all";
         ok     |= option_value[option_id] == "add_std_scale_none";
         //
         ok     |= option_value[option_id] == "add_var_scale_all";
         ok     |= option_value[option_id] == "add_var_scale_none";
         if( ! ok )
         {  msg = "meas_noise_effect = '";
            msg += option_value[option_id] + "'";
            error_exit(msg, table_name, option_id);
         }
      }
      // rate_case
      if( name_vec[match] == "rate_case" )
      {  bool ok = false;
         ok     |= option_value[option_id] == "no_ode";
         ok     |= option_value[option_id] == "trapezoidal";
         ok     |= option_value[option_id] == "iota_pos_rho_zero";
         ok     |= option_value[option_id] == "iota_zero_rho_pos";
         ok     |= option_value[option_id] == "iota_zero_rho_zero";
         ok     |= option_value[option_id] == "iota_pos_rho_pos";
         if( ! ok )
         {  msg = "rate_case = '";
            msg += option_value[option_id] + "'";
            error_exit(msg, table_name, option_id);
         }
      }
      // ode_step_size
      if( name_vec[match] == "ode_step_size" )
      {  bool ok = std::atof( option_value[option_id].c_str() ) > 0.0;
         if( ! ok )
         {  msg = "ode_step_size is <= 0.0";
            error_exit(msg, table_name, option_id);
         }
      }
      // random_seed
      if( name_vec[match] == "random_seed" )
      {  bool ok = std::atoi( option_value[option_id].c_str() ) >= 0;
         if( ! ok )
         {  msg = "random_seed is < 0";
            error_exit(msg, table_name, option_id);
         }
      }
      // quasi_fixed
      if( name_vec[match] == "quasi_fixed" )
      {  if( option_value[option_id] == "true" )
            quasi_fixed = true;
         else if( option_value[option_id] == "false" )
            quasi_fixed = false;
         else
         {  msg = "quasi_fixed is not true or false";
            error_exit(msg, table_name, option_id);
         }
      }
      // warn_on_stderr
      if( name_vec[match] == "warn_on_stderr" )
      {  if(
            option_value[option_id] != "true" &&
            option_value[option_id] != "false" )
         {  msg = "warn_on_stderr is not true or false";
            error_exit(msg, table_name, option_id);
         }
      }
      // trace_init_fit_model
      if( name_vec[match] == "trace_init_fit_model" )
      {  if(
            option_value[option_id] != "true" &&
            option_value[option_id] != "false" )
         {  msg = "trace_init_fit_model is not true or false";
            error_exit(msg, table_name, option_id);
         }
      }
      // tolerance_fixed
      // tolerance_random
      if(
         name_vec[match] == "tolerance_fixed" ||
         name_vec[match] == "tolerance_random"
      )
      {  double tolerance = std::atof( option_value[option_id].c_str() );
         bool ok = 0.0 < tolerance;
         if( ! ok )
         {  msg = name_vec[match] + " is not greater than zero";
            error_exit(msg, table_name, option_id);
         }
      }
      // bound_random
      if( name_vec[match] == "bound_random" )
      {  std::string bound_random_str = option_value[option_id];
         if( bound_random_str != "" )
         {  double bound_random = std::atof( bound_random_str.c_str() );
            bool ok = 0.0 <= bound_random;
            if( ! ok )
            {  msg = "bound_random is less than zero.";
               error_exit(msg, table_name, option_id);
            }
         }
      }
      // max_num_iter_fixed
      // max_num_iter_random
      if(
         name_vec[match] == "max_num_iter_fixed" ||
         name_vec[match] == "max_num_iter_random"
      )
      {  int max_num_iter = std::atoi( option_value[option_id].c_str() );
         bool ok = -1 <= max_num_iter;
         if( ! ok )
         {  msg = name_vec[match] + " is less than minus one";
            error_exit(msg, table_name, option_id);
         }
      }
      // print_level_fixed
      // print_level_random
      if(
         name_vec[match] == "print_level_fixed" ||
         name_vec[match] == "print_level_random"
      )
      {  int print_level = std::atoi( option_value[option_id].c_str() );
         bool ok = 0 <= print_level && print_level <= 12;
         if( ! ok )
         {  msg = name_vec[match] + " is not between 0 and 12 inclusive";
            error_exit(msg, table_name, option_id);
         }
      }
      // accept_after_max_steps_fixed
      // accept_after_max_steps_random
      // limited_memory_max_history_fixed
      if(
         name_vec[match] == "accept_after_max_steps_fixed"   ||
         name_vec[match] == "accept_after_max_steps_random"  ||
         name_vec[match] == "limited_memory_max_history_fixed"
      )
      {  int pos_integer = std::atoi( option_value[option_id].c_str() );
         bool ok = 0 < pos_integer;
         if( ! ok )
         {  msg = name_vec[match] + " is not a positive integer";
            error_exit(msg, table_name, option_id);
         }
      }
      // derivative_test_fixed
      // derivative_test_random
      if(
         name_vec[match] == "derivative_test_fixed" ||
         name_vec[match] == "derivative_test_random"
      )
      {  bool ok = false;
         ok     |= option_value[option_id] == "none";
         ok     |= option_value[option_id] == "first-order";
         ok     |= option_value[option_id] == "second-order";
         ok     |= option_value[option_id] == "only-second-order";
         ok     |= option_value[option_id] == "adaptive";
         ok     |= option_value[option_id] == "trace-adaptive";
         if( ! ok )
         {  msg  = name_vec[match] + " is not one of the following:\n";
            msg += "none, first-order, second-order, only-second-order\n";
            msg += "adaptive, trace-adaptive";
            error_exit(msg, table_name, option_id);
         }
         if( name_vec[match] == "derivative_test_fixed" )
         {  if( option_value[option_id] == "none" )
               derivative_test_fixed_level = 0;
            else if( option_value[option_id] == "second-order" )
               derivative_test_fixed_level = 2;
            else if( option_value[option_id] == "only-second-order" )
               derivative_test_fixed_level = 2;
            else
               derivative_test_fixed_level = 1;
         }
      }
      // zero_sum_child_rate
      if( name_vec[match] == "zero_sum_child_rate" )
      {  const CppAD::vector<string>& rate_list = option_value_split;
         size_t n_rate = number_rate_enum;
         for(size_t i = 0; i < rate_list.size(); i++)
         {  string rate_name = rate_list[i];
            bool found = false;
            for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
               found = found || rate_name == get_rate_name(rate_id);
            if( ! found )
            {  msg  = "zero_sum_child_rate: " + rate_name;
               msg += " is not a valid rate name";
               error_exit(msg, table_name, option_id);
            }
         }
      }
      // method_random
      if( name_vec[match] == "method_random" )
      {  if( option_value[option_id] != "ipopt_solve"  &&
             option_value[option_id] != "ipopt_random"
         )
         {  msg  = "method_random is not ";
            msg += "ipopt_solve or ipopt_random";
            error_exit(msg, table_name, option_id);
         }
      }
      if( name_vec[match] == "compress_interval" )
      {  if( option_value_split.size() != 2 )
         {  msg = "compress_interval must be two space separated elements.";
            error_exit(msg, table_name, option_id);
         }
         double age_size = std::atof( option_value_split[0].c_str() );
         if( age_size < 0.0 )
         {  msg = "compress_interval age_size is less than zero.";
            error_exit(msg, table_name, option_id);
         }
         double time_size = std::atof( option_value_split[1].c_str() );
         if( time_size < 0.0 )
         {  msg = "compress_interval time_size is less than zero.";
            error_exit(msg, table_name, option_id);
         }
      }
      // other_database
      if( name_vec[match] == "other_database" )
         other_database = true;
      //
      // other_input_table
      if( name_vec[match] == "other_input_table" )
      {  size_t size = sizeof(input_table_list) / sizeof(input_table_list[0]);
         other_input_table = 0 < size;
         for(size_t i = 0; i < option_value_split.size(); ++i)
         {  const string& table_tmp = option_value_split[i];
            if( table_tmp == "option" )
            {  msg  = table_tmp + " table name cannot appear in the ";
               msg  += "other_input_table list";
               error_exit(msg, table_tmp, option_id);
            }
            bool  found = false;
            for(size_t j = 0; j < input_table_list_size; ++j)
               found = found || table_tmp == input_table_list[j];
            if( ! found )
            {  msg  = table_tmp + " in option_value list";
               msg += " is not a valid input table name";
               error_exit(msg, table_name, option_id);
            }
         }
      }
   }
   if( quasi_fixed && (derivative_test_fixed_level > 1 ) )
   {  msg  = "quasi_fixed option is true and derivative_test_fixed";
      msg += " is second-order or only-second-order";
      error_exit(msg, table_name);
   }
   if( other_input_table && ! other_database )
   {  msg  = "other_input_table option is non-empty and other_database";
      msg += " is not specified";
      error_exit(msg, table_name);
   }
   //
   // return table
   CppAD::vector<option_struct> option_table(n_option);
   for(size_t i = 0; i < n_option; i++)
   {  option_table[i].option_name  = name_vec[i];
      option_table[i].option_value = value_vec[i];
   }
   return option_table;
}

} // END DISMOD_AT_NAMESPACE
