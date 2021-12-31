cmake_minimum_required(VERSION 3.10)

project(shinobi-online-server)

file(GLOB_RECURSE shinobi_SRC
    "src/*.cpp"
)

add_executable(shinobi-server ${shinobi_SRC})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set_target_properties(shinobi-server PROPERTIES CXX_STANDARD 17)
set_target_properties(shinobi-server PROPERTIES CXX_STANDARD_REQUIRED ON)

if (NOT WIN32)
    add_compile_options(-Wall -Werror -pipe -fvisibility=hidden)
endif ()

set(CMAKE_CXX_FLAGS_PERFORMANCE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-fno-strict-aliasing)
endif ()


find_package(Threads REQUIRED)

find_package(LibXml2 REQUIRED)

find_package(Lua REQUIRED)

find_package(MySQL REQUIRED)

find_package(Boost 1.78.0 REQUIRED COMPONENTS date_time system filesystem iostreams thread)

find_package(GMP REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ${MYSQL_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${GMP_INCLUDE_DIR})
target_link_libraries(shinobi-server PRIVATE
        Boost::date_time
        Boost::system
        Boost::filesystem
        Boost::iostreams
        Boost::thread
        ${CMAKE_THREAD_LIBS_INIT}
        ${LUA_LIBRARIES}
        ${MYSQL_CLIENT_LIBS}
        ${LIBXML2_LIBRARIES}
        ${GMP_LIBRARIES}
        )

# Precompiled header
# note: cotire() must be called last on a target
if (${CMAKE_VERSION} VERSION_GREATER "3.16.0")
    target_precompile_headers(shinobi-server PUBLIC src/otpch.h)
else ()
    include(cotire)
    set_target_properties(shinobi-server PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/otpch.h")
    set_target_properties(shinobi-server PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
    cotire(shinobi-server)
endif ()

target_compile_definitions(shinobi-server PRIVATE __USE_MYSQL__=1)
