
add_library(ee STATIC EXCLUDE_FROM_ALL)
target_sources(ee
	PUBLIC
		ee.h
		basic_calculators.h
	PRIVATE
		ee.cpp
)
