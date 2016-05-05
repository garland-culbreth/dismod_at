// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin fit_model_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ fit_model: Example and Test$$

$code
$srcfile%example/devel/model/fit_model_xam.cpp%0
	%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/null_int.hpp>

bool fit_model_xam(void)
{	bool   ok = true;
	size_t i, j;
	using CppAD::vector;
	double inf = std::numeric_limits<double>::infinity();
	double nan = std::numeric_limits<double>::quiet_NaN();


	// --------------------------------------------------------------------
	// age_table = [ 0 , 20, 40, 60, 80, 100 ]
	size_t n_age_table = 6;
	vector<double> age_table(n_age_table);
	for(i = 0; i < n_age_table; i++)
		age_table[i] = 100. * i / double(n_age_table - 1);
	//
	// time_table = [ 1995, 2005, 2015 ]
	size_t n_time_table = 3;
	vector<double> time_table(n_time_table);
	for(i = 0; i < n_time_table; i++)
		time_table[i] = 1995. + (2015- 1995) * i / double(n_time_table-1);
	// ----------------------- prior table ---------------------------------
	size_t n_prior_table = 4;
	vector<dismod_at::prior_struct> prior_table(n_prior_table);
	//
	// prior_id_zero (identically zero prior)
	prior_table[0].prior_name = "zero";
	prior_table[0].density_id = int( dismod_at::uniform_enum );
	prior_table[0].lower      = 0.0;
	prior_table[0].mean       = 0.0;
	prior_table[0].upper      = 0.0;
	prior_table[0].std        = nan;
	prior_table[0].eta        = nan;
	//
	// prior_id_one (identically one prior)
	prior_table[1].prior_name = "one";
	prior_table[1].density_id = int( dismod_at::uniform_enum );
	prior_table[1].lower      = 1.0;
	prior_table[1].mean       = 1.0;
	prior_table[1].upper      = 1.0;
	prior_table[1].std        = nan;
	prior_table[1].eta        = nan;
	//
	size_t prior_id_one       = 1;
	//
	// prior_id_positive (uniform prior on positive values)
	prior_table[2].prior_name = "none";
	prior_table[2].density_id = int( dismod_at::uniform_enum );
	prior_table[2].lower      = 1e-4;
	prior_table[2].mean       = 1e-1;
	prior_table[2].upper      = + inf;
	prior_table[2].std        = nan;
	prior_table[2].eta        = nan;
	size_t prior_id_positive  = 2;
	//
	// prior_id_gaussian_zero (Gaussian mean 0.0 and std 1e-2)
	prior_table[3].prior_name       = "gaussian_0_1e-4";
	prior_table[3].density_id       = int( dismod_at::gaussian_enum );
	prior_table[3].lower            = -inf;
	prior_table[3].mean             = 0.00;
	prior_table[3].upper            = +inf;
	prior_table[3].std              = 1e-2;
	prior_table[3].eta              = nan;
	size_t prior_id_gaussian_zero   = 3;
	// -------------------------------------------------------------------
	// smoothing information
	vector<size_t> age_id, time_id;
	vector<size_t> value_prior_id, dage_prior_id, dtime_prior_id;
	size_t mulstd_value, mulstd_dage, mulstd_dtime;
	size_t n_age, n_time, n_grid;

	vector<dismod_at::smooth_info> s_info_vec(2);
	// ------------------------- smoothing --------------------------------
	// age_id
	n_age = 1;
	age_id.resize(n_age);
	age_id[0] = n_age_table / 2;
	//
	// time_id
	n_time = 2;
	time_id.resize(n_time);
	time_id[0] = 0;
	time_id[1] = n_time_table - 1;
	//
	// scalar prior_id
	mulstd_value = prior_id_one;
	mulstd_dage  = prior_id_one;
	mulstd_dtime = prior_id_one;
	//
	// prior_id values on an n_age * n_time grid
	n_grid = n_age * n_time;
	value_prior_id.resize(n_grid);
	dage_prior_id.resize(n_grid);
	dtime_prior_id.resize(n_grid);
	//
	// smooth_id_gaussian_zero
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	value_prior_id[ i * n_time + j ] = prior_id_gaussian_zero;
			dage_prior_id[ i * n_time + j ]  = prior_id_gaussian_zero;
			dtime_prior_id[ i * n_time + j ] = prior_id_gaussian_zero;
		}
	}
	size_t smooth_id_gaussian_zero = 0;
	s_info_vec[smooth_id_gaussian_zero] = dismod_at::smooth_info(
		age_table, time_table, age_id, time_id,
		value_prior_id, dage_prior_id, dtime_prior_id,
		mulstd_value, mulstd_dage, mulstd_dtime
	);
	//
	// smooth_id_positive
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
			value_prior_id[ i * n_time + j ] = prior_id_positive;
	}
	size_t smooth_id_positive = 1;
	s_info_vec[smooth_id_positive] = dismod_at::smooth_info(
		age_table, time_table, age_id, time_id,
		value_prior_id, dage_prior_id, dtime_prior_id,
		mulstd_value, mulstd_dage, mulstd_dtime
	);
	// --------------------------------------------------------------------
	// smooth_table
	vector<dismod_at::smooth_struct> smooth_table(s_info_vec.size());
	for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
	{	smooth_table[smooth_id].n_age  = s_info_vec[smooth_id].age_size();
		smooth_table[smooth_id].n_time = s_info_vec[smooth_id].time_size();
		smooth_table[smooth_id].mulstd_value_prior_id = mulstd_value;
		smooth_table[smooth_id].mulstd_dage_prior_id  = mulstd_dage;
		smooth_table[smooth_id].mulstd_dtime_prior_id = mulstd_dtime;
	}
	//
	// mulcov_table
	vector<dismod_at::mulcov_struct> mulcov_table(0);
	//
	// rate_table
	vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	if( rate_id == dismod_at::pini_enum )
		{	// smoothing must have only one age
			rate_table[rate_id].parent_smooth_id = smooth_id_positive;
			rate_table[rate_id].child_smooth_id  = smooth_id_gaussian_zero;
		}
		else
		{	// eventually plan to use a 3 by 2 smoothing here
			rate_table[rate_id].parent_smooth_id = smooth_id_positive;
			rate_table[rate_id].child_smooth_id  = smooth_id_gaussian_zero;
		}
	}
	//
	// integrand_table
	size_t n_integrand = dismod_at::number_integrand_enum;
	vector<dismod_at::integrand_struct> integrand_table(n_integrand);
	for(i = 0; i < n_integrand; i++)
	{	integrand_table[i].integrand = dismod_at::integrand_enum(i);
		integrand_table[i].eta       = 1e-6;
	}
	//
	// node_table:    0
	//                   2
	//                  3  4
	CppAD::vector<dismod_at::node_struct> node_table(4);
	node_table[0].parent = DISMOD_AT_NULL_INT;
	node_table[1].parent =  0;
	node_table[2].parent =  1;
	node_table[3].parent =  1;
	//
	// parent_node_id
	size_t parent_node_id = 0;
	//
	// pack_object
	size_t n_child     = 1;
	bool new_file = true;
	std::string file_name = "example.db";
	sqlite3* db = dismod_at::open_connection(file_name, new_file);
	dismod_at::pack_info pack_object(
		db, n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
	//
	// w_info_vec (the constant one)
	vector<double> weight(1);
	weight[0] = 1.0;
	age_id.resize(1);
	time_id.resize(1);
	age_id[0] = 0;
	time_id[0] = 0;
	dismod_at::weight_info w_info(
		age_table, time_table, age_id, time_id, weight
	);
	vector<dismod_at::weight_info> w_info_vec(1);
	w_info_vec[0] = w_info;
	//
	// covariate table
	size_t n_covariate = 0;
	vector<dismod_at::covariate_struct> covariate_table(n_covariate);
	//
	// data_table
	dismod_at::integrand_enum integrand_vec[] = {
		dismod_at::prevalence_enum , // rate_id = 0
		dismod_at::Sincidence_enum , // rate_id = 1
		dismod_at::remission_enum  , // rate_id = 2
		dismod_at::mtexcess_enum   , // rate_id = 3
		dismod_at::mtother_enum      // rate_id = 4
	};
	size_t n_data = sizeof(integrand_vec) / sizeof(integrand_vec[0]);
	vector<dismod_at::data_struct> data_table(n_data);
	vector<double> data_cov_value(n_data * n_covariate);
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	double meas_value = 1e-2 * double( data_id + 1);
		double meas_std   = 0.2  * meas_value;
		data_table[data_id].integrand_id = integrand_vec[data_id];
		data_table[data_id].node_id      = 0;
		data_table[data_id].weight_id    = 0;
		if( data_id == 0 )
		{	// prevalence at age zero is zero
			data_table[data_id].age_lower    = 0.0;
			data_table[data_id].age_upper    = 0.0;
		}
		else
		{	data_table[data_id].age_lower    = 0.0;
			data_table[data_id].age_upper    = 100.0;
		}
		data_table[data_id].time_lower   = 1995.0;
		data_table[data_id].time_upper   = 1995.0;
		data_table[data_id].meas_value   = meas_value;
		data_table[data_id].meas_std     = meas_std;
		data_table[data_id].density_id   = dismod_at::gaussian_enum;
	}
	//
	// prior_object
	dismod_at::prior_model prior_object(
		pack_object, age_table, time_table, prior_table, s_info_vec
	);
	// child_info
	dismod_at::child_info child_object(
		parent_node_id ,
		node_table     ,
		data_table
	);
	// data_subset
	vector<dismod_at::data_subset_struct> data_subset_obj;
	vector<double> data_subset_cov_value;
	data_subset(
		data_table,
		data_cov_value,
		covariate_table,
		child_object,
		data_subset_obj,
		data_subset_cov_value
	);
	//
	// data_model
	size_t n_age_ode = 6;
	size_t n_time_ode = 2;
	double ode_step_size = 20.;
	dismod_at::data_model data_object(
		parent_node_id,
		n_covariate,
		n_age_ode,
		n_time_ode,
		ode_step_size,
		age_table,
		time_table,
		integrand_table,
		node_table,
		data_subset_obj,
		data_subset_cov_value,
		w_info_vec,
		s_info_vec,
		pack_object,
		child_object
	);
	data_object.replace_like(data_subset_obj);
	//
	// start_var
	vector<size_t> pack_prior_id =
		dismod_at::pack_value_prior(pack_object, s_info_vec
	);
	vector<double> start_var( pack_object.size() );
	for(size_t var_id = 0; var_id < start_var.size(); var_id++)
	{	size_t prior_id = pack_prior_id[var_id];
		start_var[var_id] = prior_table[prior_id].mean;
	}
	// ----------------------- run the fit -------------------------------
	bool quasi_fixed = false;
	CppAD::mixed::sparse_mat_info A_info; // empty matrix
	dismod_at::fit_model fit_object(
		db,
		pack_object,
		start_var,
		prior_table,
		s_info_vec,
		data_object,
		prior_object,
		quasi_fixed,
		A_info
	);
	std::map<std::string, std::string> option_map;
	option_map["derivative_test_fixed"]    = "second-order";
	option_map["tolerance_fixed"]          = "1e-8";
	option_map["max_num_iter_fixed"]       = "100";
	option_map["print_level_fixed"]        = "0";
	option_map["fixed_bound_frac"]         = "1e-2";
	//
	option_map["derivative_test_random"]   = "second-order";
	option_map["tolerance_random"]         = "1e-8";
	option_map["max_num_iter_random"]      = "100";
	option_map["print_level_random"]       = "0";
	fit_object.run_fit( option_map );
	CppAD::vector<double> solution, lag_value, lag_dage, lag_dtime;
	fit_object.get_solution(solution, lag_value, lag_dage, lag_dtime);

	// check against known solution
	double check_tol = 1e-3;
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	double meas_value = data_table[data_id].meas_value;
		size_t rate_id    = data_id;
		dismod_at::pack_info::subvec_info info;
		for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	info = pack_object.rate_info(rate_id, child_id);
			size_t offset = info.offset;
			size_t n_var  = info.n_var;
			for(size_t i = 0; i < n_var; i++)
			{	double err = solution[offset + i] / meas_value - 1.0;
				if( child_id != n_child )
					err = solution[offset + i] / meas_value;
				ok    &= CppAD::abs( err ) <= check_tol;
			}
		}
	}
	return ok;
}
// END C++
