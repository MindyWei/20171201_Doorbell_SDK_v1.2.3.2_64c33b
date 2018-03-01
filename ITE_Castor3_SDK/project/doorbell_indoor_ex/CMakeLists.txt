set(PROJ_NAME ${CMAKE_PROJECT_NAME})

if (DEFINED CFG_BOOTLOADER_ENABLE)
    set(CMAKE_PROJECT_NAME bootloader)
    add_subdirectory(../bootloader ../bootloader)
endif()

if (DEFINED CFG_AUTO_ANSWER)
    add_definitions(
        -DCFG_AUTO_ANSWER
        )
endif()

set(CMAKE_PROJECT_NAME ${PROJ_NAME})

include_directories(
	${PROJECT_SOURCE_DIR}/sdk/include/leaf
    ${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg
    )

add_executable(${CMAKE_PROJECT_NAME}
    audio.c	
    config.c	
    network.c
    layer_called.c
    layer_camera.c
    layer_camera_outdoor.c
    layer_camera_view.c
    layer_main.c
    layer_main_menu.c    
    layer_message.c
    layer_message_record.c
    layer_message_record_view.c
    layer_message_service.c
    layer_message_service_view.c    
    layer_message_video.c
    layer_message_video_view.c	 
    leafapi.c
    main.c
    scene.c
    function_table.c
    )

# files
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/linphonerc ${CMAKE_BINARY_DIR}/data/public/linphonerc COPYONLY)

# files
file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/rings
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/itu/doorbell_indoor.itu
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_indoor.ini
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/Esound.ini
    DESTINATION ${CMAKE_BINARY_DIR}/data/private
    )

# font
file(COPY
    ${PROJECT_SOURCE_DIR}/data/font/${CFG_FONT_FILENAME}
    DESTINATION ${CMAKE_BINARY_DIR}/data/private/font
    )

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
