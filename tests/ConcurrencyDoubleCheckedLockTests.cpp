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
#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>

// Ours
#include <concurrency/double_checked_lock.h>


TEST(Concurrency, DoubleCheckedLock_Basic)
{
	std::shared_timed_mutex go_flag; // @note: Not shared_mutex so we can compile on C++14 for tests.
	std::mutex the_mutex;
	int num_fillers_run {0};
	constexpr int the_null_value {999};
	std::atomic<int> the_atomic_value {the_null_value};

	// Stop the threads from running until they'r eboth set up.
	go_flag.lock();

	auto the_thread_function = [&](){
		// Wait for the "go" signal.
		go_flag.lock_shared();

		auto retval = DoubleCheckedLock<int, the_null_value>(the_atomic_value, the_mutex, [&](){
			num_fillers_run++;
			return 55;
		});

		// Whether this thread had to do the "filling" of the_atomic_value or not, we should have gotten the
		// correct (non-the_null_value) value either way.
		EXPECT_EQ(55, retval);
		EXPECT_EQ(55, the_atomic_value);
	};

	// Start the test threads.
	std::thread t1(the_thread_function);
	std::thread t2(the_thread_function);

	// Let the threads run.
	go_flag.unlock();

	// Wait for the threads to complete.
	t1.join();
	t2.join();

	// Threads are finished, check for problems.
	EXPECT_EQ(55, the_atomic_value);
	// Only one thread should have had to run its "cache filler" function.
	EXPECT_EQ(1, num_fillers_run);
}

