add_executable(store_basic basic_store.cpp)
target_link_libraries(store_basic PRIVATE softadastra::store)

add_executable(store_recovery recovery_demo.cpp)
target_link_libraries(store_recovery PRIVATE softadastra::store)
