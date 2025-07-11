# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# Build test / user
SET(depends "")
# BEGIN_SORT_THIS_LINE_PLUS_2
FOREACH(user_case
   asymptotic
   average_integrand
   avgint
   bound_frac
   bound_random
   censor_1
   censor_2
   censor_asymptotic
   const_value
   csv2db
   db2csv
   fit_meas_noise
   fit_sim
   hes_fixed
   hold_out
   init_covariate
   laplace
   minimum_cv
   neg_iteration
   nodata_eta0
   not_ordered
   parent_node_id
   perturb_other
   posterior
   relrisk
   scale_gamma
   scale_zero
   set_command
   subgroup_mulcov
   zero_random_1
   zero_random_2
)
# END_SORT_THIS_LINE_MINUS_2
#
   ADD_CUSTOM_TARGET(
      check_test_user_${user_case}
      ${python3_executable} bin/user_test.py test/user/${user_case}.py
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      DEPENDS dismod_at
   )
   SET(depends ${depends} check_test_user_${user_case} )
ENDFOREACH(user_case zero_random)
ADD_CUSTOM_TARGET( check_test_user DEPENDS ${depends} )
