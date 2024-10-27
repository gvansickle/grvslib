
add_library(concurrency STATIC EXCLUDE_FROM_ALL)
target_sources(concurrency
	PRIVATE
		realtime.h
		double_checked_lock.h
		realtime.cpp
)
