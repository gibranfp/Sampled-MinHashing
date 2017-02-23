option ( PYTHON_BINDINGS "Build python bindings via swig" ON )
option ( LINK_PYTHON_LIBRARY "Link against python libraries" ON )

if ( PYTHON_BINDINGS )
  find_package ( SWIG 2.0 REQUIRED )
  find_package ( PythonLibs 2.7 REQUIRED )
  find_package ( PythonInterp 2.7 REQUIRED )
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
  file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


  execute_process ( COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print( sysconfig.get_python_lib( plat_specific=True, prefix='${CMAKE_INSTALL_PREFIX}' ) )"
    OUTPUT_VARIABLE _ABS_PYTHON_MODULE_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE )
  
  get_filename_component ( _ABS_PYTHON_MODULE_PATH ${_ABS_PYTHON_MODULE_PATH} ABSOLUTE )
  file ( RELATIVE_PATH _REL_PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX} ${_ABS_PYTHON_MODULE_PATH} )
  
  set ( PYTHON_MODULE_PATH
    ${_REL_PYTHON_MODULE_PATH}
    )

  INSTALL ( FILES ${CMAKE_BINARY_DIR}/python/smh/_smh_api.so ${CMAKE_BINARY_DIR}/python/smh/smh_api.py ${CMAKE_BINARY_DIR}/python/smh/smh.py DESTINATION ${PYTHON_MODULE_PATH} )
  
else ()
  message(STATUS "Not building python bindings")
endif ()
