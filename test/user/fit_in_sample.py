# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2026 Garland Culbreth
# ----------------------------------------------------------------------------
# Test that for every avgint row the value predicted with fit_var lies inside
# the range [sam_min, sam_max] of the predicted sample values.
#
# This test is expected to fail unless the predict command recenters sample
# predictions on the fit_var prediction.
#
number_sample = 20
#
# random_seed
# Ensure samples are deterministic, up to processor rng.
random_seed   = 20
#
# age_grid
age_grid      = [ float(i) for i in range(0, 101, 5) ]
#
# iota_true, chi_true, omega_true
iota_true     = 1e-4
chi_true      = 1e-3
omega_true    = 1e-3
#
# integrand_list
integrand_list = [ 'Sincidence', 'mtexcess', 'prevalence', 'mtspecific' ]
# ------------------------------------------------------------------------
import sys
import os
test_program  = 'test/user/fit_in_sample.py'
check_program = sys.argv[0].replace('\\', '/')
if check_program != test_program  or len(sys.argv) != 1 :
   usage  = 'python3 ' + test_program + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
   sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/test/user directory
if not os.path.exists('build/test/user') :
   os.makedirs('build/test/user')
os.chdir('build/test/user')
# ------------------------------------------------------------------------
# Non-null eta in value priors makes dismod_at scale fixed effects in
# eta-offset log space during fit and sampling.
def fun_iota(a, t) :
   return ('prior_iota_value', 'prior_dage', None)
def fun_chi(a, t) :
   return ('prior_chi_value', None, None)
def fun_omega(a, t) :
   return (omega_true, None, None)
# ------------------------------------------------------------------------
def example_db (file_name) :
   # ----------------------------------------------------------------------
   # age table
   age_list    = age_grid
   #
   # time table
   time_list   = [ 2000.0 ]
   #
   # integrand table
   integrand_table = list()
   for integrand in integrand_list :
      integrand_table.append( { 'name': integrand, 'minimum_meas_cv': 0.0 } )
   #
   # node table:
   node_table = [ { 'name': 'n0',  'parent': '' } ]
   #
   # weight table
   weight_table = list()
   #
   # covariate table: no covariates
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   #
   # avgint table: one row for each integrand and age
   avgint_table = list()
   for integrand in integrand_list :
      for age in age_grid :
         row = {
            'integrand':   integrand,
            'node':        'n0',
            'subgroup':    'n0',
            'weight':      '',
            'age_lower':   age,
            'age_upper':   age,
            'time_lower':  2000.0,
            'time_upper':  2000.0,
         }
         avgint_table.append( row )
   #
   # data table:
   # Sincidence has a large meas_std so that iota is weakly identified.
   data_table = list()
   for age in age_grid :
      row = {
         'integrand':   'Sincidence',
         'node':        'n0',
         'subgroup':    'n0',
         'weight':      '',
         'density':     'gaussian',
         'hold_out':    False,
         'meas_value':  iota_true,
         'meas_std':    1.0 * iota_true,
         'age_lower':   age,
         'age_upper':   age,
         'time_lower':  2000.0,
         'time_upper':  2000.0,
      }
      data_table.append( row )
   row = {
      'integrand':   'mtexcess',
      'node':        'n0',
      'subgroup':    'n0',
      'weight':      '',
      'density':     'gaussian',
      'hold_out':    False,
      'meas_value':  chi_true,
      'meas_std':    0.1 * chi_true,
      'age_lower':   50.0,
      'age_upper':   50.0,
      'time_lower':  2000.0,
      'time_upper':  2000.0,
   }
   data_table.append( row )
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota_value
         'name':     'prior_iota_value',
         'density':  'log_gaussian',
         'mean':     iota_true,
         'std':      3.0,
         'eta':      1e-10,
         'lower':    1e-10,
         'upper':    1.0,
      },{ # prior_chi_value
         'name':     'prior_chi_value',
         'density':  'log_gaussian',
         'mean':     chi_true,
         'std':      3.0,
         'eta':      1e-10,
         'lower':    1e-10,
         'upper':    1.0,
      },{ # prior_dage
         'name':     'prior_dage',
         'density':  'log_gaussian',
         'mean':     0.0,
         'std':      3.0,
         'eta':      1e-10,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_iota
         'name':     'smooth_iota',
         'age_id':   list( range( len(age_grid) ) ),
         'time_id':  [ 0 ],
         'fun':      fun_iota
      },{ # smooth_chi
         'name':     'smooth_chi',
         'age_id':   [ 0 ],
         'time_id':  [ 0 ],
         'fun':      fun_chi
      },{ # smooth_omega
         'name':     'smooth_omega',
         'age_id':   [ 0 ],
         'time_id':  [ 0 ],
         'fun':      fun_omega
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'iota',
         'parent_smooth': 'smooth_iota',
         'child_smooth':  None,
      },{
         'name':          'chi',
         'parent_smooth': 'smooth_chi',
         'child_smooth':  None,
      },{
         'name':          'omega',
         'parent_smooth': 'smooth_omega',
         'child_smooth':  None,
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'n0'              },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'  },
      { 'name':'ode_step_size',          'value':'1.0'                },
      { 'name':'random_seed',            'value':str(random_seed)     },

      { 'name':'quasi_fixed',            'value':'false'              },
      { 'name':'max_num_iter_fixed',     'value':'200'                },
      { 'name':'print_level_fixed',      'value':'0'                  },
      { 'name':'tolerance_fixed',        'value':'1e-8'               },

      { 'name':'max_num_iter_random',    'value':'100'                },
      { 'name':'print_level_random',     'value':'0'                  },
      { 'name':'tolerance_random',       'value':'1e-8'               }
   ]
   # ----------------------------------------------------------------------
   # subgroup_table
   subgroup_table = [ { 'subgroup': 'n0', 'group': 'n0' } ]
   # ----------------------------------------------------------------------
   # create database
   dismod_at.create_database(
      file_name,
      age_list,
      time_list,
      integrand_table,
      node_table,
      subgroup_table,
      weight_table,
      covariate_table,
      avgint_table,
      data_table,
      prior_table,
      smooth_table,
      nslist_dict,
      rate_table,
      mulcov_table,
      option_table
   )
   # ----------------------------------------------------------------------
   return
# -----------------------------------------------------------------------
# Create database and run init, fit, sample, predict
file_name = 'example.db'
example_db(file_name)
program   = '../../devel/dismod_at'
#
def run_command(cmd) :
   result = dismod_at.system_command_prc(cmd, return_stderr = True)
   if result.returncode != 0 :
      print( result.stderr )
      sys.exit(f'The dismod_at {cmd[2]} command failed')
#
# init
run_command( [ program, file_name, 'init' ] )
#
# fit both
run_command( [ program, file_name, 'fit', 'both' ] )
#
# sample censor_asymptotic
run_command( [
   program, file_name, 'sample', 'censor_asymptotic', 'both', str(number_sample)
] )
#
# fit_predict
run_command( [ program, file_name, 'predict', 'fit_var' ] )
connection    = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
predict_table = dismod_at.get_table_dict(connection, 'predict')
connection.close()
fit_predict   = dict()
for row in predict_table :
   assert row['sample_index'] == None
   avgint_id = row['avgint_id']
   assert avgint_id not in fit_predict
   fit_predict[avgint_id] = row['avg_integrand']
assert len(fit_predict) == len(integrand_list) * len(age_grid)
#
# sam_predict
# TODO: Argument that recenters the sample predictions on the fit_var
# predictions to go at the end of this command once implemented.
run_command( [ program, file_name, 'predict', 'sample' ] )
connection    = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
predict_table = dismod_at.get_table_dict(connection, 'predict')
avgint_table  = dismod_at.get_table_dict(connection, 'avgint')
integrand_table = dismod_at.get_table_dict(connection, 'integrand')
connection.close()
sam_predict   = dict()
for row in predict_table :
   avgint_id = row['avgint_id']
   if avgint_id not in sam_predict :
      sam_predict[avgint_id] = list()
   sam_predict[avgint_id].append( row['avg_integrand'] )
assert set( sam_predict.keys() ) == set( fit_predict.keys() )
#
# check that every predicted fit value is inside its predicted sample range
# [sam_min, sam_max]
bad = list()
for avgint_id in fit_predict :
   assert len( sam_predict[avgint_id] ) == number_sample
   fit     = fit_predict[avgint_id]
   sam_min = min( sam_predict[avgint_id] )
   sam_max = max( sam_predict[avgint_id] )
   if not ( sam_min <= fit <= sam_max ) :
      integrand_id   = avgint_table[avgint_id]['integrand_id']
      integrand_name = integrand_table[integrand_id]['integrand_name']
      age            = avgint_table[avgint_id]['age_lower']
      line  = f'{integrand_name}, age = {age}, fit = {fit:.6g}, '
      line += f'sam_min = {sam_min:.6g}, sam_max = {sam_max:.6g}'
      bad.append(line)
if len(bad) > 0 :
   for line in bad :
      print(line)
   print( f'random_seed = {random_seed}' )
   msg  = f'{len(bad)} of {len(fit_predict)} predict rows have '
   msg += 'fit value outside [sam_min, sam_max]'
   assert False, msg
# -----------------------------------------------------------------------
print('fit_in_sample.py: OK')
# END PYTHON
