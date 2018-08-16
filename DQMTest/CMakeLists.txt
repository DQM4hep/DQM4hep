

dqm4hep_package( DQMTest
  USES DQMCore
  INCLUDE_DIRS include
)

# DQMCore tests
dqm4hep_add_test_reg ( test-directory 
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-global-header
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-me-json
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-me-mgr
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-plugin
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-qtest-chi2
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-qtest-exact-ref-comp
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-qtest-fit-param-range
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-qtest-kolmogorov
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-qtest-property-within-exp
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-root-event-streamer
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-signal
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-storage
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-wildcard-match
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
)
dqm4hep_add_test_reg ( test-xmlparser 
  BUILD_EXEC 
  REGEX_FAIL "TEST_FAILED" 
  EXEC_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/resources/test_xmlparser.xml
)

