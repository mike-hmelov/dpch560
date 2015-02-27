set (daemon_src daemon.c)

# Make build flags compiler specific
if (CMAKE_C_COMPILER_ID EQUAL "GNU")
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set (CMAKE_C_FLAGS "-D_REETRANT -ggdb -fPIC -Wall -Wextra -pedantic -O0")
	elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
		set (CMAKE_C_FLAGS "-D_REETRANT -DNDEBUG -fPIC -Wall -Wextra -pedantic -O3")
	endif ()
elseif (CMAKE_C_COMPILER_ID EQUAL "CLang")
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set (CMAKE_C_FLAGS "-D_REETRANT -ggdb3 -fPIC -Wall -Wextra -pedantic -O0")
	elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
		set (CMAKE_C_FLAGS "-D_REETRANT -DNDEBUG -fPIC -Wall -Wextra -pedantic -O3 -static")
	endif ()
endif ()

add_executable (dp-ch560-daemon ${daemon_src})

# The rule to install daemon binary
install (TARGETS dp-ch560-daemon
	RUNTIME
	DESTINATION bin)