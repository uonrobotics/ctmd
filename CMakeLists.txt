cmake_minimum_required(VERSION 3.20)

# 1. 프로젝트 이름 (Bazel 워크스페이스 명칭 유지)
project(ctmd-repo LANGUAGES CXX)

# 2. C++23 및 기본 설정 (.bazelrc 반영)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 3. 의존성 관리 (WORKSPACE 대체)
include(FetchContent)

# mdspan (v0.6.0)
FetchContent_Declare(
    mdspan
    GIT_REPOSITORY https://github.com/kokkos/mdspan
    GIT_TAG        mdspan-0.6.0
)
FetchContent_MakeAvailable(mdspan)

# mdspan 매크로 설정 (Bazel defines 반영)
target_compile_definitions(mdspan INTERFACE MDSPAN_USE_BRACKET_OPERATOR=0)

# ----------------------------------------------------------------
# 4. ctmd 라이브러리 정의 (INTERFACE 타겟)
# ----------------------------------------------------------------

# 타겟 이름은 ctmd_lib으로 생성
add_library(ctmd_lib INTERFACE)

# 외부에서 사용할 공식 별칭 설정: ctmd::ctmd
add_library(ctmd::ctmd ALIAS ctmd_lib)

# 헤더 경로 설정 (Bazel의 hdrs = glob 반영)
# 프로젝트 루트를 추가하여 #include "ctmd/..."가 가능하게 함
target_include_directories(ctmd_lib INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# 컴파일 옵션 반영 (Bazel의 copts)
# 인터페이스 타겟이므로 INTERFACE로 옵션을 전파합니다.
target_compile_options(ctmd_lib INTERFACE 
    -O3 -funroll-loops -fno-exceptions -fno-rtti -Wall -march=native
)

# 의존성 연결
target_link_libraries(ctmd_lib INTERFACE mdspan)