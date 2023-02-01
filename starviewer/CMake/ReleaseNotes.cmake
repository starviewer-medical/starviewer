set(RELEASE_NOTES_DIR ${CMAKE_SOURCE_DIR}/releasenotes)

# TODO Adapt for Windows
add_custom_command(
    OUTPUT ${RELEASE_NOTES_DIR}/changelog.html ${RELEASE_NOTES_DIR}/recentchangelog.html
    COMMAND xsltproc
    ARGS changelog.xsl changelog.xml > changelog.html
    COMMAND xsltproc
    ARGS --stringparam fullChangelog changelog.html recentchangelog.xsl changelog.xml > recentchangelog.html
    MAIN_DEPENDENCY ${RELEASE_NOTES_DIR}/changelog.xml
    DEPENDS ${RELEASE_NOTES_DIR}/changelog.xsl ${RELEASE_NOTES_DIR}/recentchangelog.xsl
    WORKING_DIRECTORY ${RELEASE_NOTES_DIR}
    VERBATIM
)
