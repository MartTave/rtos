cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(
    SensorBME280
    VERSION 0.1
    DESCRIPTION "CPP Sensor BME280 reading application."
    LANGUAGES C CXX
)

target_sources(
    app
    PRIVATE
    src/main.cpp
)
