# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin fit_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at fit: Example and Test$$
#
# $code
# $verbatim%
#	example/get_started/fit_command.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import subprocess
import distutils.dir_util
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/fit_command.py'
print(sys.argv)
if sys.argv[0] != example  or len(sys.argv) != 1 :
	usage  = 'python3 ' + example + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# add python subdirectoy to python path
sys.path.append( os.getcwd() + '/python' )
#
# import dismod_at routine
import dismod_at
#
# change into the build/example/get_started directory
distutils.dir_util.mkpath('build/exmaple/get_started')
os.chdir('build/exmaple/get_started')
# ---------------------------------------------------------------------------
def constant_weight_fun(a, t) :
	return 1.0
def gaussian_zero_fun(a, t) :
	# note that the a, t values are not used
	return ('prior_mean_zero', 'prior_mean_zero', 'prior_mean_zero')
def uniform_positive_fun(a, t) :
	# note that the a, t values are not used
	return ('prior_uniform_positive', 'prior_mean_zero', 'prior_mean_zero')
# ------------------------------------------------------------------------
# name of the database
file_name   = 'example.db'
#
# age table
age_list    = [    0.0, 50.0,    100.0 ]
#
# time table
time_list   = [ 1995.0, 2005.0, 2015.0 ]
#
# integrand table
integrand_list = [
	{ 'name':'prevalence',  'eta':1e-6 },
	{ 'name':'Sincidence',  'eta':1e-6 },
	{ 'name':'remission',   'eta':1e-6 },
	{ 'name':'mtexcess',    'eta':1e-6 },
	{ 'name':'mtother',     'eta':1e-6 }
]
#
# node table: world -> north_america
#             north_america -> (united_states, canada)
node_list = [
	{ 'name':'world',         'parent':'' },
	{ 'name':'north_america', 'parent':'world' },
	{ 'name':'united_states', 'parent':'north_america' },
	{ 'name':'canada',        'parent':'north_america' }
]
#
# weight table: The constant function 1.0 (one age and one time point)
fun = constant_weight_fun
weight_list = [
	{ 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
]
#
# covariate table: no covriates
covariate_list = list()
#
# mulcov table
mulcov_list = list()
# --------------------------------------------------------------------------
# data table: same order as list of integrands
data_list = list()
# values that are the same for all data rows
row = {
	'node':        'world',
	'density':     'gaussian',
	'weight':      'constant',
	'time_lower':   2000.0,
	'time_upper':   2000.0,
	'age_lower':    0.0
}
# values that change between rows: (one data point for each integrand)
for data_id in range( len(integrand_list) ) :
	meas_value        = 1e-2 * (data_id + 1)
	meas_std          = 0.2 * meas_value
	integrand         = integrand_list[data_id]['name']
	row['meas_value'] = meas_value
	row['meas_std']   = meas_std
	row['integrand']  = integrand
	if integrand == 'prevalence' :
		# prevalence is measured at age zero
		row['age_upper'] = 0.0
	else :
		# other integrands are averaged from age zero to one hundred
		row['age_upper'] = 100.0
	data_list.append( copy.copy(row) )
# --------------------------------------------------------------------------
# prior_table
prior_list = [
	{   # prior_zero
		'name':     'prior_zero',
		'density':  'uniform',
		'lower':    0.0,
		'upper':    0.0,
		'mean':     0.0,
		'std':      None,
		'eta':      None
	},{ # prior_one
		'name':     'prior_one',
		'density':  'uniform',
		'lower':    1.0,
		'upper':    1.0,
		'mean':     1.0,
		'std':      None,
		'eta':      None
	},{ # prior_uniform_positive
		'name':     'prior_uniform_positive',
		'density':  'uniform',
		'lower':    1e-4,
		'upper':    None,
		'mean':     1e-1,
		'std':      None,
		'eta':      None
	},{ # prior_mean_zero
		'name':     'prior_mean_zero',
		'density':  'gaussian',
		'lower':    None,
		'upper':    None,
		'mean':     0.0,
		'std':      1e-2,
		'eta':      None
	}
]
# --------------------------------------------------------------------------
# smooth table
middle_age_id  = 1
last_time_id   = 2
smooth_list = [
	{   # smooth_mean_zero
		'name':                     'smooth_mean_zero',
		'age_id':                   [ middle_age_id ],
		'time_id':                  [ 0, last_time_id ],
		'mulstd_value_prior_name':  'prior_one',
		'mulstd_dage_prior_name':   'prior_one',
		'mulstd_dtime_prior_name':  'prior_one',
		'fun':                      gaussian_zero_fun
	},{ # smooth_uniform_positive
		'name':                     'smooth_uniform_positive',
		'age_id':                   [ middle_age_id ],
		'time_id':                  [ 0, last_time_id ],
		'mulstd_value_prior_name':  'prior_one',
		'mulstd_dage_prior_name':   'prior_one',
		'mulstd_dtime_prior_name':  'prior_one',
		'fun':                       uniform_positive_fun
	}
]
# --------------------------------------------------------------------------
# rate table
rate_list = [
	{
		'name':          'pini',
		'parent_smooth': 'smooth_uniform_positive',
		'child_smooth':  'smooth_mean_zero'
	},{
		'name':          'iota',
		'parent_smooth': 'smooth_uniform_positive',
		'child_smooth':  'smooth_mean_zero'
	},{
		'name':          'rho',
		'parent_smooth': 'smooth_uniform_positive',
		'child_smooth':  'smooth_mean_zero'
	},{
		'name':          'chi',
		'parent_smooth': 'smooth_uniform_positive',
		'child_smooth':  'smooth_mean_zero'
	},{
		'name':          'omega',
		'parent_smooth': 'smooth_uniform_positive',
		'child_smooth':  'smooth_mean_zero'
	}
]
# --------------------------------------------------------------------------
# create database
dismod_at.create_database(
	file_name,
	age_list,
	time_list,
	integrand_list,
	node_list,
	weight_list,
	covariate_list,
	data_list,
	prior_list,
	smooth_list,
	rate_list,
	mulcov_list
)
# -----------------------------------------------------------------------
program        = '../../devel/dismod_at'
command        = 'fit'
arg_list= {
	'file_name':        file_name,
	'parent_node_id':   '0',
	'ode_step_size':    '20.0',
	'tolerance':        '1e-8',
	'max_num_iter':     '100',
	'rate_info':        'chi_positive'
}
cmd       = [
	program                     ,
	command                     ,
	arg_list['file_name']       ,
	arg_list['parent_node_id']  ,
	arg_list['ode_step_size']   ,
	arg_list['tolerance']       ,
	arg_list['max_num_iter']    ,
	arg_list['rate_info']
]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at fit command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# check fit_arg table
fit_arg_dict  = dismod_at.get_table_dict(connection, 'fit_arg')
for row in fit_arg_dict :
	fit_arg_name  = row['fit_arg_name']
	fit_arg_value = row['fit_arg_value']
	assert arg_list[fit_arg_name] == fit_arg_value
# -----------------------------------------------------------------------
# check variable table
variable_dict  = dismod_at.get_table_dict(connection, 'variable')
#
# mulstd variables
for smooth_id in range( len(smooth_list) ) :
	for variable_type in [ 'mulstd_value', 'mulstd_dage', 'mulstd_dtime' ] :
		count = 0
		for row in variable_dict :
			match = row['variable_type'] == variable_type
			match = match and row['smooth_id'] == smooth_id
			if match :
				count += 1
				assert row['variable_value'] == 1.0
		assert count == 1
#
# rate variables
parent_node_id = 0
child_node_id  = 1
check_tol      = 1e-3
n_rate         = 5;
for rate_id in range(n_rate) :
	integrand_id   = rate_id
	data_id        = rate_id
	meas_value     = data_list[ integrand_id ]['meas_value']
	for node_id in [ parent_node_id, child_node_id ] :
		count = 0
		for row in variable_dict :
			match = row['variable_type'] == 'rate'
			match = match and row['rate_id'] == rate_id
			match = match and row['node_id'] == node_id
			if match :
				count += 1
				if node_id == 0 :
					# parent node
					err = row['variable_value'] / meas_value - 1.0
				else :
					# child node
					err = row['variable_value'] / meas_value
				assert abs(err) <= check_tol
		# number of point in smoothing for all rates
		assert count == 2
# -----------------------------------------------------------------------
print('fit_command: OK')
# END PYTHON
