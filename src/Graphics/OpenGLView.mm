#import "OpenGLView.h"


@interface OpenGLView()
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end


@implementation OpenGLView

@synthesize context;


// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}


-(id)initWithFrame:(CGRect)frame
{
    NSLog(@"%s", __FUNCTION__);
    self = [super initWithFrame:frame];
    if (self)
    {
        self.delegate = nil;
     
        self.isBlockViewBelow = YES;
        
        //self.contentScaleFactor = 0.5f;
        
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        //self.layer.frame = CGRectMake(0, 0, 1024, 768);
        
        // HACK, usually we set to opaque = YES, but we need to see the UIImageView background which is under the opengl view
        //eaglLayer.opaque = TRUE;
        //eaglLayer.opaque = NO;
        // REMARK: in order to allow capturing openGL image to UIImage, the kEAGLDrawablePropertyRetainedBacking
        // must be set to YES
        
        
        
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8//kEAGLColorFormatRGBA8
                                        , kEAGLDrawablePropertyColorFormat,
                                        nil];
        
        
        
        
        
        //printf("Running in 16 bits color mode:\n");
        
        EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        context = aContext;
        
        if( ! context)
        {
            NSLog(@"Failed to initialize OpenGLES 2.0 context");
            exit(1);
        }
        else
        {
            NSLog(@"OpenGL ES 2.0 context created sucessfully!\n");
        }
        
        
        [self createFramebuffer];
    }
    
    
    
    return self;

}

- (id)initWithCoder:(NSCoder *)coder
{
 
    
    NSLog(@"%s", __FUNCTION__);
    self = [super initWithCoder:coder];
	if (self)
    {
        self.delegate = nil;
        //self.contentScaleFactor = 0.5f;
        
        
        self.isBlockViewBelow = YES;
        
        
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        //self.layer.frame = CGRectMake(0, 0, 1024, 768);
      
        // HACK, usually we set to opaque = YES, but we need to see the UIImageView background which is under the opengl view
        
        //eaglLayer.opaque = TRUE;
        //eaglLayer.opaque= NO;
        
        // REMARK: in order to allow capturing openGL image to UIImage, the kEAGLDrawablePropertyRetainedBacking
        // must be set to YES
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8//kEAGLColorFormatRGBA8
                                        , kEAGLDrawablePropertyColorFormat,
                                        nil];
        
        
        
        
        
        //printf("Running in 16 bits color mode:\n");
        
        EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        context = aContext;
        
        if( ! context)
        {
            NSLog(@"Failed to initialize OpenGLES 2.0 context");
            exit(1);
        }
        else
        {
            NSLog(@"OpenGL ES 2.0 context created sucessfully!\n");
        }
        
        
         [self createFramebuffer];
    }
    
    
    
    return self;
}


- (void)dealloc
{
    NSLog(@"%s\n", __FUNCTION__);
    
    [self deleteFramebuffer];
    //[context release];
    
   // [super dealloc];
}

- (void)setContext:(EAGLContext *)newContext
{
    if (context != newContext) {
        [self deleteFramebuffer];
        
       // [context release];
       // context = [newContext retain];
        
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)createFramebuffer
{
    NSLog(@"%s\n", __FUNCTION__);
    
    if (context && !defaultFramebuffer) {
        [EAGLContext setCurrentContext:context];

        float scale = [[UIScreen mainScreen] scale];
        self.layer.contentsScale = scale;
        
        // Create default framebuffer object.
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        // Create color render buffer and allocate backing store.
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        
        
        
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
        
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        
        
        
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  depthRenderbuffer);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH24_STENCIL8_OES,
                              framebufferWidth, framebufferHeight);
        
        glViewport(0,0, framebufferWidth, framebufferHeight);
        
        
        printf("FrameBuffer Size:[%d %d]\n", framebufferWidth, framebufferHeight);
        
        
        
    }
}

- (void)deleteFramebuffer
{
    NSLog(@"%s\n", __FUNCTION__);
    if (context) {
        [EAGLContext setCurrentContext:context];
        
        if (defaultFramebuffer) {
            glDeleteFramebuffers(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }
        
        if (colorRenderbuffer) {
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }
    }
}

- (void)setFramebuffer
{
    //NSLog(@"%s\n", __FUNCTION__);
    
    if (context) {
        [EAGLContext setCurrentContext:context];
        
        if (!defaultFramebuffer)
            [self createFramebuffer];
        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        
        
        
        glViewport(0, 0, framebufferWidth, framebufferHeight);
    }
}

- (BOOL)presentFramebuffer
{
    BOOL success = FALSE;
    
    if (context) {
        [EAGLContext setCurrentContext:context];
        
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        
        
        
        success = [context presentRenderbuffer:GL_RENDERBUFFER];
    }
    
    return success;
}

- (void)layoutSubviews
{
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    [self deleteFramebuffer];
}



- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if( _delegate )
    {
        [_delegate touchesBegan:touches withEvent:event];
    }
}


-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if( _delegate )
    {
        [_delegate touchesMoved:touches withEvent:event];
    }
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if( _delegate )
    {
        [_delegate touchesEnded:touches withEvent:event];
    }
}

// Override the pointInside so that we can allow the view below the GL View to get input
-(BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event {
    
    NSLog(@"OpenGL:Block under:%@", self.isBlockViewBelow ? @"YES" : @"NO" );
    
    return self.isBlockViewBelow;
}

@end
