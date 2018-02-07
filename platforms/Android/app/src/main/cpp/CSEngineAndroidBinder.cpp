//
// Created by ounol on 2018-02-05.
//

#include <OGLDef.h>
#include <Manager/MainProc.h>
#include <MacroDef.h>
#include <Manager/ResMgr.h>

MainProc* mainProc = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_ounols_csengine_SCEngineBinder_Init(JNIEnv *env, jclass type, jint width,
                                             jint height) {

    // TODO
    mainProc = new MainProc();
    mainProc->Init((int)width, (int)height);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_ounols_csengine_SCEngineBinder_SetAssetManager(JNIEnv *env, jclass type,
                                                        jobject assetManager) {

    // TODO
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    ResMgr::getInstance()->SetAssetManager(mgr);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_ounols_csengine_SCEngineBinder_Update(JNIEnv *env, jclass type, jlong ElapsedTime) {
    mainProc->Update((float)ElapsedTime);
    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_com_ounols_csengine_SCEngineBinder_Render(JNIEnv *env, jclass type, jlong ElapsedTime) {
    mainProc->Render((float)ElapsedTime);
    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_com_ounols_csengine_SCEngineBinder_Exterminate(JNIEnv *env, jclass type) {
    SAFE_DELETE(mainProc);
    // TODO


}extern "C"
JNIEXPORT void JNICALL
Java_com_ounols_csengine_SCEngineBinder_ResizeWindow(JNIEnv *env, jclass type, jint width,
                                                     jint height) {

    // TODO
    mainProc->ResizeWindow((int)width, (int)height);

}