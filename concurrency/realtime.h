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

	bool load_and_clear_if_set(PayloadType* reader_payload)
	{
		if(m_has_been_updated.test())
		{
			// The payload has been updated.  Let's try to read the value.

			// First get the Payload lock using the "test and test-and-set" protocol.
//			do
//			{
//				while(m_is_being_accessed.test())
//				{
//					// Spin.
//				}
//			} while(m_is_being_accessed.test_and_set());
			if(m_is_being_accessed.test_and_set() == true)
			{
				// It was locked, skip this attempt and try again on the next call.
				return false;
			}

			// We've got the m_is_being_accessed lock here.

			// Copy the payload out.
			*reader_payload = m_payload;

			// Clear the update notification flag.
			m_has_been_updated.clear();
			// Unblock any threads which may be waiting in store_and_set().
			m_is_being_accessed.clear();
			m_is_being_accessed.notify_all();

			// Indicate that we did a data transfer.
			return true;
		}

		// Indicate that no data was transferred.
		return false;
	}

	void store_and_set(const PayloadType* new_writer_payload)
	{
		// Wait until the payload lock is false.
		m_is_being_accessed.wait(true);
		m_is_being_accessed.test_and_set();

		m_payload = *new_writer_payload;

		// Clear the payload lock.
		m_is_being_accessed.clear();
		// In general, nobody should be waiting for us, but notify just in case.
		m_is_being_accessed.notify_all();

		// Set the update notification flag.
		m_has_been_updated.test_and_set();
	}
private:
	/**
	 * The flag which will communicate whether the payload has be updated or not.
	 * @note The "= ATOMIC_FLAG_INIT" is not needed post-C++20, but we keep it here in the interest of
	 *       minimal-pain backward compatibility.
	 */
	std::atomic_flag m_has_been_updated = ATOMIC_FLAG_INIT;
	std::atomic_flag m_is_being_accessed = ATOMIC_FLAG_INIT;
	PayloadType m_payload;
};


#endif //GRVSLIB_REALTIME_H
