package com.polysfactory.n3.demo;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.Context;

public class IOUtils {
    public static File copy(Context context, int res, String dirname, String filename) {
        InputStream is = context.getResources().openRawResource(res);
        File cascadeDir = context.getDir(dirname, Context.MODE_PRIVATE);
        File file = new File(cascadeDir, filename);
        FileOutputStream os = null;
        try {
            os = new FileOutputStream(file);
            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                is.close();
                os.close();
            } catch (IOException e) {
                // ignore
            }
        }
        return file;
    }
}
