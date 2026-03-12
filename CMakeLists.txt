cmake_minimum_required(VERSION 3.19)

project(ColAndreas LANGUAGES C CXX)

option(FORCE_32_BIT "Force 32-bit compilation on UNIX." ON)
option(BuildWizard "Compile the Wizard application." OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX AND FORCE_32_BIT)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
endif()

find_package(Bullet REQUIRED)

if(EXISTS "${PROJECT_SOURCE_DIR}/libs/omp-sdk/CMakeLists.txt")
	add_subdirectory(libs/omp-sdk)
else()
	message(FATAL_ERROR "open.mp SDK submodule not found. Run: git submodule update --init --recursive")
endif()

set(CA_VERSION "v1.5.0")
set(CA_DATABASE_VERSION 2)
add_definitions(-DCA_VERSION="${CA_VERSION}" -DCA_DATABASE_VERSION=${CA_DATABASE_VERSION})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING " " FORCE)
endif()

if(WIN32)
	add_definitions(-DWIN32 -D_WIN32)

	set(CMAKE_C_FLAGS "/nologo /EHsc /MT")
	set(CMAKE_CXX_FLAGS "/nologo /EHsc /MT")

	set(CMAKE_C_FLAGS_DEBUG "/Od /Zi")
	set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi")
	set(CMAKE_C_FLAGS_RELEASE "/Ox")
	set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Ox /Zi")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Ox /Zi")
else()
	add_definitions(-DLINUX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fshort-wchar -shared")

	set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
	set(CMAKE_C_FLAGS_RELEASE "-O3 -g0")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g3")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3")

	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-O3 -s")
endif()

file(GLOB CA_SOURCE
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/SDK/*.cpp"
)

add_library(ColAndreas SHARED ${CA_SOURCE})
set_target_properties(ColAndreas PROPERTIES PREFIX "")

target_include_directories(ColAndreas PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/SDK"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/SDK/amx"
	${BULLET_INCLUDE_DIR}
)

target_link_libraries(ColAndreas PRIVATE
	${BULLET_LIBRARIES}
	OMP-SDK
)

if(UNIX AND NOT APPLE)
	target_link_libraries(ColAndreas PRIVATE rt)
endif()

if(NOT WIN32 AND BULLET_COLLISION_LIBRARY MATCHES ".a$")
	set_target_properties(ColAndreas PROPERTIES SUFFIX "_static.so")
endif()

install(TARGETS ColAndreas DESTINATION "components/")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Server/include/colandreas.inc" DESTINATION "pawno/include/")

if(BuildWizard)
	add_subdirectory(WizardApp)
endif()
