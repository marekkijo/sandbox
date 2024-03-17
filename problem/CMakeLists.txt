set(LIBDATACHANNEL_DLL_BIN "$<IF:$<CONFIG:Debug>,${libdatachannel_PACKAGE_FOLDER_DEBUG}/bin,${libdatachannel_PACKAGE_FOLDER_RELEASE}/bin>")

add_subdirectory(my_lib)
add_subdirectory(my_app)
