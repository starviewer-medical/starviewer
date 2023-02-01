add_subdirectory(main)
add_subdirectory(contrib)

if(ENABLE_PLAYGROUND_EXTENSIONS)
    add_subdirectory(playground)
endif()
