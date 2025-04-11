target_link_libraries(firesteel PRIVATE lua54.lib)

target_include_directories(firesteel PUBLIC include)
add_subdirectory(LuaBridge/Source ${CMAKE_CURRENT_BINARY_DIR}/LuaBridge)
target_link_libraries(firesteel PRIVATE LuaBridge)
set_target_properties(LuaBridge PROPERTIES FOLDER "External/Lua")