#import <Foundation/Foundation.h>

@interface ClassA : NSObject
@property (nonatomic, assign) int prop1;
@property (nonatomic, assign) int prop2;
- (int)hello;
@end

@implementation ClassA
- (id)init {
    self = [super init];
    self.prop1 = 1000;
    self.prop2 = 100;
    return self;
}
- (int)hello {
    return self.prop1 + self.prop2;
}
@end

@interface ClassB : ClassA
@property (nonatomic, assign) int prop3;
@property (nonatomic, assign) int prop4;
- (int)hello;
@end

@implementation ClassB
- (id)init {
    self = [super init];
    self.prop3 = 10;
    self.prop4 = 1;
    return self;
}
- (int)hello {
    int superHello = [super hello];
    return superHello + self.prop3 + self.prop4;
}
@end

int run() {
    ClassB *clB = [[ClassB alloc] init];
    int result = [clB hello];
    return result;
}
