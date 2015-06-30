add_executable(sdk-test
    ViconDataStreamSDK_CPPTest.cpp
)

target_link_libraries(sdk-test
    ${Boost_THREAD_LIBRARIES}
    ${Boost_SYSTEM_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libDebugServices.so
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libViconDataStreamSDK_CPP.so
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libboost_system-mt.so.1.53.0
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libboost_thread-mt.so.1.53.0
)
