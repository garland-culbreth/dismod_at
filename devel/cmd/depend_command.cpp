// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-25 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <dismod_at/depend_command.hpp>
# include <dismod_at/depend.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
{xrst_begin depend_command}

The Depend Command
##################

Syntax
******
``dismod_at`` *database* ``depend``

database
********
Is an
`sqlite <https://sqlite.org/index.html>`_ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.

depend_var_table
****************
A new :ref:`depend_var_table-name` is created with the information
that maps a :ref:`var_table@var_id`
to its meaning in terms of the
:ref:`model_variables-name` .
{xrst_toc_hidden
   example/get_started/depend_command.py
}
Example
*******
The file :ref:`depend_command.py-name` contains an example and test
using this command.

{xrst_end depend_command}
*/
void depend_command(
   sqlite3*                                      db               ,
   const CppAD::vector<double>&                  prior_mean       ,
   data_model&                                   data_object      ,
   const CppAD::vector<subset_data_struct>&      subset_data_obj  ,
   const prior_model&                            prior_object     )
{  using std::string;
   using CppAD::vector;
   //
   string table_name = "depend_var";
   //
   data_object.replace_like(subset_data_obj);
   //
   // compute the dependencies
   vector<bool> data_depend_vec  =
      data_depend(prior_mean, data_object);
   vector<bool> prior_depend_vec =
      prior_depend(prior_mean, prior_object);
   //
   // delete old version of depend_var table
   string sql_cmd = "drop table if exists ";
   sql_cmd       += table_name;
   exec_sql_cmd(db, sql_cmd);
   //
   // create new depend_var table
   size_t n_var      = prior_mean.size();
   vector<string> col_name(2), col_type(2), row_value(2 * n_var);
   vector<bool>   col_unique(2);
   col_name[0]       = "data_depend";
   col_type[0]       = "integer";
   col_unique[0]     = false;
   col_name[1]       = "prior_depend";
   col_type[1]       = "integer";
   col_unique[1]     = false;
   for(size_t i = 0; i < n_var; i++)
   {  // values in i-th row
      //
      // data_depend
      if( data_depend_vec[i]  )
         row_value[i * 2 + 0] = "1";
      else
         row_value[i * 2 + 0] = "0";
      //
      // prior_depend
      if( prior_depend_vec[i]  )
         row_value[i * 2 + 1] = "1";
      else
         row_value[i * 2 + 1] = "0";
   }
   create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   return;
}

} // END_DISMOD_AT_NAMESPACE
