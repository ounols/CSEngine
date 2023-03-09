//
//  GameViewController.m
//  CSEngine
//
//  Created by 이진영 on 2023/02/10.
//

#import "GameViewController.h"
#import <Foundation/Foundation.h>
#import "EngineView.h"

@implementation GameViewController
{
    EngineView* _view;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    _view = (EngineView *)self.view;
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    
    [_view exterminate];
}

@end
