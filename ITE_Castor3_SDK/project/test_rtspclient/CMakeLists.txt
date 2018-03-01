if (DEFINED CFG_RTSP_CLIENT_ENABLE)
    add_definitions(
        -DCFG_RTSP_CLIENT_ENABLE
    )
endif() 

include_directories(
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg
    ${PROJECT_SOURCE_DIR}/sdk/driver/itv
)

add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
    test_rtspclient.c
    network.c
)


# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
