#import <Foundation/Foundation.h>

@interface FirstClass : NSObject
- (int)hello;
@end

@implementation FirstClass
- (int)hello {
    return 1234;
}
@end

int run() {
    FirstClass *fc = [[FirstClass alloc] init];

    int result = [fc hello];

    return result;
}
