cmake_minimum_required(VERSION 3.11...3.20)

set(PROJECT_COMPILE_FLAG )
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(PROJECT_COMPILE_FLAG "${CMAKE_CXX_FLAGS} /MDd")
else()
    set(PROJECT_COMPILE_FLAG "${CMAKE_CXX_FLAGS} /MD")
endif()
set(PROJECT_COMPILE_FLAG "${CMAKE_CXX_FLAGS} /nologo /W4")
# public include dir
set(INCLUDE_BASE ${CMAKE_SOURCE_DIR}/include)
set(THIRD_PARTY_INCLUDE ${CMAKE_SOURCE_DIR}/third-party)
include_directories(
    ${INCLUDE_BASE}
    ${INCLUDE_BASE}/lib
    ${THIRD_PARTY_INCLUDE}/imgui
    ${THIRD_PARTY_INCLUDE}/imgui/backends
    ${THIRD_PARTY_INCLUDE}/DirectXTex
)
# compile hlsl shader
add_custom_target(shaders)
set(SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/shader)
set( HLSL_SHADER_FILES 
    ${SHADER_PATH}/BlurOutlinePS.hlsl 
    ${SHADER_PATH}/BlurPS.hlsl
    ${SHADER_PATH}/Full_VS.hlsl
    ${SHADER_PATH}/lightBall_PS.hlsl
    ${SHADER_PATH}/PS.hlsl
    ${SHADER_PATH}/PSTex.hlsl
    ${SHADER_PATH}/PSTexAlpha.hlsl
    ${SHADER_PATH}/PSTexNorm.hlsl
    ${SHADER_PATH}/PSTexSpec.hlsl
    ${SHADER_PATH}/PSTexSpecNorm.hlsl
    ${SHADER_PATH}/PSTexSpecAlpha.hlsl
    ${SHADER_PATH}/PSTexSpecNormAlpha.hlsl
    ${SHADER_PATH}/SkyBox_PS.hlsl
    ${SHADER_PATH}/Solid_PS.hlsl
    ${SHADER_PATH}/Solid_VS.hlsl
    ${SHADER_PATH}/SkyBox_VS.hlsl
    ${SHADER_PATH}/Shadow_VS.hlsl
    ${SHADER_PATH}/VSTex.hlsl
    ${SHADER_PATH}/VSTexNorm.hlsl
)
# create cso output dir
execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_OUT_DIR}/res/cso/)
set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "5_0")
foreach(FILE ${HLSL_SHADER_FILES})
    set(ISPS 0)
    set(ISVS 0)
    string(FIND ${FILE} "PS" ISPS)
    string(FIND ${FILE} "VS" ISVS)

    if (ISPS GREATER -1)
        message("shader file: ${FILE} is pixel shader")
        set_source_files_properties(${FILE} PROPERTIES ShaderType "ps")
    elseif(ISVS GREATER -1)
        message("shader file: ${FILE} is vertex shader")
        set_source_files_properties(${FILE} PROPERTIES ShaderType "vs")
    endif()
    get_filename_component(FILE_WE ${FILE} NAME_WE)
    get_source_file_property(shadertype ${FILE} ShaderType)
    get_source_file_property(shadermodel ${FILE} ShaderModel)
    add_custom_command(TARGET shaders
                        COMMAND fxc.exe /nologo /Emain /T${shadertype}_${shadermodel} $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo ${BIN_OUT_DIR}/res/cso/${FILE_WE}.cso /Fd ${BIN_OUT_DIR}/res/cso/${FILE_WE}.pdb ${FILE}
                        MAIN_DEPENDENCY ${FILE}
                        COMMENT "HLSL ${FILE}"
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        VERBATIM)
endforeach(FILE)

add_subdirectory(engine)