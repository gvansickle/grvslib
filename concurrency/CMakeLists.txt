
add_library(concurrency STATIC EXCLUDE_FROM_ALL)
target_sources(concurrency
	PUBLIC
		realtime.h
		double_checked_lock.h
	PRIVATE
		realtime.cpp
)
