//
//  CustomXCTestRunner.h
//  CustomXCTestRunner (Objective-C)
//
//  Created by Stanislaw Pankevich on 30/03/18.
//  Copyright © 2016 Lowlevelbits.org. All rights reserved.
//

#import "CustomXCTestRunner.h"

#import <XCTest/XCTest.h>
#import <XCTest/XCTestObservationCenter.h>

#import <objc/runtime.h>

static NSString *getTestName(NSString *fullTestName) {
  NSString *searchedString = fullTestName;
  NSRange searchedRange = NSMakeRange(0, [searchedString length]);

  NSString *pattern = @"^-\\[[A-Za-z0-9_]+ ([A-Za-z0-9_]+)\\]$";
  NSError  *error = nil;

  NSRegularExpression* regex =
    [NSRegularExpression regularExpressionWithPattern:pattern
                                              options:0
                                                error:&error];

  NSArray *matches = [regex matchesInString:searchedString
                                    options:0
                                      range:searchedRange];

  if (matches.count == 0) {
    return nil;
  }

  for (NSTextCheckingResult* match in matches) {
      //    NSString *matchText = [searchedString substringWithRange:[match range]];
    NSRange group1 = [match rangeAtIndex:1];
    return [searchedString substringWithRange:group1];
  }
  return nil;
}

@interface CustomXCTestObserver : NSObject <XCTestObservation>
@property (assign, nonatomic) NSUInteger testsFailed;
@end

@implementation CustomXCTestObserver

- (instancetype)init {
  self = [super init];

  self.testsFailed = 0;

  return self;
}

//- (void)testBundleWillStart:(NSBundle *)testBundle {
//  NSLog(@"testBundleWillStart: %@", testBundle);
//}
//
//- (void)testBundleDidFinish:(NSBundle *)testBundle {
//  NSLog(@"testBundleDidFinish: %@", testBundle);
//}
//
//- (void)testSuiteWillStart:(XCTestSuite *)testSuite {
//  NSLog(@"testSuiteWillStart: %@", testSuite);
//}
//
//- (void)testCaseWillStart:(XCTestCase *)testCase {
//  NSLog(@"testCaseWillStart: %@", testCase);
//}
//
//- (void)testSuiteDidFinish:(XCTestSuite *)testSuite {
//  NSLog(@"testSuiteDidFinish: %@", testSuite);
//}
//
//- (void)testSuite:(XCTestSuite *)testSuite didFailWithDescription:(NSString *)description inFile:(NSString *)filePath atLine:(NSUInteger)lineNumber {
//  NSLog(@"testSuite:didFailWithDescription:inFile:atLine: %@ %@ %@ %tu",
//        testSuite, description, filePath, lineNumber);
//}
//
- (void)testCase:(XCTestCase *)testCase didFailWithDescription:(NSString *)description inFile:(NSString *)filePath atLine:(NSUInteger)lineNumber {
  //NSLog(@"testCase:didFailWithDescription:inFile:atLine: %@ %@ %@ %tu",
  //      testCase, description, filePath, lineNumber);
  self.testsFailed++;
}

//- (void)testCaseDidFinish:(XCTestCase *)testCase {
//  NSLog(@"testCaseWillFinish: %@", testCase);
//}

@end

static void ObjCEnumerateRuntimeClasses(void(^callback)(Class)) {
  int numClasses;
  numClasses = objc_getClassList(NULL, 0);

  if (numClasses == 0) {
    return;
  }

  Class * classes = (Class *)malloc(sizeof(Class) * numClasses);
  numClasses = objc_getClassList(classes, numClasses);

  for (int i = 0; i < numClasses; i++) {
    Class runtimeClass = classes[i];

    callback(runtimeClass);
  }

  free(classes);
}

int CustomXCTestRunnerRunAll(void) {
  int exitResult = 0;

  @autoreleasepool {
    CustomXCTestObserver *testObserver = [CustomXCTestObserver new];
    Class xcTestCaseClass = NSClassFromString(@"XCTestCase");
    NSCAssert(xcTestCaseClass, nil);

    XCTestSuite *customXCTestRunnerSuite =
      [[XCTestSuite alloc] initWithName:@"CustomXCTestRunner Suite"];

    ObjCEnumerateRuntimeClasses(^(__unsafe_unretained Class runtimeClass) {
      if (class_getSuperclass(runtimeClass) != xcTestCaseClass) {
        return;
      }

      XCTestSuite *suite = [XCTestSuite testSuiteForTestCaseClass:runtimeClass];

      [customXCTestRunnerSuite addTest:suite];
    });

    XCTestObservationCenter *center = [XCTestObservationCenter sharedTestObservationCenter];
    [center addTestObserver:testObserver];

    [customXCTestRunnerSuite runTest];

    if (testObserver.testsFailed > 0) {
      exitResult = 1;
    }
  }

  return exitResult;
}

int CustomXCTestRunnerRunOne(const char *const testName) {
  int exitResult = 0;

  @autoreleasepool {
    CustomXCTestObserver *testObserver = [CustomXCTestObserver new];

    NSString *testNameString = [NSString stringWithUTF8String:testName];
    NSArray *testPair = [testNameString componentsSeparatedByString:@"."];

    if (testPair.count != 2) {
      return -1;
    }

    NSString *classString = testPair[0];
    NSString *testString = testPair[1];

    XCTestSuite *customXCTestRunnerSuite =
      [[XCTestSuite alloc] initWithName:@"CustomXCTestRunner Suite"];

    XCTestSuite *suite = [XCTestSuite testSuiteForTestCaseWithName:classString];
    XCTest *foundTest = nil;
    for (XCTest *test in suite.tests) {
      NSString *testName = getTestName(test.name);

      if ([testName isEqualToString:testString]) {
        foundTest = test;
      }
    }

    if (foundTest == nil) {
      return -1;
    }

    [customXCTestRunnerSuite addTest:foundTest];

    XCTestObservationCenter *center = [XCTestObservationCenter sharedTestObservationCenter];
    [center addTestObserver:testObserver];

    [customXCTestRunnerSuite runTest];

    if (testObserver.testsFailed > 0) {
      exitResult = 1;
    }

    return exitResult;
  }
}

void CustomXCTestRunnerPrintAllTests(char *output) {
//  printf("Running CustomXCTestRunnerPrintAllTests\n");
  Class xcTestCaseClass = NSClassFromString(@"XCTestCase");
  NSCAssert(xcTestCaseClass, nil);

  NSMutableArray <NSString *> *tests = [NSMutableArray new];
  ObjCEnumerateRuntimeClasses(^(__unsafe_unretained Class runtimeClass) {
    if (class_getSuperclass(runtimeClass) != xcTestCaseClass) {
      return;
    }

    XCTestSuite *suite = [XCTestSuite testSuiteForTestCaseClass:runtimeClass];

    for (XCTest *xcTest in suite.tests) {
      NSString *testName = getTestName(xcTest.name);
      if (testName == nil) {
        continue;
      }

      NSString *test = [NSString stringWithFormat:@"%@.%@", xcTest.className, testName];

      [tests addObject:test];
    }
  });

  NSString *allTestsString = [tests componentsJoinedByString:@","];
  sprintf(output, "%s", allTestsString.UTF8String);
}
