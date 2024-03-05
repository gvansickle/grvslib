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
 * @file Concurrency classes and functions which are mostly useful in real-time situations.
 */

#ifndef GRVSLIB_REALTIME_H
#define GRVSLIB_REALTIME_H

#include <atomic>



template <typename PayloadType>
class lock_free_atomic_data_transfer
{
public:

	void update_and_clear_if_set(PayloadType* reader_payload)
	{
		if(m_has_been_updated.test())
		{
			///...@todo
		}
	}

	void set()
	{
		///...@todo
		m_has_been_updated.test_and_set();
	}
private:
	/**
	 * The flag which will communicate whether the payload has be updated or not.
	 * @note The "= ATOMIC_FLAG_INIT" is not needed post-C++20, but we keep it here in the interest of
	 *       minimal-pain backward compatibility.
	 */
	std::atomic_flag m_has_been_updated = ATOMIC_FLAG_INIT;
};


#endif //GRVSLIB_REALTIME_H
