add_executable(test_operation test_operation.cpp)
target_link_libraries(test_operation PRIVATE softadastra::store)

add_test(NAME test_operation COMMAND test_operation)

add_executable(test_index test_index.cpp)
target_link_libraries(test_index PRIVATE softadastra::store)

add_test(NAME test_index COMMAND test_index)

add_executable(test_engine test_engine.cpp)
target_link_libraries(test_engine PRIVATE softadastra::store)

add_test(NAME test_engine COMMAND test_engine)

add_executable(test_recovery test_recovery.cpp)
target_link_libraries(test_recovery PRIVATE softadastra::store)

add_test(NAME test_recovery COMMAND test_recovery)
