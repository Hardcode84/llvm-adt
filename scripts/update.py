# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from argparse import ArgumentParser
from pathlib import Path
import shutil
import filecmp

include_files = [
    "llvm-c/DataTypes.h",
    "llvm-c/Error.h",
    "llvm-c/ErrorHandling.h",
    "llvm-c/ExternC.h",
    "llvm-c/Support.h",
    "llvm-c/Types.h",
    "llvm/ADT/ADL.h",
    "llvm/ADT/APFloat.h",
    "llvm/ADT/APInt.h",
    "llvm/ADT/APSInt.h",
    "llvm/ADT/AllocatorList.h",
    "llvm/ADT/ArrayRef.h",
    "llvm/ADT/BitVector.h",
    "llvm/ADT/BitmaskEnum.h",
    "llvm/ADT/DenseMap.h",
    "llvm/ADT/DenseMapInfo.h",
    "llvm/ADT/DenseMapInfoVariant.h",
    "llvm/ADT/DenseSet.h",
    "llvm/ADT/EpochTracker.h",
    "llvm/ADT/FloatingPointMode.h",
    "llvm/ADT/FoldingSet.h",
    "llvm/ADT/GraphTraits.h",
    "llvm/ADT/Hashing.h",
    "llvm/ADT/IntrusiveRefCntPtr.h",
    "llvm/ADT/PointerIntPair.h",
    "llvm/ADT/STLExtras.h",
    "llvm/ADT/STLForwardCompat.h",
    "llvm/ADT/STLFunctionalExtras.h",
    "llvm/ADT/ScopeExit.h",
    "llvm/ADT/SmallPtrSet.h",
    "llvm/ADT/SmallString.h",
    "llvm/ADT/SmallVector.h",
    "llvm/ADT/Statistic.h",
    "llvm/ADT/StringExtras.h",
    "llvm/ADT/StringMap.h",
    "llvm/ADT/StringMapEntry.h",
    "llvm/ADT/StringRef.h",
    "llvm/ADT/StringSet.h",
    "llvm/ADT/StringSwitch.h",
    "llvm/ADT/Twine.h",
    "llvm/ADT/UniqueVector.h",
    "llvm/ADT/bit.h",
    "llvm/ADT/edit_distance.h",
    "llvm/ADT/ilist_base.h",
    "llvm/ADT/ilist_iterator.h",
    "llvm/ADT/ilist_node.h",
    "llvm/ADT/ilist_node_base.h",
    "llvm/ADT/ilist_node_options.h",
    "llvm/ADT/iterator.h",
    "llvm/ADT/iterator_range.h",
    "llvm/ADT/simple_ilist.h",
    "llvm/Demangle/Demangle.h",
    "llvm/Support/AlignOf.h",
    "llvm/Support/Alignment.h",
    "llvm/Support/Allocator.h",
    "llvm/Support/AllocatorBase.h",
    "llvm/Support/AutoConvert.h",
    "llvm/Support/CBindingWrapping.h",
    "llvm/Support/Casting.h",
    "llvm/Support/Chrono.h",
    "llvm/Support/CommandLine.h",
    "llvm/Support/Compiler.h",
    "llvm/Support/ConvertUTF.h",
    "llvm/Support/CrashRecoveryContext.h",
    "llvm/Support/DOTGraphTraits.h",
    "llvm/Support/DataTypes.h",
    "llvm/Support/Debug.h",
    "llvm/Support/DebugCounter.h",
    "llvm/Support/Duration.h",
    "llvm/Support/Endian.h",
    "llvm/Support/Errc.h",
    "llvm/Support/Errno.h",
    "llvm/Support/Error.h",
    "llvm/Support/ErrorHandling.h",
    "llvm/Support/ErrorOr.h",
    "llvm/Support/ExitCodes.h",
    "llvm/Support/ExtensibleRTTI.h",
    "llvm/Support/FileSystem.h",
    "llvm/Support/FileSystem/UniqueID.h",
    "llvm/Support/FileUtilities.h",
    "llvm/Support/Format.h",
    "llvm/Support/FormatCommon.h",
    "llvm/Support/FormatProviders.h",
    "llvm/Support/FormatVariadic.h",
    "llvm/Support/FormatVariadicDetails.h",
    "llvm/Support/GraphWriter.h",
    "llvm/Support/Locale.h",
    "llvm/Support/MD5.h",
    "llvm/Support/ManagedStatic.h",
    "llvm/Support/MathExtras.h",
    "llvm/Support/MemAlloc.h",
    "llvm/Support/MemoryBuffer.h",
    "llvm/Support/MemoryBufferRef.h",
    "llvm/Support/Mutex.h",
    "llvm/Support/NativeFormatting.h",
    "llvm/Support/Path.h",
    "llvm/Support/PointerLikeTypeTraits.h",
    "llvm/Support/Process.h",
    "llvm/Support/Program.h",
    "llvm/Support/RandomNumberGenerator.h",
    "llvm/Support/ReverseIteration.h",
    "llvm/Support/SMLoc.h",
    "llvm/Support/SaveAndRestore.h",
    "llvm/Support/Signals.h",
    "llvm/Support/Signposts.h",
    "llvm/Support/SmallVectorMemoryBuffer.h",
    "llvm/Support/SourceMgr.h",
    "llvm/Support/StringSaver.h",
    "llvm/Support/SwapByteOrder.h",
    "llvm/Support/SystemZ/zOSSupport.h",
    "llvm/Support/Threading.h",
    "llvm/Support/Timer.h",
    "llvm/Support/TypeSize.h",
    "llvm/Support/Unicode.h",
    "llvm/Support/UnicodeCharRanges.h",
    "llvm/Support/VirtualFileSystem.h",
    "llvm/Support/WindowsError.h",
    "llvm/Support/WithColor.h",
    "llvm/Support/YAMLParser.h",
    "llvm/Support/YAMLTraits.h",
    "llvm/Support/circular_raw_ostream.h",
    "llvm/Support/float128.h",
    "llvm/Support/raw_os_ostream.h",
    "llvm/Support/raw_ostream.h",
    "llvm/Support/thread.h",
    "llvm/Support/type_traits.h",
    "llvm/Support/xxhash.h",
]

src_files = [
    "Support/APFloat.cpp",
    "Support/APInt.cpp",
    "Support/CommandLine.cpp",
    "Support/ConvertUTF.cpp",
    "Support/ConvertUTFWrapper.cpp",
    "Support/CrashRecoveryContext.cpp",
    "Support/Debug.cpp",
    "Support/DebugCounter.cpp",
    "Support/DebugOptions.h",
    "Support/Errno.cpp",
    "Support/Error.cpp",
    "Support/ErrorHandling.cpp",
    "Support/ExtensibleRTTI.cpp",
    "Support/FormatVariadic.cpp",
    "Support/GraphWriter.cpp",
    "Support/Locale.cpp",
    "Support/MD5.cpp",
    "Support/ManagedStatic.cpp",
    "Support/MemAlloc.cpp",
    "Support/MemoryBuffer.cpp",
    "Support/NativeFormatting.cpp",
    "Support/Path.cpp",
    "Support/Process.cpp",
    "Support/Program.cpp",
    "Support/RandomNumberGenerator.cpp",
    "Support/Signals.cpp",
    "Support/Signposts.cpp",
    "Support/SmallPtrSet.cpp",
    "Support/SmallVector.cpp",
    "Support/SourceMgr.cpp",
    "Support/Statistic.cpp",
    "Support/StringExtras.cpp",
    "Support/StringMap.cpp",
    "Support/StringRef.cpp",
    "Support/StringSaver.cpp",
    "Support/Threading.cpp",
    "Support/Timer.cpp",
    "Support/Twine.cpp",
    "Support/TypeSize.cpp",
    "Support/Unicode.cpp",
    "Support/Unix/Path.inc",
    "Support/Unix/Process.inc",
    "Support/Unix/Program.inc",
    "Support/Unix/Signals.inc",
    "Support/Unix/Threading.inc",
    "Support/Unix/Unix.h",
    "Support/VirtualFileSystem.cpp",
    "Support/Windows/Path.inc",
    "Support/Windows/Process.inc",
    "Support/Windows/Program.inc",
    "Support/Windows/Signals.inc",
    "Support/Windows/Threading.inc",
    "Support/WithColor.cpp",
    "Support/YAMLParser.cpp",
    "Support/circular_raw_ostream.cpp",
    "Support/raw_os_ostream.cpp",
    "Support/raw_ostream.cpp",
    "Support/xxhash.cpp",
]

test_files = [
    "ADT/CountCopyAndMove.cpp",
    "ADT/CountCopyAndMove.h",
    "ADT/DenseMapTest.cpp",
    "ADT/STLExtrasTest.cpp",
    "ADT/SmallVectorTest.cpp",
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
            file = Path(file)
            dst_path = dst / file.parent
            src_file = src / file
            dst_file = dst / file
            if dst_file.exists() and filecmp.cmp(src_file, dst_file):
                print(f'"{dst_file}" is up to date')
                continue

            print(f'Copying "{file}" to "{dst}"')
            dst_path.mkdir(parents=True, exist_ok=True)
            shutil.copy(src=src_file, dst=dst_path)

    # Amalgamate lib sources
    print("Amalgamating lib sources")
    with open(src_dir / "Support.cpp", "w") as f:
        for file in src_files:
            if file.endswith(".inc"):
                continue

            f.write(f'#include "{file}"\n')
            f.write("#undef DEBUG_TYPE\n")

    print("Done")
