if(MSVC)
    add_compile_options(
        /W4
        /WX-
        /MP          # parallel compilation
        /std:c++20
        /utf-8
        /permissive-
        # CEF needs these
        /D NOMINMAX
        /D WIN32_LEAN_AND_MEAN
        /D UNICODE
        /D _UNICODE
    )
    # Subsystem: Windows (no console window)
    add_link_options(/SUBSYSTEM:WINDOWS)
endif()
