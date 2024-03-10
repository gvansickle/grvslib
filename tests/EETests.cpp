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
 * @file Tests for the ee subdirectory.
 */

#include <gtest/gtest.h>

#include <ee/basic_calculators.h>

TEST(EE_BasicCalculators, fc1)
{
	EXPECT_NEAR(15915.9637, fc(10'000.0, 1.0e-9), 0.0001);
}

TEST(EE_BasicCalculators, rpar1) {
	EXPECT_DOUBLE_EQ(5000.0, rpar(10'000.0, 10'000.0));
}

TEST(EE_BasicCalculators, rdiv_to_gain1)
{
	EXPECT_FLOAT_EQ(0.5f, rdiv_to_gain(20'000.0f, 20'000.0f));
}

TEST(EE_BasicCalculators, rdiv_to_gaindb1)
{
	EXPECT_FLOAT_EQ(-6.0206f, rdiv_to_gaindb(20'000.0f, 20'000.0f));
}

