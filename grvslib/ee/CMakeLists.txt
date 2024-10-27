
add_library(ee STATIC EXCLUDE_FROM_ALL)
target_sources(ee
	PRIVATE
		ee.h
		basic_calculators.h
		ee.cpp
)
