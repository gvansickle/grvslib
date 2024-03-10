/*
 * Copyright 2024 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of grvslib.
 *
 * grvslib is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * grvslib is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * grvslib.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file A number of functions to calculate basic EE things like cutoff frequencies of RC filters, the resistance of two
 *       parallel resistors, etc.
 */

#ifndef GRVSLIB_BASIC_CALCULATORS_H
#define GRVSLIB_BASIC_CALCULATORS_H

#include <cmath>

/**
 * Calculates the cutoff frequency of an RC section.
 *
 * @tparam T  The value type of the parameters.
 * @param r   The resistance, in ohms.
 * @param c   The capacitance, in Farads.
 * @return    The calculated cutoff frequency of the RC filter, in Hz.
 */
template<typename T>
constexpr auto fc(T r, T c)
{
	return (1.0 / (2.0 * 3.1415 * r * c));
}

template<typename T>
constexpr auto rpar(T r1, T r2)
{
	return 1.0 / ((1.0 / r1) + (1.0 / r2));
}

template<typename T>
constexpr auto rdiv_to_gain(T rtop, T rbot)
{
	return rbot / (rtop + rbot);
}

template<typename T>
constexpr auto rdiv_to_gaindb(T rtop, T rbot)
{
	return 20.0 * std::log10(rdiv_to_gain(rtop, rbot));
}

#endif //GRVSLIB_BASIC_CALCULATORS_H
