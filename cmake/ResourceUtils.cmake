# ResourceUtils.cmake - Utility functions for handling resources in both native
# and Emscripten builds

#[=======================================================================[.rst:
add_target_resources
--------------------

Adds resources (files or directories) to a target, handling both native and Emscripten builds.

For native builds: Copies resources to the target's output directory using custom commands
For Emscripten builds: Preloads resources using --preload-file linker options

Syntax:
  add_target_resources(TARGET target_name
                      RESOURCES resource1 [resource2 ...]
                      [DESTINATION dest1 [dest2 ...]]
                      [COPY_IF_DIFFERENT])

Arguments:
  TARGET - The target to add resources to
  RESOURCES - List of source files/directories (relative to CMAKE_CURRENT_SOURCE_DIR)
  DESTINATION - List of destination paths (optional, defaults to same name as source)
  COPY_IF_DIFFERENT - Use copy_directory_if_different instead of copy_directory for native builds

Examples:
  # Copy data and shaders directories
  add_target_resources(TARGET myapp RESOURCES data shaders)

  # Copy with different destination names
  add_target_resources(TARGET myapp
                       RESOURCES data config
                       DESTINATION assets settings)

  # Copy single file
  add_target_resources(TARGET myapp RESOURCES config.json)
#]=======================================================================]

function(add_target_resources)
  set(options COPY_IF_DIFFERENT)
  set(oneValueArgs TARGET)
  set(multiValueArgs RESOURCES DESTINATION)

  cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${oneValueArgs}"
                        "${multiValueArgs}")

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "add_target_resources: TARGET argument is required")
  endif()

  if(NOT ARG_RESOURCES)
    message(FATAL_ERROR "add_target_resources: RESOURCES argument is required")
  endif()

  # If no destination specified, use same names as resources
  if(NOT ARG_DESTINATION)
    set(ARG_DESTINATION ${ARG_RESOURCES})
  endif()

  # Check that RESOURCES and DESTINATION lists have same length
  list(LENGTH ARG_RESOURCES resources_count)
  list(LENGTH ARG_DESTINATION destination_count)
  if(NOT resources_count EQUAL destination_count)
    message(
      FATAL_ERROR
        "add_target_resources: RESOURCES and DESTINATION lists must have same length"
    )
  endif()

  if(DEFINED EMSCRIPTEN)
    # Emscripten build - use preload-file
    math(EXPR last_index "${resources_count} - 1")
    foreach(index RANGE ${last_index})
      list(GET ARG_RESOURCES ${index} resource)
      list(GET ARG_DESTINATION ${index} destination)

      set(source_path "${CMAKE_CURRENT_SOURCE_DIR}/${resource}")
      target_link_options(${ARG_TARGET} PRIVATE
                          "SHELL:--preload-file ${source_path}@/${destination}")
    endforeach()
  else()
    # Native build - use custom commands
    set(copy_command "copy_directory")
    if(ARG_COPY_IF_DIFFERENT)
      set(copy_command "copy_directory_if_different")
    endif()

    math(EXPR last_index "${resources_count} - 1")
    foreach(index RANGE ${last_index})
      list(GET ARG_RESOURCES ${index} resource)
      list(GET ARG_DESTINATION ${index} destination)

      set(source_path "${CMAKE_CURRENT_SOURCE_DIR}/${resource}")
      set(dest_path "$<TARGET_FILE_DIR:${ARG_TARGET}>/${destination}")

      add_custom_command(
        TARGET ${ARG_TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E ${copy_command} "${source_path}"
                "${dest_path}"
        COMMENT "Copying ${resource} to ${destination}")
    endforeach()
  endif()
endfunction()

#[=======================================================================[.rst:
add_target_resource_file
------------------------

Adds a single resource file to a target (convenience wrapper around add_target_resources)

Syntax:
  add_target_resource_file(TARGET target_name
                          SOURCE source_file
                          [DESTINATION dest_file])

Examples:
  add_target_resource_file(TARGET myapp SOURCE config.json)
  add_target_resource_file(TARGET myapp SOURCE settings.ini DESTINATION config.ini)
#]=======================================================================]

function(add_target_resource_file)
  set(options)
  set(oneValueArgs TARGET SOURCE DESTINATION)
  set(multiValueArgs)

  cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${oneValueArgs}"
                        "${multiValueArgs}")

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "add_target_resource_file: TARGET argument is required")
  endif()

  if(NOT ARG_SOURCE)
    message(FATAL_ERROR "add_target_resource_file: SOURCE argument is required")
  endif()

  if(ARG_DESTINATION)
    add_target_resources(TARGET ${ARG_TARGET} RESOURCES ${ARG_SOURCE}
                         DESTINATION ${ARG_DESTINATION})
  else()
    add_target_resources(TARGET ${ARG_TARGET} RESOURCES ${ARG_SOURCE})
  endif()
endfunction()
