
# The test exe.
# Note: It seems we cant put the tests in a library and just link to it here; any tests in the library don't get
#       discovered by gtest_discover_tests() for some reason.
# Update: It's GCC not linking in unreferenced binaries.  See: https://github.com/google/googletest/issues/481
add_executable(gttests
	ConcurrencyDoubleCheckedLockTests.cpp
	ConcurrencyRealtimeTests.cpp
	EETests.cpp
	gttests.cpp
)
target_link_libraries(gttests
	PRIVATE
		grvslib
		GTest::gtest_main
)

gtest_discover_tests(gttests)
