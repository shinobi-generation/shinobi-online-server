cmake_minimum_required(VERSION 3.10)

project(shinobi-online-server)

file(GLOB shinobi_SRC
    "src/*.cpp"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(ENABLE_MYSQL "" ON)
option(ENABLE_SQLITE "" OFF)
option(ENABLE_POSTGRES "" OFF)
option(ENABLE_ODBC "" OFF)

if (ENABLE_MYSQL)
    find_package(MySQL REQUIRED)

    list(APPEND shinobi_SRC src/database/databasemysql.cpp)
    message("-- mysql source has added to server executable")
endif()

if (ENABLE_SQLITE)
    add_definitions(COMPILE_DEFINITIONS __USE_SQLITE__=1)

    list(APPEND shinobi_SRC ${CMAKE_CURRENT_SOURCE_DIR}/databasesqlite.cpp PARENT_SCOPE)
endif()

if (ENABLE_POSTGRES)
    add_definitions(COMPILE_DEFINITIONS __USE_PGSQL__=1)
    list(APPEND database_SRC "${database_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/databasepqsql.cpp" PARENT_SCOPE)
endif()

if (ENABLE_ODBC)
    add_definitions(COMPILE_DEFINITIONS __USE_ODBC__=1)
    list(APPEND database_SRC "${database_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/databaseodbc.cpp" PARENT_SCOPE)
endif()

add_executable(shinobi-server ${shinobi_SRC})

set_target_properties(shinobi-server PROPERTIES CXX_STANDARD 17)
set_target_properties(shinobi-server PROPERTIES CXX_STANDARD_REQUIRED ON)

if(ENABLE_MYSQL)
    target_compile_definitions(shinobi-server PRIVATE __USE_MYSQL__=1)
endif()

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

find_package(Boost 1.78.0 REQUIRED COMPONENTS date_time system filesystem iostreams thread)

find_package(GMP REQUIRED)

include_directories(src ${Boost_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ${MYSQL_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${GMP_INCLUDE_DIR})
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
