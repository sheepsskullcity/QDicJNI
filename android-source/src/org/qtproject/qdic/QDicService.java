package org.qtproject.qdic;

import android.util.Log;

public class QDicService {

    static {
        try {
            System.loadLibrary("gnustl_shared");
            System.loadLibrary("Qt5Core");
        }
        catch( UnsatisfiedLinkError e ) {
            System.err.println("Native code library failed to load.\n" + e);
        }
            System.loadLibrary("QDic");
    }

    public static native int initController(int a, int b);
    public static native void startContrTask();
    public static native void destroyContr();

    public static void testJNI() {
        Log.i("Qt", "initController returned " + initController(1,2));
    }

}

