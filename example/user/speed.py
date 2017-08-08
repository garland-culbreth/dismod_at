# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_speed.py$$ $newlinech #$$
# $spell
#	py
#	init
# $$
#
# $section A Simulate Data Speed Test$$
#
# $head Syntax$$
# $icode%python3% example/user/speed.py %random_seed%$$
#
# $head python3$$
# This is the $cref/python3_executable/run_cmake.sh/python3_executable/$$
# on your system.
#
# $head random_seed$$
# Is the $cref/random_seed/option_table/random_seed/$$ used during the
# simulation.
#
# $head Commands$$
# init, simulate, start, fit
#
# $code
# $srcfile%
#	example/user/speed.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
iota_parent               = 0.05
rho_parent                = 0.2
mulcov_income_iota_true   = 1.0
mulcov_sex_rho_true       = -1.0
n_children                = 10
n_data                    = 200
# ------------------------------------------------------------------------
import sys
import os
import time
import distutils.dir_util
import subprocess
test_program = 'example/user/speed.py'
if sys.argv[0] != test_program  or len(sys.argv) != 2 :
	usage  = 'python3 ' + test_program + ' random_seed\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
random_seed_arg = sys.argv[1]
start_time      = time.time();
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/speed directory
distutils.dir_util.mkpath('build/speed')
os.chdir('build/speed')
# ------------------------------------------------------------------------
def constant_weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_iota_parent(a, t) :
	return ('prior_iota_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_rho_parent(a, t) :
	return ('prior_rho_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_mulcov(a, t) :
	return ('prior_mulcov', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	import copy
	import dismod_at
	import math
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
	#
	# integrand table:
	integrand_list = [
		'Sincidence',
		'prevalence'
	]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	for i in range(n_children) :
		name = 'child_' + str(i + 1)
		node_table.append( { 'name':name, 'parent':'world' } )
	#
	# weight table:
	# The constant function 1.0, note any valid age and time id would work
	name        = 'constant'
	fun         = constant_weight_fun
	age_mid_id  = int( len(age_list) / 2 )
	time_mid_id = int( len(time_list) / 2 )
	weight_table = [ {
		'name':name, 'age_id':[age_mid_id], 'time_id':[time_mid_id], 'fun':fun
	} ]
	#
	# covariate table:
	covariate_table = [
		{'name':'income', 'reference':0.5, 'max_difference':None},
		{'name':'sex',    'reference':0.0, 'max_difference':0.6}
	]
	#
	# mulcov table:
	# income has been scaled the same as sex so man use same smoothing
	mulcov_table = [
		{
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_mulcov'
		},{
			'covariate': 'sex',
			'type':      'rate_value',
			'effected':  'rho',
			'smooth':    'smooth_mulcov'
		}
	]
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	# values that are the same for all data rows
	row = {
		'meas_value':  0.0,             # not used (will be simulated)
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'age_lower':    50.,
		'age_upper':    50.,
		'time_lower':   2000.,
		'time_upper':   2000.
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		row['node']      = 'child_' + str( (data_id % n_children) + 1 )
		row['income']    = data_id / float(n_data-1)
		row['sex']       = ( data_id % 3 - 1.0 ) / 2.0
		row['integrand'] = integrand_list[ data_id % 2 ]
		if row['integrand'] == 'Sincidence' :
			row['meas_std']  = 0.05 * iota_parent
			row['eta']       = None;
		elif row['integrand'] == 'prevalence' :
			row['meas_std']  = 0.05 * (iota_parent / rho_parent)
			row['eta']       = None;
		else :
			assert(False)
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      0.01,
			'eta':      None
		},{ # prior_loggauss_zero
			'name':     'prior_loggauss_zero',
			'density':  'log_gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      0.1,
			'eta':      1e-6
		},{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    0.001,
			'upper':    1.0,
			'mean':     0.1,
			'std':      None,
			'eta':      None
		},{ # prior_iota_parent
			'name':     'prior_rho_parent',
			'density':  'uniform',
			'lower':    0.001,
			'upper':    1.0,
			'mean':     0.1,
			'std':      None,
			'eta':      None
		},{ # prior_mulcov
			'name':     'prior_mulcov',
			'density':  'uniform',
			'lower':    -2.0,
			'upper':    +2.0,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	name           = 'smooth_rate_child'
	fun            = fun_rate_child
	age_grid       = [ 0, age_mid_id, len(age_list)-1 ]
	time_grid      = [ 0, time_mid_id, len(time_list)-1 ]
	smooth_table = [
		{'name':name, 'age_id':age_grid, 'time_id':time_grid, 'fun':fun }
	]
	name = 'smooth_iota_parent'
	fun  = fun_iota_parent
	smooth_table.append( {
		'name':name, 'age_id':[age_mid_id], 'time_id':[time_mid_id], 'fun':fun
	} )
	name = 'smooth_rho_parent'
	fun  = fun_rho_parent
	smooth_table.append( {
		'name':name, 'age_id':[age_mid_id], 'time_id':[time_mid_id], 'fun':fun
	} )
	name = 'smooth_mulcov'
	fun  = fun_mulcov
	smooth_table.append( {
		'name':name, 'age_id':[age_mid_id], 'time_id':[time_mid_id], 'fun':fun
	} )
	# no standard deviation multipliers
	for dictionary in smooth_table :
		for name in [ 'value' , 'dage', 'dtime' ] :
			key   = 'mulstd_' + name + '_prior_name'
			value = None
			dictionary[key] = value
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'pini',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_rate_child',
			'child_nslist':  None
		},{	'name':          'rho',
			'parent_smooth': 'smooth_rho_parent',
			'child_smooth':  'smooth_rate_child',
			'child_nslist':  None
		},{	'name':          'chi',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'omega',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos' },
		{ 'name':'parent_node_name',       'value':'world'            },
		{ 'name':'ode_step_size',          'value':'10.0'             },
		{ 'name':'random_seed',            'value':random_seed_arg    },

		{ 'name':'quasi_fixed',            'value':'true'             },
		{ 'name':'derivative_test_fixed',  'value':'none'             },
		{ 'name':'max_num_iter_fixed',     'value':'100'              },
		{ 'name':'print_level_fixed',      'value':'5'                },
		{ 'name':'tolerance_fixed',        'value':'1e-8'             },
		{ 'name':'accept_after_max_steps_fixed',  'value':'2'         },

		{ 'name':'derivative_test_random', 'value':'none'             },
		{ 'name':'max_num_iter_random',    'value':'100'              },
		{ 'name':'print_level_random',     'value':'0'                },
		{ 'name':'tolerance_random',       'value':'1e-8'             }
	]
	# ----------------------------------------------------------------------
	# avgint table: empty
	avgint_table = list()
	# ----------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_list,
		node_table,
		weight_table,
		covariate_table,
		avgint_table,
		data_table,
		prior_table,
		smooth_table,
		nslist_table,
		rate_table,
		mulcov_table,
		option_table
	)
	# ----------------------------------------------------------------------
	return
# ===========================================================================
# Run the init command to create the var table
file_name      = 'example.db'
example_db(file_name)
program        = '../devel/dismod_at'
cmd            = [ program, file_name, 'init' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
# -----------------------------------------------------------------------
# read database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table        = dismod_at.get_table_dict(connection, 'var')
rate_table     = dismod_at.get_table_dict(connection, 'rate')
covariate_table= dismod_at.get_table_dict(connection, 'covariate')
# -----------------------------------------------------------------------
# truth table:
tbl_name     = 'truth_var'
col_name     = [ 'truth_var_value' ]
col_type     = [ 'real' ]
row_list     = list()
var_id2true  = list()
for var_id in range( len(var_table) ) :
	var_info        = var_table[var_id]
	truth_var_value = None
	var_type = var_info['var_type']
	if var_type == 'mulcov_rate_value' :
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		if rate_name == 'iota' :
			covariate_id   = var_info['covariate_id']
			covariate_name = covariate_table[covariate_id]['covariate_name' ]
			assert( covariate_name == 'income' )
			truth_var_value = mulcov_income_iota_true
		else :
			assert( rate_name == 'rho' )
			covariate_id   = var_info['covariate_id']
			covariate_name = covariate_table[covariate_id]['covariate_name' ]
			assert( covariate_name == 'sex' )
			truth_var_value = mulcov_sex_rho_true
	else :
		assert( var_type == 'rate' )
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		node_id   = var_info['node_id']
		# node zero is the world
		if node_id == 0 and rate_name == 'iota' :
			truth_var_value = iota_parent
		elif node_id == 0 and rate_name == 'rho' :
			truth_var_value = rho_parent
		else :
			truth_var_value = 0.0
	var_id2true.append( truth_var_value )
	row_list.append( [ truth_var_value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# Run the simulate and sample commands
for command in [ 'simulate', 'start', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'simulate' :
		number_simulate = '1'
		cmd.append(number_simulate)
	if command == 'start' :
		cmd.append('prior_mean')
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	if command == 'fit' :
		simulate_index = '0'
		cmd.append(simulate_index)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# check simulation results
new          = False
connection   = dismod_at.create_connection(file_name, new)
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
log_dict     = dismod_at.get_table_dict(connection, 'log')
connection.close()
# -----------------------------------------------------------------------
# determine random seed the was used
random_seed = int(random_seed_arg)
if random_seed == 0 :
	for row in log_dict :
		if row['message_type'] == 'command' :
			message = row['message'].split()
			if message[0] == 'begin' and message[1] == 'simulate' :
				random_seed = int(row['unix_time'])
	assert random_seed != 0
# -----------------------------------------------------------------------
number_variable = len(var_table)
assert( len(fit_var_table) == number_variable )
max_error       = 0.0
for var_id in range( number_variable ) :
	row        = fit_var_table[var_id]
	fit_value  = row['variable_value']
	true_value = var_id2true[var_id]
	if true_value == 0.0 :
		max_error = max(abs(fit_value) , max_error)
	else :
		max_error = max( abs(fit_value / true_value - 1.0), max_error)
print('elapsed seconds =', time.time() - start_time)
print('random_seed = ', random_seed)
if max_error > 5e-2 :
	print('max_error = ', max_error)
	print('simulated.py: Error')
	assert(False)
# -----------------------------------------------------------------------------
print('simulated.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON