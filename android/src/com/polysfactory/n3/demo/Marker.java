package com.polysfactory.n3.demo;

import java.io.File;

import android.content.Context;

public enum Marker {
    RED("marker_red.jpg"), BLUE("marker_blue.jpg");

    String filename;

    Marker(String file) {
        filename = file;
    }

    public File getFile(Context context) {
        return IOUtils.getFilePath(context, Constants.MARKER_FILE_DIR, filename);
    }

    public String getFilePath(Context context) {
        return this.getFile(context).getAbsolutePath();
    }
}
