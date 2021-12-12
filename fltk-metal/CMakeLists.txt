set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework MetalKit -framework QuartzCore -framework Metal")
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Cocoa -framework MetalKit -framework QuartzCore -framework Metal")

set(HDRS
metal_window.h
    )
	
set(SRCS
metal_window.mm
main.cpp
)

set(SHADERS
shaders.metal
)

add_executable(ftlk-metal ${HDRS} ${SRCS} ${SHADERS})

source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs} ${SHADERS})
set_source_files_properties(${SHADERS} PROPERTIES LANGUAGEMETAL)

 target_include_directories(ftlk-metal
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../fltk/
    ${CMAKE_CURRENT_BINARY_DIR}/../fltk/
    )	
	
target_link_libraries(ftlk-metal   
    fltk
    fltk_images
    )	
