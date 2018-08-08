
# =========== helper macro to add a standard ilctest ==========================
ADD_CUSTOM_TARGET( tests )
MACRO( ADD_ILCTEST _name )

    INCLUDE_DIRECTORIES( "${ILCTEST_INCLUDE_DIRS}" )

    IF( BUILD_TESTING )
        ADD_EXECUTABLE( ${_name} ${ARGN} )
    ELSE()
        ADD_EXECUTABLE( ${_name} EXCLUDE_FROM_ALL ${ARGN} )
    ENDIF()

    ADD_DEPENDENCIES( tests ${_name} )

    #ADD_TEST( t_${_name} "${CMAKE_CURRENT_BINARY_DIR}/${_name}" )
    ADD_TEST( t_${_name} "${EXECUTABLE_OUTPUT_PATH}/${_name}" )
    SET_TESTS_PROPERTIES( t_${_name} PROPERTIES PASS_REGULAR_EXPRESSION "TEST_PASSED" )
    SET_TESTS_PROPERTIES( t_${_name} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED" )

ENDMACRO()
# =============================================================================

