
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
#applyPipes.cpp
#placeholders.cpp
#moveOnly.cpp
#endPipes.cpp
#streams.cpp
#outputAsIterator.cpp

#detail/isRange.cpp
#detail/isPushable.cpp
detail/isDereferencable.cpp
#detail/isIncrementable.cpp
#detail/pipeTraits.cpp
#detail/outputAsIteratorTraits.cpp

detail/SyntaxTree.cpp
#detail/apiHelpers.cpp
)

add_executable(${PROJECT_NAME}_catch main.cpp ${SOURCES})
target_link_libraries(${PROJECT_NAME}_catch ${PROJECT_NAME})