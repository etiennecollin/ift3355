add_library(bitmap_image "")
set_target_properties(bitmap_image PROPERTIES LINKER_LANGUAGE CXX)


target_sources(bitmap_image
                PUBLIC
                    ${CMAKE_CURRENT_LIST_DIR}/bitmap_image.h
                )

target_include_directories(bitmap_image
                            PUBLIC
                                ${CMAKE_CURRENT_LIST_DIR}
                            )