
add_library(concurrency EXCLUDE_FROM_ALL)
target_sources(concurrency
	PUBLIC
		realtime.h
	PRIVATE
		realtime.cpp
)
