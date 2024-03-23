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
#include <type_traits>

namespace grvslib::impl
{
template<typename T>
constexpr static bool is_atomic = false;

template<typename T>
constexpr static bool is_atomic<std::atomic<T>> = true;
}

/**
 * This class was designed for a fairly specific use case:
 *
 * 1. You have one consumer thread which is periodic and has hard or fairly-hard deadlines.  This thread needs to
 *    periodically pick up some parameter data updated by other threads, but this is not time-critical.
 * 2. You have one or more producer threads which are updating the parameter data.  These threads do not have any
 *    particular deadline requirements.
 * 3. The parameter data may be larger that the integral types.
 * 4. The consumer thread in 1 only cares about the last value written by the thread(s) in 2.
 *
 * A specific example of this would be a DSP thread which needs to pick up filter coefficients calculated and updated
 * due to user input by a UI thread.
 *
 * Note that this class is always lock-free if:
 * - PayloadType is a std::is_arithmetic type and std::atomic\<PayloadType\> is always-lock-free.
 * - PayloadType is a std:atomic\<\> type and it is always lock-free.
 *
 * Calls to load_and_clear_if_set() are always lock-free when there is not a newly-written value to load.
 *
 * @tparam PayloadType
 */
template<typename PayloadType>
class atomic_notifying_parameter
{
	template<typename T>
	constexpr static bool type_is_atomic_and_always_lock_free()
	{
		if constexpr(grvslib::impl::is_atomic<T>)
		{
			return T::is_always_lock_free;
		}
		else
		{
			return false;
		}
	}

	using PayloadStorageType = std::conditional_t<
			!grvslib::impl::is_atomic<PayloadType> && std::is_arithmetic<PayloadType>::value,
			std::atomic<PayloadType>, PayloadType>;
	static constexpr bool PayloadStorageType_is_atomic = grvslib::impl::is_atomic<PayloadStorageType>;
	static constexpr bool PayloadStorageType_is_always_lock_free = type_is_atomic_and_always_lock_free<PayloadStorageType>();

public:

	/// If the type of our @a m_payload member (PayloadStorageType) is always lock free, the algorithms of this
	/// class will be always lock free.
	static constexpr bool is_always_lock_free = PayloadStorageType_is_always_lock_free;


	/**
	 * Function the consuming thread should call to atomically check for and load a newly-written value.  Clears the
	 * notify flag, if set.  If no newly-written data is available (i.e. there hasn't been a call to store_and_set()
	 * since the last call of this function), does not touch @p reader_payload.
	 *
	 * @note This function is lock-free when there is not a newly-written value to load.
	 *
	 * @param reader_payload  Pointer to the variable you want to atomically load the latest data into, if there's
	 *                        been a write since the last call.
	 * @return true if there was a newly-stored value to load, false if not.
	 */
	bool load_and_clear_if_set(PayloadType *reader_payload)
	{
		if(m_has_been_updated.test())
		{
			// The payload has been updated.

			if constexpr(PayloadStorageType_is_atomic)
			{
				// Payload is std::atomic<>.

				// Clear the update notification flag.
				// Note that we do this before the .load() so we don't lose any notifications.
				m_has_been_updated.clear();

				// Note that we don't care that we have a race here between the clearing of the "has been updated" flag
				// and reading the payload, because we always only want the value that was writtem last.
				// If another thread sneaks in here and (atomically) updates the value, that's the value we want.
				// We will get a spurious "has been updated" notification, so we'll double-read the same value in this
				// case.

				// Atomically read the value.  This will be lock-free if PayloadStorageType is lock-free.
				*reader_payload = m_payload.load();
			}
			else
			{
				// Payload isn't atomic.

				// Let's try to read the value.

				if(m_is_being_accessed.test_and_set() == true)
				{
					// It was already locked, skip this read attempt and try again on the next call.
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
			}

			// Indicate that we did a data transfer.
			return true;
		}

		// Indicate that no data was transferred.
		return false;
	}


	/**
	 * Function the producing thread(s) should call to store a new parameter value and set the notify flag.
	 *
	 * @param new_writer_payload  The new value to write.
	 */
	void store_and_set(const PayloadType& new_writer_payload)
	{
		if constexpr(PayloadStorageType_is_atomic)
		{
			m_payload.store(new_writer_payload);
		}
		else
		{
			// Wait until the payload lock is false.
			// This is not lock-free.
			m_is_being_accessed.wait(true);

			// Another thread may sneak in here and re-set m_is_being_accessed to true.
			// So, we spin to eliminate that race.
			while(m_is_being_accessed.test_and_set() == true){};

			// We've got the m_is_being_accessed lock here.

			// Copy the new payload.
			m_payload = new_writer_payload;

			// Clear the payload lock.
			m_is_being_accessed.clear();
			// In general, nobody should be waiting for us, but notify just in case.
			m_is_being_accessed.notify_all();
		}

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
	PayloadStorageType m_payload;
};


#endif //GRVSLIB_REALTIME_H
