# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/lekix/esp/esp-idf/components/bootloader/subproject"
  "/home/lekix/Desktop/DEV/42/42-LED_MATRIX_ESP32/build/bootloader"
  "/home/lekix/Desktop/DEV/42/42-LED_MATRIX_ESP32/build/bootloader-prefix"
  "/home/lekix/Desktop/DEV/42/42-LED_MATRIX_ESP32/build/bootloader-prefix/tmp"
  "/home/lekix/Desktop/DEV/42/42-LED_MATRIX_ESP32/build/bootloader-prefix/src/bootloader-stamp"
  "/home/lekix/Desktop/DEV/42/42-LED_MATRIX_ESP32/build/bootloader-prefix/src"
  "/home/lekix/Desktop/DEV/42/42-LED_MATRIX_ESP32/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/lekix/Desktop/DEV/42/42-LED_MATRIX_ESP32/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/lekix/Desktop/DEV/42/42-LED_MATRIX_ESP32/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
