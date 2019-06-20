#include <cmath>
#include <ctime>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <algorithm>

#include "linear/dense.h"
#include "linear/linear.h"

#include "equations/equations.h"
#include "benchmarks/benchmarks.h"

int main(int, char*[])
{
	//tests
//	tests::dof::static_linear();
//	tests::dof::dynamic_linear();
//	tests::dof::static_nonlinear();
//	tests::dof::dynamic_nonlinear();

//	tests::bar::static_nonlinear::tent_unit();
//	tests::bar::static_nonlinear::tent_roof();
//	tests::bar::dynamic_nonlinear::tent_roof();
//	tests::bar::dynamic_nonlinear::double_pendulum_2D();

//	tests::beam::static_nonlinear::elastic::frame_lee();
//	tests::beam::static_nonlinear::elastic::hockling_cable();
//	tests::beam::static_nonlinear::elastic::column_buckling();

//	tests::beam::static_nonlinear::inelastic::steel::point_test_3();
//	tests::beam::static_nonlinear::inelastic::steel::point_test_4();
//	tests::beam::static_nonlinear::inelastic::steel::column_buckling();
//	tests::beam::static_nonlinear::inelastic::steel::cantilever_tip_force();
//	tests::beam::static_nonlinear::inelastic::steel::cantilever_tip_moment();
//	tests::beam::static_nonlinear::inelastic::steel::propped_force_midspan();
//	tests::beam::static_nonlinear::inelastic::steel::propped_force_distributed();

//	tests::joint::hinge::shear();
//	tests::joint::rigid::motion();
//	tests::joint::rigid::spring();
//	tests::joint::hinge::bending();
//	tests::joint::hinge::torsion();
//	tests::joint::pinned::scissor();
//	tests::joint::hinge::cantilever();
//	tests::joint::pinned::multi_scissor();
//	tests::joint::hinge::two_beams_axial();
//	tests::joint::hinge::two_beams_torsion();

//	tests::deployable::state::arch_polar();
//	tests::deployable::static_linear::arch_polar();
//	tests::deployable::static_linear::arch_trans();
//	tests::deployable::static_nonlinear::tent_unit();
//	tests::deployable::static_nonlinear::tent_size();
//	tests::deployable::static_nonlinear::slut_fric();
//	tests::deployable::static_nonlinear::slut_mult();
//	tests::deployable::static_nonlinear::slut_beam();
//	tests::deployable::static_nonlinear::slut_unit();
//	tests::deployable::static_nonlinear::slut_line();
//	tests::deployable::static_nonlinear::slut_wall();
//	tests::deployable::static_nonlinear::slut_roof();
//	tests::deployable::static_nonlinear::slut_test();
//	tests::deployable::static_nonlinear::arch_polar();
//	tests::deployable::static_nonlinear::arch_trans();

	tests::tensegrity::static_nonlinear::pentagon();

//	tests::solid::static_linear::axial();
//	tests::solid::static_linear::hinge();

//	equations::tent::dynamic::vertical::frequency();

//	equations::electromag::solve();
//	equations::pendulum::double_2D::newmark();

	//return
	return 0;
}
