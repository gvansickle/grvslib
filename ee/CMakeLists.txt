
add_library(ee EXCLUDE_FROM_ALL)
target_sources(ee
		PUBLIC
			ee.h
		PRIVATE
			ee.cpp
)
