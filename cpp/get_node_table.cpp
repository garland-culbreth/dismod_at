/*
$begin get_node_table$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Node Table Information$$
$index get, node table$$
$index node, get table$$
$index table, get node$$

$head Syntax$$
$codei%# include <get_node_table>
%$$
$icode%node_table% = get_node_table(%db%)%$$

$head Purpose$$
To read the $cref node_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head node_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description 
$rnext
$code std::string$$ $cnext $code node_name$$  $cnext 
	The $cref/node_name/node_table/node_name/$$ for this node  
$rnext
$code int$$ $cnext $code parent$$  $cnext 
	The $cref/parent/node_table/parent/$$ $icode node_id$$
$tend        

$head node_table$$
The return value $icode node_table$$ has prototype
$codei%
	CppAD::vector<node_struct>  %node_table%
%$$
For each $cref/node_id/node_table/node_id/$$,
$codei%
	%node_table%[%node_id%]
%$$
is the information for the corresponding node.

$children%example/cpp/get_node_table_xam.cpp
%$$
$head Example$$
The file $cref get_node_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_node_table.hpp>
# include <dismod_at/table_error_exit.hpp>


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<node_struct> get_node_table(sqlite3* db)
{	using std::string;

	string table_name  = "node";
	string column_name = "node_id";
	CppAD::vector<int>    node_id;
	get_table_column(db, table_name, column_name, node_id);

	column_name        =  "node_name";
	CppAD::vector<string>  node_name;
	get_table_column(db, table_name, column_name, node_name);

	column_name        =  "parent";
	CppAD::vector<int>     parent;
	get_table_column(db, table_name, column_name, parent);

	size_t n_node = node_id.size();
	assert( n_node == node_name.size() );
	assert( n_node == parent.size() );

	CppAD::vector<node_struct> node_table(n_node);
	for(size_t i = 0; i < n_node; i++)
	{	if( node_id[i] != i )
		{	string s = "node_id must start at zero and increment by one.";
			table_error_exit("node", i, s);
		}
		node_table[i].node_name  = node_name[i];
		node_table[i].parent     = parent[i];
	}
	return node_table;
}

} // END DISMOD_AT_NAMESPACE
