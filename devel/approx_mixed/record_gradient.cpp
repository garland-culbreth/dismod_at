// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/approx_mixed.hpp>

/*
$begin approx_mixed_record_gradient$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Record Gradient of Joint Density w.r.t Random Effects$$

$head Syntax$$
$codei%record_gradient(%fixed_vec%, %random_vec%)%$$

$head Private$$
This function is $code private$$ to the $code approx_mixed$$ class
and cannot be used by a derived
$cref/approx_object/approx_mixed_derived_ctor/approx_object/$$.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ at which the recording is made.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$ at which the recording is made.

$head gradient_$$
The input value of the member variable
$codei%
	CppAD::ADFun<a1_double> gradient_
%$$
does not matter.
Upon return it contains the corresponding recording for the gradient
$latex f_u^{(1)} ( \theta , u )$$.

$head joint_density$$
The member function $code joint_density$$ called
with arguments of type $code a3d_vector$$.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_gradient(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	size_t i, j;

	//	create an a2d_vector containing (theta, u)
	a2d_vector a2_both( n_fixed_ + n_random_ );
	for(j = 0; j < n_fixed_; j++)
		a2_both[j] = a2_double( fixed_vec[j] );
	for(j = 0; j < n_random_; j++)
		a2_both[n_fixed_ + j] = a2_double( random_vec[j] );

	// start recording f_uu (theta, u) using a2_double operations
	CppAD::Independent( a2_both );

	// create an a3d_vector containing theta and u
	a3d_vector a3_theta(n_fixed_), a3_u(n_random_);
	for(j = 0; j < n_fixed_; j++)
		a3_theta[j] = a2_both[j];
	for(j = 0; j < n_random_; j++)
		a3_u[j] = a2_both[n_fixed_ + j];

	// compute f(u) using a3_double operations
	CppAD::Independent(a3_u);
	a3d_vector a3_vec = joint_density(a3_theta, a3_u);
	a3d_vector a3_sum(1);
	a3_sum[0]    = a3_vec[0];
	size_t n_abs = a3_vec.size() - 1;
	for(i = 0; i < n_abs; i++)
		a3_sum[0] += abs( a3_vec[1 + i] );
	CppAD::ADFun<a2_double> a2_f;
	a2_f.Dependent(a3_u, a3_sum);

	// zero order forward mode
	a2d_vector a2_u(n_random_);
	for(j = 0; j < n_random_; j++)
		a2_u[j] = a2_both[n_fixed_ + j];
	a2_f.Forward(0, a2_u);

	// first order reverse
	a2d_vector a2_grad(n_random_), a2_w(1);
	a2_w[0] = a2_double( 1.0 );
	a2_grad = a2_f.Reverse(1, a2_w);

	// complete recording of f_u^{(1)} (u, theta)
	gradient_.Dependent(a2_both, a2_grad);

	// optimize the recording
	gradient_.optimize();
}


} // END_DISMOD_AT_NAMESPACE
