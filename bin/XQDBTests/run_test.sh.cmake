${CMAKE_BINARY_DIR}/bin/zorba -c -f -q ${CMAKE_CURRENT_SOURCE_DIR}/Queries/$1 < ${CMAKE_CURRENT_SOURCE_DIR}/Commands/$2 > ${CMAKE_CURRENT_BINARY_DIR}/$3
${CMAKE_COMMAND} -E compare_files ${CMAKE_CURRENT_BINARY_DIR}/$3 ${CMAKE_CURRENT_SOURCE_DIR}/ExpectedResults/$3
