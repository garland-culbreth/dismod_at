# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2013-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin data_table.py$$ $newlinech #$$
#
# $section data_table: Example and Text$$
#
# $index data, , example$$
# $index example, data table$$
# $index table, data example$$
#
# $code
# $verbatim%example/data_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def data_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	# required columns
	col_name2type  = dict()
	col_name2type['data_id']       = 'integer primary key'
	col_name2type['integrand_id']  = 'integer'
	col_name2type['likelihood_id'] = 'integer'
	col_name2type['location_id']   = 'integer'
	col_name2type['meas_value']    = 'real'
	col_name2type['meas_stdev']    = 'real'
	col_name2type['age_lower']     = 'real'
	col_name2type['age_upper']     = 'real'
	col_name2type['time_lower']    = 'real'
	col_name2type['time_upper']    = 'real'
	#
	# covariates
	col_name2type['c_sex']         = 'real'
	col_name2type['c_income']      = 'real'
	#
	# create the table
	tbl_name = 'data'
	dismod_at.create_table(connection, tbl_name, col_name2type)
	#
	# now pretend like we do not know all the columns in the table
	# nor the order of the table columns.
	col_name2type = dismod_at.get_name2type(connection, tbl_name)
	#
	index         = 0
	col_name      = col_name2type.keys()
	for i in range(len(col_name)) :
		col_name[i] = str(col_name[i])
	col_value     = list()
	for name in col_name2type :
		if name == 'data_id':
			# primary key
			col_value.append('null')
		elif col_name2type[name] == 'integer' :
			col_value.append(index)
		else :
			col_value.append(index + 0.5 )
		index += 1
	col_name  = str( tuple( col_name ) )
	col_value = str( tuple( col_value ) )
	#
	col_name  = col_name.replace("'", "")
	col_value = col_value.replace("'", "")
	cmd       = 'insert into data ' + col_name + ' values ' + col_value + ';'
	#
	cursor     = connection.cursor()
	cursor.execute(cmd)
	#
	# check values in table
	for row in cursor.execute('select * from data') :
		index = 0
		for name in col_name2type :
			if name == 'data_id' :
				# primary key starts at one, but where is this specified ?
				assert row[index] == 1 
			elif col_name2type[name] == 'integer' :
				assert row[index] == index
			else :
				assert col_name2type[name] == 'real'
				assert row[index] == index + 0.5
			index += 1
	print('data_table: OK')
# END PYTHON
