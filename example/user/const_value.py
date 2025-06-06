# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_const_value.py}
# {xrst_comment_ch #}
#
# Constrain Omega Using const_value
# #################################
#
# Node Table
# **********
# For this example the :ref:`node_table-name` is
# ::
#
#             universe
#                |
#              world
#             /      \
#         child_1   child_2
#
# Problem Parameters
# ******************
# The following values are used to simulate the data and set the priors
# for fitting the data:
# {xrst_spell_off}
# {xrst_code py}
iota_true    = { 'world':0.004, 'child_1':0.002,  'child_2':0.008 }
omega_true   = { 'world':0.01, 'child_1':0.005,  'child_2':0.002  }
n_data       = 51
# {xrst_code}
# {xrst_spell_on}
#
# Child Random Effects
# ********************
# The following code converts the child rates to child rate effects:
# {xrst_spell_off}
# {xrst_code py}
import math
for key in [ 'child_1', 'child_2' ] :
   iota_true[key]  = math.log( iota_true[key] / iota_true['world'] )
   omega_true[key] = math.log( omega_true[key] / omega_true['world'] )
# {xrst_code}
# {xrst_spell_on}
#
# Omega Priors
# ************
# The parent and child rates for omega are constrained to be their
# true value using the :ref:`smooth_grid_table@const_value`
# column in the smooth_grid table.
#
# nslist
# ======
# Note that the different children have different priors for the
# child random effects which requires using the
# :ref:`child_nslist<rate_table@child_nslist_id>` option in the
# rate table.
#
# Iota Priors
# ***********
# The iota priors are uniform.
#
# Parent
# ======
# For the parent, the lower (upper) limit is the true value
# divided by ten (multiplied by ten).
# The mean, which is two times the true value, is used for the
# :ref:`start_var_table-name` and :ref:`scale_var_table-name` ;
# see :ref:`init_command-name` .
#
# Data
# ****
# All of the data for this example is direct measurements of the
# :ref:`avg_integrand@Integrand, I_i(a,t)@susceptible`
# population (as a fraction of the initial population).
#
# random_seed
# ***********
# Use the clock to seed random number generator, but pass it into
# dismod_at using the :ref:`option table<option_table@random_seed>` :
# {xrst_spell_off}
# {xrst_code py}
import time
random_seed = int( time.time() )
# {xrst_code}
# {xrst_spell_on}
#
# Source Code
# ***********
# Given the problem parameters define above, below is the rest of the
# source code for this example:
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_const_value.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# begin problem parameters
# end problem parameters
# ------------------------------------------------------------------------
import sys
import os
import copy
test_program  = 'example/user/const_value.py'
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
# change into the build/example/user directory
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
# ----------------------------------------------------------------------------
#
#
def example_db (file_name) :
   from math import log
   def fun_iota_world(a, t) :
      value = 'prior_iota_world_value'
      dage  = 'prior_iota_world_smooth'
      return ('prior_iota_world_value', None, None)
   def fun_iota_child(a, t) :
      return ('prior_iota_child_value', None, None)
   def fun_omega_world(a, t) :
      return (omega_true['world'], None, None)
   def fun_omega_child_1(a, t) :
      return (omega_true['child_1'], None, None)
   def fun_omega_child_2(a, t) :
      return (omega_true['child_2'], None, None)
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2200.0 ]
   #
   # integrand table:
   integrand_table = [
       { 'name':'susceptible' }
   ]
   #
   # node table:
   node_table = [
      { 'name':'universe', 'parent':''         },
      { 'name':'world',    'parent':'universe' },
      { 'name':'child_1',  'parent':'world'    },
      { 'name':'child_2',  'parent':'world'    }
   ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table:
   covariate_table = list()
   #
   # mulcov table:
   mulcov_table = list()
   #
   # avgint table: empty
   avgint_table = list()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   # values that are the same for all data rows
   row = {
      'meas_value':  0.0,             # not used (will be simulated)
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.,
      'time_upper':   2000.,
      'integrand':    'susceptible',
      'meas_std':     0.005,
      'subgroup':     'world'
   }
   # values that change between rows:
   node_name  = [ 'world', 'child_1', 'child_2' ]
   age_value  = [ 10, 100, 10, 100 ]
   time_value = [ time_list[0], time_list[0], time_list[-1], time_list[-1] ]
   for data_id in range( n_data ) :
      age  =  age_value[ data_id % 4]
      time = time_value[ data_id % 4]
      row['age_lower']  = age
      row['age_upper']  = age
      row['time_lower'] = time
      row['time_upper'] = time
      row['node']       = node_name[ data_id % 3 ]
      data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota_world_value
         'name':     'prior_iota_world_value',
         'density':  'uniform',
         'lower':    iota_true['world'] / 10.,
         'upper':    iota_true['world'] * 10.,
         'mean':     iota_true['world'] * 2.0,
      },{ # prior_iota_child_value
         'name':     'prior_iota_child_value',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      1.0
      },{ # prior_iota_world_smooth
         'name':     'prior_iota_world_smooth',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      iota_true['world'] / 10.0
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      {  # smooth_iota_world
         'name':'smooth_iota_world',
         'age_id':[0, 1],
         'time_id':[0, 1],
         'fun':fun_iota_world
      },{  # smooth_iota_child
         'name':'smooth_iota_child',
         'age_id':[0],
         'time_id':[0],
         'fun':fun_iota_child
      },{  # smooth_omega_world
         'name':'smooth_omega_world',
         'age_id':[0, 1],
         'time_id':[0, 1],
         'fun':fun_omega_world
      },{  # smooth_omega_child_1
         'name':'smooth_omega_child_1',
         'age_id':[0],
         'time_id':[0],
         'fun':fun_omega_child_1
      },{  # smooth_omega_child_2
         'name':'smooth_omega_child_2',
         'age_id':[0],
         'time_id':[0],
         'fun':fun_omega_child_2
      }
   ]
   #
   # nslist_dict:
   nslist_dict = dict()
   nslist_dict['omega_nslist'] = [
      ('child_1', 'smooth_omega_child_1'),
      ('child_2', 'smooth_omega_child_2')
   ]
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota_world',
         'child_smooth':  'smooth_iota_child',
      },{
         'name':          'omega',
         'parent_smooth': 'smooth_omega_world',
         'child_nslist':  'omega_nslist',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },
      { 'name':'parent_node_name',       'value':'world'             },
      { 'name':'ode_step_size',          'value':'10.0'              },
      { 'name':'random_seed',            'value':str(random_seed)    },

      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'max_num_iter_fixed',     'value':'50'           },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-8'         },

      { 'name':'max_num_iter_random',    'value':'50'           },
      { 'name':'print_level_random',     'value':'0'            },
      { 'name':'tolerance_random',       'value':'1e-10'        },
      { 'name':'zero_sum_child_rate',    'value':'iota'         }
   ]
   # ----------------------------------------------------------------------
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
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
# ===========================================================================
# Run the init command to create the var table
file_name      = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
# -----------------------------------------------------------------------
# read database
connection    = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
var_table     = dismod_at.get_table_dict(connection, 'var')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
node_table    = dismod_at.get_table_dict(connection, 'node')
# -----------------------------------------------------------------------
# truth table:
tbl_name     = 'truth_var'
col_name     = [ 'truth_var_value' ]
col_type     = [ 'real' ]
row_list     = list()
var_id2true  = list()
for var_id in range( len(var_table) ) :
   var_row         = var_table[var_id]
   var_type        = var_row['var_type']
   assert var_type == 'rate'
   rate_id         = var_row['rate_id']
   rate_name       = rate_table[rate_id]['rate_name']
   node_id         = var_row['node_id']
   node_name       = node_table[node_id]['node_name']
   if rate_name == 'iota' :
      value = iota_true[node_name]
   else :
      assert rate_name == 'omega'
      value     = omega_true[node_name]
   row_list.append( [ value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# Simulate one data set and fit it
from dismod_at import system_command_prc
system_command_prc([ program, file_name, 'simulate', '1' ])
system_command_prc([ program, file_name, 'set', 'start_var', 'truth_var' ])
system_command_prc([ program, file_name, 'set', 'scale_var', 'truth_var' ])
system_command_prc([ program, file_name, 'fit', 'both', '0' ])
# -----------------------------------------------------------------------
# check fit results
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
truth_var_table = dismod_at.get_table_dict(connection, 'truth_var')
connection.close()
#
for var_id in range( len(var_table) ) :
   fit_value  = fit_var_table[var_id]['fit_var_value']
   true_value = truth_var_table[var_id]['truth_var_value']
   var_row    = var_table[var_id]
   rate_id    = var_row['rate_id']
   rate_name = rate_table[rate_id]['rate_name']
   relerr    = fit_value / true_value - 1.0
   if rate_name == 'iota' :
      ok = abs(relerr) < .1
      if not ok :
         node_id    = var_row['node_id']
         node_name  = node_table[node_id]['node_name']
         print(random_seed)
         print(node_name, rate_name, fit_value, true_value, relerr)
         print( "iota relative error = ", relerr)
   else :
      assert rate_name == 'omega'
      ok = abs(relerr) < 1e-10
      if not ok :
         print( "iota relative error = ", relerr)
# -----------------------------------------------------------------------------
print('const_value.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
