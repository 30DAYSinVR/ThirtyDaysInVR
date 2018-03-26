// ++ LICENSE-HIDDEN SOURCE ++
package com.htc.vr.unreal;

/**
 * Created by devinyu on 2017/9/28.
 */

import android.app.Service;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.content.Intent;

import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;
import android.os.RemoteException;

import com.htc.packageinstallerforvr.permission.IPermissionCallback;
import com.htc.packageinstallerforvr.permission.IPermission;

import static android.content.pm.PackageManager.PERMISSION_GRANTED;

public class PermissionManager {
    private static Context mContext = null;
    private static final String TAG = "Java_PermissionMgr";
    private int connectState = 0;   // 0=not start yet, 1 = connecting, 2 = connected
    private static PermissionManager mInstance = null;
    private static String mPackageName;
    private String[] mRequestString = null;
    private boolean[] mResultArray = null;

    private IPermission PermissionService = null;

    private final int CONNECT_NOT_START = 0;
    private final int CONNECTING = 1;
    private final int CONNECTED = 2;
    private boolean isPermissionServiceExist = false;
    private boolean mShowOnVRScene = true;
    private boolean isInProcess = false;
    private boolean isOnPause = false;
	
	public native void initNative();
	public native void requestCallbackNative(String[] requestNames, boolean[] result);

    private Runnable requestPermissionRunnable = new Runnable() {
        @Override
        public void run() {
            if (isPermissionServiceExist == true) {
                if (connectState == CONNECTED && PermissionService != null) {
                    requestPermission();
                } else {
                    Log.d(TAG, "PermissionService is not ready, wait 0.05 sec");
                    handler.postDelayed(requestPermissionRunnable, 50);
                }
            } else {
                // TODO:: request from phone
                Log.w(TAG, "Do not show permission dialog on scene!");
            }
        }
    };
    private Handler handler = new Handler(Looper.getMainLooper());

    public PermissionManager() {
        Log.i(TAG, "constructor and initNative");
		initNative();
    }

    public static PermissionManager getInstance() {
        if (mInstance == null) {
            mInstance = new PermissionManager();
        }

        return mInstance;
    }

    IPermissionCallback cb = new IPermissionCallback.Stub() {
        public void updatePermissionResult(String[] requestNames, int[] result) {
            //Log.i(TAG, "LOCKER ++");
            synchronized (PermissionManager.class) {
                Log.d(TAG, "callback permission count = " + requestNames.length + " result count = " + result.length);

                if (isInProcess) {
                    mResultArray = new boolean[result.length];
                    for (int i = 0; i < result.length; i++) {
                        Log.i(TAG, "Permission[" + i + "] = " + requestNames[i] + " result[" + i + "] = " + result[i]);
                        mResultArray[i] = (result[i] == PERMISSION_GRANTED);
                    }
                    requestPermissionCompleted();
                }
            }
            //Log.i(TAG, "LOCKER --");
        }
    };

    private ServiceConnection checkConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder boundService) {
            Log.d(TAG, "PermissionService is exist");
            mContext.unbindService(checkConnection);
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.d(TAG, "checkConnection onServiceDisconnected");
        }
    };

    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder boundService) {
            PermissionService = IPermission.Stub.asInterface(boundService);
            connectState = CONNECTED;
            Log.d(TAG, "PermissionService connected");
        }

        public void onServiceDisconnected(ComponentName name) {
            PermissionService = null;
            connectState = CONNECT_NOT_START;
            Log.d(TAG, "onServiceDisconnected");
        }
    };

    private void checkServerConnection() {
        Log.i(TAG, "checkServerConnection");
        if (mContext != null) {
            Intent intent = new Intent("com.htc.packageinstallerforvr.permission.PermissionService");
            intent.setPackage("com.htc.packageinstallerforvr");
            try {
                isPermissionServiceExist = mContext.bindService(intent, checkConnection, Service.BIND_AUTO_CREATE);
            } catch (SecurityException e) {
                isPermissionServiceExist = false;
            }

            if (!isPermissionServiceExist) {
                Log.e(TAG, "checkServerConnection fail, PermissionService might haven't been installed");
                mContext.unbindService(checkConnection);
            }
        }
    }

    public void setContext(Context ctx) {
        mContext = ctx;
        checkServerConnection();
    }

    public void setPackageName(String pn) {
        mPackageName = pn;
        Log.i(TAG, "Package name = " + mPackageName);
    }

    public void onPauseAndReturnFail() {
        Log.i(TAG, "onPause");
        //Log.i(TAG, "LOCKER ++");
        synchronized (PermissionManager.class) {
            isOnPause = true;
            if (isInProcess) {
                Log.i(TAG, "onPause and request still in process");
                mResultArray = new boolean[mRequestString.length];
                for (int i = 0; i < mRequestString.length; i++) {
                    mResultArray[i] = isPermissionGranted(mRequestString[i]);
                    Log.i(TAG, "Permission[" + i + "] = " + mRequestString[i] + " result[" + i + "] = " + mResultArray[i]);
                }

                requestPermissionCompleted();
            }
        }
        //Log.i(TAG, "LOCKER --");
    }

    public void onResume() {
        Log.i(TAG, "onResume");
        //Log.i(TAG, "LOCKER ++");
        synchronized (PermissionManager.class) {
            isOnPause = false;
        }
        //Log.i(TAG, "LOCKER --");
    }

    private void connectPermissionService() {
        Log.i(TAG, "start to bind PermissionService ");
        connectState = CONNECT_NOT_START;
        if (mContext == null) {
            Log.i(TAG, "mContext is null ");
            return ;
        }

        Intent intent = new Intent("com.htc.packageinstallerforvr.permission.PermissionService");
        intent.setPackage("com.htc.packageinstallerforvr");
        try {
            isPermissionServiceExist = mContext.bindService(intent, mConnection, Service.BIND_AUTO_CREATE);
        } catch (SecurityException e) {
            isPermissionServiceExist = false;
            connectState = CONNECT_NOT_START;
            Log.e(TAG, "bind PermissionService fail due to SecurityException");
        }

        if (isPermissionServiceExist) {
            connectState = CONNECTING;
        } else {
            Log.e(TAG, "bind PermissionService fail");
            mContext.unbindService(mConnection);
        }
    }

    private void requestPermission() {
        if ((connectState != CONNECTED) || (PermissionService == null))
            return;

        try {
            PermissionService.requestPermissionsWithPackage(mPackageName, mRequestString, cb);
        } catch (RemoteException e) {
            Log.e(TAG, "remote exception when call requestPermission");
        }
    }

    public boolean requestPermissions(String[] requestPermissions) {
        //Log.i(TAG, "LOCKER ++");
        synchronized (PermissionManager.class) {
            if (!isPermissionServiceExist) {
                Log.e(TAG, "[SKIP] Permission Service is not initialized.");
                return false;
            }

            if (isOnPause) {
                Log.e(TAG, "[SKIP] Activity is going to onPause.");
                return false;
            }

            if (isInProcess) {
                Log.e(TAG, "[SKIP] Permission Manager is requesting.");
                return false;
            }
            isInProcess = true;
            connectPermissionService();

            int requestCount = requestPermissions.length;
            mRequestString = new String[requestCount];

            for (int i = 0; i < requestCount; i++) {
                mRequestString[i] = requestPermissions[i];
            }
            Log.i(TAG, "requestPermissions, length = " + mRequestString.length);
        }
        //Log.i(TAG, "LOCKER --");
        handler.postDelayed(requestPermissionRunnable, 50);
		return true;
    }

    public boolean isPermissionGranted(String permission) {
        return (mContext.checkSelfPermission(permission) == PackageManager.PERMISSION_GRANTED);
    }

    public boolean shouldGrantPermission(String permission) {
        return (mContext.checkSelfPermission(permission) != PackageManager.PERMISSION_GRANTED);
    }

    public boolean showDialogOnVRScene() {
        return mShowOnVRScene;
    }

    public boolean isInitialized() {
        if (mContext == null) {
            Log.i(TAG, "mContext is null, isInitialized return false");
            return false;
        }

        if (isInProcess)
            return false;

        if (isOnPause)
            return false;

        if (!isPermissionServiceExist) {
            checkServerConnection();
        }

        return isPermissionServiceExist;
    }

    public void requestPermissionCompleted() {
        requestCallbackNative(mRequestString, mResultArray);
        connectState = CONNECT_NOT_START;
        mContext.unbindService(mConnection);

        isInProcess = false;
    }
}
