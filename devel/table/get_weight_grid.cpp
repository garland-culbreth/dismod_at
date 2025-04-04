// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-24 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_weight_grid dev}

C++: Get the Weight Grid Information
####################################

Syntax
******
*weight_grid* = ``get_weight_grid`` ( *db* , *data_table*, *avgint_table* )

Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

Purpose
*******
To read the :ref:`weight_grid_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

data_table
**********
is the :ref:`get_data_table@data_table`
(used to check its weights are positive).

avgint_table
************
is the :ref:`get_avgint_table@avgint_table`
(used to check its weights are positive).

weight_grid_struct
******************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``int``
     - ``weight_id``
     - The :ref:`weight_grid_table@weight_id` for this weighting.
   * - ``int``
     - ``age_id``
     - The :ref:`weight_grid_table@age_id` for this weight value
   * - ``int``
     - ``time_id``
     - The :ref:`weight_grid_table@time_id` for this weight value
   * - ``double``
     - ``weight``
     - The :ref:`weight_grid_table@weight` value

weight_grid
***********
The return value *weight_grid* has prototype

   ``CppAD::vector<weight_grid_struct>`` *weight_grid*

For each :ref:`weight_grid_table@weight_grid_id` ,

   *weight_grid* [ *weight_grid_id* ]

is the information for a specific weighting and specific age and time.
{xrst_toc_hidden
   example/devel/table/get_weight_grid_xam.cpp
}
Example
*******
The file :ref:`get_weight_grid_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_weight_grid}
-----------------------------------------------------------------------------
*/

# include <cmath>
# include <set>
# include <cppad/utility/to_string.hpp>

# include <dismod_at/get_weight_grid.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<weight_grid_struct> get_weight_grid(
   sqlite3*                            db           ,
   const CppAD::vector<data_struct>&   data_table   ,
   const CppAD::vector<avgint_struct>& avgint_table )
// END_PROTOTYPE
{  using std::string;
   //
   // data_weight_set
   std::set<int> data_weight_set;
   for(size_t data_id = 0; data_id < data_table.size(); ++data_id)
      data_weight_set.insert( data_table[data_id].weight_id );
   //
   // avgint_weight_set
   std::set<int> avgint_weight_set;
   for(size_t avgint_id = 0; avgint_id < avgint_table.size(); ++avgint_id)
      avgint_weight_set.insert( avgint_table[avgint_id].weight_id );
   //
   // itr
   std::set<int>::const_iterator itr;

   string table_name  = "weight_grid";
   size_t n_grid      = check_table_id(db, table_name);

   string column_name =  "weight_id";
   CppAD::vector<int>     weight_id;
   get_table_column(db, table_name, column_name, weight_id);
   assert( weight_id.size() == n_grid );

   column_name        =  "age_id";
   CppAD::vector<int>     age_id;
   get_table_column(db, table_name, column_name, age_id);
   assert( age_id.size() == n_grid );

   column_name        =  "time_id";
   CppAD::vector<int>     time_id;
   get_table_column(db, table_name, column_name, time_id);
   assert( time_id.size() == n_grid );

   column_name        =  "weight";
   CppAD::vector<double>  weight;
   get_table_column(db, table_name, column_name, weight);
   assert( weight.size() == n_grid );

   CppAD::vector<weight_grid_struct> weight_grid(n_grid);
   for(size_t i = 0; i < n_grid; i++)
   {  weight_grid[i].weight_id = weight_id[i];
      weight_grid[i].age_id    = age_id[i];
      weight_grid[i].time_id   = time_id[i];
      weight_grid[i].weight    = weight[i];
      if( weight[i] <= 0.0 )
      {  bool weight_error = false;
         //
         // data_id
         size_t data_id   = DISMOD_AT_NULL_SIZE_T;
         itr = data_weight_set.find( weight_id[i] );
         if( itr != data_weight_set.end() )
         {  for(size_t id = 0; id < data_table.size(); ++id)
               if( data_table[id].weight_id == weight_id[i] )
                     data_id = id;
            assert( data_id != DISMOD_AT_NULL_SIZE_T );
            weight_error = true;
         }
         //
         // avgint_id
         size_t avgint_id   = DISMOD_AT_NULL_SIZE_T;
         itr = avgint_weight_set.find( weight_id[i] );
         if( itr != avgint_weight_set.end() )
         {  for(size_t id = 0; id < avgint_table.size(); ++id)
               if( avgint_table[id].weight_id == weight_id[i] )
                     avgint_id = id;
            assert( avgint_id != DISMOD_AT_NULL_SIZE_T );
            weight_error = true;
         }
         if( weight_error )
         {  string msg = "\nweight_grid_table";
            msg += ": weight_grid_id = " + CppAD::to_string(i);
            msg += ", weight_id = " + CppAD::to_string( weight_id[i] );
            msg += ", weight = " + CppAD::to_string( weight[i] );
            msg += "\nThis weight is <= 0 and appears in data or avgint table";
            if( data_id != DISMOD_AT_NULL_SIZE_T )
            {  msg += "\ndata_table";
               msg += ": data_id = " + CppAD::to_string(data_id);
               msg += ", weight_id = " +  CppAD::to_string( weight_id[i] );
            }
            if( avgint_id != DISMOD_AT_NULL_SIZE_T )
            {  msg += "\navgint_table";
               msg += ": avgint_id = " + CppAD::to_string(avgint_id);
               msg += ", weight_id = " +  CppAD::to_string( weight_id[i] );
            }
            error_exit(msg, table_name, i);
         }
      }
   }
   return weight_grid;
}

} // END DISMOD_AT_NAMESPACE
