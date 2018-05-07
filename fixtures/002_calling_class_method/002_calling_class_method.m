#import <Foundation/Foundation.h>

@interface FirstClass : NSObject
+ (int)hello;
@end

@implementation FirstClass
+ (int)hello {
    return 1234;
}
@end

int run() {
    int result = [FirstClass hello];

    return result;
}
