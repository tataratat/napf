set(NAPF_SOURCES
    classes/radius_search_result_vectors.cpp classes/int_trees.cpp
    classes/long_trees.cpp classes/float_trees.cpp classes/double_trees.cpp
    napf.cpp)

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

python_add_library(_napf MODULE ${NAPF_SOURCES})
target_link_libraries(_napf PRIVATE pybind11::headers napf)
target_compile_definitions(_napf PRIVATE $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(_napf PRIVATE $<$<NOT:$<CONFIG:Debug>>:-O3>)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSCV")
  target_compile_options(_napf PRIVATE $<$<NOT:$<CONFIG:Debug>>:/O2>)
endif()

install(
  TARGETS _napf
  DESTINATION napf
  COMPONENT PythonModule)
