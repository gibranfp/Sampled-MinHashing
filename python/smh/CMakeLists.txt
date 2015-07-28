option ( PYTHON_BINDINGS "Build python bindings via swig" ON )
option ( LINK_PYTHON_LIBRARY "Link against python libraries" ON )

if ( PYTHON_BINDINGS )
	find_package ( SWIG )
	find_package ( PythonLibs )
endif ()

find_program(PYTHON "python")

if ( PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )
	include ( UseSWIG )
  	include_directories ( BEFORE ${CMAKE_SOURCE_DIR}/include/smh )
  	include_directories ( ${PYTHON_INCLUDE_DIRS} )
  	link_directories ( ${CMAKE_CURRENT_BINARY_DIR}/src/smh )

   swig_add_module(smh_api python smh_api.i)
   swig_link_libraries(smh_api ${PYTHON_LIBRARIES} smh)
    
	file(GLOB DEPS "*.py")

   add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/smh.py 
     COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/smh.py ${CMAKE_CURRENT_BINARY_DIR}/)

   add_custom_target(copy ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/smh.py )
                   
else ()
  message(STATUS "Not building python bindings")
endif ()