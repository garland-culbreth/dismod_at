# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
import time
import sys
import os
import subprocess
import random
import math
#
# import dismod_at
path_to_example = 'example/user/new_diabetes.py'
if sys.argv[0] != path_to_example or not os.path.isdir( 'python/dismod_at' ) :
   usage  = 'python3 ' + path_to_example + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
sys.path.insert(0, os.getcwd() + '/python')
import dismod_at
#
# build/example/user
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
#
# random_seed
random_seed = 0
#
# age_list, time_list
age_list  = [ 0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 70.0, 100.0 ]
time_list = [ 1960, 1980, 1990, 2000, 2010, 2020 ]
#
# number_nodes
number_nodes = 20
#
# std_random_effects_sim
std_random_effects_sim = 0.2
#
# mulcov_sim
mulcov_sim = { 'sex':0.5, 'bmi':0.02, 'ms_2000':0.3  }
#
# no_effect_rate
no_effect_rate = {
   'iota'  : 0.001 ,
   'chi'   : 0.02  ,
   'omega' : 0.01  ,
}
#
# minimum_prealance_std
minimum_prevalence_std = 1e-4
#
# average_integrand_step_size
average_integrand_step_size = 5.0
#
# ode_step_size
ode_step_size = 5.0
#
# hold_out_max_fit
hold_out_max_fit = 500
# ---------------------------------------------------------------------------
#
# random_seed
if random_seed == 0 :
   random_seed = int( time.time() )

# random_effect_list
# In this example, the random effects are the same iota and chi.
# Note that index zero corresponds to the parent node and has no random effect.
random_effect_list = [ 0.0 ]
for i in range(number_nodes-1) :
   random_effect = std_random_effects_sim * random.gauss(mu = 0.0, sigma = 1.0)
   random_effect_list.append( random_effect )
#
# rate_true
# Assume all covariates are relative to their referece values
def rate_true(rate_name, age, time, node_index, sex, bmi) :
   rate   = no_effect_rate[rate_name]
   effect = random_effect_list[node_index]
   if rate_name == 'iota' :
      effect += mulcov_sim['bmi'] * bmi
   elif rate_name == 'chi' :
      effect += mulcov_sim['sex'] * sex
   elif rate_name == 'omega' :
      effect += 0.0
   else :
      assert False
   rate = rate * math.exp( effect )
   return rate
#
# eample_db
def example_db(file_name) :
   #
   # sex_dict
   sex_dict = { 'female' : -0.5 , 'male' : +0.5 }
   #
   # integrand_table
   integrand_table = integrand_table = [
      { 'name':'mtspecific' },
      { 'name':'prevalence' }
   ]
   #
   # node_table
   node_table = list()
   for i in range( number_nodes ) :
      name = f'n{i}'
      parent = '' if i == 0 else 'n0'
      node_table.append( { 'name' : name, 'parent' : parent } )
   #
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   #
   # bmi_lower, bmi_upper, bmi_average
   bmi_lower   = 17.0
   bmi_upper   = 35.0
   bmi_average = (bmi_lower + bmi_upper) / 2.0
   #
   # covariate table:
   covariate_table = [
      {'name':'sex',     'reference':0.0,         'max_difference':0.6  } ,
      {'name':'bmi',     'reference':bmi_average, 'max_difference':None } ,
      {'name':'ms_2000', 'reference':0.0,         'max_difference':None } ,
   ]
   #
   # integrand_list
   integrand_list = list()
   for row in integrand_table :
      integrand_list.append( row['name'] )
   #
   # random_effects
   node_list = list()
   for row in node_table :
      node_list.append( row['name'] )
   #
   # data_table
   data_table   = list()
   #
   # count, integrand_name, age, time, node_name, sex_name
   count = 0
   for integrand_name in integrand_list :
      for age in age_list[1:] :
         for time in time_list :
            for node_index in range(number_nodes) :
               for sex_name in sex_dict :
                  count += 1
                  #
                  # node_name
                  node_name = node_list[node_index]
                  #
                  # sex
                  sex = sex_dict[sex_name]
                  #
                  # ms_2000
                  ms_2000 = 1 if time == 2000 else 0
                  #
                  # bmi
                  bmi = random.uniform(bmi_lower, bmi_upper)
                  #
                  # rate_fun
                  rate_fun  = dict()
                  relative_bmi = bmi - bmi_average
                  rate_fun['iota'] = lambda age, time : rate_true(
                     'iota', age, time, node_index, sex, relative_bmi
                  )
                  rate_fun['chi'] = lambda age, time : rate_true(
                     'chi', age, time, node_index, sex, relative_bmi
                  )
                  rate_fun['omega'] = lambda age, time : rate_true(
                     'omega', age, time, node_index, sex, relative_bmi
                  )
                  #
                  # meas_mean
                  if age == 0.0 :
                     meas_mean = 0
                  else :
                     #
                     # grid
                     grid         = dict()
                     n_step       = int(age / average_integrand_step_size) + 1
                     d_age        = age / n_step
                     grid['age']  = [ age ]
                     grid['time'] = [ time ]
                     #
                     # abs_tol
                     abs_tol = 1e-7
                     #
                     # meas_mean
                     meas_mean = dismod_at.average_integrand(
                        rate_fun, integrand_name, grid, abs_tol
                     )
                     if integrand_name == 'prevalence' :
                        effect    = ms_2000 * mulcov_sim['ms_2000']
                        meas_mean = meas_mean * math.exp(effect)
                  #
                  # meas_std
                  meas_std = meas_mean * 0.2
                  min_std  = minimum_prevalence_std
                  if integrand_name == 'mtspecific' :
                     min_std = minimum_prevalence_std * no_effect_rate['chi']
                  meas_std = max(meas_std, min_std)
                  #
                  # data_table
                  row = {
                     # avgint columns
                     'integrand':   integrand_name,
                     'node':        node_name,
                     'subgroup':    'world',
                     'density':     'gaussian',
                     'weight':      '',
                     'age_lower':    age,
                     'age_upper':    age,
                     'time_lower':   time,
                     'time_upper':   time,
                     'sex':          sex,
                     'ms_2000':      ms_2000,
                     'bmi':          bmi,
                     #
                     # other columns
                     'hold_out':      0,
                     'density':       'gaussian',
                     'meas_value':    meas_mean,
                     'meas_std':      meas_std,
                  }
                  data_table.append(row)
   #
   # prior_table
   prior_table = [ {
         # parent_rate_value_prior
         'name':    'parent_rate_value_prior',
         'density': 'log_gaussian',
         'mean':    0.01,
         'std':     0.2,
         'lower':   1e-6,
         'upper':   1.0,
         'eta':     1e-6,
      },{
         # gauss_0_0.1
         'name':    'gauss_0_0.1',
         'density': 'gaussian',
         'mean':    0.0,
         'std':     0.1,
      },{
         # log_gauss_0_0.01
         'name':    'log_gauss_0_0.01',
         'density': 'log_gaussian',
         'mean':    0.0,
         'std':     0.01,
         'eta':     1e-6,
      },{
         # unoform_-inf_+inf
         'name':    'uniform_-inf_+inf',
         'density': 'uniform',
         'mean':    0.0,
   } ]
   #
   # prior_fun
   prior_fun = dict()
   prior_fun['parent'] = lambda age, time : \
      ('parent_rate_value_prior', 'log_gauss_0_0.01', 'log_gauss_0_0.01')
   prior_fun['child'] = lambda age, time : \
      ('gauss_0_0.1', None, None)
   prior_fun['mulcov'] = lambda age, time : \
      ('uniform_-inf_+inf', None, None)
   prior_fun['omega'] = lambda age, time : \
      (no_effect_rate['omega'], None, None)
   #
   # smooth_table
   smooth_table = [ {
         # smooth_omega
         'name':     'smooth_omega',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      prior_fun['omega']
      },{
         # smooth_parent
         'name':     'smooth_parent',
         'age_id':   range( len(age_list) ),
         'time_id':  range( len(time_list) ),
         'fun':      prior_fun['parent']
      },{
         # smooth_child
         'name':     'smooth_child',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      prior_fun['child']
      },{
         # smooth_mulcov
         'name':     'smooth_mulcov',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      prior_fun['mulcov']
   } ]
   #
   # rate_table
   rate_table = [ {
         'name':           'iota',
         'parent_smooth':  'smooth_parent',
         'child_smooth':   'smooth_child',
         'child_nslist':   None,
      },{
         'name':           'chi',
         'parent_smooth':  'smooth_parent',
         'child_smooth':   'smooth_child',
         'child_nslist':   None,
      },{
         'name':           'omega',
         'parent_smooth':  'smooth_omega',
         'child_smooth':   None,
         'child_nslist':   None,
   } ]
   #
   # mulcov_table
   mulcov_table = [ {
      'covariate':     'sex',
      'type':          'rate_value',
      'effected':      'chi',
      'group':         'world',
      'smooth':        'smooth_mulcov',
   },{
      'covariate':     'bmi',
      'type':          'rate_value',
      'effected':      'iota',
      'group':         'world',
      'smooth':        'smooth_mulcov',
   },{
      'covariate':     'ms_2000',
      'type':          'meas_value',
      'effected':      'prevalence',
      'group':         'world',
      'smooth':        'smooth_mulcov',
   } ]
   #
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'    },
      { 'name':'parent_node_name',       'value':'n0'                   },
      { 'name':'ode_step_size',          'value':str(ode_step_size)     },
      { 'name':'random_seed',            'value':str(random_seed)       },
      { 'name':'bound_random',           'value':'1.0'                  },

      { 'name':'quasi_fixed',            'value':'false'                },
      { 'name':'max_num_iter_fixed',     'value':'50'                   },
      { 'name':'print_level_fixed',      'value':'5'                    },
      { 'name':'tolerance_fixed',        'value':'1e-4'                 },

      { 'name':'max_num_iter_random',    'value':'50'                   },
      { 'name':'print_level_random',     'value':'0'                    },
      { 'name':'tolerance_random',       'value':'1e-8'                 },
   ]
   #
   # create_database
   dismod_at.create_database(
      file_name       = file_name,
      age_list        = age_list,
      time_list       = time_list,
      integrand_table = integrand_table,
      node_table      = node_table,
      subgroup_table  = subgroup_table,
      weight_table    = list(),
      covariate_table = covariate_table,
      avgint_table    = list(),
      data_table      = data_table,
      prior_table     = prior_table,
      smooth_table    = smooth_table,
      nslist_dict     = dict(),
      rate_table      = rate_table,
      mulcov_table    = mulcov_table,
      option_table    = option_table,
      rate_eff_cov_table  = list(),
   )
# -----------------------------------------------------------------------------
#
# file_nanme
file_name = 'example.db'
example_db(file_name)
#
# program
program = '../../devel/dismod_at'
#
# init
dismod_at.system_command_prc([ program, file_name, 'init' ])
#
# hold_out
for integrand_name in [ 'prevalence', 'mtspecific' ] :
   str_max_fit = str( hold_out_max_fit )
   dismod_at.system_command_prc(
      [ program, file_name, 'hold_out', integrand_name, str_max_fit ]
   )
#
if True :
   #
   # fit fixed
   dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
   #
   # start_var table
   dismod_at.system_command_prc(
      [ program, file_name, 'set', 'start_var', 'fit_var' ]
   )
#
# fit both
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
#
print('diabetes.py: OK')
