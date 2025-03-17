# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from argparse import ArgumentParser
from pathlib import Path
import shutil

include_files = [
    "llvm-c/DataTypes.h",
    "llvm-c/Error.h",
    "llvm-c/ExternC.h",
    "llvm/ADT/ADL.h",
    "llvm/ADT/APInt.h",
    "llvm/ADT/APSInt.h",
    "llvm/ADT/ArrayRef.h",
    "llvm/ADT/DenseMap.h",
    "llvm/ADT/DenseMapInfo.h",
    "llvm/ADT/EpochTracker.h",
    "llvm/ADT/Hashing.h",
    "llvm/ADT/STLExtras.h",
    "llvm/ADT/STLForwardCompat.h",
    "llvm/ADT/STLFunctionalExtras.h",
    "llvm/ADT/SmallString.h",
    "llvm/ADT/SmallVector.h",
    "llvm/ADT/StringExtras.h",
    "llvm/ADT/StringRef.h",
    "llvm/ADT/StringSwitch.h",
    "llvm/ADT/Twine.h",
    "llvm/ADT/bit.h",
    "llvm/ADT/iterator.h",
    "llvm/ADT/iterator_range.h",
    "llvm/Support/AlignOf.h",
    "llvm/Support/AutoConvert.h",
    "llvm/Support/Chrono.h",
    "llvm/Support/Compiler.h",
    "llvm/Support/DataTypes.h",
    "llvm/Support/Debug.h",
    "llvm/Support/Duration.h",
    "llvm/Support/Endian.h",
    "llvm/Support/Error.h",
    "llvm/Support/ErrorHandling.h",
    "llvm/Support/ErrorOr.h",
    "llvm/Support/FileSystem.h",
    "llvm/Support/FileSystem/UniqueID.h",
    "llvm/Support/Format.h",
    "llvm/Support/FormatCommon.h",
    "llvm/Support/FormatProviders.h",
    "llvm/Support/FormatVariadic.h",
    "llvm/Support/FormatVariadicDetails.h",
    "llvm/Support/MD5.h",
    "llvm/Support/MathExtras.h",
    "llvm/Support/MemAlloc.h",
    "llvm/Support/NativeFormatting.h",
    "llvm/Support/PointerLikeTypeTraits.h",
    "llvm/Support/Process.h",
    "llvm/Support/Program.h",
    "llvm/Support/ReverseIteration.h",
    "llvm/Support/SwapByteOrder.h",
    "llvm/Support/float128.h",
    "llvm/Support/raw_os_ostream.h",
    "llvm/Support/raw_ostream.h",
    "llvm/Support/type_traits.h",
]

src_files = [
    "Support/raw_os_ostream.cpp",
    "Support/raw_ostream.cpp",
]

test_files = [
    "ADT/STLExtrasTest.cpp",
]

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("llvm_root")
    parser.add_argument("dst_dir")

    args = parser.parse_args()
    llvm_include_dir = Path(args.llvm_root) / "llvm" / "include"
    llvm_src_dir = Path(args.llvm_root) / "llvm" / "lib"
    llvm_test_dir = Path(args.llvm_root) / "llvm" / "unittests"

    dst_dir = Path(args.dst_dir)
    include_dir = dst_dir / "include"
    src_dir = dst_dir / "lib"
    test_dir = dst_dir / "tests"

    dirs = [
        (include_dir, llvm_include_dir, include_files),
        (src_dir, llvm_src_dir, src_files),
        (test_dir, llvm_test_dir, test_files),
    ]

    for dst, src, files in dirs:
        dst.mkdir(parents=True, exist_ok=True)
        for file in files:
            print(f'Copying "{file}" to "{dst}"')
            file = Path(file)
            dst_path = dst / file.parent
            dst_path.mkdir(parents=True, exist_ok=True)
            shutil.copy(src=src / file, dst=dst_path)
