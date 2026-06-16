# ==========================================================
# ⚙️ Standard Project Settings
# ==========================================================

# ---- Build mode ----
set(_BUILD_TYPES "executable" "static" "shared" "header-only")
set(${PROJECT_NAME}_BUILD_TYPE "executable" CACHE STRING "Build type: executable, static, shared, or header-only")
option(${PROJECT_NAME}_USE_ALT_NAMES       "Use lowercase include directory name." ON)
option(${PROJECT_NAME}_VERBOSE_OUTPUT      "Enable verbose configuration messages." ON)

# Validate user input
if(NOT ${PROJECT_NAME}_BUILD_TYPE IN_LIST _BUILD_TYPES)
  message(FATAL_ERROR "Invalid BUILD_TYPE '${${PROJECT_NAME}_BUILD_TYPE}'. Must be one of: ${_BUILD_TYPES}")
endif()

# ---- Compiler warnings ----
option(${PROJECT_NAME}_WARNINGS_AS_ERRORS  "Treat compiler warnings as errors." OFF)

# ---- Compiler-specific flags ----
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  if(${PROJECT_NAME}_WARNINGS_AS_ERRORS)
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()
endif()

# ---- Threading support ----
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

# ---- Debug / Release flags ----
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  string(APPEND CMAKE_C_FLAGS_DEBUG " -ggdb3 -O0")
endif()
string(APPEND CMAKE_C_FLAGS_RELEASE " -O2")

# ---- Unit testing ----
option(${PROJECT_NAME}_ENABLE_UNIT_TESTING "Enable unit tests from /test subfolder." OFF)
option(${PROJECT_NAME}_USE_GTEST           "Use GoogleTest for unit tests." ON)
option(${PROJECT_NAME}_USE_GOOGLE_MOCK     "Use GoogleMock for unit tests." OFF)
