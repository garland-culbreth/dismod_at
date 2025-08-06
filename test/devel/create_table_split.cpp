// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
Check that create table commands with split insertions execute properly.
*/
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/null_int.hpp>

namespace { // BEGIN EMPTY_NAMESPACE

// put bool on its own line so that test_one.sh works correctly
bool
test_create_table_split(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   string table_name = "age";
   size_t n_col      = 1;
   size_t n_row      = 5;
   string col_name   = "age";
   string col_type   = "real";
   bool   col_unique = false;
   vector<string> row_value(n_col * n_subset);
   size_t cut_size   = 3;

// for(size_t k = 0; k < n_row; ++k)
   for(size_t k = 0; k < n_row; k++)
   {  
      row_value[n_col * k] = to_string(k); 
   }
   create_table(
      db, table_name, col_name, col_type, col_unique, row_value, cut_size
   );

   // get age table
   vector<double> age_table = dismod_at::get_age_table(db);

   std::cout << "age table: " << age_table << std::endl;

   ok  &= age_table[ 0 ]   == {0, 0},
   ok  &= age_table[ 1 ]   == {1, 1},
   ok  &= age_table[ 2 ]   == {2, 2},
   ok  &= age_table[ 3 ]   == {3, 3},
   ok  &= age_table[ 4 ]   == {4, 4},

   return ok;
} // END EMPTY_NAMESPACE


bool create_table_split(void)
{  bool ok = true;
   ok &= test_create_table_split();
   return ok;
}
