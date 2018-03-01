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

if (DEFINED CFG_SCREENSHOT_ENABLE)
    add_definitions(
        -DCFG_SCREENSHOT_ENABLE
        )
endif()

if (DEFINED CFG_REGISTER_SIP_SERVER)
    add_definitions(
        -DCFG_REGISTER_SIP_SERVER
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
    ${PROJECT_SOURCE_DIR}/sdk/share/microhttpd
    ${PROJECT_SOURCE_DIR}/sdk/share/microhttpd/include
    ${PROJECT_SOURCE_DIR}/sdk/share/sntp
    ${PROJECT_SOURCE_DIR}/sdk/share/sntp_server
    )

add_executable(${CMAKE_PROJECT_NAME}
    addressbook.c
    audio.c
    calllog.c
    capture.c
    config.c
    device_info.c
    doorbell.h
    event.c
    guard.c
    linphone.c
    main.c
    network.c
    py_ime.c
    remote.c
    scene.c
    scene.h
    layer_account.c
    layer_account_keyboard.c
    layer_called.c
    layer_calling.c
    layer_camera.c
    layer_camera_lobby.c
    layer_camera_view.c
    layer_date.c
    layer_error.c
    layer_guard_log.c
    layer_guard_log_view.c
    layer_login.c
    layer_logo.c
    layer_main.c
    layer_main_menu.c
    layer_phone.c
    layer_phone_admin.c
    layer_phone_misc.c
    layer_phone_resident.c
    layer_record.c
    layer_record_view.c
    layer_redirect.c
    layer_redirect_admin.c
    layer_redirect_misc.c
    layer_redirect_resident.c
    layer_screensaver.c
    layer_setting_display.c
    layer_setting_doorcamera.c
    layer_setting_lang.c
    layer_setting_misc.c
    layer_setting_network.c
    layer_setting_sound.c
    layer_setting_sysinfo.c
    layer_setting_terminal.c
    layer_setting_voice.c
    layer_time.c
    layer_touch_calibration.c
    layer_upgrade.c
    layer_videomemo.c
    screen.c
    string.c
    upgrade.c
    userlist.c
    video_record.c        
    webserver.c
    function_table.c
    reset.c
    )

# directories
file(MAKE_DIRECTORY
    ${CMAKE_BINARY_DIR}/data/temp/capture
)

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
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/userlist.xml ${CMAKE_BINARY_DIR}/data/private/userlist.xml COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_admin.ini ${CMAKE_BINARY_DIR}/data/public/doorbell_admin.ini COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/linphonerc ${CMAKE_BINARY_DIR}/data/public/linphonerc COPYONLY)

file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/rings
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/itu/doorbell_admin.itu
    DESTINATION ${CMAKE_BINARY_DIR}/data/private
    )

# backups
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_admin.ini ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/doorbell_admin.ini COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/linphonerc ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/linphonerc COPYONLY)

# codec
if ($ENV{CFG_PLATFORM} STREQUAL openrtos)
    configure_file(${PROJECT_SOURCE_DIR}/build/$ENV{CFG_BUILDPLATFORM}/codec/lib/sm32/mp3.codecs ${CMAKE_BINARY_DIR}/data/private/codec/mp3.codecs COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/build/$ENV{CFG_BUILDPLATFORM}/codec/lib/sm32/wave.codecs ${CMAKE_BINARY_DIR}/data/private/codec/wave.codecs COPYONLY)
endif()

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
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/userlist.xml
    DESTINATION ${CMAKE_BINARY_DIR}/pack/input/userlist
    )

file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/doorbell_admin.ini
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
