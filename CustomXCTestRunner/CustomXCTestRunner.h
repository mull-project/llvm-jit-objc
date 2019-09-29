//
//  CustomXCTestRunner.h
//  CustomXCTestRunner (Objective-C)
//
//  Created by Stanislaw Pankevich on 30/03/18
//  Copyright © 2018 Lowlevelbits.org. All rights reserved.
//

#ifndef CustomXCTestRunner_h
#define CustomXCTestRunner_h

extern int CustomXCTestRunnerRunAll(void);
extern int CustomXCTestRunnerRunOne(const char *const testName);
extern void CustomXCTestRunnerPrintAllTests(char *output);

#endif /* CustomXCTestRunner_h */
