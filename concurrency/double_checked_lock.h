/*
 * Copyright 2016, 2024 Gary R. Van Sickle (grvs@users.sourceforge.net).
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

#ifndef GRVSLIB_DOUBLE_CHECKED_LOCK_H
#define GRVSLIB_DOUBLE_CHECKED_LOCK_H

/** @file Double-checked lock implementation. */

// Std C++
#include <atomic>
#include <mutex>

/**
 * Function template implementing a double-checked lock.
 * A primary use case for this is in the creation of singletons, in their get_instance() function.  It makes the
 * hot "singleton-is-already-constructed" path lock-free.  For example:
 *
 * @code{.cpp}
 * std::atomic<Singleton*> f_the_singleton_instance {nullptr};
 * std::mutex f_the_singleton_creation_mutex {};
 * Singleton* Singleton::get_instance()
 * {
 *    return DoubleCheckedLock<Singleton*, nullptr>(f_the_singleton_instance, f_the_singleton_creation_mutex,
 *    													[&](){ return new Singleton(); });
 * }
 * @endcode
 *
 * @note I apologize for the completely unintuitive parameter names here.  I'll address this some day.
 *
 * @tparam ReturnType
 * @tparam NullVal             The value which @p wrap will have before it is initialized.  E.g., this should
 *                             be @a nullptr for any pointer type.
 * @tparam AtomicTypeWrapper
 *
 * @param wrap          Non-const reference to an instance of std::atomic\<ReturnType\>.  This is where the pointer/value
 *                      we're one-time initializing lives.
 * @param mutex         Non-const reference to a mutex.  This is the same mutex we'd otherwise be locking to access and
 * 						update @p wrap.
 * @param cache_filler  A callable which fills the "cache", i.e., @p wrap.  This callable will be called exactly once
 *                      during the program run to populate @p wrap; subsequent calls will simply return @p wrap.
 */
template<typename ReturnType,
		ReturnType NullVal = nullptr,
		typename AtomicTypeWrapper = std::atomic <ReturnType>,
		typename CacheFillerType = std::function < ReturnType()> &,
		typename MutexType = std::mutex >
ReturnType
DoubleCheckedLock(AtomicTypeWrapper &wrap, MutexType &mutex, CacheFillerType cache_filler)
{
	ReturnType temp_retval = wrap.load(std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_acquire);    // Guaranteed to observe everything done in another thread before
			 	 	 	 	 	 	 	 	 	 	 	 	// the std::atomic_thread_fence(std::memory_order_release) below.
	if(temp_retval == NullVal)
	{
		// First check says we don't have the cached value yet.
		std::unique_lock<MutexType> lock(mutex);
		// One more try.
		temp_retval = wrap.load(std::memory_order_relaxed);
		if(temp_retval == NullVal)
		{
			// Still no cached value.  We'll have to do the heavy lifting.
			//temp_retval = const_cast<ReturnType>(cache_filler());
			temp_retval = (std::remove_const_t<ReturnType>)(cache_filler());
			std::atomic_thread_fence(std::memory_order_release);
			wrap.store(temp_retval, std::memory_order_relaxed);
		}
	}

	return temp_retval;
}

#if 0 // I'm not sure this even works, I don't see anywhere that I actually use it.
/**
 * Function template implementing a double-checked lock protecting multiple subsets of objects.
 *
 * @param wrap          An instance of std::atomic\<BitmaskType\>.
 * @param bits          The bits which need to be set in @p wrap to indicate there's no need to call @p cache_filler.
 * @param mutex         Reference to a mutex to be std::unique_lock'ed if @p cache_filler needs to be called.
 * @param cache_filler  Function object which fills the cache.  Must return the bits to be set in @p wrap, which must
 *                      include @p bits, but may also include other bits.
 * @return
 */
template < typename BitmaskType,
	BitmaskType NullVal = 0,
	typename AtomicTypeWrapper = std::atomic<BitmaskType>,
	typename CacheFillerType = std::function<BitmaskType()>&,
	typename MutexType = std::mutex >
void DoubleCheckedMultiLock(AtomicTypeWrapper &wrap, const BitmaskType bits, MutexType &mutex, CacheFillerType cache_filler)
{
	BitmaskType temp_retval = wrap.load(std::memory_order_relaxed) & bits;
	std::atomic_thread_fence(std::memory_order_acquire);  	// Guaranteed to observe everything done in another thread before
			 	 	 	 	 	 	 	 	 	 	 	 	// the std::atomic_thread_fence(std::memory_order_release) below.
	if(temp_retval == NullVal)
	{
		// First check says we don't have the cached value yet.
		std::unique_lock<MutexType> lock(mutex);
		// One more try.
		temp_retval = wrap.load(std::memory_order_relaxed) & bits;
		if(temp_retval == NullVal)
		{
			// Still no cached value.  We'll have to do the heavy lifting.
			temp_retval = cache_filler();

			// or-in the new cache status.
			std::atomic_thread_fence(std::memory_order_release);
			wrap.fetch_or(temp_retval, std::memory_order_relaxed);
		}
	}
}
#endif

#endif //GRVSLIB_DOUBLE_CHECKED_LOCK_H
