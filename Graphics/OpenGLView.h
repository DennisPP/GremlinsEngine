#import <UIKit/UIKit.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>



@protocol MouseEventDelegate <NSObject>

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end


@class EAGLContext;

@interface OpenGLView : UIView
{
    // The pixel dimensions of the CAEAGLLayer.
@public
    GLint framebufferWidth;
    GLint framebufferHeight;
    
    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
    GLuint defaultFramebuffer, colorRenderbuffer;
    GLuint depthRenderbuffer;
}

@property (weak) id<MouseEventDelegate> delegate;


@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) BOOL isBlockViewBelow;

- (void)createFramebuffer;
- (void)setFramebuffer;
- (BOOL)presentFramebuffer;
@end
