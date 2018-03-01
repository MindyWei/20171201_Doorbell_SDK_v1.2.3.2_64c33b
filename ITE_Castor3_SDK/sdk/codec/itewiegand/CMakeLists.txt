INCLUDE_DIRECTORIES(
    ${SMEDIA_INC_DIRS}
    )

SET(ITEWIEGAND_SRCS
    main.c
    ith_clock.c
    wiegand.c
)
IF(ENABLE_CODECS_PLUGIN)
    ADD_EXECUTABLE(itewiegand.plugin
        ${ITEWIEGAND_SRCS}
    )
  if (DEFINED CFG_ROM_COMPRESS)
		set(args ${args} -z -b 512K)
    if (CFG_CHIP_FAMILY EQUAL 9070)
        ADD_CUSTOM_COMMAND(
            TARGET itewiegand.plugin
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY}
                    #-O binary ${CMAKE_CURRENT_BINARY_DIR}/itewiegand.plugin ${LIBRARY_OUTPUT_PATH}/itewiegand.codecs
                    -O binary -j .risc1_header -j .text -j .data ${CMAKE_CURRENT_BINARY_DIR}/itewiegand.plugin ${LIBRARY_OUTPUT_PATH}/itewiegand.rawcodecs
            COMMAND ${READELF} -a ${CMAKE_CURRENT_BINARY_DIR}/itewiegand.plugin > ${LIBRARY_OUTPUT_PATH}/itewiegand.symbol
            COMMAND ${DATACONV} -x ${LIBRARY_OUTPUT_PATH}/itewiegand.rawcodecs
            COMMAND mkrom
            ARGS ${args} ${PROJECT_SOURCE_DIR}/sdk/target/ram/${CFG_RAM_INIT_SCRIPT} ${LIBRARY_OUTPUT_PATH}/itewiegand.rawcodecs ${LIBRARY_OUTPUT_PATH}/itewiegand.codecs
        )
     else ()
        ADD_CUSTOM_COMMAND(
            TARGET itewiegand.plugin
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY}
                    #-O binary ${CMAKE_CURRENT_BINARY_DIR}/itewiegand.plugin ${LIBRARY_OUTPUT_PATH}/itewiegand.codecs
                    -O binary ${CMAKE_CURRENT_BINARY_DIR}/itewiegand.plugin ${LIBRARY_OUTPUT_PATH}/itewiegand.rawcodecs
            COMMAND ${READELF} -a ${CMAKE_CURRENT_BINARY_DIR}/itewiegand.plugin > ${LIBRARY_OUTPUT_PATH}/itewiegand.symbol
            COMMAND ${DATACONV} -x ${LIBRARY_OUTPUT_PATH}/itewiegand.rawcodecs
            COMMAND mkrom
            ARGS ${args} ${PROJECT_SOURCE_DIR}/sdk/target/ram/${CFG_RAM_INIT_SCRIPT} ${LIBRARY_OUTPUT_PATH}/itewiegand.rawcodecs ${LIBRARY_OUTPUT_PATH}/itewiegand.codecs
        )
     endif ()
  endif (DEFINED CFG_ROM_COMPRESS)   
ELSE(ENABLE_CODECS_PLUGIN)
    ADD_LIBRARY(itewiegand STATIC
        ${itewiegand_SRCS}
    )
ENDIF(ENABLE_CODECS_PLUGIN)
