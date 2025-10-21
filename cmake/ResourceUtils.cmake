#[=======================================================================[.rst:
ResourceUtils
-------------

Functions for copying resource directories to build targets.

.. command:: add_target_resources

  Copy resource directories to the target's output directory.
  For Emscripten builds, directories are embedded using --preload-file.
  For native builds, directories are copied using custom commands.

  .. code-block:: cmake

    add_target_resources(<target-name>
                         RESOURCES <dir1> [<dir2> ...]
                         [DESTINATIONS <dest1> [<dest2> ...]]
                         [COPY_IF_DIFFERENT])

  ``<target-name>``
    The target to copy resource directories to (required, first argument).

  ``RESOURCES <dir1> [<dir2> ...]``
    List of resource directories to copy (required).
    Paths can be absolute or relative to CMAKE_CURRENT_SOURCE_DIR.

  ``DESTINATIONS <dest1> [<dest2> ...]``
    List of destination directory names (optional).
    If not specified, uses the same names as RESOURCES.
    Must have the same length as RESOURCES if provided.

  ``COPY_IF_DIFFERENT``
    For native builds, only copy directories if content differs (optional).
    Uses copy_directory_if_different instead of copy_directory.

  **Examples:**

  .. code-block:: cmake

    # Copy data and shaders directories
    add_target_resources(myapp RESOURCES data shaders)

    # Copy with custom destination names
    add_target_resources(myapp
                         RESOURCES data config
                         DESTINATIONS assets settings)

    # Copy only if directories are different
    add_target_resources(myapp RESOURCES textures COPY_IF_DIFFERENT)

#]=======================================================================]

function(add_target_resources target_name)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "COPY_IF_DIFFERENT" ""
                        "RESOURCES;DESTINATIONS")

  if(NOT TARGET ${target_name})
    message(
      FATAL_ERROR "add_target_resources: target '${target_name}' does not exist"
    )
  endif()

  if(NOT ARG_RESOURCES)
    message(FATAL_ERROR "add_target_resources: RESOURCES argument is required")
  endif()

  # If no destination specified, use same names as resources
  if(NOT ARG_DESTINATIONS)
    set(ARG_DESTINATIONS ${ARG_RESOURCES})
  endif()

  # Check that RESOURCES and DESTINATION lists have same length
  list(LENGTH ARG_RESOURCES resources_count)
  list(LENGTH ARG_DESTINATIONS destinations_count)
  if(NOT resources_count EQUAL destinations_count)
    message(
      FATAL_ERROR
        "add_target_resources: RESOURCES and DESTINATIONS lists must have same length"
    )
  endif()

  # Make resource paths absolute
  set(absolute_resources)
  foreach(resource IN LISTS ARG_RESOURCES)
    cmake_path(ABSOLUTE_PATH resource BASE_DIRECTORY
               "${CMAKE_CURRENT_SOURCE_DIR}")
    list(APPEND absolute_resources "${resource}")
  endforeach()

  # Set copy command
  set(copy_command "copy_directory")
  if(ARG_COPY_IF_DIFFERENT)
    set(copy_command "copy_directory_if_different")
  endif()

  math(EXPR last_index "${resources_count} - 1")
  foreach(index RANGE ${last_index})
    list(GET absolute_resources ${index} resource)
    list(GET ARG_DESTINATIONS ${index} destination)

    if(EMSCRIPTEN)
      target_link_options(${target_name} PRIVATE
                          "SHELL:--preload-file ${resource}@/${destination}")
    else()
      set(full_destination "$<TARGET_FILE_DIR:${target_name}>/${destination}")
      add_custom_command(
        TARGET ${target_name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E ${copy_command} "${resource}"
                "${full_destination}")
    endif()
  endforeach()
endfunction()
