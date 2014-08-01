// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

/*
$begin pack_var_ctor$$
$spell
	mulcov
	CppAD
	struct
	dismod
	var
	const
	integrands
$$

$section Pack Variables: Constructor$$

$head Syntax$$
$codei%dismod_at::pack_var %var%(
	%n_integrand%, %smooth_table%, %mulcov_table%
)
%$$
$icode%size%  = %var%.size()
%$$

$head n_integrand$$
This argument has prototype
$codei%
	size_t %n_integrand%
%$$
and is the number of integrands; i.e., the size of
$cref/integrand_table/get_integrand_table/integrand_table/$$.

$head smooth_table$$
This argument has prototype
$codei%
	CppAD::vector<smooth_struct>& %smooth_table%
%$$
and is the
$cref/smooth_table/get_smooth_table/smooth_table/$$.
Only the following fields of this table are used:
$code n_age$$, $code n_time$$.

$head mulcov_table$$
This argument has prototype
$codei%
	CppAD::vector<mulcov_struct>& %mulcov_table%
%$$
and is the
$cref/mulcov_table/get_mulcov_table/mulcov_table/$$.

$head size$$
This function is $code const$$.
Its return value has prototype
$codei%
	size_t %size%
%$$
and is the total number of variables; i.e.,
the number of elements in the packed variable vector.

$head Example$$
See $cref/pack_var Example/pack_var/Example/$$.

$end
*/

# include <cppad/cppad.hpp>
# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

pack_var::pack_var(
	size_t                               n_integrand  ,
	const CppAD::vector<smooth_struct>&  smooth_table ,
	const CppAD::vector<mulcov_struct>&  mulcov_table 
) :
n_smooth_( smooth_table.size() ) ,
n_integrand_( n_integrand ) 
{	using std::string;

	// initialize offset
	size_t offset = 0;

	// offset_mulstd_
	offset_mulstd_  = offset; offset += 3 * n_smooth_;

	// meas_mean_mulcov_info_ and meas_std_mulcov_info_ 
	meas_mean_mulcov_info_.resize( n_integrand );
	meas_std_mulcov_info_.resize( n_integrand );
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t mulcov_id;
		for(mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
		{	bool match;
			match  = mulcov_table[mulcov_id].mulcov_type  == meas_mean_enum;
			match |= mulcov_table[mulcov_id].mulcov_type  == meas_std_enum;
			match &= mulcov_table[mulcov_id].integrand_id == int(integrand_id);
			if( match )
			{	size_t covariate_id = size_t(
					mulcov_table[mulcov_id].covariate_id
				); 
				string mulcov_type;
				CppAD::vector<mulcov_info>* info_vec = nullptr;
				if( mulcov_table[mulcov_id].mulcov_type == meas_mean_enum )
				{	info_vec    = &( meas_mean_mulcov_info_[integrand_id]) ;
					mulcov_type = "'meas_mean'";
				}
				if( mulcov_table[mulcov_id].mulcov_type == meas_std_enum )
				{	info_vec    = &( meas_std_mulcov_info_[integrand_id]) ;
					mulcov_type = "'meas_std'";
				}
				for(size_t j = 0; j < info_vec->size(); j++)
				{	if( (*info_vec)[j].covariate_id == covariate_id )
					{	string msg = "covariate_id appears twice with "
							"mulcov_type equal to";
						msg += mulcov_type;
						string table_name = "mulcov";
						table_error_exit(table_name, mulcov_id, msg);
					}
				}
				size_t smooth_id = mulcov_table[mulcov_id].smooth_id;
				size_t n_age     = smooth_table[smooth_id].n_age;
				size_t n_time    = smooth_table[smooth_id].n_time;
				//
				mulcov_info info;
				info.covariate_id = covariate_id;
				info.smooth_id    = smooth_id;
				info.n_var        = n_age * n_time;
				info.offset       = offset; 
				info_vec->push_back(info);
				//
				offset           += info.n_var;
			}
		}
	}

	// size is final offset
	size_ = offset;
};

// size
size_t pack_var::size(void) const
{	return size_; }

/*
$begin pack_var_mulstd$$
$spell
	var
	mulstd
	dage
	dtime
	const
	dismod
$$

$section Pack Variables: Standard Deviations Multipliers$$

$head Syntax$$
$icode%offset% = %var%.mulstd(%smooth_id%)
%$$

$head var$$
This object has prototype
$codei%
	const dismod_at::pack_var %var%
%$$

$head smooth_id$$
This argument has prototype
$codei%
	size_t %smooth_id%
%$$
and is the 
$cref/smooth_id/smooth_table/smooth_id/$$.

$subhead offset$$
The return value has prototype
$codei%
	size_t offset
%$$
and is the offset (index) in the packed variable vector
where the three variables for this smoothing begin.
The three variables for each smoothing are the
value, dage, and dtime standard deviation multipliers.

$head Example$$
See $cref/pack_var Example/pack_var/Example/$$.

$end

*/
size_t pack_var::mulstd(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return offset_mulstd_ + 3 * smooth_id;
}

/*
$begin pack_var_meas$$
$spell
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
$$

$section Pack Variables: Measurement Multipliers$$

$head Syntax$$
$icode%n_cov% = %var%.meas_mean_mulcov_n_cov(%integrand_id%)
%$$
$icode%n_cov% = %var%.meas_std_mulcov_n_cov(%integrand_id%)
%$$
$icode%info% = %var%.meas_mean_mulcov_info(%integrand_id%, %j%)
%$$
$icode%info% = %var%.meas_std_mulcov_info(%integrand_id%, %j%)
%$$

$head meas_mean$$
The functions 
$code meas_mean_mulcov_n_cov$$ and 
$code meas_mean_mulcov_info$$ 
return information about the measurement mean covariate multipliers.

$head meas_std$$
The functions 
$code meas_std_mulcov_n_cov$$ and 
$code meas_std_mulcov_info$$ 
return information about the measurement standard deviation
covariate multipliers.

$head mulcov_info$$
The type $code dismod_at::pack-var::mulcov_info$$ is defined as follows:
$code
$verbatim%dismod_at/include/pack_var.hpp
%5%// BEGIN MULCOV_INFO%// END MULCOV_INFO%$$
$$

$head var$$
This object has prototype
$codei%
	const dismod_at::pack_var %var%
%$$

$head integrand_id$$
This argument has prototype 
$codei%
	size_t %integrand_id%
%$$
and it specifies the 
$cref/integrand_id/integrand_table/integrand_id/$$ the covariate
multiplier.

$head n_cov$$
This return value has prototype
$codei%
	size_t %n_cov%
%$$ 
and is the number of covariate multipliers for the specified 
$icode integrand_id$$.
This is referred to as $codei%n_cov(%integrand_id%)%$$ below.

$head j$$
This argument has prototype
$codei%
	size_t %j%
%$$
and $icode%j% < n_cov(%integrand_id%)%$$.

$head info$$
this return value has prototype
$codei%
	mulcov_info %info%
%$$

$subhead covariate_id$$
is the $cref/covariate_id/covariate_table/covariate_id/$$ for the
$th j$$ covariate multiplier for this $icode integrand_id$$.

$subhead smooth_id$$
is the $cref/smooth_id/smooth_table/smooth_id/$$ for the
$th j$$ covariate multiplier fro this $icode integrand_id$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number are age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset in the packed variable vector where the
$icode n_var$$ variables begin (for this covariate multiplier).

$head Example$$
See $cref/pack_var Example/pack_var/Example/$$.

$end
*/
size_t 
pack_var::meas_mean_mulcov_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return meas_mean_mulcov_info_[integrand_id].size();
}
size_t 
pack_var::meas_std_mulcov_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return meas_std_mulcov_info_[integrand_id].size();
}
//
pack_var::mulcov_info 
pack_var::meas_mean_mulcov_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return meas_mean_mulcov_info_[integrand_id][j];
}
pack_var::mulcov_info 
pack_var::meas_std_mulcov_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return meas_std_mulcov_info_[integrand_id][j];
}

} // END DISMOD_AT_NAMESPACE
