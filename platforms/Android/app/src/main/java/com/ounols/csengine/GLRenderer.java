package com.ounols.csengine;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by ounol on 2018-02-04.
 */

public class GLRenderer implements GLSurfaceView.Renderer {

    long StartTime;
    AssetManager assets;


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        //surface가 생성될 때 호출되는 콜백함수 입니다.
        StartTime = System.currentTimeMillis();

        SCEngineBinder.SetAssetManager(assets);
        SCEngineBinder.Init(0, 0);


    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        //surface가 변경될 때 호출되는 콜백함수 입니다.
        SCEngineBinder.ResizeWindow(width, height);
    }



    @Override
    public void onDrawFrame(GL10 gl) {

        /**
         * 여기서 계속 그립니다.
         */
        long elapsedTime = System.currentTimeMillis() - StartTime;

        SCEngineBinder.Update(elapsedTime);
        SCEngineBinder.Render(elapsedTime);

    }


    public void SetAssets(AssetManager assets){
        this.assets = assets;
    }


}
