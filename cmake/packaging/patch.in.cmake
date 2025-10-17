file(GLOB_RECURSE CMAKE_FILE "@CMAKE_PROJECT_NAME@Targets.cmake")
file(READ ${CMAKE_FILE} FILE_CONTENTS)

# By default, the INTERFACE_LINK_LIBRARIES entry lists all of libOpenCOR's dependencies, but we don't need them since we
# embed them in our static version of libOpenCOR. However, on Windows and macOS, the INTERFACE_LINK_LIBRARIES entry
# should list some needed system libraries. So, depending on the platform, we either set the INTERFACE_LINK_LIBRARIES
# entry to those needed system libraries or remove the INTERFACE_LINK_LIBRARIES entry altogether.

if(WIN32)
    string(REGEX REPLACE "INTERFACE_LINK_LIBRARIES[^\n]*\n" "INTERFACE_LINK_LIBRARIES \"$<LINK_ONLY:crypt32.lib>;$<LINK_ONLY:version.lib>;$<LINK_ONLY:wldap32.lib>;$<LINK_ONLY:ws2_32.lib>\"\n" FILE_CONTENTS "${FILE_CONTENTS}")
elseif(APPLE)
    string(REGEX REPLACE "INTERFACE_LINK_LIBRARIES[^\n]*\n" "INTERFACE_LINK_LIBRARIES \"-framework CoreFoundation;-framework SystemConfiguration;$<LINK_ONLY:ldap>\"\n" FILE_CONTENTS "${FILE_CONTENTS}")
else()
    string(REGEX REPLACE "\n[^\n]*INTERFACE_LINK_LIBRARIES[^\n]*\n" "\n" FILE_CONTENTS "${FILE_CONTENTS}")
endif()

file(WRITE ${CMAKE_FILE} "${FILE_CONTENTS}")
