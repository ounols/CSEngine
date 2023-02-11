//
//  GameViewController.h
//  CSEngine
//
//  Created by 이진영 on 2023/02/10.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

// Our iOS view controller
@interface GameViewController : UIViewController {
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    
    GLuint _colorRenderBuffer;
    GLuint _frameBuffer;
    
    CADisplayLink* _displayLink;
    BOOL _running;
}

@end
