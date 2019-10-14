<<<<<<< HEAD
# Install script for directory: /cygdrive/c/Users/minht/CLionProjects/os/hw3
=======
# Install script for directory: /afs/ee.cooper.edu/user/n/nguyen8/CLionProjects/hw3
>>>>>>> 6fdac1f53a1dbc1928b49d2b98a5612266e52b52

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

<<<<<<< HEAD
=======
# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

>>>>>>> 6fdac1f53a1dbc1928b49d2b98a5612266e52b52
# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
<<<<<<< HEAD
file(WRITE "/cygdrive/c/Users/minht/CLionProjects/os/hw3/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
=======
file(WRITE "/afs/ee.cooper.edu/user/n/nguyen8/CLionProjects/hw3/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
>>>>>>> 6fdac1f53a1dbc1928b49d2b98a5612266e52b52
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
