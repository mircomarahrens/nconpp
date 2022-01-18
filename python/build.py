"""Build script."""

import shutil
from distutils import log as distutils_log
from pathlib import Path
from typing import Any, Dict

import skbuild
import skbuild.constants

__all__ = ("build",)


def build(setup_kwargs: Dict[str, Any]) -> None:
    """Build C-extensions."""
    skbuild.setup(**setup_kwargs, script_args=["build_ext"])

    src_dir = Path(skbuild.constants.CMAKE_INSTALL_DIR()) / "hello"
    dest_dir = Path("hello")

    # Delete C-extensions copied in previous runs, just in case.
    remove_files(dest_dir, "**/*.so")

    # Copy built C-extensions back to the project.
    copy_files(src_dir, dest_dir, "**/*.so")


def remove_files(target_dir: Path, pattern: str) -> None:
    """Delete files matched with a glob pattern in a directory tree."""
    for path in target_dir.glob(pattern):
        if path.is_dir():
            shutil.rmtree(path)
        else:
            path.unlink()
        distutils_log.info(f"removed {path}")  # type: ignore[call-arg]
        # will be fixed in mypy 0.800, https://github.com/python/typeshed/pull/4573


def copy_files(src_dir: Path, dest_dir: Path, pattern: str) -> None:
    """Copy files matched with a glob pattern in a directory tree to another."""
    for src in src_dir.glob(pattern):
        dest = dest_dir / src.relative_to(src_dir)
        if src.is_dir():
            # NOTE: inefficient if subdirectories also match to the pattern.
            copy_files(src, dest, "*")
        else:
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src, dest)
            distutils_log.info(f"copied {src} to {dest}")  # type: ignore[call-arg]
            # will be fixed in mypy 0.800, https://github.com/python/typeshed/pull/4573


if __name__ == "__main__":

    # general
    __name__ = "nconpp"
    __version__ = "0.1"

    # cmake specific
    __cmake_args__= []
    # For our 3rd party library handling we used the C++ package manager vcpkg 
    # (see https://github.com/microsoft/vcpkg for details). Vcpkg offers a cmake
    # toolchain file which we have to reveal to the cmake installer.
    __toolchain__ = "/home/mircomarahrens/vcpkg/scripts/buildsystems/vcpkg.cmake"

    __cmake_args__.append("-DCMAKE_TOOLCHAIN_FILE={}".format(__toolchain__))
    __cmake_args__.append("-DVERSION_INFO={}".format(__version__))

    __cmake_source_dir__ = Path.cwd().parents[0]

    build({
        "name": __name__,
        "cmake_args": __cmake_args__,
        "cmake_source_dir": __cmake_source_dir__
        })
