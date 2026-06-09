# Блокировка повторного включения модуля
if(TARGET doc)
    return()
endif()

find_package(Doxygen)

if(DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )

    if(BUILD_DOCS)
        set(ALL_KEYWORD "ALL")
    else()
        set(ALL_KEYWORD "")
    endif()

    add_custom_target(doc ${ALL_KEYWORD}
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_SOURCE_DIR}/docs/html"
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Clear old and generate new docs Doxygen..."
        VERBATIM
    )
else()
    message(WARNING "Doxygen not found!")
endif()
