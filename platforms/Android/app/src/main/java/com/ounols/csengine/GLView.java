package com.ounols.csengine;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;

/**
 * Created by ounol on 2018-02-04.
 */

public class GLView extends GLSurfaceView {
    public static final boolean DEBUG = true;

    public GLView(Context context) {
        super(context);
        init(false, 10, 0);

    }

    @Override
    public void onPause() {
        super.onPause();
    }



    @Override
    public void onResume() {
        super.onResume();
    }


    public void onDestroy() {
        SCEngineBinder.Exterminate();
    }


    private void init(boolean translucent, int depth, int stencil) {

        /* GLSurfaceView()는 생성될 때 기본적으로 RGB_565의 알파값이 없는 surface를 만듭니다.
         * 만약 알파값이 존재하는 surface를 설정하려면 surface의 포맷을
         * PixelFormat.TRANSLUCENT 라고 설정하면 됩니다.
         * PixelFormat.TRANSLUCENT 는 SurfaceFlinger 에 의해 32비트 알파값이 적용된 surface로
         * 해석을 하게 됩니다.
         */
        if (translucent) {
            this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        }

        // Context Factory를 2.0버전으로 렌더링 하기 위한 환경을 설정합니다.
        // 자세한 내용은 GLContextFactory 클래스가 정의된 곳을 보시면 됩니다.
        setEGLContextFactory(new GLContextFactory());


        /* EGLConfig를 이 surface와 포맷을 확실하게 맞춰줘야하기 때문에 설정을 해야합니다.
         * 새로 정의한 Config Chooser를 적용합니다.
         * 자세한 내용은 GLConfigChooser 클래스가 정의된 곳을 보시면 됩니다.
         */
        setEGLConfigChooser(translucent ?
                new GLConfigChooser(8, 8, 8, 8, depth, stencil) :
                new GLConfigChooser(5, 6, 5, 0, depth, stencil));




        // 매 프레임 마다 렌더링을 담당하는 Renderer를 설정합니다.
        GLRenderer renderer = new GLRenderer();
        renderer.SetAssets(getContext().getAssets());
        setRenderer(renderer);

        getHeight();


    }
}
