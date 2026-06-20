cmake_minimum_required(VERSION 3.15)

# ==========================================================
# 📦 Package Managers (Vcpkg) must be included before project definition
# ==========================================================
include(cmake/Vcpkg.cmake)

# ==========================================================
# 🧩 Project details
# ==========================================================
project(
  ChattingApp
  VERSION 0.1.0
  LANGUAGES C
)

message(STATUS "🚀 Starting configuration for ${PROJECT_NAME} v${PROJECT_VERSION}")

# ==========================================================
# 🔧 Compiler Validation & Information
# ==========================================================
message(STATUS "🔧 C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "🔧 C Compiler ID: ${CMAKE_C_COMPILER_ID}")
if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  message(WARNING "⚠️ Unsupported compiler ID: ${CMAKE_C_COMPILER_ID}. This project is tested with GCC and Clang only.")
endif()

# so Zed Editor can see all includes
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==========================================================
# ⚙️  Load custom project settings
# ==========================================================
include(cmake/StandardSettings.cmake)
include(cmake/Utils.cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Define a cross-platform debug macro
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)

# ==========================================================
# 🚫 Prevent in-source builds
# ==========================================================
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "❌ In-source builds are not allowed! Please use a separate build directory.")
endif()


# ==========================================================
# 📁 Source & Header Files + Assets
# ==========================================================
include(cmake/SourcesAndHeaders.cmake)
message(STATUS "📂 Found project sources and headers")

# ==========================================================
# 🧱 Build Mode Logic
# ==========================================================
if(${PROJECT_NAME}_BUILD_TYPE STREQUAL "executable")
  message(STATUS "🏗️ Building as EXECUTABLE")
  add_executable(${PROJECT_NAME} ${exe_sources})
  set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

  if(${PROJECT_NAME}_ENABLE_UNIT_TESTING)
    message(STATUS "📚 Building test library: ${PROJECT_NAME}_LIB")
    add_library(${PROJECT_NAME}_LIB ${headers} ${sources})
  endif()

elseif(${PROJECT_NAME}_BUILD_TYPE STREQUAL "header-only")
  message(STATUS "📜 Building as HEADER-ONLY library")
  add_library(${PROJECT_NAME} INTERFACE)

elseif(${PROJECT_NAME}_BUILD_TYPE STREQUAL "shared")
  message(STATUS "📦 Building as SHARED library (.so / .dll / .dylib)")
  add_library(${PROJECT_NAME} SHARED ${headers} ${sources})
  set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE C
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
  )

elseif(${PROJECT_NAME}_BUILD_TYPE STREQUAL "static")
  message(STATUS "📦 Building as STATIC library (.a / .lib)")
  add_library(${PROJECT_NAME} STATIC ${headers} ${sources})
  set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)
endif()

# Verbose output
if(${PROJECT_NAME}_VERBOSE_OUTPUT)
  if(${PROJECT_NAME}_BUILD_EXECUTABLE)
    verbose_message("Found the following sources:")
    foreach(source IN LISTS exe_sources)
      verbose_message("* ${source}")
    endforeach()
  elseif(NOT ${PROJECT_NAME}_BUILD_HEADERS_ONLY)
    verbose_message("Found the following sources:")
    foreach(source IN LISTS sources)
      verbose_message("* ${source}")
    endforeach()
  endif()

  verbose_message("Found the following headers:")
  foreach(header IN LISTS headers)
    verbose_message("* ${header}")
  endforeach()
endif()

# ==========================================================
# 🎨 get Assets
# ==========================================================

include(cmake/Assets.cmake)
if(EXISTS "${ASSETS_SOURCE_DIR}")
  message(STATUS "📂 Found project sources and headers")
endif()

# ==========================================================
# 🧾 Output directories
# ==========================================================
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
)

if(${PROJECT_NAME}_BUILD_TYPE STREQUAL "executable" AND ${PROJECT_NAME}_ENABLE_UNIT_TESTING)
  set_target_properties(
    ${PROJECT_NAME}_LIB
    PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
      OUTPUT_NAME ${PROJECT_NAME}
  )
endif()

# ==========================================================
# 🔗 Dependencies
# ==========================================================

# Vcpkg packages
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

# Non-vcpkg external libraries
include(cmake/ExternalLib.cmake)

if(NOT ${PROJECT_NAME}_BUILD_TYPE STREQUAL "header-only")
  set(LINK_LIBS
    ${EXTERNAL_LIBS}
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_image::SDL3_image
)

  target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBS})
  target_include_directories(${PROJECT_NAME} PUBLIC ${EXTERNAL_LIBS_INCLUDE})

  if(${PROJECT_NAME}_BUILD_TYPE STREQUAL "executable" AND ${PROJECT_NAME}_ENABLE_UNIT_TESTING)
    target_link_libraries(${PROJECT_NAME}_LIB PRIVATE ${LINK_LIBS})
    target_include_directories(${PROJECT_NAME}_LIB PUBLIC ${EXTERNAL_LIBS_INCLUDE})
  endif()
endif()

# ==========================================================
# 📚 Include Directories
# ==========================================================
if(${PROJECT_NAME}_BUILD_TYPE STREQUAL "header-only")
  target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
else()
  target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

  if(${PROJECT_NAME}_BUILD_TYPE STREQUAL "executable" AND ${PROJECT_NAME}_ENABLE_UNIT_TESTING)
    target_include_directories(
      ${PROJECT_NAME}_LIB
      PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
  endif()
endif()

message(STATUS "✅ Include directories configured.")

# ==========================================================
# 📦 Installation Rules
# ==========================================================
install(
  TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/${PROJECT_NAME_LOWERCASE}
  DESTINATION include
)

verbose_message("Install targets successfully built. Install with `cmake --build <build_directory> --target install --config <build_config>`.")

message(STATUS "📦 Installation setup complete.")

# ==========================================================
# 🧪 Unit Testing
# ==========================================================
if(${PROJECT_NAME}_ENABLE_UNIT_TESTING)
  enable_testing()
  message(STATUS "🧪 Unit testing ENABLED — looking for tests in /test/")
  add_subdirectory(test)
endif()

# ==========================================================
# 🧠 Build Summary
# ==========================================================
message(STATUS "")
message(STATUS "====== ${PROJECT_NAME} Configuration Summary ======")
message(STATUS "🔧 C Compiler:            ${CMAKE_C_COMPILER} (${CMAKE_C_COMPILER_ID})")
message(STATUS "🔧 Build type:            ${CMAKE_BUILD_TYPE}")
message(STATUS "⚙️  Project type:          ${${PROJECT_NAME}_BUILD_TYPE}")
message(STATUS "📦 Install prefix:        ${CMAKE_INSTALL_PREFIX}")
message(STATUS "🧪 Unit testing enabled:  ${${PROJECT_NAME}_ENABLE_UNIT_TESTING}")
message(STATUS "=====================================================")
message(STATUS "")
