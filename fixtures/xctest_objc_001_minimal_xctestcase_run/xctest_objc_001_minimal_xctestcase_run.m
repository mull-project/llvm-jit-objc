#import <XCTest/XCTest.h>

@interface TestCase : XCTestCase
@end

@implementation TestCase
- (void)testHello {
    XCTAssertTrue(YES);
}
@end
