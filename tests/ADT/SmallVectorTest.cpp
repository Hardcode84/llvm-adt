//===- llvm/unittest/ADT/SmallVectorTest.cpp ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// SmallVector unit tests.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Compiler.h"
#include "gtest/gtest.h"
#include <list>
#include <stdarg.h>

using namespace llvm;

namespace {

/// A helper class that counts the total number of constructor and
/// destructor calls.
class Constructable {
private:
  static int numConstructorCalls;
  static int numMoveConstructorCalls;
  static int numCopyConstructorCalls;
  static int numDestructorCalls;
  static int numAssignmentCalls;
  static int numMoveAssignmentCalls;
  static int numCopyAssignmentCalls;

  bool constructed;
  int value;

public:
  Constructable() : constructed(true), value(0) {
    ++numConstructorCalls;
  }

  Constructable(int val) : constructed(true), value(val) {
    ++numConstructorCalls;
  }

  Constructable(const Constructable & src) : constructed(true) {
    value = src.value;
    ++numConstructorCalls;
    ++numCopyConstructorCalls;
  }

  Constructable(Constructable && src) : constructed(true) {
    value = src.value;
    src.value = 0;
    ++numConstructorCalls;
    ++numMoveConstructorCalls;
  }

  ~Constructable() {
    EXPECT_TRUE(constructed);
    ++numDestructorCalls;
    constructed = false;
  }

  Constructable & operator=(const Constructable & src) {
    EXPECT_TRUE(constructed);
    value = src.value;
    ++numAssignmentCalls;
    ++numCopyAssignmentCalls;
    return *this;
  }

  Constructable & operator=(Constructable && src) {
    EXPECT_TRUE(constructed);
    value = src.value;
    src.value = 0;
    ++numAssignmentCalls;
    ++numMoveAssignmentCalls;
    return *this;
  }

  int getValue() const {
    return abs(value);
  }

  static void reset() {
    numConstructorCalls = 0;
    numMoveConstructorCalls = 0;
    numCopyConstructorCalls = 0;
    numDestructorCalls = 0;
    numAssignmentCalls = 0;
    numMoveAssignmentCalls = 0;
    numCopyAssignmentCalls = 0;
  }

  static int getNumConstructorCalls() {
    return numConstructorCalls;
  }

  static int getNumMoveConstructorCalls() {
    return numMoveConstructorCalls;
  }

  static int getNumCopyConstructorCalls() {
    return numCopyConstructorCalls;
  }

  static int getNumDestructorCalls() {
    return numDestructorCalls;
  }

  static int getNumAssignmentCalls() {
    return numAssignmentCalls;
  }

  static int getNumMoveAssignmentCalls() {
    return numMoveAssignmentCalls;
  }

  static int getNumCopyAssignmentCalls() {
    return numCopyAssignmentCalls;
  }

  friend bool operator==(const Constructable &c0, const Constructable &c1) {
    return c0.getValue() == c1.getValue();
  }

  friend bool LLVM_ATTRIBUTE_UNUSED operator!=(const Constructable &c0,
                                               const Constructable &c1) {
    return c0.getValue() != c1.getValue();
  }

  friend bool operator<(const Constructable &c0, const Constructable &c1) {
    return c0.getValue() < c1.getValue();
  }
  friend bool LLVM_ATTRIBUTE_UNUSED operator<=(const Constructable &c0,
                                               const Constructable &c1) {
    return c0.getValue() <= c1.getValue();
  }
  friend bool LLVM_ATTRIBUTE_UNUSED operator>(const Constructable &c0,
                                              const Constructable &c1) {
    return c0.getValue() > c1.getValue();
  }
  friend bool LLVM_ATTRIBUTE_UNUSED operator>=(const Constructable &c0,
                                               const Constructable &c1) {
    return c0.getValue() >= c1.getValue();
  }
};

int Constructable::numConstructorCalls;
int Constructable::numCopyConstructorCalls;
int Constructable::numMoveConstructorCalls;
int Constructable::numDestructorCalls;
int Constructable::numAssignmentCalls;
int Constructable::numCopyAssignmentCalls;
int Constructable::numMoveAssignmentCalls;

struct NonCopyable {
  NonCopyable() {}
  NonCopyable(NonCopyable &&) {}
  NonCopyable &operator=(NonCopyable &&) { return *this; }
private:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
};

LLVM_ATTRIBUTE_USED void CompileTest() {
  SmallVector<NonCopyable, 0> V;
  V.resize(42);
}

TEST(SmallVectorTest, ConstructNonCopyableTest) {
  SmallVector<NonCopyable, 0> V(42);
  EXPECT_EQ(V.size(), (size_t)42);
}

// Assert that v contains the specified values, in order.
template <typename VectorT>
void assertValuesInOrder(VectorT &v, size_t size, ...) {
  EXPECT_EQ(size, v.size());

  va_list ap;
  va_start(ap, size);
  for (size_t i = 0; i < size; ++i) {
    int value = va_arg(ap, int);
    EXPECT_EQ(value, v[i].getValue());
  }

  va_end(ap);
}

template <typename VectorT> void assertEmpty(VectorT &v) {
  // Size tests
  EXPECT_EQ(0u, v.size());
  EXPECT_TRUE(v.empty());

  // Iterator tests
  EXPECT_TRUE(v.begin() == v.end());
}

// Generate a sequence of values to initialize the vector.
template <typename VectorT> void makeSequence(VectorT &v, int start, int end) {
  for (int i = start; i <= end; ++i) {
    v.push_back(Constructable(i));
  }
}

template <typename T, unsigned N>
constexpr static unsigned NumBuiltinElts(const SmallVector<T, N> &) {
  return N;
}

class SmallVectorTestBase : public testing::Test {
protected:
  void SetUp() override { Constructable::reset(); }
};

// Test fixture class
template <typename VectorT>
class SmallVectorTest : public SmallVectorTestBase {
protected:
  VectorT theVector;
  VectorT otherVector;
};


typedef ::testing::Types<SmallVector<Constructable, 0>,
                         SmallVector<Constructable, 1>,
                         SmallVector<Constructable, 2>,
                         SmallVector<Constructable, 4>,
                         SmallVector<Constructable, 5>
                         > SmallVectorTestTypes;
TYPED_TEST_SUITE(SmallVectorTest, SmallVectorTestTypes, );

// Constructor test.
TYPED_TEST(SmallVectorTest, ConstructorNonIterTest) {
  SCOPED_TRACE("ConstructorTest");
  auto &V = this->theVector;
  V = SmallVector<Constructable, 2>(2, 2);
  assertValuesInOrder(V, 2u, 2, 2);
}

// Constructor test.
TYPED_TEST(SmallVectorTest, ConstructorIterTest) {
  SCOPED_TRACE("ConstructorTest");
  int arr[] = {1, 2, 3};
  auto &V = this->theVector;
  V = SmallVector<Constructable, 4>(std::begin(arr), std::end(arr));
  assertValuesInOrder(V, 3u, 1, 2, 3);
}

// Constructor test.
TYPED_TEST(SmallVectorTest, ConstructorFromArrayRefSimpleTest) {
  SCOPED_TRACE("ConstructorFromArrayRefSimpleTest");
  std::array<Constructable, 3> StdArray = {Constructable(1), Constructable(2),
                                           Constructable(3)};
  ArrayRef<Constructable> Array = StdArray;
  auto &V = this->theVector;
  V = SmallVector<Constructable, 4>(Array);
  assertValuesInOrder(V, 3u, 1, 2, 3);
  ASSERT_EQ(NumBuiltinElts(TypeParam{}), NumBuiltinElts(V));
}

// New vector test.
TYPED_TEST(SmallVectorTest, EmptyVectorTest) {
  SCOPED_TRACE("EmptyVectorTest");
  auto &V = this->theVector;
  assertEmpty(V);
  EXPECT_TRUE(V.rbegin() == V.rend());
  EXPECT_EQ(0, Constructable::getNumConstructorCalls());
  EXPECT_EQ(0, Constructable::getNumDestructorCalls());
}

// Simple insertions and deletions.
TYPED_TEST(SmallVectorTest, PushPopTest) {
  SCOPED_TRACE("PushPopTest");
  auto &V = this->theVector;
  // Track whether the vector will potentially have to grow.
  bool RequiresGrowth = V.capacity() < 3;

  // Push an element
  V.push_back(Constructable(1));

  // Size tests
  assertValuesInOrder(V, 1u, 1);
  EXPECT_FALSE(V.begin() == V.end());
  EXPECT_FALSE(V.empty());

  // Push another element
  V.push_back(Constructable(2));
  assertValuesInOrder(V, 2u, 1, 2);

  // Insert at beginning. Reserve space to avoid reference invalidation from
  // V[1].
  V.reserve(V.size() + 1);
  V.insert(V.begin(), V[1]);
  assertValuesInOrder(V, 3u, 2, 1, 2);

  // Pop one element
  V.pop_back();
  assertValuesInOrder(V, 2u, 2, 1);

  // Pop remaining elements
  V.pop_back_n(2);
  assertEmpty(V);

  // Check number of constructor calls. Should be 2 for each list element,
  // one for the argument to push_back, one for the argument to insert,
  // and one for the list element itself.
  if (!RequiresGrowth) {
    EXPECT_EQ(5, Constructable::getNumConstructorCalls());
    EXPECT_EQ(5, Constructable::getNumDestructorCalls());
  } else {
    // If we had to grow the vector, these only have a lower bound, but should
    // always be equal.
    EXPECT_LE(5, Constructable::getNumConstructorCalls());
    EXPECT_EQ(Constructable::getNumConstructorCalls(),
              Constructable::getNumDestructorCalls());
  }
}

// Clear test.
TYPED_TEST(SmallVectorTest, ClearTest) {
  SCOPED_TRACE("ClearTest");
  auto &V = this->theVector;
  V.reserve(2);
  makeSequence(V, 1, 2);
  V.clear();

  assertEmpty(V);
  EXPECT_EQ(4, Constructable::getNumConstructorCalls());
  EXPECT_EQ(4, Constructable::getNumDestructorCalls());
}

// Resize smaller test.
TYPED_TEST(SmallVectorTest, ResizeShrinkTest) {
  SCOPED_TRACE("ResizeShrinkTest");
  auto &V = this->theVector;
  V.reserve(3);
  makeSequence(V, 1, 3);
  V.resize(1);

  assertValuesInOrder(V, 1u, 1);
  EXPECT_EQ(6, Constructable::getNumConstructorCalls());
  EXPECT_EQ(5, Constructable::getNumDestructorCalls());
}

// Truncate test.
TYPED_TEST(SmallVectorTest, TruncateTest) {
  SCOPED_TRACE("TruncateTest");
  auto &V = this->theVector;
  V.reserve(3);
  makeSequence(V, 1, 3);
  V.truncate(1);

  assertValuesInOrder(V, 1u, 1);
  EXPECT_EQ(6, Constructable::getNumConstructorCalls());
  EXPECT_EQ(5, Constructable::getNumDestructorCalls());

#if !defined(NDEBUG) && GTEST_HAS_DEATH_TEST
  EXPECT_DEATH(V.truncate(2), "Cannot increase size");
#endif
  V.truncate(1);
  assertValuesInOrder(V, 1u, 1);
  EXPECT_EQ(6, Constructable::getNumConstructorCalls());
  EXPECT_EQ(5, Constructable::getNumDestructorCalls());

  V.truncate(0);
  assertEmpty(V);
  EXPECT_EQ(6, Constructable::getNumConstructorCalls());
  EXPECT_EQ(6, Constructable::getNumDestructorCalls());
}

// Resize bigger test.
TYPED_TEST(SmallVectorTest, ResizeGrowTest) {
  SCOPED_TRACE("ResizeGrowTest");
  auto &V = this->theVector;
  V.resize(2);

  EXPECT_EQ(2, Constructable::getNumConstructorCalls());
  EXPECT_EQ(0, Constructable::getNumDestructorCalls());
  EXPECT_EQ(2u, V.size());
}

TYPED_TEST(SmallVectorTest, ResizeWithElementsTest) {
  auto &V = this->theVector;
  V.resize(2);

  Constructable::reset();

  V.resize(4);

  size_t Ctors = Constructable::getNumConstructorCalls();
  EXPECT_TRUE(Ctors == 2 || Ctors == 4);
  size_t MoveCtors = Constructable::getNumMoveConstructorCalls();
  EXPECT_TRUE(MoveCtors == 0 || MoveCtors == 2);
  size_t Dtors = Constructable::getNumDestructorCalls();
  EXPECT_TRUE(Dtors == 0 || Dtors == 2);
}

// Resize with fill value.
TYPED_TEST(SmallVectorTest, ResizeFillTest) {
  SCOPED_TRACE("ResizeFillTest");
  auto &V = this->theVector;
  V.resize(3, Constructable(77));
  assertValuesInOrder(V, 3u, 77, 77, 77);
}

TEST(SmallVectorTest, ResizeForOverwrite) {
  {
    // Heap allocated storage.
    SmallVector<unsigned, 0> V;
    V.push_back(5U);
    V.pop_back();
    V.resize_for_overwrite(V.size() + 1U);
    EXPECT_EQ(5U, V.back());
    V.pop_back();
    V.resize(V.size() + 1);
    EXPECT_EQ(0U, V.back());
  }
  {
    // Inline storage.
    SmallVector<unsigned, 2> V;
    V.push_back(5U);
    V.pop_back();
    V.resize_for_overwrite(V.size() + 1U);
    EXPECT_EQ(5U, V.back());
    V.pop_back();
    V.resize(V.size() + 1);
    EXPECT_EQ(0U, V.back());
  }
}

// Overflow past fixed size.
TYPED_TEST(SmallVectorTest, OverflowTest) {
  SCOPED_TRACE("OverflowTest");
  auto &V = this->theVector;
  // Push more elements than the fixed size.
  makeSequence(V, 1, 10);

  // Test size and values.
  EXPECT_EQ(10u, V.size());
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(i + 1, V[i].getValue());
  }

  // Now resize back to fixed size.
  V.resize(1);

  assertValuesInOrder(V, 1u, 1);
}

// Iteration tests.
TYPED_TEST(SmallVectorTest, IterationTest) {
  auto &V = this->theVector;
  makeSequence(V, 1, 2);

  // Forward Iteration
  typename TypeParam::iterator it = V.begin();
  EXPECT_TRUE(*it == V.front());
  EXPECT_TRUE(*it == V[0]);
  EXPECT_EQ(1, it->getValue());
  ++it;
  EXPECT_TRUE(*it == V[1]);
  EXPECT_TRUE(*it == V.back());
  EXPECT_EQ(2, it->getValue());
  ++it;
  EXPECT_TRUE(it == V.end());
  --it;
  EXPECT_TRUE(*it == V[1]);
  EXPECT_EQ(2, it->getValue());
  --it;
  EXPECT_TRUE(*it == V[0]);
  EXPECT_EQ(1, it->getValue());

  // Reverse Iteration
  typename TypeParam::reverse_iterator rit = V.rbegin();
  EXPECT_TRUE(*rit == V[1]);
  EXPECT_EQ(2, rit->getValue());
  ++rit;
  EXPECT_TRUE(*rit == V[0]);
  EXPECT_EQ(1, rit->getValue());
  ++rit;
  EXPECT_TRUE(rit == V.rend());
  --rit;
  EXPECT_TRUE(*rit == V[0]);
  EXPECT_EQ(1, rit->getValue());
  --rit;
  EXPECT_TRUE(*rit == V[1]);
  EXPECT_EQ(2, rit->getValue());
}

// Swap test.
TYPED_TEST(SmallVectorTest, SwapTest) {
  SCOPED_TRACE("SwapTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  makeSequence(V, 1, 2);
  std::swap(V, U);

  assertEmpty(V);
  assertValuesInOrder(U, 2u, 1, 2);
}

// Append test
TYPED_TEST(SmallVectorTest, AppendTest) {
  SCOPED_TRACE("AppendTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  makeSequence(U, 2, 3);

  V.push_back(Constructable(1));
  V.append(U.begin(), U.end());

  assertValuesInOrder(V, 3u, 1, 2, 3);
}

// Append repeated test
TYPED_TEST(SmallVectorTest, AppendRepeatedTest) {
  SCOPED_TRACE("AppendRepeatedTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.append(2, Constructable(77));
  assertValuesInOrder(V, 3u, 1, 77, 77);
}

// Append test
TYPED_TEST(SmallVectorTest, AppendNonIterTest) {
  SCOPED_TRACE("AppendRepeatedTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.append(2, 7);
  assertValuesInOrder(V, 3u, 1, 7, 7);
}

struct output_iterator {
  typedef std::output_iterator_tag iterator_category;
  typedef int value_type;
  typedef int difference_type;
  typedef value_type *pointer;
  typedef value_type &reference;
  operator int() { return 2; }
  operator Constructable() { return 7; }
};

TYPED_TEST(SmallVectorTest, AppendRepeatedNonForwardIterator) {
  SCOPED_TRACE("AppendRepeatedTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.append(output_iterator(), output_iterator());
  assertValuesInOrder(V, 3u, 1, 7, 7);
}

TYPED_TEST(SmallVectorTest, AppendSmallVector) {
  SCOPED_TRACE("AppendSmallVector");
  auto &V = this->theVector;
  SmallVector<Constructable, 3> otherVector = {7, 7};
  V.push_back(Constructable(1));
  V.append(otherVector);
  assertValuesInOrder(V, 3u, 1, 7, 7);
}

// Assign test
TYPED_TEST(SmallVectorTest, AssignTest) {
  SCOPED_TRACE("AssignTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.assign(2, Constructable(77));
  assertValuesInOrder(V, 2u, 77, 77);
}

// Assign test
TYPED_TEST(SmallVectorTest, AssignRangeTest) {
  SCOPED_TRACE("AssignTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  int arr[] = {1, 2, 3};
  V.assign(std::begin(arr), std::end(arr));
  assertValuesInOrder(V, 3u, 1, 2, 3);
}

// Assign test
TYPED_TEST(SmallVectorTest, AssignNonIterTest) {
  SCOPED_TRACE("AssignTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.assign(2, 7);
  assertValuesInOrder(V, 2u, 7, 7);
}

TYPED_TEST(SmallVectorTest, AssignSmallVector) {
  SCOPED_TRACE("AssignSmallVector");
  auto &V = this->theVector;
  SmallVector<Constructable, 3> otherVector = {7, 7};
  V.push_back(Constructable(1));
  V.assign(otherVector);
  assertValuesInOrder(V, 2u, 7, 7);
}

// Move-assign test
TYPED_TEST(SmallVectorTest, MoveAssignTest) {
  SCOPED_TRACE("MoveAssignTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  // Set up our vector with a single element, but enough capacity for 4.
  V.reserve(4);
  V.push_back(Constructable(1));

  // Set up the other vector with 2 elements.
  U.push_back(Constructable(2));
  U.push_back(Constructable(3));

  // Move-assign from the other vector.
  V = std::move(U);

  // Make sure we have the right result.
  assertValuesInOrder(V, 2u, 2, 3);

  // Make sure the # of constructor/destructor calls line up. There
  // are two live objects after clearing the other vector.
  U.clear();
  EXPECT_EQ(Constructable::getNumConstructorCalls()-2,
            Constructable::getNumDestructorCalls());

  // There shouldn't be any live objects any more.
  V.clear();
  EXPECT_EQ(Constructable::getNumConstructorCalls(),
            Constructable::getNumDestructorCalls());
}

// Erase a single element
TYPED_TEST(SmallVectorTest, EraseTest) {
  SCOPED_TRACE("EraseTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);
  const auto &theConstVector = V;
  V.erase(theConstVector.begin());
  assertValuesInOrder(V, 2u, 2, 3);
}

// Erase a range of elements
TYPED_TEST(SmallVectorTest, EraseRangeTest) {
  SCOPED_TRACE("EraseRangeTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);
  const auto &theConstVector = V;
  V.erase(theConstVector.begin(), theConstVector.begin() + 2);
  assertValuesInOrder(V, 1u, 3);
}

// Insert a single element.
TYPED_TEST(SmallVectorTest, InsertTest) {
  SCOPED_TRACE("InsertTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);
  typename TypeParam::iterator I = V.insert(V.begin() + 1, Constructable(77));
  EXPECT_EQ(V.begin() + 1, I);
  assertValuesInOrder(V, 4u, 1, 77, 2, 3);
}

// Insert a copy of a single element.
TYPED_TEST(SmallVectorTest, InsertCopy) {
  SCOPED_TRACE("InsertTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);
  Constructable C(77);
  typename TypeParam::iterator I = V.insert(V.begin() + 1, C);
  EXPECT_EQ(V.begin() + 1, I);
  assertValuesInOrder(V, 4u, 1, 77, 2, 3);
}

// Insert repeated elements.
TYPED_TEST(SmallVectorTest, InsertRepeatedTest) {
  SCOPED_TRACE("InsertRepeatedTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 4);
  Constructable::reset();
  auto I = V.insert(V.begin() + 1, 2, Constructable(16));
  // Move construct the top element into newly allocated space, and optionally
  // reallocate the whole buffer, move constructing into it.
  // FIXME: This is inefficient, we shouldn't move things into newly allocated
  // space, then move them up/around, there should only be 2 or 4 move
  // constructions here.
  EXPECT_TRUE(Constructable::getNumMoveConstructorCalls() == 2 ||
              Constructable::getNumMoveConstructorCalls() == 6);
  // Move assign the next two to shift them up and make a gap.
  EXPECT_EQ(1, Constructable::getNumMoveAssignmentCalls());
  // Copy construct the two new elements from the parameter.
  EXPECT_EQ(2, Constructable::getNumCopyAssignmentCalls());
  // All without any copy construction.
  EXPECT_EQ(0, Constructable::getNumCopyConstructorCalls());
  EXPECT_EQ(V.begin() + 1, I);
  assertValuesInOrder(V, 6u, 1, 16, 16, 2, 3, 4);
}

TYPED_TEST(SmallVectorTest, InsertRepeatedNonIterTest) {
  SCOPED_TRACE("InsertRepeatedTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 4);
  Constructable::reset();
  auto I = V.insert(V.begin() + 1, 2, 7);
  EXPECT_EQ(V.begin() + 1, I);
  assertValuesInOrder(V, 6u, 1, 7, 7, 2, 3, 4);
}

TYPED_TEST(SmallVectorTest, InsertRepeatedAtEndTest) {
  SCOPED_TRACE("InsertRepeatedTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 4);
  Constructable::reset();
  auto I = V.insert(V.end(), 2, Constructable(16));
  // Just copy construct them into newly allocated space
  EXPECT_EQ(2, Constructable::getNumCopyConstructorCalls());
  // Move everything across if reallocation is needed.
  EXPECT_TRUE(Constructable::getNumMoveConstructorCalls() == 0 ||
              Constructable::getNumMoveConstructorCalls() == 4);
  // Without ever moving or copying anything else.
  EXPECT_EQ(0, Constructable::getNumCopyAssignmentCalls());
  EXPECT_EQ(0, Constructable::getNumMoveAssignmentCalls());

  EXPECT_EQ(V.begin() + 4, I);
  assertValuesInOrder(V, 6u, 1, 2, 3, 4, 16, 16);
}

TYPED_TEST(SmallVectorTest, InsertRepeatedEmptyTest) {
  SCOPED_TRACE("InsertRepeatedTest");
  auto &V = this->theVector;
  makeSequence(V, 10, 15);

  // Empty insert.
  EXPECT_EQ(V.end(), V.insert(V.end(), 0, Constructable(42)));
  EXPECT_EQ(V.begin() + 1, V.insert(V.begin() + 1, 0, Constructable(42)));
}

// Insert range.
TYPED_TEST(SmallVectorTest, InsertRangeTest) {
  SCOPED_TRACE("InsertRangeTest");
  auto &V = this->theVector;
  Constructable Arr[3] =
    { Constructable(77), Constructable(77), Constructable(77) };

  makeSequence(V, 1, 3);
  Constructable::reset();
  auto I = V.insert(V.begin() + 1, Arr, Arr + 3);
  // Move construct the top 3 elements into newly allocated space.
  // Possibly move the whole sequence into new space first.
  // FIXME: This is inefficient, we shouldn't move things into newly allocated
  // space, then move them up/around, there should only be 2 or 3 move
  // constructions here.
  EXPECT_TRUE(Constructable::getNumMoveConstructorCalls() == 2 ||
              Constructable::getNumMoveConstructorCalls() == 5);
  // Copy assign the lower 2 new elements into existing space.
  EXPECT_EQ(2, Constructable::getNumCopyAssignmentCalls());
  // Copy construct the third element into newly allocated space.
  EXPECT_EQ(1, Constructable::getNumCopyConstructorCalls());
  EXPECT_EQ(V.begin() + 1, I);
  assertValuesInOrder(V, 6u, 1, 77, 77, 77, 2, 3);
}


TYPED_TEST(SmallVectorTest, InsertRangeAtEndTest) {
  SCOPED_TRACE("InsertRangeTest");
  auto &V = this->theVector;
  Constructable Arr[3] =
    { Constructable(77), Constructable(77), Constructable(77) };

  makeSequence(V, 1, 3);

  // Insert at end.
  Constructable::reset();
  auto I = V.insert(V.end(), Arr, Arr + 3);
  // Copy construct the 3 elements into new space at the top.
  EXPECT_EQ(3, Constructable::getNumCopyConstructorCalls());
  // Don't copy/move anything else.
  EXPECT_EQ(0, Constructable::getNumCopyAssignmentCalls());
  // Reallocation might occur, causing all elements to be moved into the new
  // buffer.
  EXPECT_TRUE(Constructable::getNumMoveConstructorCalls() == 0 ||
              Constructable::getNumMoveConstructorCalls() == 3);
  EXPECT_EQ(0, Constructable::getNumMoveAssignmentCalls());
  EXPECT_EQ(V.begin() + 3, I);
  assertValuesInOrder(V, 6u, 1, 2, 3, 77, 77, 77);
}

TYPED_TEST(SmallVectorTest, InsertEmptyRangeTest) {
  SCOPED_TRACE("InsertRangeTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);

  // Empty insert.
  EXPECT_EQ(V.end(), V.insert(V.end(), V.begin(), V.begin()));
  EXPECT_EQ(V.begin() + 1, V.insert(V.begin() + 1, V.begin(), V.begin()));
}

// Comparison tests.
TYPED_TEST(SmallVectorTest, ComparisonEqualityTest) {
  SCOPED_TRACE("ComparisonEqualityTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  makeSequence(V, 1, 3);
  makeSequence(U, 1, 3);

  EXPECT_TRUE(V == U);
  EXPECT_FALSE(V != U);

  U.clear();
  makeSequence(U, 2, 4);

  EXPECT_FALSE(V == U);
  EXPECT_TRUE(V != U);
}

// Comparison tests.
TYPED_TEST(SmallVectorTest, ComparisonLessThanTest) {
  SCOPED_TRACE("ComparisonLessThanTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  V = {1, 2, 4};
  U = {1, 4};

  EXPECT_TRUE(V < U);
  EXPECT_TRUE(V <= U);
  EXPECT_FALSE(V > U);
  EXPECT_FALSE(V >= U);

  EXPECT_FALSE(U < V);
  EXPECT_FALSE(U <= V);
  EXPECT_TRUE(U > V);
  EXPECT_TRUE(U >= V);

  U = {1, 2, 4};

  EXPECT_FALSE(V < U);
  EXPECT_TRUE(V <= U);
  EXPECT_FALSE(V > U);
  EXPECT_TRUE(V >= U);

  EXPECT_FALSE(U < V);
  EXPECT_TRUE(U <= V);
  EXPECT_FALSE(U > V);
  EXPECT_TRUE(U >= V);
}

// Constant vector tests.
TYPED_TEST(SmallVectorTest, ConstVectorTest) {
  const TypeParam constVector;

  EXPECT_EQ(0u, constVector.size());
  EXPECT_TRUE(constVector.empty());
  EXPECT_TRUE(constVector.begin() == constVector.end());
}

// Direct array access.
TYPED_TEST(SmallVectorTest, DirectVectorTest) {
  auto &V = this->theVector;
  EXPECT_EQ(0u, V.size());
  V.reserve(4);
  EXPECT_LE(4u, V.capacity());
  EXPECT_EQ(0, Constructable::getNumConstructorCalls());
  V.push_back(1);
  V.push_back(2);
  V.push_back(3);
  V.push_back(4);
  EXPECT_EQ(4u, V.size());
  EXPECT_EQ(8, Constructable::getNumConstructorCalls());
  EXPECT_EQ(1, V[0].getValue());
  EXPECT_EQ(2, V[1].getValue());
  EXPECT_EQ(3, V[2].getValue());
  EXPECT_EQ(4, V[3].getValue());
}

TYPED_TEST(SmallVectorTest, IteratorTest) {
  auto &V = this->theVector;
  std::list<int> L;
  V.insert(V.end(), L.begin(), L.end());
}

template <typename InvalidType> class DualSmallVectorsTest;

template <typename VectorT1, typename VectorT2>
class DualSmallVectorsTest<std::pair<VectorT1, VectorT2>> : public SmallVectorTestBase {
protected:
  VectorT1 theVector;
  VectorT2 otherVector;
};

typedef ::testing::Types<
    // Small mode -> Small mode.
    std::pair<SmallVector<Constructable, 4>, SmallVector<Constructable, 4>>,
    // Small mode -> Big mode.
    std::pair<SmallVector<Constructable, 4>, SmallVector<Constructable, 2>>,
    // Big mode -> Small mode.
    std::pair<SmallVector<Constructable, 2>, SmallVector<Constructable, 4>>,
    // Big mode -> Big mode.
    std::pair<SmallVector<Constructable, 2>, SmallVector<Constructable, 2>>
  > DualSmallVectorTestTypes;

TYPED_TEST_SUITE(DualSmallVectorsTest, DualSmallVectorTestTypes, );

TYPED_TEST(DualSmallVectorsTest, MoveAssignment) {
  SCOPED_TRACE("MoveAssignTest-DualVectorTypes");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  // Set up our vector with four elements.
  for (unsigned I = 0; I < 4; ++I)
    U.push_back(Constructable(I));

  const Constructable *OrigDataPtr = U.data();

  // Move-assign from the other vector.
  V = std::move(static_cast<SmallVectorImpl<Constructable> &>(U));

  // Make sure we have the right result.
  assertValuesInOrder(V, 4u, 0, 1, 2, 3);

  // Make sure the # of constructor/destructor calls line up. There
  // are two live objects after clearing the other vector.
  U.clear();
  EXPECT_EQ(Constructable::getNumConstructorCalls()-4,
            Constructable::getNumDestructorCalls());

  // If the source vector (otherVector) was in small-mode, assert that we just
  // moved the data pointer over.
  EXPECT_TRUE(NumBuiltinElts(U) == 4 || V.data() == OrigDataPtr);

  // There shouldn't be any live objects any more.
  V.clear();
  EXPECT_EQ(Constructable::getNumConstructorCalls(),
            Constructable::getNumDestructorCalls());

  // We shouldn't have copied anything in this whole process.
  EXPECT_EQ(Constructable::getNumCopyConstructorCalls(), 0);
}

struct notassignable {
  int &x;
  notassignable(int &x) : x(x) {}
};

TEST(SmallVectorCustomTest, NoAssignTest) {
  int x = 0;
  SmallVector<notassignable, 2> vec;
  vec.push_back(notassignable(x));
  x = 42;
  EXPECT_EQ(42, vec.pop_back_val().x);
}

struct MovedFrom {
  bool hasValue;
  MovedFrom() : hasValue(true) {
  }
  MovedFrom(MovedFrom&& m) : hasValue(m.hasValue) {
    m.hasValue = false;
  }
  MovedFrom &operator=(MovedFrom&& m) {
    hasValue = m.hasValue;
    m.hasValue = false;
    return *this;
  }
};

TEST(SmallVectorTest, MidInsert) {
  SmallVector<MovedFrom, 3> v;
  v.push_back(MovedFrom());
  v.insert(v.begin(), MovedFrom());
  for (MovedFrom &m : v)
    EXPECT_TRUE(m.hasValue);
}

enum EmplaceableArgState {
  EAS_Defaulted,
  EAS_Arg,
  EAS_LValue,
  EAS_RValue,
  EAS_Failure
};
template <int I> struct EmplaceableArg {
  EmplaceableArgState State;
  EmplaceableArg() : State(EAS_Defaulted) {}
  EmplaceableArg(EmplaceableArg &&X)
      : State(X.State == EAS_Arg ? EAS_RValue : EAS_Failure) {}
  EmplaceableArg(EmplaceableArg &X)
      : State(X.State == EAS_Arg ? EAS_LValue : EAS_Failure) {}

  explicit EmplaceableArg(bool) : State(EAS_Arg) {}

private:
  EmplaceableArg &operator=(EmplaceableArg &&) = delete;
  EmplaceableArg &operator=(const EmplaceableArg &) = delete;
};

enum EmplaceableState { ES_Emplaced, ES_Moved };
struct Emplaceable {
  EmplaceableArg<0> A0;
  EmplaceableArg<1> A1;
  EmplaceableArg<2> A2;
  EmplaceableArg<3> A3;
  EmplaceableState State;

  Emplaceable() : State(ES_Emplaced) {}

  template <class A0Ty>
  explicit Emplaceable(A0Ty &&A0)
      : A0(std::forward<A0Ty>(A0)), State(ES_Emplaced) {}

  template <class A0Ty, class A1Ty>
  Emplaceable(A0Ty &&A0, A1Ty &&A1)
      : A0(std::forward<A0Ty>(A0)), A1(std::forward<A1Ty>(A1)),
        State(ES_Emplaced) {}

  template <class A0Ty, class A1Ty, class A2Ty>
  Emplaceable(A0Ty &&A0, A1Ty &&A1, A2Ty &&A2)
      : A0(std::forward<A0Ty>(A0)), A1(std::forward<A1Ty>(A1)),
        A2(std::forward<A2Ty>(A2)), State(ES_Emplaced) {}

  template <class A0Ty, class A1Ty, class A2Ty, class A3Ty>
  Emplaceable(A0Ty &&A0, A1Ty &&A1, A2Ty &&A2, A3Ty &&A3)
      : A0(std::forward<A0Ty>(A0)), A1(std::forward<A1Ty>(A1)),
        A2(std::forward<A2Ty>(A2)), A3(std::forward<A3Ty>(A3)),
        State(ES_Emplaced) {}

  Emplaceable(Emplaceable &&) : State(ES_Moved) {}
  Emplaceable &operator=(Emplaceable &&) {
    State = ES_Moved;
    return *this;
  }

private:
  Emplaceable(const Emplaceable &) = delete;
  Emplaceable &operator=(const Emplaceable &) = delete;
};

TEST(SmallVectorTest, EmplaceBack) {
  EmplaceableArg<0> A0(true);
  EmplaceableArg<1> A1(true);
  EmplaceableArg<2> A2(true);
  EmplaceableArg<3> A3(true);
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back();
    EXPECT_TRUE(&back == &V.back());
    EXPECT_TRUE(V.size() == 1);
    EXPECT_TRUE(back.State == ES_Emplaced);
    EXPECT_TRUE(back.A0.State == EAS_Defaulted);
    EXPECT_TRUE(back.A1.State == EAS_Defaulted);
    EXPECT_TRUE(back.A2.State == EAS_Defaulted);
    EXPECT_TRUE(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(std::move(A0));
    EXPECT_TRUE(&back == &V.back());
    EXPECT_TRUE(V.size() == 1);
    EXPECT_TRUE(back.State == ES_Emplaced);
    EXPECT_TRUE(back.A0.State == EAS_RValue);
    EXPECT_TRUE(back.A1.State == EAS_Defaulted);
    EXPECT_TRUE(back.A2.State == EAS_Defaulted);
    EXPECT_TRUE(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(A0);
    EXPECT_TRUE(&back == &V.back());
    EXPECT_TRUE(V.size() == 1);
    EXPECT_TRUE(back.State == ES_Emplaced);
    EXPECT_TRUE(back.A0.State == EAS_LValue);
    EXPECT_TRUE(back.A1.State == EAS_Defaulted);
    EXPECT_TRUE(back.A2.State == EAS_Defaulted);
    EXPECT_TRUE(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(A0, A1);
    EXPECT_TRUE(&back == &V.back());
    EXPECT_TRUE(V.size() == 1);
    EXPECT_TRUE(back.State == ES_Emplaced);
    EXPECT_TRUE(back.A0.State == EAS_LValue);
    EXPECT_TRUE(back.A1.State == EAS_LValue);
    EXPECT_TRUE(back.A2.State == EAS_Defaulted);
    EXPECT_TRUE(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(std::move(A0), std::move(A1));
    EXPECT_TRUE(&back == &V.back());
    EXPECT_TRUE(V.size() == 1);
    EXPECT_TRUE(back.State == ES_Emplaced);
    EXPECT_TRUE(back.A0.State == EAS_RValue);
    EXPECT_TRUE(back.A1.State == EAS_RValue);
    EXPECT_TRUE(back.A2.State == EAS_Defaulted);
    EXPECT_TRUE(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(std::move(A0), A1, std::move(A2), A3);
    EXPECT_TRUE(&back == &V.back());
    EXPECT_TRUE(V.size() == 1);
    EXPECT_TRUE(back.State == ES_Emplaced);
    EXPECT_TRUE(back.A0.State == EAS_RValue);
    EXPECT_TRUE(back.A1.State == EAS_LValue);
    EXPECT_TRUE(back.A2.State == EAS_RValue);
    EXPECT_TRUE(back.A3.State == EAS_LValue);
  }
  {
    SmallVector<int, 1> V;
    V.emplace_back();
    V.emplace_back(42);
    EXPECT_EQ(2U, V.size());
    EXPECT_EQ(0, V[0]);
    EXPECT_EQ(42, V[1]);
  }
}

TEST(SmallVectorTest, DefaultInlinedElements) {
  SmallVector<int> V;
  EXPECT_TRUE(V.empty());
  V.push_back(7);
  EXPECT_EQ(V[0], 7);

  // Check that at least a couple layers of nested SmallVector<T>'s are allowed
  // by the default inline elements policy. This pattern happens in practice
  // with some frequency, and it seems fairly harmless even though each layer of
  // SmallVector's will grow the total sizeof by a vector header beyond the
  // "preferred" maximum sizeof.
  SmallVector<SmallVector<SmallVector<int>>> NestedV;
  NestedV.emplace_back().emplace_back().emplace_back(42);
  EXPECT_EQ(NestedV[0][0][0], 42);
}

TEST(SmallVectorTest, InitializerList) {
  SmallVector<int, 2> V1 = {};
  EXPECT_TRUE(V1.empty());
  V1 = {0, 0};
  EXPECT_TRUE(ArrayRef(V1).equals({0, 0}));
  V1 = {-1, -1};
  EXPECT_TRUE(ArrayRef(V1).equals({-1, -1}));

  SmallVector<int, 2> V2 = {1, 2, 3, 4};
  EXPECT_TRUE(ArrayRef(V2).equals({1, 2, 3, 4}));
  V2.assign({4});
  EXPECT_TRUE(ArrayRef(V2).equals({4}));
  V2.append({3, 2});
  EXPECT_TRUE(ArrayRef(V2).equals({4, 3, 2}));
  V2.insert(V2.begin() + 1, 5);
  EXPECT_TRUE(ArrayRef(V2).equals({4, 5, 3, 2}));
}

TEST(SmallVectorTest, ToVector) {
  {
    std::vector<char> v = {'a', 'b', 'c'};
    auto Vector = to_vector<4>(v);
    static_assert(NumBuiltinElts(Vector) == 4u);
    ASSERT_EQ(3u, Vector.size());
    for (size_t I = 0; I < v.size(); ++I)
      EXPECT_EQ(v[I], Vector[I]);
  }
  {
    std::vector<char> v = {'a', 'b', 'c'};
    auto Vector = to_vector(v);
    static_assert(NumBuiltinElts(Vector) != 4u);
    ASSERT_EQ(3u, Vector.size());
    for (size_t I = 0; I < v.size(); ++I)
      EXPECT_EQ(v[I], Vector[I]);
  }
}

struct To {
  int Content;
  friend bool operator==(const To &LHS, const To &RHS) {
    return LHS.Content == RHS.Content;
  }
};

class From {
public:
  From() = default;
  From(To M) { T = M; }
  operator To() const { return T; }

private:
  To T;
};

TEST(SmallVectorTest, ConstructFromArrayRefOfConvertibleType) {
  To to1{1}, to2{2}, to3{3};
  std::vector<From> StdVector = {From(to1), From(to2), From(to3)};
  ArrayRef<From> Array = StdVector;
  {
    llvm::SmallVector<To> Vector(Array);

    ASSERT_EQ(Array.size(), Vector.size());
    for (size_t I = 0; I < Array.size(); ++I)
      EXPECT_EQ(Array[I], Vector[I]);
  }
  {
    llvm::SmallVector<To, 4> Vector(Array);

    ASSERT_EQ(Array.size(), Vector.size());
    ASSERT_EQ(4u, NumBuiltinElts(Vector));
    for (size_t I = 0; I < Array.size(); ++I)
      EXPECT_EQ(Array[I], Vector[I]);
  }
}

TEST(SmallVectorTest, ToVectorOf) {
  To to1{1}, to2{2}, to3{3};
  std::vector<From> StdVector = {From(to1), From(to2), From(to3)};
  {
    llvm::SmallVector<To> Vector = llvm::to_vector_of<To>(StdVector);

    ASSERT_EQ(StdVector.size(), Vector.size());
    for (size_t I = 0; I < StdVector.size(); ++I)
      EXPECT_EQ(StdVector[I], Vector[I]);
  }
  {
    auto Vector = llvm::to_vector_of<To, 4>(StdVector);

    ASSERT_EQ(StdVector.size(), Vector.size());
    static_assert(NumBuiltinElts(Vector) == 4u);
    for (size_t I = 0; I < StdVector.size(); ++I)
      EXPECT_EQ(StdVector[I], Vector[I]);
  }
}

template <class VectorT>
class SmallVectorReferenceInvalidationTest : public SmallVectorTestBase {
protected:
  const char *AssertionMessage =
      "Attempting to reference an element of the vector in an operation \" "
      "\"that invalidates it";

  VectorT V;

  template <class T> static bool isValueType() {
    return std::is_same_v<T, typename VectorT::value_type>;
  }

  void SetUp() override {
    SmallVectorTestBase::SetUp();

    // Fill up the small size so that insertions move the elements.
    for (int I = 0, E = NumBuiltinElts(V); I != E; ++I)
      V.emplace_back(I + 1);
  }
};

// Test one type that's trivially copyable (int) and one that isn't
// (Constructable) since reference invalidation may be fixed differently for
// each.
using SmallVectorReferenceInvalidationTestTypes =
    ::testing::Types<SmallVector<int, 3>, SmallVector<Constructable, 3>>;

TYPED_TEST_SUITE(SmallVectorReferenceInvalidationTest,
                 SmallVectorReferenceInvalidationTestTypes, );

TYPED_TEST(SmallVectorReferenceInvalidationTest, PushBack) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  int N = NumBuiltinElts(V);

  // Push back a reference to last element when growing from small storage.
  V.push_back(V.back());
  EXPECT_EQ(N, V.back());

  // Check that the old value is still there (not moved away).
  EXPECT_EQ(N, V[V.size() - 2]);

  // Fill storage again.
  V.back() = V.size();
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Push back a reference to last element when growing from large storage.
  V.push_back(V.back());
  EXPECT_EQ(int(V.size()) - 1, V.back());
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, PushBackMoved) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  int N = NumBuiltinElts(V);

  // Push back a reference to last element when growing from small storage.
  V.push_back(std::move(V.back()));
  EXPECT_EQ(N, V.back());
  if (this->template isValueType<Constructable>()) {
    // Check that the value was moved (not copied).
    EXPECT_EQ(0, V[V.size() - 2]);
  }

  // Fill storage again.
  V.back() = V.size();
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Push back a reference to last element when growing from large storage.
  V.push_back(std::move(V.back()));

  // Check the values.
  EXPECT_EQ(int(V.size()) - 1, V.back());
  if (this->template isValueType<Constructable>()) {
    // Check the value got moved out.
    EXPECT_EQ(0, V[V.size() - 2]);
  }
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, Resize) {
  auto &V = this->V;
  (void)V;
  int N = NumBuiltinElts(V);
  V.resize(N + 1, V.back());
  EXPECT_EQ(N, V.back());

  // Resize to add enough elements that V will grow again. If reference
  // invalidation breaks in the future, sanitizers should be able to catch a
  // use-after-free here.
  V.resize(V.capacity() + 1, V.front());
  EXPECT_EQ(1, V.back());
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, Append) {
  auto &V = this->V;
  (void)V;
  V.append(1, V.back());
  int N = NumBuiltinElts(V);
  EXPECT_EQ(N, V[N - 1]);

  // Append enough more elements that V will grow again. This tests growing
  // when already in large mode.
  //
  // If reference invalidation breaks in the future, sanitizers should be able
  // to catch a use-after-free here.
  V.append(V.capacity() - V.size() + 1, V.front());
  EXPECT_EQ(1, V.back());
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, AppendRange) {
  auto &V = this->V;
  (void)V;
#if !defined(NDEBUG) && GTEST_HAS_DEATH_TEST
  EXPECT_DEATH(V.append(V.begin(), V.begin() + 1), this->AssertionMessage);

  ASSERT_EQ(3u, NumBuiltinElts(V));
  ASSERT_EQ(3u, V.size());
  V.pop_back();
  ASSERT_EQ(2u, V.size());

  // Confirm this checks for growth when there's more than one element
  // appended.
  EXPECT_DEATH(V.append(V.begin(), V.end()), this->AssertionMessage);
#endif
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, Assign) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  (void)V;
  int N = NumBuiltinElts(V);
  ASSERT_EQ(unsigned(N), V.size());
  ASSERT_EQ(unsigned(N), V.capacity());

  // Check assign that shrinks in small mode.
  V.assign(1, V.back());
  EXPECT_EQ(1u, V.size());
  EXPECT_EQ(N, V[0]);

  // Check assign that grows within small mode.
  ASSERT_LT(V.size(), V.capacity());
  V.assign(V.capacity(), V.back());
  for (int I = 0, E = V.size(); I != E; ++I) {
    EXPECT_EQ(N, V[I]);

    // Reset to [1, 2, ...].
    V[I] = I + 1;
  }

  // Check assign that grows to large mode.
  ASSERT_EQ(2, V[1]);
  V.assign(V.capacity() + 1, V[1]);
  for (int I = 0, E = V.size(); I != E; ++I) {
    EXPECT_EQ(2, V[I]);

    // Reset to [1, 2, ...].
    V[I] = I + 1;
  }

  // Check assign that shrinks in large mode.
  V.assign(1, V[1]);
  EXPECT_EQ(2, V[0]);
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, AssignRange) {
  auto &V = this->V;
#if !defined(NDEBUG) && GTEST_HAS_DEATH_TEST
  EXPECT_DEATH(V.assign(V.begin(), V.end()), this->AssertionMessage);
  EXPECT_DEATH(V.assign(V.begin(), V.end() - 1), this->AssertionMessage);
#endif
  V.assign(V.begin(), V.begin());
  EXPECT_TRUE(V.empty());
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, Insert) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  (void)V;

  // Insert a reference to the back (not at end() or else insert delegates to
  // push_back()), growing out of small mode. Confirm the value was copied out
  // (moving out Constructable sets it to 0).
  V.insert(V.begin(), V.back());
  EXPECT_EQ(int(V.size() - 1), V.front());
  EXPECT_EQ(int(V.size() - 1), V.back());

  // Fill up the vector again.
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Grow again from large storage to large storage.
  V.insert(V.begin(), V.back());
  EXPECT_EQ(int(V.size() - 1), V.front());
  EXPECT_EQ(int(V.size() - 1), V.back());
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, InsertMoved) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  (void)V;

  // Insert a reference to the back (not at end() or else insert delegates to
  // push_back()), growing out of small mode. Confirm the value was copied out
  // (moving out Constructable sets it to 0).
  V.insert(V.begin(), std::move(V.back()));
  EXPECT_EQ(int(V.size() - 1), V.front());
  if (this->template isValueType<Constructable>()) {
    // Check the value got moved out.
    EXPECT_EQ(0, V.back());
  }

  // Fill up the vector again.
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Grow again from large storage to large storage.
  V.insert(V.begin(), std::move(V.back()));
  EXPECT_EQ(int(V.size() - 1), V.front());
  if (this->template isValueType<Constructable>()) {
    // Check the value got moved out.
    EXPECT_EQ(0, V.back());
  }
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, InsertN) {
  auto &V = this->V;
  (void)V;

  // Cover NumToInsert <= this->end() - I.
  V.insert(V.begin() + 1, 1, V.back());
  int N = NumBuiltinElts(V);
  EXPECT_EQ(N, V[1]);

  // Cover NumToInsert > this->end() - I, inserting enough elements that V will
  // also grow again; V.capacity() will be more elements than necessary but
  // it's a simple way to cover both conditions.
  //
  // If reference invalidation breaks in the future, sanitizers should be able
  // to catch a use-after-free here.
  V.insert(V.begin(), V.capacity(), V.front());
  EXPECT_EQ(1, V.front());
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, InsertRange) {
  auto &V = this->V;
  (void)V;
#if !defined(NDEBUG) && GTEST_HAS_DEATH_TEST
  EXPECT_DEATH(V.insert(V.begin(), V.begin(), V.begin() + 1),
               this->AssertionMessage);

  ASSERT_EQ(3u, NumBuiltinElts(V));
  ASSERT_EQ(3u, V.size());
  V.pop_back();
  ASSERT_EQ(2u, V.size());

  // Confirm this checks for growth when there's more than one element
  // inserted.
  EXPECT_DEATH(V.insert(V.begin(), V.begin(), V.end()), this->AssertionMessage);
#endif
}

TYPED_TEST(SmallVectorReferenceInvalidationTest, EmplaceBack) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  int N = NumBuiltinElts(V);

  // Push back a reference to last element when growing from small storage.
  V.emplace_back(V.back());
  EXPECT_EQ(N, V.back());

  // Check that the old value is still there (not moved away).
  EXPECT_EQ(N, V[V.size() - 2]);

  // Fill storage again.
  V.back() = V.size();
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Push back a reference to last element when growing from large storage.
  V.emplace_back(V.back());
  EXPECT_EQ(int(V.size()) - 1, V.back());
}

template <class VectorT>
class SmallVectorInternalReferenceInvalidationTest
    : public SmallVectorTestBase {
protected:
  const char *AssertionMessage =
      "Attempting to reference an element of the vector in an operation \" "
      "\"that invalidates it";

  VectorT V;

  void SetUp() override {
    SmallVectorTestBase::SetUp();

    // Fill up the small size so that insertions move the elements.
    for (int I = 0, E = NumBuiltinElts(V); I != E; ++I)
      V.emplace_back(I + 1, I + 1);
  }
};

// Test pairs of the same types from SmallVectorReferenceInvalidationTestTypes.
using SmallVectorInternalReferenceInvalidationTestTypes =
    ::testing::Types<SmallVector<std::pair<int, int>, 3>,
                     SmallVector<std::pair<Constructable, Constructable>, 3>>;

TYPED_TEST_SUITE(SmallVectorInternalReferenceInvalidationTest,
                 SmallVectorInternalReferenceInvalidationTestTypes, );

TYPED_TEST(SmallVectorInternalReferenceInvalidationTest, EmplaceBack) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  int N = NumBuiltinElts(V);

  // Push back a reference to last element when growing from small storage.
  V.emplace_back(V.back().first, V.back().second);
  EXPECT_EQ(N, V.back().first);
  EXPECT_EQ(N, V.back().second);

  // Check that the old value is still there (not moved away).
  EXPECT_EQ(N, V[V.size() - 2].first);
  EXPECT_EQ(N, V[V.size() - 2].second);

  // Fill storage again.
  V.back().first = V.back().second = V.size();
  while (V.size() < V.capacity())
    V.emplace_back(V.size() + 1, V.size() + 1);

  // Push back a reference to last element when growing from large storage.
  V.emplace_back(V.back().first, V.back().second);
  EXPECT_EQ(int(V.size()) - 1, V.back().first);
  EXPECT_EQ(int(V.size()) - 1, V.back().second);
}

} // end namespace
