// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_APPROX_MIXED_HPP
# define DISMOD_AT_APPROX_MIXED_HPP
# include <cppad/cppad.hpp>

//
extern bool constraint_eval_xam(void);
extern bool constraint_jac_xam(void);
extern bool constraint_hes_xam(void);
extern bool ran_like_grad_xam(void);
extern bool ran_like_hes_xam(void);
extern bool laplace_eval_xam(void);
extern bool laplace_beta_xam(void);
extern bool laplace_hes_fix_xam(void);
extern bool prior_eval_xam(void);
extern bool fix_like_jac_xam(void);
extern bool fix_like_hes_xam(void);

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class approx_mixed {
	friend class optimize_random_eval;
	friend class ipopt_fixed;
public:
/*
$begin approx_mixed_public$$
$spell
	const
	vec
	typedef
	CppAD
	ctor
	std
$$

$section approx_mixed Public Declarations$$

$head AD Types$$
$index a_double$$
$codep */
	typedef CppAD::AD<double>          a1_double;
	typedef CppAD::AD<a1_double>       a2_double;
	typedef CppAD::AD<a2_double>       a3_double;
	typedef CppAD::AD<a3_double>       a4_double;
	typedef CppAD::AD<a4_double>       a5_double;
/* $$

$head Vector Types$$
$mindex d_vector a1d_vector a2d_vector a3d_vector a4d_vector$$
$codep */
	typedef CppAD::vector<double>      d_vector;
	typedef CppAD::vector<a1_double>   a1d_vector;
	typedef CppAD::vector<a2_double>   a2d_vector;
	typedef CppAD::vector<a3_double>   a3d_vector;
	typedef CppAD::vector<a4_double>   a4d_vector;
	typedef CppAD::vector<a5_double>   a5d_vector;
/* $$
$head User Defined$$
The following are $code approx_mixed$$ pure virtual functions and hence must
be defined by the user's derived class:

$subhead ran_like$$
$codep */
	virtual CppAD::vector<a5_double> ran_like(
		const CppAD::vector<a5_double>& fixed_vec  ,
		const CppAD::vector<a5_double>& random_vec
	) = 0;
/* $$
See $cref/ran_like/approx_mixed_ran_like/$$.

$subhead fix_like$$
$codep */
	virtual CppAD::vector<a1_double> fix_like(
		const CppAD::vector<a1_double>& fixed_vec
	) = 0 ;
/* $$
See $cref/fix_like/approx_mixed_fix_like/$$.

$subhead constraint$$
$codep */
	virtual CppAD::vector<a1_double> constraint(
		const CppAD::vector<a1_double>& fixed_vec
	) = 0 ;
/* $$
See $cref/constraint/approx_mixed_constraint/$$.

$subhead fatal_error$$
$codep */
	virtual void fatal_error(const std::string& error_message) = 0;
/* $$
This routine displays error message and then exits the program
(hence it does not return).

$subhead warning$$
$codep */
	virtual void warning(const std::string& warning_message) = 0;
/* $$
This routine displays a warning message and then returns.

$head constructor$$
Construct an $code approx_mixed$$ derived class object; see
$cref/derived_ctor/approx_mixed_derived_ctor/$$.
$codep */
	approx_mixed(size_t n_fixed, size_t n_random)
/* $$
$comment */
	:
	n_fixed_(n_fixed)               ,
	n_random_(n_random)             ,
	initialize_done_(false)         ,
	record_fix_like_done_(false)  ,
	record_ran_like_done_(false)       ,
	record_hes_ran_done_(false)     ,
	record_hes_fix_done_(false)     ,
	record_grad_ran_done_(false)    ,
	record_constraint_done_(false)  ,
	record_laplace_done_(3)
	{	for(size_t order = 0; order < 3; order++)
			record_laplace_done_[order] = false;
	}
/* $$
$head initialize$$
Directly after construction, use this function to initialize
the derived class object; see $cref/initialize/approx_mixed_initialize/$$.
$codep */
	void initialize(const d_vector& fixed_vec, const d_vector& random_vec);
/* $$
$head optimize_random$$
Given the fixed effects, optimize with respect to the random effects;
see  $cref/optimize_random/approx_mixed_optimize_random/$$.
$codep */
	d_vector optimize_random(
		const d_vector& fixed_vec ,
		const d_vector& random_in
	);
/* $$
$head optimize_fixed$$
Optimize the total objective with respect to the fixed effects;
see  $cref/optimize_fixed/approx_mixed_optimize_fixed/$$.
$codep */
	d_vector optimize_fixed(
		const std::string& options          ,
		const d_vector&    fixed_lower      ,
		const d_vector&    fixed_upper      ,
		const d_vector&    constraint_lower ,
		const d_vector&    constraint_upper ,
		const d_vector&    fixed_in         ,
		const d_vector&    random_in
	);
/* $$
$childtable%
	devel/approx_mixed/derived_ctor.omh%
	devel/approx_mixed/initialize.cpp%
	devel/approx_mixed/ran_like.omh%
	devel/approx_mixed/fix_like.omh%
	devel/approx_mixed/constraint.omh%
	devel/approx_mixed/optimize_random.cpp%
	devel/approx_mixed/optimize_fixed.cpp
%$$
$end
*/
private:
/*
------------------------------------------------------------------------------
$begin approx_mixed_private$$
$spell
	jac
	Jacobians
	jacobian
	hes
	eval
	typedef
	CppAD
	vec
	const
	bool
	xam
$$

$section approx_mixed Private Declarations$$

$childtable%include/dismod_at/approx_pack.hpp
	%include/dismod_at/approx_unpack.hpp
	%devel/approx_mixed/constraint_eval.cpp
	%devel/approx_mixed/constraint_jac.cpp
	%devel/approx_mixed/constraint_hes.cpp
	%devel/approx_mixed/record_ran_like.cpp
	%devel/approx_mixed/record_grad_ran.cpp
	%devel/approx_mixed/record_hes_ran.cpp
	%devel/approx_mixed/record_laplace.cpp
	%devel/approx_mixed/record_hes_fix.cpp
	%devel/approx_mixed/record_fix_like.cpp
	%devel/approx_mixed/record_constraint.cpp
	%devel/approx_mixed/ran_like_grad.cpp
	%devel/approx_mixed/ran_like_hes.cpp
	%devel/approx_mixed/laplace_eval.cpp
	%devel/approx_mixed/laplace_beta.cpp
	%devel/approx_mixed/laplace_hes_fix.cpp
	%devel/approx_mixed/prior_eval.cpp
	%devel/approx_mixed/fix_like_jac.cpp
	%devel/approx_mixed/fix_like_hes.cpp
%$$

$head n_fixed_$$
The number of fixed effects is given by
$codep */
	const size_t n_fixed_;
/* $$
$head n_random_$$
The number of random effects is given by
$codep */
	const size_t n_random_;
/* $$
$head initialize_done_$$
The following flag is false after construction and true after
the corresponding member function is called:
$codep */
	bool                initialize_done_;
	bool                record_fix_like_done_;
	bool                record_ran_like_done_;
	bool                record_hes_ran_done_;
	bool                record_hes_fix_done_;
	bool                record_grad_ran_done_;
	bool                record_constraint_done_;
	CppAD::vector<bool> record_laplace_done_; // index is order in call
/* $$
$head n_random_ > 0$$
The following values are only defined when $icode%n_random_% > 0%$$:
$cref/ran_like_/approx_mixed_private/n_random_ > 0/ran_like_/$$,
$cref/grad_ran_/approx_mixed_private/n_random_ > 0/grad_ran_/$$,
$cref/hes_ran_/approx_mixed_private/n_random_ > 0/hes_ran_/$$,
$cref/laplace_k_/approx_mixed_private/n_random_ > 0/laplace_k_/$$,
$cref/hes_fix_/approx_mixed_private/n_random_ > 0/hes_fix_/$$,

$subhead ran_like_$$
Recording of the $cref/ran_like/approx_mixed_ran_like/$$ function
which evaluates a
$cref/negative log-density vector/approx_mixed/Negative Log-Density Vector/$$
corresponding to
$cref/f(theta, u)/approx_mixed_theory/Random Negative Log-Likelihood, f(theta, u)/$$
for different levels of AD:
$codep */
	CppAD::ADFun<double>      a0_ran_like_;
	CppAD::ADFun<a1_double>   a1_ran_like_;
	CppAD::ADFun<a2_double>   a2_ran_like_;
	CppAD::ADFun<a3_double>   a3_ran_like_;
	CppAD::ADFun<a4_double>   a4_ran_like_;
/* $$
$subhead grad_ran_$$
The gradient of the random likelihood w.r.t. the random effects
$latex f_u^{(1)} ( \theta , u )^T$$. Because this is a simple vector
there is no difference between the gradient and the derivative; i.e.,
the transpose does not matter.
$codep */
	CppAD::ADFun<a3_double> grad_ran_;   // computes the gradient values
/* $$
$subhead hes_ran_$$
The Hessian of the random likelihood w.r.t. the random effects
$latex f_{uu}^{(2)} ( \theta , u )$$ is as a sparse matrix
represented by the following variables:
$codep */
	CppAD::ADFun<a3_double> hes_ran_;     // computes the hessian values
	CppAD::vector<size_t>   hes_ran_row_; // corresponding row indices
	CppAD::vector<size_t>   hes_ran_col_; // corresponding column indices
/* $$
$subhead laplace_k_$$
For $icode%k% = 0 , 1, 2%$$, $codei%laplace_%k%_%$$ is $th k$$ order accurate
in $latex \beta$$ recording of the Joint part of the Laplace approximation;
i.e., $latex H( \beta , \theta , u)$$.
$codep */
	CppAD::ADFun<double>    laplace_0_;     // for computing H
	CppAD::ADFun<double>    laplace_1_;     // for computing H_beta
	CppAD::ADFun<double>    laplace_2_;     // for computing H_beta_beta
/* $$
$subhead hes_fix_$$
Information used to calculate the sparse Hessian of the random likelihood
w.r.t. fixed effects $latex H_{\beta \beta}^{(2)} ( \beta, \theta , u )$$.
matrix
$codep */
	CppAD::vector<size_t>      hes_fix_row_; // corresponding row indices
	CppAD::vector<size_t>      hes_fix_col_; // corresponding column indices
	CppAD::sparse_hessian_work hes_fix_work_;
/* $$
Note that if $code record_hes_fix_done_$$ is true and
$code hes_fix_row_.size() == 0$$, then this Hessian is zero; i.e.,
the second derivative of the Laplace approximation is zero.

$head fix_like_$$
Recording of the $cref/fix_like/approx_mixed_fix_like/$$ function
which evaluates a
$cref/negative log-density vector/approx_mixed/Negative Log-Density Vector/$$
corresponding to
$cref/g(theta)/approx_mixed_theory/Fixed Negative Log-Likelihood, g(theta)/$$.
$codep */
	CppAD::ADFun<double>    fix_like_; // computes fixed negative log-likelihood
	CppAD::vector<size_t>   fix_like_jac_row_; // prior jacobian row indices
	CppAD::vector<size_t>   fix_like_jac_col_; // prior jacobian column indices
	CppAD::sparse_jacobian_work fix_like_jac_work_;
	CppAD::vector<size_t>   fix_like_hes_row_; // prior hessian row indices
	CppAD::vector<size_t>   fix_like_hes_col_; // prior hessian column indices
	CppAD::sparse_hessian_work fix_like_hes_work_;
/* $$
$head constraint_$$
Recording of the $cref/constraint/approx_mixed_constraint/$$ function.
$codep */
	// computes constraint function
	CppAD::ADFun<double>        constraint_;
	//
	CppAD::vector<size_t>       constraint_jac_row_; // jacobian row indices
	CppAD::vector<size_t>       constraint_jac_col_; // jacobian column indices
	CppAD::sparse_jacobian_work constraint_jac_work_;
	CppAD::vector<size_t>       constraint_hes_row_; // hessian row indices
	CppAD::vector<size_t>       constraint_hes_col_; // hessian column indices
	CppAD::sparse_hessian_work  constraint_hes_work_;
/* $$
------------------------------------------------------------------------------
$head pack$$
See $cref approx_mixed_pack$$.
$codep */
	template <class Float_unpack, class Float_pack>
	void pack(
		const CppAD::vector<Float_unpack>& fixed_one  ,
		const CppAD::vector<Float_unpack>& random_vec ,
		CppAD::vector<Float_pack>&         both_vec
	) const;
	template <class Float_unpack, class Float_pack>
	void pack(
		const CppAD::vector<Float_unpack>& fixed_one  ,
		const CppAD::vector<Float_unpack>& fixed_two  ,
		const CppAD::vector<Float_unpack>& random_vec ,
		CppAD::vector<Float_pack>&         three_vec
	) const;
/* $$
$head unpack$$
See $cref approx_mixed_unpack$$.
$codep */
	template <class Float_unpack, class Float_pack>
	void unpack(
		CppAD::vector<Float_unpack>&       fixed_one  ,
		CppAD::vector<Float_unpack>&       random_vec ,
		const CppAD::vector<Float_pack>&   both_vec
	) const;
	template <class Float_unpack, class Float_pack>
	void unpack(
		CppAD::vector<Float_unpack>&       fixed_one  ,
		CppAD::vector<Float_unpack>&       fixed_two  ,
		CppAD::vector<Float_unpack>&       random_vec ,
		const CppAD::vector<Float_pack>&   three_vec
	) const;
/* $$
------------------------------------------------------------------------------
$head record_ran_like$$
See $cref approx_mixed_record_ran_like$$.
$codep */
	void record_ran_like(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_grad_ran$$
See $cref approx_mixed_record_grad_ran$$.
$codep */
	void record_grad_ran(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_hes_ran$$
See $cref approx_mixed_record_hes_ran$$.
$codep */
	void record_hes_ran(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_laplace$$
See $cref approx_mixed_record_laplace$$.
$codep */
	void record_laplace(
		size_t          order     ,
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_hes_fix$$
See $cref approx_mixed_record_hes_fix$$.
$codep */
	void record_hes_fix(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_fix_like$$
See $cref approx_mixed_record_fix_like$$.
$codep */
	void record_fix_like(const d_vector& fixed_vec);
/* $$
$head record_constraint$$
See $cref approx_mixed_record_constraint$$.
$codep */
	void record_constraint(const d_vector& fixed_vec);
/* $$
------------------------------------------------------------------------------
$head ran_like_grad$$
See $cref approx_mixed_ran_like_grad$$
$codep */
	// ran_like_grad
	a3d_vector ran_like_grad(
		const a3d_vector&       fixed_vec   ,
		const a3d_vector&       random_vec
	);
	friend bool ::ran_like_grad_xam(void);
/* $$
$head ran_like_hes$$
See $cref approx_mixed_ran_like_hes$$
$codep */
	// ran_like_hes
	void ran_like_hes(
		const a3d_vector&       fixed_vec   ,
		const a3d_vector&       random_vec  ,
		CppAD::vector<size_t>&  row_out     ,
		CppAD::vector<size_t>&  col_out     ,
		a3d_vector&             val_out
	);
	friend bool ::ran_like_hes_xam(void);
/* $$
------------------------------------------------------------------------------
$head laplace_eval$$
See $cref approx_mixed_laplace_eval$$
$codep */
	// laplace_eval
	double laplace_eval(
		const d_vector& beta   ,
		const d_vector& theta  ,
		const d_vector& u
	);
	friend bool ::laplace_eval_xam(void);
/* $$
$head laplace_beta$$
See $cref approx_mixed_laplace_beta$$
$codep */
	// laplace_beta
	d_vector laplace_beta(
		const d_vector& beta   ,
		const d_vector& theta  ,
		const d_vector& u
	);
	friend bool ::laplace_beta_xam(void);
/* $$
$head laplace_hes_fix$$
See $cref approx_mixed_laplace_hes_fix$$
$codep */
	// laplace_hes_fix
	void laplace_hes_fix(
		const d_vector&         fixed_vec   ,
		const d_vector&         random_vec  ,
		CppAD::vector<size_t>&  row_out     ,
		CppAD::vector<size_t>&  col_out     ,
		d_vector&               val_out
	);
	friend bool ::laplace_hes_fix_xam(void);
/* $$
-------------------------------------------------------------------------------
$head constraint_eval$$
See $cref approx_mixed_constraint_eval$$
$codep */
	// constraint_eval
	d_vector constraint_eval(const d_vector& fixed_vec);
	friend bool ::constraint_eval_xam(void);
/* $$
$end
$head constraint_jac$$
See $cref approx_mixed_constraint_jac$$
$codep */
	// constraint_jac
	void constraint_jac(
		const d_vector&        fixed_vec   ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::constraint_jac_xam(void);
/* $$
$end
$head constraint_hes$$
See $cref approx_mixed_constraint_hes$$
$codep */
	// constraint_hes
	void constraint_hes(
		const d_vector&        fixed_vec   ,
		const d_vector&        weight      ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::constraint_hes_xam(void);
/* $$
-------------------------------------------------------------------------------
$head prior_eval$$
See $cref approx_mixed_prior_eval$$
$codep */
	// prior_eval
	d_vector prior_eval(const d_vector& fixed_vec);
	friend bool ::prior_eval_xam(void);
/* $$
$end
$head fix_like_jac$$
See $cref approx_mixed_fix_like_jac$$
$codep */
	// fix_like_jac
	void fix_like_jac(
		const d_vector&        fixed_vec   ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::fix_like_jac_xam(void);
/* $$
$end
$head fix_like_hes$$
See $cref approx_mixed_fix_like_hes$$
$codep */
	// fix_like_hes
	void fix_like_hes(
		const d_vector&        fixed_vec   ,
		const d_vector&        weight      ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::fix_like_hes_xam(void);
/* $$
$end
-------------------------------------------------------------------------------
*/
};

} // END_DISMOD_AT_NAMESPACE

# include <dismod_at/approx_pack.hpp>
# include <dismod_at/approx_unpack.hpp>

# endif
