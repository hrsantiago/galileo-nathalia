#include <cmath>
#include <ctime>
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <algorithm>

#include "course/course.h"
#include "equations/equations.h"
#include "benchmarks/benchmarks.h"

int main(int argc, char* argv[])
{
	//tests
//	tests::dof::static_linear();
//	tests::dof::dynamic_linear();
//	tests::dof::static_nonlinear();
//	tests::dof::dynamic_nonlinear();

//	tests::bar::buckling::single_bar();
//	tests::bar::buckling::von_mises_truss_2D();

//	tests::bar::static_linear::truss_carol();

//	tests::bar::static_nonlinear::tower();
//	tests::bar::static_nonlinear::tent_unit();
//	tests::bar::static_nonlinear::tent_roof();
//	tests::bar::static_nonlinear::dome_star();
//	tests::bar::static_nonlinear::cantilever();
//	tests::bar::dynamic_nonlinear::tent_roof();
//	tests::bar::dynamic_nonlinear::single_pendulum_2D();
//	tests::bar::dynamic_nonlinear::single_pendulum_3D();
//	tests::bar::dynamic_nonlinear::double_pendulum_2D();

//	tests::cable::modal::tower();
//	tests::cable::modal::cable();
//	tests::cable::modal::mixed();
//	tests::cable::buckling::tower();
//	tests::cable::buckling::cable();
//	tests::cable::buckling::mixed();
//	tests::cable::static_nonlinear::line_bridge();
//	tests::cable::dynamic_nonlinear::line_bridge();

//	tests::beam::buckling::column_buckling();

//	tests::beam::static_nonlinear::elastic::frame_lee();
//	tests::beam::static_nonlinear::elastic::hockling_cable();
//	tests::beam::static_nonlinear::elastic::frame_buckling();
//	tests::beam::static_nonlinear::elastic::column_buckling();
//	tests::beam::static_nonlinear::elastic::cantilever_tip_moment();

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

//	tests::deployable::modal::slut_unit();
//	tests::deployable::state::slut_grid();
//	tests::deployable::state::arch_polar();
//	tests::deployable::buckling::slut_unit();
//	tests::deployable::static_linear::arch_polar();
//	tests::deployable::static_linear::arch_trans();
//	tests::deployable::dynamic_linear::slut_unit();
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

	//std::vector<double> pretensions = {0.05, 0.10, 0.25, 0.5, 0.75, 1};
//	std::vector<double> pretensions = {0.05};
//	for(double pretension : pretensions)
//		tests::tensegrity::static_nonlinear::pentagon(pretension);
//	for(double pretension = 0.015; pretension <= 1; pretension += 0.01)
//		tests::tensegrity::static_nonlinear::pentagon(pretension);
	tests::tensegrity::modal::pentagon();

//	tests::plane::static_linear::axial(); 

//	tests::solid::static_linear::axial();
//	tests::solid::static_linear::hinge();

	//equations
//	equations::tent::dynamic::vertical::frequency();

//	equations::electromag::solve();
//	equations::pendulum::double_2D::newmark();

	//course
//	course::modal::truss();
//	course::modal::frame();

//	course::static_linear::truss();
//	course::static_linear::frame();

//	course::dynamic_linear::truss();
//	course::dynamic_linear::frame();

	//return
	return 0;
}
