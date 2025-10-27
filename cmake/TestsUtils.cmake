#[=======================================================================[.rst:
TestsUtils
----------

Utilities for creating and configuring Google Test targets.

Global Variables
^^^^^^^^^^^^^^^^

.. variable:: TESTS_DIRECTORY

  Directory name used for organizing test targets in IDE folders.
  Default value is ``"_tests_"``.

.. command:: add_test_target

  Create a Google Test executable target with automatic configuration.
  The target is automatically linked with Google Test libraries and
  registered with CTest.

  .. code-block:: cmake

    add_test_target(<target-name>
                    FILES <test-file1> [<test-file2> ...]
                    [DEPS <dependency1> [<dependency2> ...]])

  ``<target-name>``
    Name of the test executable target to create (required).

  ``FILES <test-file1> [<test-file2> ...]``
    List of test source files to compile (required).
    Typically contains .cpp files with Google Test cases.

  ``DEPS <dependency1> [<dependency2> ...]``
    List of additional libraries or targets to link against (optional).
    Common dependencies include project libraries being tested.

  **Automatic Configuration:**

  The function automatically configures:

  * C++23 standard compilation
  * Google Test and Google Mock libraries (gtest, gtest_main, gmock, gmock_main)
  * IDE folder organization under ``${SOLUTION_FOLDER}/${TESTS_DIRECTORY}``
  * Visual Studio debugger working directory
  * CTest registration for ``ctest`` command

  **Examples:**

  .. code-block:: cmake

    # Basic test target
    add_test_target(math_tests FILES test_math.cpp)

    # Test with dependencies
    add_test_target(engine_tests
                    FILES test_engine.cpp test_physics.cpp
                    DEPS engine_lib physics_lib)

    # Multiple test files
    add_test_target(network_tests
                    FILES test_socket.cpp test_protocol.cpp test_client.cpp
                    DEPS networking)

#]=======================================================================]

set(TESTS_DIRECTORY "_tests_")

function(add_test_target target_name)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "FILES;DEPS")

  if(NOT ARG_FILES)
    message(FATAL_ERROR "add_test_target: FILES argument is required")
  endif()

  # Create test executable
  add_executable(${target_name} ${ARG_FILES})
  source_group(${SOURCE_GROUP_LABEL} FILES ${ARG_FILES})
  target_compile_features(${target_name} PRIVATE cxx_std_23)

  # Link Google Test libraries
  target_link_libraries(${target_name} PRIVATE GTest::gtest GTest::gtest_main
                                               GTest::gmock GTest::gmock_main)

  # Link additional dependencies
  foreach(dependency IN LISTS ARG_DEPS)
    target_link_libraries(${target_name} PRIVATE ${dependency})
  endforeach()

  # Configure IDE and debugger settings
  set_target_properties(
    ${target_name}
    PROPERTIES FOLDER ${SOLUTION_FOLDER}/${TESTS_DIRECTORY}
               VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${target_name}>)

  # Register with CTest
  add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()
