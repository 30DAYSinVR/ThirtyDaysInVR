// IPermission.aidl
package com.htc.packageinstallerforvr.permission;

import com.htc.packageinstallerforvr.permission.IPermissionCallback;

// Declare any non-default types here with import statements

interface IPermission {
    void requestPermissionsWithPackage(String callingPackageName, in String[] permissionName, IPermissionCallback cb);
    boolean isPermissionGranted(String callingPackageName, String permission);
    boolean shouldGrantPermission(String callingPackageName, String permission);
}
