package com.ounols.csengine;

import android.content.res.AssetManager;

/**
 * Created by ounol on 2018-02-05.
 */

public class SCEngineBinder {

    static{
        System.loadLibrary("CSEngineLib");
    }

    public static native void Init(int width, int height);
    public static native void SetAssetManager(AssetManager assetManager);
    public static native void ResizeWindow(int width, int height);
    public static native void Update(long ElapsedTime);
    public static native void Render(long ElapsedTime);
    public static native void Exterminate();

}
