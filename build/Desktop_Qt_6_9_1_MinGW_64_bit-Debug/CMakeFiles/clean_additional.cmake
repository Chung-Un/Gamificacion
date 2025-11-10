# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Estru2Parcial1Qt_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Estru2Parcial1Qt_autogen.dir\\ParseCache.txt"
  "Estru2Parcial1Qt_autogen"
  )
endif()
