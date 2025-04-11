add_subdirectory(external/box2d ${CMAKE_CURRENT_BINARY_DIR}/box2d)
target_link_libraries(firesteel PRIVATE box2d)
set_target_properties(box2d PROPERTIES FOLDER "External")