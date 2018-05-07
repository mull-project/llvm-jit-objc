#import <Foundation/Foundation.h>

@interface ClassA : NSObject
- (int)hello;
@end

@implementation ClassA {
    int _ivar1;
    int _ivar2;
}
- (id)init {
    self = [super init];
    _ivar1 = 1000;
    _ivar2 = 100;
    return self;
}
- (int)hello {
    return _ivar1 + _ivar2;
}
@end

@interface ClassB : ClassA
- (int)hello;
@end

@implementation ClassB {
    int _ivar1;
    int _ivar2;
}
- (id)init {
    self = [super init];
    _ivar1 = 10;
    _ivar2 = 1;
    return self;
}
- (int)hello {
    int superHello = [super hello];
    return superHello + _ivar1 + _ivar2;
}
@end

int run() {
    ClassB *clB = [[ClassB alloc] init];
    int result = [clB hello];
    return result;
}
