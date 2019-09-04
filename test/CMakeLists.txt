#################################################
##################### GTEST #####################
#################################################

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
     remove_definitions("/Za")
endif()

set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" FORCE)
add_subdirectory(googletest)

if(MSVC)
    add_definitions("/Za")
		
	set_target_properties(gtest PROPERTIES FOLDER dependencies)
	set_target_properties(gtest_main PROPERTIES FOLDER dependencies)
		
endif()

#############################
########## DEFINES ##########
#############################

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	set(GCC true)
else()
	set(GCC false)
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CLANG true)
else()
	set(CLANG false)
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	add_definitions("/permissive-")

	add_definitions("/MP")
	add_definitions("/WX")

	# Force to always compile with W4
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()

	add_definitions("/experimental:external")
	add_definitions("/external:anglebrackets")
	add_definitions("/external:W0")

elseif(GCC)
	add_definitions("-Wall -Werror
	-Wno-long-long")
elseif(CLANG)
	add_definitions("-Weverything -Werror
	-Wno-c++98-compat -Wno-c++98-c++11-compat -Wno-c++98-c++11-c++14-compat -Wno-c++98-compat-pedantic 
	-Wno-global-constructors -Wno-padded -Wno-newline-eof -Wno-weak-vtables -Wno-missing-prototypes")
endif()


############################
########## TARGET ##########
############################

set(SOURCES
src/detail/isRange.cpp
src/detail/isPushable.cpp
)

add_executable(${PROJECT_NAME}_testing main.cpp ${SOURCES})

target_link_libraries(${PROJECT_NAME}_testing gtest)
target_link_libraries(${PROJECT_NAME}_testing gtest_main)
target_link_libraries(${PROJECT_NAME}_testing ${PROJECT_NAME})