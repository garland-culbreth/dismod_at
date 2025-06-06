# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_compress.py}
# {xrst_spell
#     def
# }
# {xrst_comment_ch #}
#
# Using Data Interval Compression
# ###############################
#
# Purpose
# *******
# This example shows how to use the
# :ref:`compression intervals<option_table@compress_interval>` option.
#
# Integrands
# **********
# For this example there are one integrand, ``Sincidence`` .
#
# Nodes
# *****
# There is only one node called ``world`` for this example.
# There are no random effects because there are no child nodes.
#
# True Iota
# *********
# For this example, the true model incidence rate *iota* is
# {xrst_code py}
def iota_true(age) :
   return 0.01 * ( 1 + ((age - 50) / 50)**2 )
# {xrst_code}
#
# Model
# *****
# There is only one rate *iota* and it piecewise linear in age
# with knots at the age points 0, 50, and 100.
#
# Data
# ****
# There is one data point measuring Sincidence
# with the true value of *iota* at times 0.0, 50, 100.
# The corresponding age intervals are [0,0], [0,100], [100,100].
# The age interval for the second measurement should be [50,50]
# and using interval compression with make it so.
# This is a cooked up example where interval compression makes the solution
# more accurate. Under normal circumstances, the answer less accurate
# but faster to compute.
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_compress.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
import sys
import os
import csv
import copy
import math
test_program  = 'example/user/compress.py'
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
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
   # note that the a, t values are not used for this case
   def fun_iota(a, t) :
      return ('prior_iota', None, None)
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 50.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2000.0, 2010.0, 2020.0 ]
   #
   # integrand table:
   integrand_table = [
       { 'name':'Sincidence' }
   ]
   #
   # node table:
   node_table = [ { 'name':'world', 'parent':'' } ]
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
   #
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   #
   # values that are the same for all data points
   row = {
      'integrand':   'Sincidence',
      'hold_out':    False,
      'density':     'gaussian',
      'meas_std':    iota_true(0.0) / 10.,
      'weight':      '',
      'time_lower':   2000.,
      'time_upper':   2000.,
      'node':         'world',
      'subgroup':     'world',
   }
   # Sincidence at age 0.0
   row['meas_value'] = iota_true(0.0)
   row['age_lower']  = 0.0
   row['age_upper']  = 0.0
   data_table.append( copy.copy(row) )
   #
   # average Sincidence between age 0.0 and 100
   row['meas_value'] = iota_true(50.0)
   row['age_lower']  = 0.0
   row['age_upper']  = 100.0
   data_table.append( copy.copy(row) )
   #
   # Sincidence between at age 100
   row['meas_value'] = iota_true(100.0)
   row['age_lower']  = 100.0
   row['age_upper']  = 100.0
   data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota
         'name':     'prior_iota',
         'density':  'uniform',
         'lower':    iota_true(0) / 10.0,
         'upper':    iota_true(0) * 10.0,
         'mean':     iota_true(0) * 2.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   name           = 'smooth_iota'
   fun            = fun_iota
   smooth_table = [
      {  'name':name,
         'age_id':[0, 1, 2], # ages 0, 50, 100
         'time_id':[0],
         'fun':fun
      }
   ]
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'   },
      { 'name':'parent_node_name',       'value':'world'               },

      { 'name':'quasi_fixed',            'value':'false'               },
      { 'name':'max_num_iter_fixed',     'value':'50'                  },
      { 'name':'print_level_fixed',      'value':'0'                   },
      { 'name':'tolerance_fixed',        'value':'1e-9'                },

      { 'name':'max_num_iter_random',    'value':'50'                  },
      { 'name':'print_level_random',     'value':'0'                   },
      { 'name':'tolerance_random',       'value':'1e-10'               },
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
# Create database
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
# -----------------------------------------------------------------------
# read database
connection            = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
age_table             = dismod_at.get_table_dict(connection, 'age')
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
# There are three values for iota in this model
assert len(var_table) == 3
assert len(fit_var_table) == 3
#
# check that this fit is not accurate
for var_id in range( len(var_table) ) :
   age_id      = var_table[var_id]['age_id']
   if age_id == 1 :
      age         = age_table[age_id]['age']
      true_value  = iota_true(age)
      fit_value   = fit_var_table[var_id]['fit_var_value']
      rel_error   = 1.0 - fit_value/true_value
      assert age == 50.0
      assert abs(rel_error) > 0.5
# -----------------------------------------------------------------------
# Now compress the age intervals for all the data. This only affects the second
# data points because the others have intervals of size zero.
dismod_at.system_command_prc([
   program, file_name, 'set', 'option', 'compress_interval', '100.0 0.0'
])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
#
# read database
connection            = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
# check that this fit is accurate
for var_id in range( len(var_table) ) :
   age_id      = var_table[var_id]['age_id']
   age         = age_table[age_id]['age']
   true_value  = iota_true(age)
   fit_value   = fit_var_table[var_id]['fit_var_value']
   rel_error   = 1.0 - fit_value/true_value
   assert abs(rel_error) < 1e-6
# ---------------------------------------------------------------------------
# Now check data.csv for interval compression
os.chdir('../../..')
python_exe = dismod_at.python3_executable
program    = 'python/bin/dismodat.py'
file_name  = 'build/example/user/' + file_name
dismod_at.system_command_prc([ python_exe, program, file_name, 'db2csv' ])
data_file      = open('build/example/user/data.csv', 'r')
reader         = csv.DictReader(data_file)
for (data_id, row) in enumerate(reader) :
   # check flag for age compression
   if int(data_id) == 0 :
      assert float( row['age_lo'] ) == 0.0
      assert float( row['age_up'] ) == 0.0
   if int(data_id) == 1 :
      assert float( row['age_lo'] ) == 50.0
      assert float( row['age_up'] ) == 50.0
   if int(data_id) == 2 :
      assert float( row['age_lo'] ) == 100.0
      assert float( row['age_up'] ) == 100.0
# -----------------------------------------------------------------------------
print('compress.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
