# Copy CEF runtime DLLs + resources to output directory after build.
# Called from CMakeLists.txt via add_custom_command(TARGET ... POST_BUILD).

function(lumen_copy_cef_resources target)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CEF_BIN "${CEF_ROOT}/Debug")
    else()
        set(CEF_BIN "${CEF_ROOT}/Release")
    endif()

    # Core DLLs
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BIN}"
            "$<TARGET_FILE_DIR:${target}>"
        COMMENT "Copying CEF binaries"
    )

    # CEF Resources (icudtl.dat, cef.pak, etc.)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_ROOT}/Resources"
            "$<TARGET_FILE_DIR:${target}>"
        COMMENT "Copying CEF resources"
    )

    # Our own resources/
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:${target}>/resources"
        COMMENT "Copying Lumen resources"
    )
endfunction()
