set(PROJ_NAME ${CMAKE_PROJECT_NAME})

if (DEFINED CFG_BOOTLOADER_ENABLE)
    set(CMAKE_PROJECT_NAME bootloader)
    add_subdirectory(../bootloader ../bootloader)
endif()

if (DEFINED CFG_LOOP_DIALING)
    add_definitions(
        -DCFG_LOOP_DIALING
        )
endif()

set(CMAKE_PROJECT_NAME ${PROJ_NAME})

include_directories(
	${PROJECT_SOURCE_DIR}/sdk/include/leaf
	${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg
    )

add_executable(${CMAKE_PROJECT_NAME}
    network.c
    layer_calling.c
    layer_main.c
    function_table.c
    main.c
    scene.c
    scene.h
    config.c
    )

# files
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_lobby.ini ${CMAKE_BINARY_DIR}/data/public/doorbell_lobby.ini COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/fw/${SENSOR_FW_NAME}/jedi.rom ${CMAKE_BINARY_DIR}/data/private/jedi.rom COPYONLY)

# files
file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/rings
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/itu/doorbell_lobby_chs.itu
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_lobby.ini    
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
