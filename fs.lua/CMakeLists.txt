target_link_libraries(firesteel PRIVATE lua54.lib)

option(LUABRIDGE_TESTING OFF)
target_include_directories(firesteel PUBLIC include)
target_include_directories(firesteel PUBLIC LuaBridge/Source)
add_subdirectory(LuaBridge ${CMAKE_CURRENT_BINARY_DIR}/LuaBridge)
target_link_libraries(firesteel PRIVATE LuaBridge)
set_target_properties(LuaBridge PROPERTIES FOLDER "External/lua")
set_target_properties(Documentation PROPERTIES FOLDER "External/lua")