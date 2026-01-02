workspace(name = "ctmd-repo")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:local.bzl", "local_repository")

# Other bazel rules
git_repository(
    name = "rules_foreign_cc",
    remote = "https://github.com/bazelbuild/rules_foreign_cc",
    tag = "0.9.0",
)
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
rules_foreign_cc_dependencies()

# Google Test
git_repository(
    name = "gtest",
    remote = "https://github.com/google/googletest",
    tag = "v1.14.0",
)

git_repository(
    name = "glog",
    remote = "https://github.com/google/glog",
    tag = "v0.5.0",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp",
    tag = "20211102.0",
)

git_repository(
    name = "com_google_benchmark",
    remote = "https://github.com/google/benchmark",
    tag = "v1.5.1",
)

new_local_repository(
    name = "usr_local",
    build_file = "third_party/usr_local.BUILD",
    path = "/usr/local",
)

new_local_repository(
    name = "zlib",
    build_file = "third_party/zlib.BUILD",
    path = "/usr/local/opt/zlib",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/gflags/gflags",
    tag = "v2.2.2",
)

# Eigen
http_archive(
    name = "eigen",
    sha256 = "b4c198460eba6f28d34894e3a5710998818515104d6e74e5cc331ce31e46e626",
    strip_prefix = "eigen-3.4.0",
    urls = ["https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.bz2"],
    build_file_content =
"""
cc_library(
    name = "eigen",
    hdrs = glob(["Eigen/**/*"]),
    srcs = [],
    includes = ["."], 
    visibility = ["//visibility:public"],
)
"""
)

# mdspan // TODO: Remove when C++23 mdspan supports.
git_repository(
    name = "mdspan",
    remote = "https://github.com/kokkos/mdspan",
    tag = "mdspan-0.6.0",
    build_file_content =
"""
cc_library(
    name = "mdspan",
    hdrs = glob(["include/**/*"]),
    srcs = [],
    includes = ["include/"],
    defines = ["MDSPAN_USE_BRACKET_OPERATOR=0"], # NOTE: For C++20 compatibility
    visibility = ["//visibility:public"],
)
"""
)
