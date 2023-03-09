#import "EngineView.h"

#include "../../../src/Manager/MainProc.h"

#import <Foundation/Foundation.h>
#include <OpenGLES/ES3/gl.h>

using namespace CSE;

@interface EngineView()
- (void)setupLayer;
- (void)setupContext;
- (void)setupBuffers;
- (void)destoryBuffers;
@end

@implementation EngineView
{
    MainProc* _mainProc;
    NSTimeInterval _startEngineTime;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        [self setupLayer];
        [self setupContext];
        [self setupBuffers];

        _mainProc = new MainProc();
        _mainProc->Init(self.bounds.size.height, self.bounds.size.width);
        
        _startEngineTime = [NSDate timeIntervalSinceReferenceDate];
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        _running = YES;
    }

    return self;
}

- (void)layoutSubviews
{
    [EAGLContext setCurrentContext:_context];
    [self destoryBuffers];
    [self setupBuffers];

//    _mainProc->ResizeWindow(self.bounds.size.width, self.bounds.size.height);
}

- (void)drawFrame {
    if (_running) {
        float elapsedTime = ([NSDate timeIntervalSinceReferenceDate] - _startEngineTime) * 1000;
        _mainProc->Update(elapsedTime);
        _mainProc->Render(elapsedTime);
    }
    
    [_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)stopDisplayLink {
    _running = NO;
    [_displayLink invalidate];
}

- (void)exterminate {
    [self stopDisplayLink];
    
    _mainProc->Exterminate();
    delete _mainProc;
}

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void)setupLayer
{
    _eaglLayer = (CAEAGLLayer*) self.layer;
    _eaglLayer.opaque = YES;
    _eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
}

- (void)setupContext {
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES3;
    _context = [[EAGLContext alloc] initWithAPI:api];
    if (!_context) {
        NSLog(@" >> Error: Failed to initialize OpenGLES 3.0 context");
        exit(1);
    }
    
    if (![EAGLContext setCurrentContext:_context]) {
        _context = nil;
        NSLog(@" >> Error: Failed to set current OpenGL context");
        exit(1);
    }
}

- (void)setupBuffers {
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];

    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, _colorRenderBuffer);
}

- (void)destoryBuffers
{
    glDeleteRenderbuffers(1, &_colorRenderBuffer);
    _colorRenderBuffer = 0;

    glDeleteFramebuffers(1, &_frameBuffer);
    _frameBuffer = 0;
}

@end
