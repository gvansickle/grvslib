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

#include <gtest/gtest.h>

// Std C++

// Ours.
#include <grvslib/concurrency/realtime.h>


TEST(Concurrency, atomic_notifying_parameter_int)
{
	atomic_notifying_parameter<int> the_parameter;

	EXPECT_TRUE(the_parameter.is_always_lock_free);

	int new_value {5};
	the_parameter.store_and_set(new_value);

	int retreived_value {0};
	the_parameter.load_and_clear_if_set(&retreived_value);

	EXPECT_EQ(5, retreived_value);
}

TEST(Concurrency, atomic_notifying_parameter_atomic_int)
{
	atomic_notifying_parameter<std::atomic<int>> the_parameter2;
	EXPECT_TRUE(the_parameter2.is_always_lock_free);
//	std::atomic<int>
}

TEST(Concurrency, atomic_notifying_parameter_big_struct)
{
	// A struct too big for std:atomic<BigStruct> to be lock-free.
	struct BigStruct
	{
		float m_float;
		long double m_ld;
		uint64_t m_uint64;
	};
	atomic_notifying_parameter<std::atomic<BigStruct>> the_parameter;
	EXPECT_FALSE(the_parameter.is_always_lock_free);
//	std::atomic<int>
}

