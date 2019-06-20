#pragma once

namespace equations
{
	namespace gravity
	{
		void orbity_moon(bool);
		void orbity_earth(bool);
		void orbity_newmark(const double*, const double*, const double*, double, unsigned, unsigned);
		void orbity_runge_kutta(const double*, const double*, const double*, double, unsigned, unsigned);
	}
}
