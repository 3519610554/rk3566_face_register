include(FetchContent)

#fmt库
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.0  # 具体版本
)

FetchContent_MakeAvailable(fmt)

#googletest库
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

#让cmake识别 fmt, googletest
find_package(fmt CONFIG REQUIRED)
