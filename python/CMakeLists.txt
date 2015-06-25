option ( PYTHON_BINDINGS "Build python bindings via swig" ON )
option ( LINK_PYTHON_LIBRARY "Link against python libraries" ON )

if ( PYTHON_BINDINGS )
	find_package ( SWIG )
	find_package ( PythonLibs )
endif ()

if ( SWIG_FOUND AND PYTHONLIBS_FOUND )
	include ( UseSWIG )
  	include_directories ( BEFORE ${CMAKE_SOURCE_DIR}/include )
  	include_directories ( ${PYTHON_INCLUDE_DIRS} )
  	link_directories ( ${CMAKE_CURRENT_BINARY_DIR}/src/smh )

    swig_add_module(smh_api python smh_api.i)
    swig_link_libraries(smh_api ${PYTHON_LIBRARIES} smh)
    
    set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
    set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
    set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")

    configure_file(${SETUP_PY_IN} ${SETUP_PY})

    add_custom_command(OUTPUT ${OUTPUT}
                       COMMAND ${PYTHON} ${SETUP_PY} build
                       COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
                       DEPENDS ${DEPS})

    add_custom_target(target ALL DEPENDS ${OUTPUT})

    install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install)")

else ()
  message(STATUS "Not building python bindings")
endif ()
