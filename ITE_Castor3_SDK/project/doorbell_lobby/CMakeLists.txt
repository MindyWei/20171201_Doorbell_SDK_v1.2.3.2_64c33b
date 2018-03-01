set(PROJ_NAME ${CMAKE_PROJECT_NAME})

if (DEFINED CFG_BOOTLOADER_ENABLE)
    set(CMAKE_PROJECT_NAME bootloader)
    add_subdirectory(../bootloader ../bootloader)
endif()

set(CMAKE_PROJECT_NAME ${PROJ_NAME})

add_definitions(
    -DCFG_SNTP_PORT=${CFG_SNTP_PORT}
    -DCFG_SNTP_UPDATE_DELAY=${CFG_SNTP_UPDATE_DELAY}
    -DCFG_WEBSERVER_PORT=${CFG_WEBSERVER_PORT}
    -DCFG_WEBSERVER_PORT_STR="${CFG_WEBSERVER_PORT}"
    -DCFG_HW_VERSION="${CFG_HW_VERSION}"
    )

if (DEFINED CFG_GUARD_ENABLE)
    add_definitions(
        -DCFG_GUARD_ENABLE
        -DCFG_GPIO_GUARD_SENSORS=${CFG_GPIO_GUARD_SENSORS}
        )
endif()

if (DEFINED CFG_CHECK_FILES_CRC_ON_BOOTING)
    add_definitions(
        -DCFG_CHECK_FILES_CRC_ON_BOOTING
        )
endif()

if (DEFINED CFG_UDP_HEARTBEAT)
    add_definitions(
        -DCFG_UDP_HEARTBEAT
        -DCFG_UDP_HEARTBEAT_PORT=${CFG_UDP_HEARTBEAT_PORT}
        )
endif()

if (DEFINED CFG_CARDLIST_ENABLE)
    add_definitions(
        -DCFG_CARDLIST_ENABLE
        )
endif()

if (DEFINED CFG_SCREENSHOT_ENABLE)
    add_definitions(
        -DCFG_SCREENSHOT_ENABLE
        )
endif()

if (DEFINED CFG_LOOP_DIALING)
    add_definitions(
        -DCFG_LOOP_DIALING
        -DCFG_LOOP_DIALING_ADDRESS="${CFG_LOOP_DIALING_ADDRESS}"
        )
endif()

if (DEFINED CFG_REGISTER_SIP_SERVER)
    add_definitions(
        -DCFG_REGISTER_SIP_SERVER
        )
endif()

if (DEFINED CFG_CALL_SIP_SERVER)
    add_definitions(
        -DCFG_CALL_SIP_SERVER
        )
endif()

if (DEFINED CFG_RTSP_CLIENT_ENABLE)
    add_definitions(
        -DCFG_RTSP_CLIENT_ENABLE
        )
endif()

if (DEFINED CFG_RTSP_STREAMING)
    add_definitions(
        -DCFG_RTSP_STREAMING
        )
endif()

if (DEFINED CFG_DOWNLOAD_TO_SD_CARD)
    add_definitions(
        -DCFG_DOWNLOAD_TO_SD_CARD
        )
endif()

if (DEFINED CFG_DUMP_AEC_DATA)
    add_definitions(
        -DCFG_DUMP_AEC_DATA
        )
endif()


include_directories(
    ${PROJECT_SOURCE_DIR}/sdk/driver/mmp/include
    ${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg/libavcodec
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg/libavformat
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg/libavdevice
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg/libavutil
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg/libswresample
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg/libswscale
    ${PROJECT_SOURCE_DIR}/sdk/driver/itv/ferrari_foundry/
    ${PROJECT_SOURCE_DIR}/sdk/driver/itv/ferrari_foundry/core
    ${PROJECT_SOURCE_DIR}/sdk/share/microhttpd
    ${PROJECT_SOURCE_DIR}/sdk/share/microhttpd/include
    ${PROJECT_SOURCE_DIR}/sdk/share/sntp
    )

add_executable(${CMAKE_PROJECT_NAME}
    addressbook.c
    config.c
    device_info.c
    doorbell.h
    event.c
    guard.c
    layer_called.c
    layer_calling.c
    layer_main.c
    layer_user.c
    layer_opendoor.c
    layer_password.c
    layer_screensaver.c
    layer_setting.c
    layer_setting_audiovolume.c
    layer_setting_custom.c
    layer_setting_datasync.c
    layer_setting_doorpassword.c
    layer_setting_guard.c
    layer_setting_info.c
    layer_setting_localpassword.c
    layer_setting_localposition.c
    layer_setting_screensaver.c
    layer_setting_system.c
    layer_setting_time.c
    layer_upgrade.c
    layer_voicememo.c
    linphone.c
    main.c
    network.c
    peripheral.c
    remote.c
    function_table.c
    reset.c
    scene.c
    scene.h
    screen.c
    snapshot.c
    upgrade.c
    webserver.c
    backup.c
    )
 
if (DEFINED CFG_TV_ENCODER_ENABLE)
    add_executable(${CMAKE_PROJECT_NAME}
        tv_out.c
        )
endif() 

if (DEFINED CFG_CARDLIST_ENABLE)
    add_executable(${CMAKE_PROJECT_NAME}
        cardlist.c
        )
endif()

#remove old addressbook
file(GLOB files ${CMAKE_BINARY_DIR}/addressbook*)
foreach (src ${files})
	file(REMOVE ${src})
endforeach()
file(GLOB files ${CMAKE_BINARY_DIR}/data/private/*.ucl)
foreach (src ${files})
	file(REMOVE ${src})
endforeach()
file(REMOVE ${CMAKE_BINARY_DIR}/pack/input/addressbook)

#cut addressbook.xml that record > 20000
execute_process(COMMAND AddressBookSplitter --input ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/addressbook.xml --output ${CMAKE_BINARY_DIR}/)

# addressbook
if (CMAKE_HOST_WIN32)
    execute_process(COMMAND cmd /c for %I in (${CMAKE_BINARY_DIR}/addressbook.xml) do @echo %~zI OUTPUT_VARIABLE ADDRESSBOOK_FILESIZE)
else (CMAKE_HOST_WIN32)
    execute_process(COMMAND stat --format=%s ${CMAKE_BINARY_DIR}/addressbook.xml OUTPUT_VARIABLE ADDRESSBOOK_FILESIZE)
endif (CMAKE_HOST_WIN32)
#message("ADDRESSBOOK_FILESIZE=${ADDRESSBOOK_FILESIZE}")
math(EXPR ADDRESSBOOK_ALIGN "${ADDRESSBOOK_FILESIZE} % 4")
#message("ADDRESSBOOK_ALIGN=${ADDRESSBOOK_ALIGN}")
if (${ADDRESSBOOK_ALIGN} EQUAL 1)
    file(APPEND ${CMAKE_BINARY_DIR}/addressbook.xml "   ")
elseif (${ADDRESSBOOK_ALIGN} EQUAL 2)
    file(APPEND ${CMAKE_BINARY_DIR}/addressbook.xml "  ")
elseif (${ADDRESSBOOK_ALIGN} EQUAL 3)
    file(APPEND ${CMAKE_BINARY_DIR}/addressbook.xml " ")
endif()

execute_process(COMMAND uclpack --10 --nrv2e ${CMAKE_BINARY_DIR}/addressbook.xml ${CMAKE_BINARY_DIR}/addressbook.ucl)

# big addressbook
set(ADDRESSBOOK_NUM 0)
file(GLOB files ${CMAKE_BINARY_DIR}/addressbook*.xml)
foreach (src ${files})
	math(EXPR ADDRESSBOOK_NUM "${ADDRESSBOOK_NUM} + 1")
endforeach()
# sub addressbook.xml count
math(EXPR ADDRESSBOOK_NUM "${ADDRESSBOOK_NUM} - 1")
if(${ADDRESSBOOK_NUM} GREATER 0)
	foreach(Index RANGE 1 ${ADDRESSBOOK_NUM} 1)
		if (CMAKE_HOST_WIN32)
			execute_process(COMMAND cmd /c for %I in (${CMAKE_BINARY_DIR}/addressbook${Index}.xml) do @echo %~zI OUTPUT_VARIABLE ADDRESSBOOK_FILESIZE)
		else (CMAKE_HOST_WIN32)
			execute_process(COMMAND stat --format=%s ${CMAKE_BINARY_DIR}/addressbook${Index}.xml OUTPUT_VARIABLE ADDRESSBOOK_FILESIZE)
		endif (CMAKE_HOST_WIN32)
		math(EXPR ADDRESSBOOK_ALIGN "${ADDRESSBOOK_FILESIZE} % 4")
		if (${ADDRESSBOOK_ALIGN} EQUAL 1)
			file(APPEND ${CMAKE_BINARY_DIR}/addressbook${Index}.xml "   ")
		elseif (${ADDRESSBOOK_ALIGN} EQUAL 2)
			file(APPEND ${CMAKE_BINARY_DIR}/addressbook${Index}.xml "  ")
		elseif (${ADDRESSBOOK_ALIGN} EQUAL 3)
			file(APPEND ${CMAKE_BINARY_DIR}/addressbook${Index}.xml " ")
		endif()
		execute_process(COMMAND uclpack --10 --nrv2e ${CMAKE_BINARY_DIR}/addressbook${Index}.xml ${CMAKE_BINARY_DIR}/addressbook${Index}.ucl)
	endforeach(Index)
endif()

# files
configure_file(${CMAKE_BINARY_DIR}/addressbook.ucl ${CMAKE_BINARY_DIR}/data/private/addressbook.ucl COPYONLY)
if(${ADDRESSBOOK_NUM} GREATER 0)
	foreach(Index RANGE 1 ${ADDRESSBOOK_NUM} 1)
		configure_file(${CMAKE_BINARY_DIR}/addressbook${Index}.ucl ${CMAKE_BINARY_DIR}/data/private/addressbook${Index}.ucl COPYONLY)
	endforeach(Index)
endif()
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_lobby.ini ${CMAKE_BINARY_DIR}/data/public/doorbell_lobby.ini COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/fw/${SENSOR_FW_NAME}/jedi.rom ${CMAKE_BINARY_DIR}/data/private/jedi.rom COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/linphonerc ${CMAKE_BINARY_DIR}/data/public/linphonerc COPYONLY)

if (DEFINED CFG_CARDLIST_ENABLE)
    configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/cardlist.xml ${CMAKE_BINARY_DIR}/data/private/cardlist.xml COPYONLY)
endif()

file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/rings
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/itu/doorbell_lobby.itu
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/itu/doorbell_lobby_chs.itu
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/itu/doorbell_lobby_cht.itu    
    DESTINATION ${CMAKE_BINARY_DIR}/data/private
    )

# backups
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_lobby.ini ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/doorbell_lobby.ini COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/linphonerc ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/linphonerc COPYONLY)

# crc
if (DEFINED CFG_CHECK_FILES_CRC_ON_BOOTING)
    file(REMOVE ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/ite_crc.dat)
    execute_process(COMMAND makecrc -c ${CMAKE_BINARY_DIR}/data/public/ite_crc.dat -i ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE})
    execute_process(COMMAND makecrc -v ${CMAKE_BINARY_DIR}/data/public/ite_crc.dat -i ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE})
    file(COPY ${CMAKE_BINARY_DIR}/data/public/ite_crc.dat DESTINATION ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE})
endif()

# font
file(COPY
    ${PROJECT_SOURCE_DIR}/data/font/${CFG_FONT_FILENAME}
    DESTINATION ${CMAKE_BINARY_DIR}/data/private/font
    )

# web
file(GLOB files ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/web/*.*)
foreach (src ${files})
    string(REPLACE "${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/web/" "${CMAKE_BINARY_DIR}/data/private/web/" dest ${src})
    configure_file(${src} ${dest} COPYONLY)
endforeach()

# screen saver
file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/screensaver.jpg
    DESTINATION ${CMAKE_BINARY_DIR}/data/public
    )

# pack
file(MAKE_DIRECTORY
    ${CMAKE_BINARY_DIR}/pack/input/firmware/private
    )

file(COPY
    ${PROJECT_SOURCE_DIR}/tool/bin/pkgtool.exe
    DESTINATION ${CMAKE_BINARY_DIR}/pack
    )

file(COPY
    ${CMAKE_BINARY_DIR}/addressbook.ucl
    DESTINATION ${CMAKE_BINARY_DIR}/pack/input/addressbook
    )

if(${ADDRESSBOOK_NUM} GREATER 0)
	foreach(Index RANGE 1 ${ADDRESSBOOK_NUM} 1)
		file(COPY
		${CMAKE_BINARY_DIR}/addressbook${Index}.ucl
		DESTINATION ${CMAKE_BINARY_DIR}/pack/input/addressbook
		)
	endforeach(Index)
endif()

file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/cardlist.xml
    DESTINATION ${CMAKE_BINARY_DIR}/pack/input/cardlist
    )

file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_lobby.ini
    DESTINATION ${CMAKE_BINARY_DIR}/pack/input/setting
    )

file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/screensaver.jpg
    DESTINATION ${CMAKE_BINARY_DIR}/pack/input/screensaver
    )

file(GLOB files ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/pack/*.in)
foreach (src ${files})
    string(REPLACE "${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/pack/" "${CMAKE_BINARY_DIR}/pack/" tmp ${src})
    string(REPLACE ".in" "" dest ${tmp})
    configure_file(${src} ${dest})
endforeach()

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
