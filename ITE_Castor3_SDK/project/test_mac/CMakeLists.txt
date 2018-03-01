
if (DEFINED CFG_ETH_MODE)
    add_definitions(
        -DCFG_ETH_MODE=${CFG_ETH_MODE}
        )
else()        
    add_definitions(
        -DCFG_ETH_MODE=1
        )
endif()


add_executable(${CMAKE_PROJECT_NAME}
    test_openrtos.c
    )

if (DEFINED CFG_TEST_WINDOW)
    add_executable(${CMAKE_PROJECT_NAME}
        test_mac100_window.c
        )
elseif (DEFINED CFG_TEST_LB_1)
    add_executable(${CMAKE_PROJECT_NAME}
        test_mac100_lb.c
        )
elseif (DEFINED CFG_TEST_LB_2)
    add_executable(${CMAKE_PROJECT_NAME}
        test_mac100_lb_2.c
        )
else()
    add_executable(${CMAKE_PROJECT_NAME}
        test_mac100_lwip.c
        )
endif()


# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
