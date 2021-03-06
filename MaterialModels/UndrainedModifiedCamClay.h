#ifndef __Undrained_Modified_Cam_Clay_h__
#define __Undrained_Modified_Cam_Clay_h__

#include <cmath>

#include "ModifiedCamClay.h"
#include "MaterialModel.h"

namespace Model_hdf5_utilities
{
	struct UndrainedModifiedCamClayStateData;
}

namespace MatModel
{
	int undrained_modified_cam_clay_integration_function(MaterialModel* _self, double dstrain[6]);

	// This model uses constant stiffness instead of
	// deducing it from recompression line
	class UndrainedModifiedCamClay : public MaterialModel
	{
		friend int undrained_modified_cam_clay_integration_function(MaterialModel* _self, double dstrain[6]);
		friend struct Model_hdf5_utilities::UndrainedModifiedCamClayStateData;
	protected:
		ModifiedCamClay mcc;
		double Kw;
		double pore_pressure;

		double Kw_div_n;

		inline void init_cal_var() noexcept
		{
			Kw_div_n = Kw * (1.0 / get_e_by_model() + 1.0);
			cal_total_stress();
			cal_Kmat_with_Kw();
		}

		inline void cal_total_stress_inc(double dp) noexcept
		{
			const double* eff_stress_inc = mcc.get_dstress();
			dstress[0] = eff_stress_inc[0] - dp;
			dstress[1] = eff_stress_inc[1] - dp;
			dstress[2] = eff_stress_inc[2] - dp;
			dstress[3] = eff_stress_inc[3];
			dstress[4] = eff_stress_inc[4];
			dstress[5] = eff_stress_inc[5];
		}

		inline void cal_total_stress() noexcept
		{
			const double* eff_stress = mcc.get_stress();
			stress[0] = eff_stress[0] - pore_pressure;
			stress[1] = eff_stress[1] - pore_pressure;
			stress[2] = eff_stress[2] - pore_pressure;
			stress[3] = eff_stress[3];
			stress[4] = eff_stress[4];
			stress[5] = eff_stress[5];
		}

		inline void cal_Kmat_with_Kw() noexcept
		{
			const double(*eff_De)[6] = reinterpret_cast<const double (*)[6]>(mcc.get_De_mat());
			const double(*eff_Dep)[6] = reinterpret_cast<const double(*)[6]>(mcc.get_Dep_mat());
			for (size_t i = 0; i < 6; ++i)
				for (size_t j = 0; j < 6; ++j)
				{
					De_mat[i][j] = eff_De[i][j];
					Dep_mat[i][j] = eff_Dep[i][j];
				}
			// De
			De_mat[0][0] += Kw_div_n;
			De_mat[0][1] += Kw_div_n;
			De_mat[0][2] += Kw_div_n;
			De_mat[1][0] += Kw_div_n;
			De_mat[1][1] += Kw_div_n;
			De_mat[1][2] += Kw_div_n;
			De_mat[2][0] += Kw_div_n;
			De_mat[2][1] += Kw_div_n;
			De_mat[2][2] += Kw_div_n;
			// Dep
			Dep_mat[0][0] += Kw_div_n;
			Dep_mat[0][1] += Kw_div_n;
			Dep_mat[0][2] += Kw_div_n;
			Dep_mat[1][0] += Kw_div_n;
			Dep_mat[1][1] += Kw_div_n;
			Dep_mat[1][2] += Kw_div_n;
			Dep_mat[2][0] += Kw_div_n;
			Dep_mat[2][1] += Kw_div_n;
			Dep_mat[2][2] += Kw_div_n;
		}

	public:
		UndrainedModifiedCamClay() :
			MaterialModel(undrained_modified_cam_clay_integration_function, "UndrainedModifiedCamClay"),
			Kw(0.0), pore_pressure(0.0)
		{
			for (size_t i = 0; i < 6; ++i)
			{
				stress[i] = 0.0;
				dstress[i] = 0.0;
				dstrain_e[i] = 0.0;
				dstrain_p[i] = 0.0;
			}
		}
		~UndrainedModifiedCamClay() {}

		inline void set_Kw(double _Kw) noexcept { Kw = _Kw; }
		inline void set_pore_pressure(double _pore_pressure) { pore_pressure = _pore_pressure; }

		// angle unit is degree
		inline void set_param_NC(
			double _niu,
			double _kappa,
			double _lambda,
			double _fric_ang,
			double _N, // void ratio at p = kPa on NCL
			double _s[6],
			double _Kw,
			double _pp = 0.0
			)
		{
			Kw = _Kw;
			pore_pressure = _pp;
			stress[0] = _s[0];
			stress[1] = _s[1];
			stress[2] = _s[2];
			stress[3] = _s[3];
			stress[4] = _s[4];
			stress[5] = _s[5];
			double eff_s[6];
			eff_s[0] = _s[0] + _pp;
			eff_s[1] = _s[1] + _pp;
			eff_s[2] = _s[2] + _pp;
			eff_s[3] = _s[3];
			eff_s[4] = _s[4];
			eff_s[5] = _s[5];
			mcc.set_param_NC(_niu, _kappa, _lambda, _fric_ang, _N, eff_s);
			init_cal_var();
		}

		inline void set_param_OC(
			double _niu,
			double _kappa,
			double _lambda,
			double _fric_ang,
			double _N,
			double _s[6],
			double _pc,
			double _Kw,
			double _pp = 0.0
			)
		{
			Kw = _Kw;
			pore_pressure = _pp;
			stress[0] = _s[0];
			stress[1] = _s[1];
			stress[2] = _s[2];
			stress[3] = _s[3];
			stress[4] = _s[4];
			stress[5] = _s[5];
			double eff_s[6];
			eff_s[0] = _s[0] + _pp;
			eff_s[1] = _s[1] + _pp;
			eff_s[2] = _s[2] + _pp;
			eff_s[3] = _s[3];
			eff_s[4] = _s[4];
			eff_s[5] = _s[5];
			mcc.set_param_OC(_niu, _kappa, _lambda, _fric_ang, _N, eff_s, _pc);
			init_cal_var();
		}

		inline const double *get_stress() noexcept { return stress; }
		inline const double *get_effective_stress() noexcept { return mcc.get_stress(); }
		inline double get_p() { return (s11 + s22 + s33) / 3.0;; }
		inline double get_effective_p() const noexcept { return mcc.get_p(); }
		inline double get_pore_pressure() { return pore_pressure; }
		inline double get_q() const noexcept { return mcc.get_q(); }
		inline double get_pc() const noexcept { return mcc.get_pc(); }
		inline double get_e_by_strain() const noexcept { return mcc.get_e_by_strain(); }
		inline double get_e_by_model() const noexcept { return mcc.get_e_by_model(); }
		inline double get_f() const noexcept { return mcc.get_f(); }
		inline double get_norm_f() const noexcept { return mcc.get_norm_f(); }
	};
}

#endif