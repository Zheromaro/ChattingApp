# ==========================================================
# 📁 Source & Header Files
# ==========================================================

set(SRC_DIR     "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(TEST_DIR    "${PROJECT_SOURCE_DIR}/test")

# ---- All source files (including main) ----
file(GLOB_RECURSE all_sources
    CONFIGURE_DEPENDS
    "${SRC_DIR}/*.c"
)

# exe_sources = everything (for the executable)
set(exe_sources ${all_sources})

# sources = everything EXCEPT main.* (for the library / test lib)
set(sources ${all_sources})
list(FILTER sources EXCLUDE REGEX ".*/main\\.c$")

# ---- Headers ----
file(GLOB_RECURSE headers
    CONFIGURE_DEPENDS
    "${INCLUDE_DIR}/*.h"
)

# ---- Test directories ----
file(GLOB TEST_ENTRIES
    LIST_DIRECTORIES TRUE
    RELATIVE "${TEST_DIR}"
    "${TEST_DIR}/*"
)

set(test_dirs "")
foreach(entry IN LISTS TEST_ENTRIES)
  if(IS_DIRECTORY "${TEST_DIR}/${entry}")
    list(APPEND test_dirs "${TEST_DIR}/${entry}")
  endif()
endforeach()

# ---- Test helper sources (C utilities in test/) ----
file(GLOB_RECURSE test_helper_sources
    CONFIGURE_DEPENDS
    "${TEST_DIR}/*.c"
)
