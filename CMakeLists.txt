cmake_minimum_required(VERSION 3.16)
project(QuantPricingEngine VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build unit tests" ON)

add_library(quantlib
    src/BlackScholes.cpp
    src/ImpliedVolSolver.cpp
    src/MarketSurface.cpp
    src/CsvReader.cpp
    src/ImpliedVolatilitySurfaceBuilder.cpp
    src/SurfaceExporter.cpp
    src/HestonFourierPricer.cpp
    src/HestonSurfaceAnalyzer.cpp
    src/HestonCalibrator.cpp
)

target_include_directories(quantlib PUBLIC include)

add_executable(run_bs_demo apps/run_bs_demo.cpp)
target_link_libraries(run_bs_demo PRIVATE quantlib)

add_executable(run_market_analysis apps/run_market_analysis.cpp)
target_link_libraries(run_market_analysis PRIVATE quantlib)

add_executable(run_heston_demo apps/run_heston_demo.cpp)
target_link_libraries(run_heston_demo PRIVATE quantlib)

add_executable(run_heston_surface_analysis apps/run_heston_surface_analysis.cpp)
target_link_libraries(run_heston_surface_analysis PRIVATE quantlib)

add_executable(run_heston_calibration apps/run_heston_calibration.cpp)
target_link_libraries(run_heston_calibration PRIVATE quantlib)

if(BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(test_black_scholes tests/test_black_scholes.cpp)
    target_link_libraries(test_black_scholes PRIVATE quantlib GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(test_black_scholes)
endif()