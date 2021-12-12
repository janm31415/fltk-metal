set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework QuartzCore -framework Metal")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Cocoa -framework QuartzCore -framework Metal")

set(HDRS
../fltk-metal/Fl_Metal_Window.h
    )
	
set(SRCS
../fltk-metal/Fl_Metal_Window.mm
main.cpp
)


add_executable(ftlk-metal-simple ${HDRS} ${SRCS})

source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

 target_include_directories(ftlk-metal-simple
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../fltk/
    ${CMAKE_CURRENT_BINARY_DIR}/../fltk/
    )

target_link_libraries(ftlk-metal-simple
    fltk
    )
