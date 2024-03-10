
add_library(concurrency STATIC EXCLUDE_FROM_ALL)
target_sources(concurrency
	PUBLIC
		realtime.h
	PRIVATE
		realtime.cpp
)
