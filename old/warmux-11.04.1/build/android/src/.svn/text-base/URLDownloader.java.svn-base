package org.warmux;

import java.io.*;

import org.apache.http.client.methods.*;
import org.apache.http.*;
import org.apache.http.impl.*;
import org.apache.http.impl.client.*;

class URLDownloader
{
  private byte ContentError[];

  public URLDownloader()
  {
    nativeInitCallbacks();
    ContentError = new byte[0];
  }

  public byte[] FetchURL(String URL)
  {
    HttpGet request = new HttpGet(URL);
    request.addHeader("Accept", "*/*");

    HttpResponse response = null;
    try {
      DefaultHttpClient client = new DefaultHttpClient();
      client.getParams().setBooleanParameter("http.protocol.handle-redirects", true);
      response = client.execute(request);
    } catch (IOException e) {
      System.out.println("Failed to connect to " + URL);
      return ContentError;
    }

    if (response != null)  {
      if (response.getStatusLine().getStatusCode() != 200) {
        System.out.println("Failed to connect to " + URL);
        return ContentError;
      }
    }

    long full_len   = response.getEntity().getContentLength();
    if (full_len < 1) {
      System.out.println("Content from " + URL + " too small");
      return ContentError;
    }

    ByteArrayOutputStream outStream = new ByteArrayOutputStream();
    try {
      response.getEntity().writeTo(outStream);
      return outStream.toByteArray();
    } catch (IOException e) {
      System.out.println("Failed reading from " + URL);
      return ContentError;
    }
  }

  private native void nativeInitCallbacks();
}
