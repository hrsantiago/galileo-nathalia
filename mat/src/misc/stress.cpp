//std
#include <cmath>

//mat
#include "misc/defs.h"
#include "misc/util.h"
#include "misc/stress.h"

namespace mat
{
	double stress_value(const double* s, unsigned t, unsigned c)
	{
		return t & c ? s[bit_find(t, c)] : 0;
	}
	double* stress_vector(double* vp, const double* vc, unsigned t, double p)
	{
		for(unsigned i = 0, c = 0; i < 6; i++)
		{
			if(t & 1 << i)
			{
				vp[c++] = vc[i] / p;
			}
		}
		return vp;
	}
	double* stress_matrix(double* mp, const double* mc, unsigned t, double p)
	{
		const unsigned n = bit_find(t, (unsigned) stress::last);
		for(unsigned i = 0, a = 0; i < 6; i++)
		{
			if(t & 1 << i)
			{
				for(unsigned j = 0, b = 0; j < 6; j++)
				{
					if(t & 1 << j)
					{
						mp[n * b + a] = mc[6 * j + i] / p;
						b++;
					}
				}
				a++;
			}
		}
		return mp;
	}
	
	double stress_invariant_I1(const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		//return
		return sxx + syy + szz;
	}
	double stress_invariant_I2(const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//return
		return sxx * syy + sxx * szz + syy * szz - sxy * sxy - sxz * sxz - syz * syz;
	}
	double stress_invariant_I3(const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//return
		return sxx * syy * szz - 2 * sxy * sxz * syz - sxx * syz * syz - syy * sxz * sxz - szz * sxy * sxy;
	}
	
	double stress_principal_s1(const double* s, unsigned t)
	{
		const double tl = lode_angle(s, t);
		const double sm = hydrostatic_stress(s, t);
		const double J2 = deviatoric_invariant_J2(s, t);
		return sm + 2 * sqrt(J2 / 3) * cos(tl + 2 * M_PI / 3);
	}
	double stress_principal_s2(const double* s, unsigned t)
	{
		const double tl = lode_angle(s, t);
		const double sm = hydrostatic_stress(s, t);
		const double J2 = deviatoric_invariant_J2(s, t);
		return sm + 2 * sqrt(J2 / 3) * cos(tl - 2 * M_PI / 3);
	}
	double stress_principal_s3(const double* s, unsigned t)
	{
		const double tl = lode_angle(s, t);
		const double sm = hydrostatic_stress(s, t);
		const double J2 = deviatoric_invariant_J2(s, t);
		return sm + 2 * sqrt(J2 / 3) * cos(tl);
	}
	
	double* stress_gradient_I1(double* dI1, const double*, unsigned t)
	{
		//I1 gradient
		const double dIc[] = {1, 1, 1, 0, 0, 0};
		//return
		return stress_vector(dI1, dIc, t);
	}
	double* stress_gradient_I2(double* dI2, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//I2 gradient
		double dIc[6];
		dIc[3] = -2 * sxy;
		dIc[4] = -2 * sxz;
		dIc[5] = -2 * syz;
		dIc[0] = syy + szz;
		dIc[1] = sxx + szz;
		dIc[2] = sxx + syy;
		//return
		return stress_vector(dI2, dIc, t);
	}
	double* stress_gradient_I3(double* dI3, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//I3 gradient
		double dIc[6];
		dI3[0] = syy * szz - syz * syz;
		dI3[1] = sxx * szz - sxz * sxz;
		dI3[2] = sxx * syy - sxy * sxy;
		dI3[3] = -2 * (sxy * szz + sxz * syz);
		dI3[4] = -2 * (sxz * syy + sxy * syz);
		dI3[5] = -2 * (syz * sxx + sxy * sxz);
		//return
		return stress_vector(dI3, dIc, t);
	}
	
	double* stress_hessian_I2(double* d2I2, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//I2 hessian
		double d2Ic[36];
		d2Ic[6 * 0 + 0] = 0;
		d2Ic[6 * 1 + 1] = 0;
		d2Ic[6 * 2 + 2] = 0;
		d2Ic[6 * 3 + 3] = -2;
		d2Ic[6 * 4 + 4] = -2;
		d2Ic[6 * 6 + 5] = -2;
		d2Ic[6 * 1 + 0] = d2Ic[6 * 0 + 1] = 1;
		d2Ic[6 * 2 + 0] = d2Ic[6 * 0 + 2] = 1;
		d2Ic[6 * 2 + 1] = d2Ic[6 * 1 + 2] = 1;
		d2Ic[6 * 3 + 0] = d2Ic[6 * 0 + 3] = 0;
		d2Ic[6 * 4 + 0] = d2Ic[6 * 0 + 4] = 0;
		d2Ic[6 * 5 + 0] = d2Ic[6 * 0 + 5] = 0;
		d2Ic[6 * 3 + 1] = d2Ic[6 * 1 + 3] = 0;
		d2Ic[6 * 4 + 1] = d2Ic[6 * 1 + 4] = 0;
		d2Ic[6 * 5 + 1] = d2Ic[6 * 1 + 5] = 0;
		d2Ic[6 * 3 + 2] = d2Ic[6 * 2 + 3] = 0;
		d2Ic[6 * 4 + 2] = d2Ic[6 * 2 + 4] = 0;
		d2Ic[6 * 5 + 2] = d2Ic[6 * 2 + 5] = 0;
		d2Ic[6 * 4 + 3] = d2Ic[6 * 3 + 4] = 0;
		d2Ic[6 * 5 + 3] = d2Ic[6 * 3 + 5] = 0;
		d2Ic[6 * 5 + 4] = d2Ic[6 * 4 + 5] = 0;
		//return
		return stress_matrix(d2I2, d2Ic, t);
	}
	double* stress_hessian_I3(double* d2I3, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//I3 hessian
		double d2Ic[36];
		d2Ic[6 * 0 + 0] = 0;
		d2Ic[6 * 1 + 1] = 0;
		d2Ic[6 * 2 + 2] = 0;
		d2Ic[6 * 3 + 3] = -2 * szz;
		d2Ic[6 * 4 + 4] = -2 * syy;
		d2Ic[6 * 5 + 5] = -2 * sxx;
		d2Ic[6 * 3 + 0] = d2Ic[6 * 0 + 3] = 0;
		d2Ic[6 * 4 + 0] = d2Ic[6 * 0 + 4] = 0;
		d2Ic[6 * 3 + 1] = d2Ic[6 * 1 + 3] = 0;
		d2Ic[6 * 5 + 1] = d2Ic[6 * 1 + 5] = 0;
		d2Ic[6 * 4 + 2] = d2Ic[6 * 2 + 4] = 0;
		d2Ic[6 * 5 + 2] = d2Ic[6 * 2 + 5] = 0;
		d2Ic[6 * 1 + 0] = d2Ic[6 * 0 + 1] = szz;
		d2Ic[6 * 2 + 0] = d2Ic[6 * 0 + 2] = syy;
		d2Ic[6 * 2 + 1] = d2Ic[6 * 1 + 2] = sxx;
		d2Ic[6 * 5 + 0] = d2Ic[6 * 0 + 5] = -2 * syz;
		d2Ic[6 * 4 + 1] = d2Ic[6 * 1 + 4] = -2 * sxz;
		d2Ic[6 * 3 + 2] = d2Ic[6 * 2 + 3] = -2 * sxy;
		d2Ic[6 * 4 + 3] = d2Ic[6 * 3 + 4] = -2 * syz;
		d2Ic[6 * 5 + 3] = d2Ic[6 * 3 + 5] = -2 * sxz;
		d2Ic[6 * 5 + 4] = d2Ic[6 * 4 + 5] = -2 * sxy;
		//return
		return stress_matrix(d2I3, d2Ic, t);
	}
	
	double hydrostatic_stress(const double* s, unsigned t)
	{
		return stress_invariant_I1(s, t) / 3;
	}
	double* hydrostatic_gradient(double* dsm, const double*, unsigned t)
	{
		const double dIc[] = {1, 1, 1, 0, 0, 0};
		return stress_vector(dsm, dIc, t, 3);
	}
	
	double deviatoric_invariant_J2(const double* s, unsigned t)
	{
		const double I1 = stress_invariant_I1(s, t);
		const double I2 = stress_invariant_I2(s, t);
		return I1 * I1 / 3 - I2;
	}
	double deviatoric_invariant_J3(const double* s, unsigned t)
	{
		const double I1 = stress_invariant_I1(s, t);
		const double I2 = stress_invariant_I2(s, t);
		const double I3 = stress_invariant_I3(s, t);
		return 2 * I1 * I1 * I1 / 27 - I1 * I2 / 3 + I3;
	}
	double deviatoric_principal_s1(const double* s, unsigned t)
	{
		return stress_principal_s1(s, t) - hydrostatic_stress(s, t);
	}
	double deviatoric_principal_s2(const double* s, unsigned t)
	{
		return stress_principal_s2(s, t) - hydrostatic_stress(s, t);
	}
	double deviatoric_principal_s3(const double* s, unsigned t)
	{
		return stress_principal_s3(s, t) - hydrostatic_stress(s, t);
	}
	
	double* deviatoric_gradient_J2(double* dJ2, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//J2 gradient
		double dJc[6];
		dJc[3] = 2 * sxy;
		dJc[4] = 2 * sxz;
		dJc[5] = 2 * syz;
		dJc[0] = (2 * sxx - syy - szz) / 3;
		dJc[1] = (2 * syy - sxx - szz) / 3;
		dJc[2] = (2 * szz - sxx - syy) / 3;
		//return
		return stress_vector(dJ2, dJc, t);
	}
	double* deviatoric_gradient_J3(double* dJ3, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//J3 gradient
		double dJc[6];
		dJc[3] = (2 * sxy * (sxx + syy - 2 * szz)) / 3 - 2 * sxz*syz;
		dJc[4] = (2 * sxz * (sxx + szz - 2 * syy)) / 3 - 2 * sxy*syz;
		dJc[5] = (2 * syz * (syy + szz - 2 * sxx)) / 3 - 2 * sxy*sxz;
		dJc[0] = (2 * sxx * sxx - syy * syy - szz * szz + 4 * syy * szz - 2 * sxx * (syy + szz) + 3 * (sxy * sxy + sxz * sxz - 2 * syz * syz)) / 9;
		dJc[1] = (2 * syy * syy - sxx * sxx - szz * szz + 4 * sxx * szz - 2 * syy * (sxx + szz) + 3 * (sxy * sxy + syz * syz - 2 * sxz * sxz)) / 9;
		dJc[2] = (2 * szz * szz - sxx * sxx - syy * syy + 4 * sxx * syy - 2 * szz * (sxx + syy) + 3 * (sxz * sxz + syz * syz - 2 * sxy * sxy)) / 9;
		//return
		return stress_vector(dJ3, dJc, t);
	}
	
	double* deviatoric_hessian_J2(double* d2J2, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//J2 hessian
		double d2Jc[36];
		d2Jc[6 * 3 + 3] = 2;
		d2Jc[6 * 4 + 4] = 2;
		d2Jc[6 * 5 + 5] = 2;
		d2Jc[6 * 0 + 0] = (double) 2 / 3;
		d2Jc[6 * 1 + 1] = (double) 2 / 3;
		d2Jc[6 * 2 + 2] = (double) 2 / 3;
		d2Jc[6 * 3 + 0] = d2Jc[6 * 0 + 3] = 0;
		d2Jc[6 * 4 + 0] = d2Jc[6 * 0 + 4] = 0;
		d2Jc[6 * 5 + 0] = d2Jc[6 * 0 + 5] = 0;
		d2Jc[6 * 3 + 1] = d2Jc[6 * 1 + 3] = 0;
		d2Jc[6 * 4 + 1] = d2Jc[6 * 1 + 4] = 0;
		d2Jc[6 * 5 + 1] = d2Jc[6 * 1 + 5] = 0;
		d2Jc[6 * 3 + 2] = d2Jc[6 * 2 + 3] = 0;
		d2Jc[6 * 4 + 2] = d2Jc[6 * 2 + 4] = 0;
		d2Jc[6 * 5 + 2] = d2Jc[6 * 2 + 5] = 0;
		d2Jc[6 * 4 + 3] = d2Jc[6 * 3 + 4] = 0;
		d2Jc[6 * 5 + 3] = d2Jc[6 * 3 + 5] = 0;
		d2Jc[6 * 5 + 4] = d2Jc[6 * 4 + 5] = 0;
		d2Jc[6 * 1 + 0] = d2Jc[6 * 0 + 1] = -(double) 1 / 3;
		d2Jc[6 * 2 + 0] = d2Jc[6 * 0 + 2] = -(double) 1 / 3;
		d2Jc[6 * 2 + 1] = d2Jc[6 * 1 + 2] = -(double) 1 / 3;
		//return 
		return stress_matrix(d2J2, d2Jc, t);
	}
	double* deviatoric_hessian_J3(double* d2J3, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//J3 hessian
		double d2Jc[36];
		d2Jc[6 * 0 + 0] = 2 * (2 * sxx - syy - szz) / 9;
		d2Jc[6 * 1 + 1] = 2 * (2 * syy - sxx - szz) / 9;
		d2Jc[6 * 2 + 2] = 2 * (2 * szz - sxx - syy) / 9;
		d2Jc[6 * 3 + 3] = 2 * (sxx + syy - 2 * szz) / 3;
		d2Jc[6 * 4 + 4] = 2 * (sxx + szz - 2 * syy) / 3;
		d2Jc[6 * 5 + 5] = 2 * (syy + szz - 2 * sxx) / 3;
		d2Jc[6 * 4 + 3] = d2Jc[6 * 3 + 4] = -2 * syz;
		d2Jc[6 * 5 + 3] = d2Jc[6 * 3 + 5] = -2 * sxz;
		d2Jc[6 * 5 + 4] = d2Jc[6 * 4 + 5] = -2 * sxy;
		d2Jc[6 * 5 + 0] = d2Jc[6 * 0 + 5] = -4 * syz / 3;
		d2Jc[6 * 4 + 1] = d2Jc[6 * 1 + 4] = -4 * sxz / 3;
		d2Jc[6 * 3 + 2] = d2Jc[6 * 2 + 3] = -4 * sxy / 3;
		d2Jc[6 * 3 + 0] = d2Jc[6 * 0 + 3] = +2 * sxy / 3;
		d2Jc[6 * 4 + 0] = d2Jc[6 * 0 + 4] = +2 * sxz / 3;
		d2Jc[6 * 3 + 1] = d2Jc[6 * 1 + 3] = +2 * sxy / 3;
		d2Jc[6 * 5 + 1] = d2Jc[6 * 1 + 5] = +2 * syz / 3;
		d2Jc[6 * 4 + 2] = d2Jc[6 * 2 + 4] = +2 * sxz / 3;
		d2Jc[6 * 5 + 2] = d2Jc[6 * 2 + 5] = +2 * syz / 3;
		d2Jc[6 * 1 + 0] = d2Jc[6 * 0 + 1] = 2 * (2 * szz - sxx - syy) / 9;
		d2Jc[6 * 2 + 0] = d2Jc[6 * 0 + 2] = 2 * (2 * syy - sxx - szz) / 9;
		d2Jc[6 * 2 + 1] = d2Jc[6 * 1 + 2] = 2 * (2 * sxx - syy - szz) / 9;
		//return
		return stress_matrix(d2J3, d2Jc, t);
	}
	
	double von_mises_stress(const double* s, unsigned t)
	{
		return sqrt(3 * deviatoric_invariant_J2(s, t));
	}
	double* von_mises_gradient(double* dse, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//von misses stress
		const double se = von_mises_stress(s, t);
		//von misses gradient
		double dsc[6];
		dsc[3] = se != 0 ? 3 * sxy / se : 1;
		dsc[4] = se != 0 ? 3 * sxz / se : 1;
		dsc[5] = se != 0 ? 3 * syz / se : 1;
		dsc[0] = se != 0 ? (sxx - syy / 2 - szz / 2) / se : 1;
		dsc[1] = se != 0 ? (syy - sxx / 2 - szz / 2) / se : 1;
		dsc[2] = se != 0 ? (szz - sxx / 2 - syy / 2) / se : 1;
		//return
		return stress_vector(dse, dsc, t);
	}
	double* von_mises_hessian(double* d2se, const double* s, unsigned t)
	{
		//stress
		const double sxx = stress_value(s, t, (unsigned) stress::sxx);
		const double syy = stress_value(s, t, (unsigned) stress::syy);
		const double szz = stress_value(s, t, (unsigned) stress::szz);
		const double sxy = stress_value(s, t, (unsigned) stress::sxy);
		const double sxz = stress_value(s, t, (unsigned) stress::sxz);
		const double syz = stress_value(s, t, (unsigned) stress::syz);
		//von misses stress
		const double se = von_mises_stress(s, t);
		//von mises hessian
		double d2sc[36];
		d2sc[6 * 0 + 0] = pow(syy - szz, 2) + 4 * (sxy * sxy + sxz * sxz + syz * syz);
		d2sc[6 * 1 + 1] = pow(sxx - szz, 2) + 4 * (sxy * sxy + sxz * sxz + syz * syz);
		d2sc[6 * 2 + 2] = pow(sxx - syy, 2) + 4 * (sxy * sxy + sxz * sxz + syz * syz);
		d2sc[6 * 3 + 3] = 4 * (sxx * sxx + syy * syy + szz * szz - syy * szz - sxx * (syy + szz) + 3 * (sxz * sxz + syz * syz));
		d2sc[6 * 4 + 4] = 4 * (sxx * sxx + syy * syy + szz * szz - syy * szz - sxx * (syy + szz) + 3 * (sxy * sxy + syz * syz));
		d2sc[6 * 5 + 5] = 4 * (sxx * sxx + syy * syy + szz * szz - syy * szz - sxx * (syy + szz) + 3 * (sxy * sxy + sxz * sxz));
		d2sc[6 * 4 + 3] = d2sc[6 * 3 + 4] = -12 * sxy * sxz;
		d2sc[6 * 5 + 3] = d2sc[6 * 3 + 5] = -12 * sxy * syz;
		d2sc[6 * 5 + 4] = d2sc[6 * 4 + 5] = -12 * sxz * syz;
		d2sc[6 * 3 + 0] = d2sc[6 * 0 + 3] = 2 * sxy * (syy + szz - 2 * sxx);
		d2sc[6 * 4 + 0] = d2sc[6 * 0 + 4] = 2 * sxz * (syy + szz - 2 * sxx);
		d2sc[6 * 5 + 0] = d2sc[6 * 0 + 5] = 2 * syz * (syy + szz - 2 * sxx);
		d2sc[6 * 3 + 1] = d2sc[6 * 1 + 3] = 2 * sxy * (sxx + szz - 2 * syy);
		d2sc[6 * 4 + 1] = d2sc[6 * 1 + 4] = 2 * sxz * (sxx + szz - 2 * syy);
		d2sc[6 * 5 + 1] = d2sc[6 * 1 + 5] = 2 * syz * (sxx + szz - 2 * syy);
		d2sc[6 * 3 + 2] = d2sc[6 * 2 + 3] = 2 * sxy * (sxx + syy - 2 * szz);
		d2sc[6 * 4 + 2] = d2sc[6 * 2 + 4] = 2 * sxz * (sxx + syy - 2 * szz);
		d2sc[6 * 5 + 2] = d2sc[6 * 2 + 5] = 2 * syz * (sxx + syy - 2 * szz);
		d2sc[6 * 1 + 0] = d2sc[6 * 0 + 1] = (sxx - szz) * (szz - syy) - 2 * (sxy * sxy + sxz * sxz + syz * syz);
		d2sc[6 * 2 + 0] = d2sc[6 * 0 + 2] = (sxx - syy) * (syy - szz) - 2 * (sxy * sxy + sxz * sxz + syz * syz);
		d2sc[6 * 2 + 1] = d2sc[6 * 1 + 2] = (sxx - syy) * (szz - sxx) - 2 * (sxy * sxy + sxz * sxz + syz * syz);
		//return
		return stress_matrix(d2se, d2sc, t, se == 0 ? 1 : 4 * se * se * se / 3);
	}
	
	double lode_angle(const double* s, unsigned t)
	{
		//deviatoric invariants
		const double J2 = deviatoric_invariant_J2(s, t);
		const double J3 = deviatoric_invariant_J3(s, t);
		//lode angle
		return fabs(J2) > 1e-5 ? acos(3 * sqrt(3) / 2 * J3 / sqrt(J2 * J2 * J2)) / 3 : 0;
	}
}
