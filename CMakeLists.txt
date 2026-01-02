cmake_minimum_required(VERSION 3.20)
project(ctmd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 1. 의존성 관리
include(FetchContent)

FetchContent_Declare(
    mdspan
    GIT_REPOSITORY https://github.com/kokkos/mdspan
    GIT_TAG        mdspan-0.6.0
)
FetchContent_MakeAvailable(mdspan)

# ----------------------------------------------------------------
# 2. ctmd 라이브러리 정의
# ----------------------------------------------------------------

add_library(ctmd INTERFACE)
add_library(ctmd::ctmd ALIAS ctmd)

# 헤더 경로 설정
target_include_directories(ctmd INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# [핵심 수정 1] 매크로 설정 전파
# 상위 프로젝트에서 헤더를 인클루드할 때 이 매크로들이 반드시 살아있어야 합니다.
target_compile_definitions(ctmd INTERFACE 
    MDSPAN_USE_BRACKET_OPERATOR=0
    MDSPAN_IMPL_STANDARD_CPP_LIB_AVAILABLE=0
    USE_EIGEN
)

# [핵심 수정 2] 컴파일 옵션 조정
# -fno-exceptions 같은 옵션을 INTERFACE로 두면 pybind11이 죽습니다.
# Bazel의 동작을 모사하되, 상위에서 조절 가능하도록 최적화 위주로 설정합니다.
target_compile_options(ctmd INTERFACE 
    -O3 -funroll-loops -Wall -march=native
)

# 의존성 연결
target_link_libraries(ctmd INTERFACE mdspan)