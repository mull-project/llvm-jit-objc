  //
  //  main.m
  //  CustomXCTestRunnerObjC
  //
  //  Created by Stanislav Pankevich on 30.03.18.
  //  Copyright © 2018 Stanislav Pankevich. All rights reserved.
  //

#import "CustomXCTestRunner.h"

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

@interface TestCase1 : XCTestCase
- (void)testHelloWorld1;
@end

@implementation TestCase1
- (void)testHelloWorld1 {
  XCTAssert(YES);
}
@end

@interface TestCase2 : XCTestCase
- (void)testHelloWorld2;
@end

@implementation TestCase2
- (void)testHelloWorld2 {
  XCTAssert(YES);
}
@end

int main(int argc, const char * argv[]) {
  int testRunResult = 0;
  @autoreleasepool {
    NSLog(@"Hello, World!");

    testRunResult = CustomXCTestRunnerRunAll();
    testRunResult = CustomXCTestRunnerRunAll();

    char tests[1024];
    CustomXCTestRunnerPrintAllTests(tests);

    printf("tests: %s\n", tests);
    CustomXCTestRunnerRunOne("TestCase1.testHelloWorld1");

  }
  return testRunResult;
}

