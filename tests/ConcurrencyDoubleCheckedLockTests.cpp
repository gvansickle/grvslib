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
	std::shared_mutex go_flag;
	std::shared_mutex the_mutex;
	int the_value {0};
	std::atomic<int> the_atomic_value {999};

	// Stop the threads from running until they'r eboth set up.
	go_flag.lock();

	auto the_thread_function = [&](){
		// Wait for the "go" signal.
		go_flag.lock_shared();

		auto retval = DoubleCheckedLock<int, 999>(the_atomic_value, the_mutex, [&](){
			the_value += 1;
			return 1;
		});

//		EXPECT_EQ(3, retval);
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
	EXPECT_EQ(2, the_value);
}
