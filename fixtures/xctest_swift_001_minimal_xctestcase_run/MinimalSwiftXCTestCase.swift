import XCTest

// The recursive version of binary search.
public func binarySearch<T: Comparable>(_ a: [T], key: T, range: Range<Int>) -> Int? {
    if range.lowerBound >= range.upperBound {
        return nil
    } else {
        let midIndex = range.lowerBound + (range.upperBound - range.lowerBound) / 2
        if a[midIndex] > key {
            return binarySearch(a, key: key, range: range.lowerBound ..< midIndex)
        } else if a[midIndex] < key {
            return binarySearch(a, key: key, range: midIndex + 1 ..< range.upperBound)
        } else {
            return midIndex
        }
    }
}

/**
 The iterative version of binary search.

 Notice how similar these functions are. The difference is that this one
 uses a while loop, while the other calls itself recursively.
 **/

public func binarySearch<T: Comparable>(_ a: [T], key: T) -> Int? {
    var lowerBound = 0
    var upperBound = a.count
    while lowerBound < upperBound {
        let midIndex = lowerBound + (upperBound - lowerBound) / 2
        if a[midIndex] == key {
            return midIndex
        } else if a[midIndex] < key {
            lowerBound = midIndex + 1
        } else {
            upperBound = midIndex
        }
    }
    return nil
}

@objc(BinarySearchTest)
class BinarySearchTest: XCTestCase {
  var searchList = [Int]()

  override func setUp() {
    super.setUp()
    for number in 1...500 {
      searchList.append(number)
    }
  }

  func testEmptyArray() {
    let array = [Int]()
    let index = binarySearch(array, key: 123)
    XCTAssertNil(index)
  }

  func testBinarySearch() {
    for i in 1...100 {
      var array = [Int]()
      for number in 1...i {
        array.append(number)
      }
      let randomIndex = Int(arc4random_uniform(UInt32(i)))
      let testValue = array[randomIndex]

      let index = binarySearch(array, key: testValue)
      XCTAssertNotNil(index)
      XCTAssertEqual(index!, randomIndex)
      XCTAssertEqual(array[index!], testValue)
    }
  }

  func testLowerBound() {
    let index = binarySearch(searchList, key: 1)
    XCTAssertNotNil(index)
    XCTAssertEqual(index!, 0)
    XCTAssertEqual(searchList[index!], 1)
  }

  func testUpperBound() {
    let index = binarySearch(searchList, key: 500)
    XCTAssertNotNil(index)
    XCTAssertEqual(index!, 499)
    XCTAssertEqual(searchList[index!], 500)
  }

  func testOutOfLowerBound() {
    let index = binarySearch(searchList, key: 0)
    XCTAssertNil(index)
  }

  func testOutOfUpperBound() {
    let index = binarySearch(searchList, key: 501)
    XCTAssertNil(index)
  }
}
