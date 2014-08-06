// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_rate_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_rate_table: Example and Test$$
$index example, C++ get_rate_table$$
$index get_rate_table, C++ example$$

$code
$verbatim%example/devel/table/get_rate_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/include/get_rate_table.hpp>
# include <dismod_at/include/exec_sql_cmd.hpp>
# include <dismod_at/include/open_connection.hpp>

bool get_rate_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;	

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = { 
		"create table rate("
			"rate_id          integer primary key,"
			"rate_name        text,"
			"parent_smooth_id int,"
			"child_smooth_id  int)",
		"insert into rate values(0, 'iota',  0, 1)",
		"insert into rate values(1, 'rho',   0, 1)",
		"insert into rate values(2, 'chi',   0, 1)",
		"insert into rate values(3, 'omega', 0, 1)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the rate table
	vector<dismod_at::rate_struct> rate_table = dismod_at::get_rate_table(db);
	ok  &= rate_table.size() == 4;
	for(size_t rate_id = 0; rate_id < 4; rate_id++)
	{	ok &= rate_table[rate_id].parent_smooth_id == 0;
		ok &= rate_table[rate_id].child_smooth_id  == 1;
	}
 
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
