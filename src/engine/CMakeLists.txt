# 这个项目输出可执行文件
project(engine)
# 让ide识别头文件
file(GLOB include_file "${INCLUDE_BASE}/${PROJECT_NAME}/*.h")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src main)
add_executable(${PROJECT_NAME} ${main} ${include_file})
# setting up include dir
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${INCLUDE_BASE}/${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC
    DirectXTex
    assimp
    imgui
)
target_compile_definitions(${PROJECT_NAME} PUBLIC _WINDOWS)
target_link_options(${PROJECT_NAME} PUBLIC /subsystem:windows)
# file(COPY "${PROJECT_SOURCE_DIR}/shader/" DESTINATION "${BIN_OUT_DIR}/shader")
# setting up compile flags
set_property(TARGET ${PROJECT_NAME} APPEND_STRING
    PROPERTY COMPILE_FLAGS ${PROJECT_COMPILE_FLAG}
)
add_dependencies(${PROJECT_NAME}
    DirectXTex
    assimp
    imgui
    shaders
)