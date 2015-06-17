# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin pack_info.py$$ $newlinech #$$
#
# $section pack_info: Example and Test$$
#
# $code
# $verbatim%example/table/pack_info.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
def pack_info() :
	import dismod_at
	#
	n_integrand = 4
	n_child     = 2
	smooth_dict =  [
		{ 'n_age':2, 'n_time':3 },
		{ 'n_age':2, 'n_time':3 },
		{ 'n_age':1, 'n_time':1 },
		{ 'n_age':3, 'n_time':3 }
	]
	mulcov_dict = [ {
			'mulcov_type':'meas_mean',
			'rate_id':     -1,
			'integrand_id': 0,
			'covariate_id': 0,
			'smooth_id':    0
		},{
			'mulcov_type':'meas_mean',
			'rate_id':     -1,
			'integrand_id': 1,
			'covariate_id': 1,
			'smooth_id':    1
		},{
			'mulcov_type':'meas_std',
			'rate_id':     -1,
			'integrand_id': 2,
			'covariate_id': 2,
			'smooth_id':    2
		},{
			'mulcov_type':'rate_mean',
			'rate_id':      3,
			'integrand_id':-1,
			'covariate_id': 3,
			'smooth_id':    3
	} ]
	rate_dict = [
		{ 'rate_name':'pini',  'parent_smooth_id':2, 'child_smooth_id':2 },
		{ 'rate_name':'iota',  'parent_smooth_id':0, 'child_smooth_id':1 },
		{ 'rate_name':'rho',   'parent_smooth_id':0, 'child_smooth_id':1 },
		{ 'rate_name':'chi',   'parent_smooth_id':0, 'child_smooth_id':1 },
		{ 'rate_name':'omega', 'parent_smooth_id':0, 'child_smooth_id':1 }
	]
	#
	# constructor
	pack_object = dismod_at.pack_info(
		n_integrand, n_child, smooth_dict, mulcov_dict, rate_dict
	)
# END PYTHON
