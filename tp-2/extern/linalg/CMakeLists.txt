add_library(linalg "")
set_target_properties(linalg PROPERTIES LINKER_LANGUAGE CXX)


target_sources(linalg
                PUBLIC
                    ${CMAKE_CURRENT_LIST_DIR}/linalg.h
                )

target_include_directories(linalg
                            PUBLIC
                                ${CMAKE_CURRENT_LIST_DIR}
                            )