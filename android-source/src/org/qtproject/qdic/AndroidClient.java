package org.qtproject.qdic;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;
import android.util.Log;
import android.content.Context;
import android.os.Bundle;
import java.util.Locale;
import java.lang.String;

public class AndroidClient extends QtActivity
{
    public AndroidClient()
    {
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
    }

    @Override
    protected void onDestroy() {
            super.onDestroy();
    }
}
