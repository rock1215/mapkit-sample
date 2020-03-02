#import "UITextField+swizzle.h"
#import "SwizzleStyle.h"

@implementation UITextField (swizzle)
+(void)load {
  [SwizzleStyle swizzle:[self class] methodName:@"didMoveToWindow"];
}
@end
