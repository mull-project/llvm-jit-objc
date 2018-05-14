#import <Foundation/Foundation.h>

@interface ClassWithCategory : NSObject
@property (nonatomic, assign) int prop;
@end

@implementation ClassWithCategory
- (id)init {
    self = [super init];
    self.prop = 1000;
    return self;
}
@end

@interface ClassWithCategory (CategoryThatReadsProperty)
+ (int)classMethodOnACategory;
- (int)readProperty;
@end

@implementation ClassWithCategory (CategoryThatReadsProperty)
+ (int)classMethodOnACategory {
    return 1234;
}
- (int)readProperty {
    int propertyValue = [self prop];
    return propertyValue;
}
@end

int run() {
    ClassWithCategory *obj = [[ClassWithCategory alloc] init];
    int instanceMethodResult = [obj readProperty];
    int classMethodResult = [ClassWithCategory classMethodOnACategory];
    return instanceMethodResult + classMethodResult;
}
