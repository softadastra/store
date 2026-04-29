add_executable(store_basic basic_store.cpp)
target_link_libraries(store_basic PRIVATE softadastra::store)

add_executable(store_wal_usage wal_usage.cpp)
target_link_libraries(store_wal_usage PRIVATE softadastra::store)

add_executable(store_recovery recovery_demo.cpp)
target_link_libraries(store_recovery PRIVATE softadastra::store)

add_executable(store_memory_only memory_only.cpp)
target_link_libraries(store_memory_only PRIVATE softadastra::store)

add_executable(store_operation_codec operation_codec.cpp)
target_link_libraries(store_operation_codec PRIVATE softadastra::store)

add_executable(store_snapshot_from_wal snapshot_from_wal.cpp)
target_link_libraries(store_snapshot_from_wal PRIVATE softadastra::store)
