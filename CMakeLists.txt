cmake_minimum_required(VERSION 3.5)
idf_component_register(SRCS "OtaUpdateManager.cpp" INCLUDE_DIRS "." REQUIRES freertos esp-nimble-cpp esp-nimble-cpp app_update )

