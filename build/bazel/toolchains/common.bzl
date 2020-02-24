TEST_CPP_FILE = "empty.cpp"

_INC_DIR_MARKER_BEGIN = "#include <...>"
_INC_DIR_MARKER_END = "End of search list"

# OSX add " (framework directory)" at the end of line, strip it.
_MAC_FRAMEWORK_SUFFIX = " (framework directory)"
_MAC_FRAMEWORK_SUFFIX_LEN = len(_MAC_FRAMEWORK_SUFFIX)

def _prepare_include_path(repo_ctx, path):
    """Resolve and sanitize include path before outputting it into the crosstool.
    Args:
      repo_ctx: repository_ctx object.
      path: an include path to be sanitized.
    Returns:
      Sanitized include path that can be written to the crosstoot. Resulting path
      is absolute if it is outside the repository and relative otherwise.
    """
    path = path.strip()
    if path.endswith(_MAC_FRAMEWORK_SUFFIX):
        path = path[:-_MAC_FRAMEWORK_SUFFIX_LEN].strip()

    # We're on UNIX, so the path delimiter is '/'.
    repo_root = str(repo_ctx.path(".")) + "/"
    path = str(repo_ctx.path(path))
    if path.startswith(repo_root):
        return path[len(repo_root):]
    return path


def get_cxx_inc_directories(repo_ctx, cc, lang_flag, additional_flags = []):
    """Compute the list of default C++ include directories."""
    result = repo_ctx.execute([cc, "-E", lang_flag, "-", "-v"] + additional_flags)
    index0 = result.stderr.rfind(_INC_DIR_MARKER_BEGIN)
    index1 = result.stderr.find("\n", index0)
    index2 = result.stderr.rfind(_INC_DIR_MARKER_END)
    inc_dirs = result.stderr[index1:index2].strip()
    return [ _prepare_include_path(repo_ctx, p) for p in inc_dirs.split("\n") ]


def is_compiler_option_supported(repo_ctx, cc, option):
    """Checks that `option` is supported by the C compiler."""
    result = repo_ctx.execute([
        cc,
        option,
        "-o",
        "/dev/null",
        "-c",
        str(repo_ctx.path(TEST_CPP_FILE)),
    ])
    return result.stderr.find(option) == -1


def is_linker_option_supported(repo_ctx, cc, option, pattern):
    """Checks that `option` is supported by the C linker."""
    result = repo_ctx.execute([
        cc,
        option,
        "-o",
        "/dev/null",
        str(repo_ctx.path(TEST_CPP_FILE)),
    ])
    return result.stderr.find(pattern) == -1


def add_compiler_option_if_supported(repo_ctx, cc, option):
    """Returns `[option]` if supported, `[]` otherwise."""
    return [option] if is_compiler_option_supported(repo_ctx, cc, option) else []


def add_linker_option_if_supported(repo_ctx, cc, option, pattern):
    """Returns `[option]` if supported, `[]` otherwise."""
    return [option] if is_linker_option_supported(repo_ctx, cc, option, pattern) else []


def get_no_canonical_prefixes_opt(repo_ctx, cc):
    # If the compiler sometimes rewrites paths in the .d files without symlinks
    # (ie when they're shorter), it confuses Bazel's logic for verifying all
    # #included header files are listed as inputs to the action.

    # The '-fno-canonical-system-headers' should be enough, but clang does not
    # support it, so we also try '-no-canonical-prefixes' if first option does
    # not work.
    opt = add_compiler_option_if_supported(
        repo_ctx,
        cc,
        "-fno-canonical-system-headers",
    )
    if len(opt) == 0:
        return add_compiler_option_if_supported(
            repo_ctx,
            cc,
            "-no-canonical-prefixes",
        )
    return opt


def get_toolchain_identifier(reqs):
    return "{}-{}-{}-{}".format(reqs.os_id, reqs.target_arch_id,
                                reqs.compiler_id, reqs.compiler_version)
