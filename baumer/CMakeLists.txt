
EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message(STATUS "Detected architecture ${ARCHITECTURE}")

install(DIRECTORY DESTINATION lib/rc_genicam_api
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

if (${ARCHITECTURE} STREQUAL "armv7l")
  install(FILES Ubuntu-14.04/armv7l/libbgapi2_gige.cti COMPONENT bin DESTINATION lib/rc_genicam_api)
elseif (${ARCHITECTURE} STREQUAL "x86_64")
  install(FILES Ubuntu-14.04/x86_64/libbgapi2_gige.cti COMPONENT bin DESTINATION lib/rc_genicam_api)
elseif (${ARCHITECTURE} STREQUAL "i686")
  install(FILES Ubuntu-14.04/i686/libbgapi2_gige.cti COMPONENT bin DESTINATION lib/rc_genicam_api)
else ()
  message(FATAL_ERROR "Unknown architecture")
endif ()

install(FILES bsysgige.xml COMPONENT bin DESTINATION lib/rc_genicam_api)
