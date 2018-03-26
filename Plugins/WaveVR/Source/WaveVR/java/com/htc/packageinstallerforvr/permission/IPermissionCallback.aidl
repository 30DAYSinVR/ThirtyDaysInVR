// IPermissionCallback.aidl
package com.htc.packageinstallerforvr.permission;

// Declare any non-default types here with import statements

interface IPermissionCallback {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void updatePermissionResult(in String[] requestNames, in int[] result);
}
