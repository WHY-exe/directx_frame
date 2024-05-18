# cmake root file
cmake_minimum_required(VERSION 3.11...3.20)

if (WIN32)
    # enter your project name here
    project(directx11_frame)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    # specify c++ standar
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    # msvc compile option
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        # set debug compile option
        if (POLICY CMP0141)
            cmake_policy(SET CMP0141 NEW)
            set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF: $< AND: $<C_COMPILER_ID:MSVC>, $<CXX_COMPILER_ID:MSVC> >, $< $<CONFIG:Debug,RelWithDebInfo>:EditAndContinue >, $< $<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
        endif()
        # active multi-thread compliation flag on msvc
        cmake_host_system_information(RESULT CPU_NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${CPU_NUMBER_OF_LOGICAL_CORES}")
        add_compile_definitions(
            _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
        )
        add_definitions(-DIS_DEBUG)
    endif()
    # solve the cmake windows dll problem by
    # specifying the global dll ouput path
    set(BIN_OUT_DIR "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BIN_OUT_DIR}/static_lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BIN_OUT_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_OUT_DIR}")
    file(COPY "${CMAKE_SOURCE_DIR}/asset/Models" DESTINATION "${BIN_OUT_DIR}/res")
    file(COPY "${CMAKE_SOURCE_DIR}/asset/SkyBox" DESTINATION "${BIN_OUT_DIR}/res")
    # cpp source dir
    add_subdirectory(src)
    add_subdirectory(third-party)
else()
    message("this project doesn't support compiler other than MSVC")
endif()