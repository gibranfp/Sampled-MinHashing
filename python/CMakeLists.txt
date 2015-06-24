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

    swig_add_module(smh python smh.i)
    swig_link_libraries(smh ${PYTHON_LIBRARIES} smh)


else ()
  message(STATUS "Not building python bindings")
endif ()
