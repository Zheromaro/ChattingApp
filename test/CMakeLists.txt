# ==========================================================
# 🧪 Unit Tests
# ==========================================================

verbose_message("Configuring unit tests for ${CMAKE_PROJECT_NAME}...")

# ---- Determine the library target under test ----
if(${CMAKE_PROJECT_NAME}_BUILD_EXECUTABLE)
  set(TEST_LINK_LIB ${CMAKE_PROJECT_NAME}_LIB)
else()
  set(TEST_LINK_LIB ${CMAKE_PROJECT_NAME})
endif()

# ---- Locate testing framework ----
if(${CMAKE_PROJECT_NAME}_USE_GTEST)
  find_package(GTest CONFIG REQUIRED)

  set(GTEST_TARGETS
    GTest::GTest
    GTest::Main
  )

  if(${CMAKE_PROJECT_NAME}_USE_GOOGLE_MOCK)
    list(APPEND GTEST_TARGETS
      GTest::gmock
      GTest::gmock_main
    )
  endif()
else()
  message(FATAL_ERROR "Unsupported testing framework. Please configure target_link_libraries manually.")
endif()

# ---- Build one test executable per subdirectory in /test ----
foreach(test_dir IN LISTS test_dirs)
  file(GLOB test_sources CONFIGURE_DEPENDS "${test_dir}/*.cpp")

  if(NOT test_sources)
    verbose_message("  ⚠️ No .cpp sources in ${test_dir}, skipping.")
    continue()
  endif()

  get_filename_component(test_name ${test_dir} NAME)
  set(test_target ${test_name}_Test)

  add_executable(${test_target} ${test_sources})

  target_compile_features(${test_target} PUBLIC cxx_std_17)

  target_link_libraries(
    ${test_target}
    PRIVATE
      ${GTEST_TARGETS}
      ${TEST_LINK_LIB}
  )

  set_target_properties(${test_target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )

  add_test(
    NAME ${test_name}
    COMMAND ${test_target}
  )

  verbose_message("  ✅ ${test_target}")
endforeach()

verbose_message("Unit test configuration complete.")
