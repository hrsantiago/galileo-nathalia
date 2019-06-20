#pragma once

namespace mat
{
	enum class stress : unsigned
	{
		sxx = 1 << 0,
		syy = 1 << 1,
		szz = 1 << 2,
		sxy = 1 << 3,
		sxz = 1 << 4,
		syz = 1 << 5,
		last
	};
	
	double stress_value(const double*, unsigned, unsigned);
	double* stress_vector(double*, const double*, unsigned, double = 1);
	double* stress_matrix(double*, const double*, unsigned, double = 1);

	double stress_invariant_I1(const double*, unsigned);
	double stress_invariant_I2(const double*, unsigned);
	double stress_invariant_I3(const double*, unsigned);
	double stress_principal_s1(const double*, unsigned);
	double stress_principal_s2(const double*, unsigned);
	double stress_principal_s3(const double*, unsigned);
	
	double* stress_gradient_I1(double*, const double*, unsigned);
	double* stress_gradient_I2(double*, const double*, unsigned);
	double* stress_gradient_I3(double*, const double*, unsigned);
	
	double* stress_hessian_I2(double*, const double*, unsigned);
	double* stress_hessian_I3(double*, const double*, unsigned);

	double hydrostatic_stress(const double*, unsigned);
	double* hydrostatic_gradient(double*, const double*, unsigned);

	double deviatoric_invariant_J2(const double*, unsigned);
	double deviatoric_invariant_J3(const double*, unsigned);
	double deviatoric_principal_s1(const double*, unsigned);
	double deviatoric_principal_s2(const double*, unsigned);
	double deviatoric_principal_s3(const double*, unsigned);
	
	double* deviatoric_gradient_J2(double*, const double*, unsigned);
	double* deviatoric_gradient_J3(double*, const double*, unsigned);
	
	double* deviatoric_hessian_J2(double*, const double*, unsigned);
	double* deviatoric_hessian_J3(double*, const double*, unsigned);

	double von_mises_stress(const double*, unsigned);
	double* von_mises_gradient(double*, const double*, unsigned);
	double* von_mises_hessian(double*, const double*, unsigned);

	double lode_angle(const double*, unsigned);
}
