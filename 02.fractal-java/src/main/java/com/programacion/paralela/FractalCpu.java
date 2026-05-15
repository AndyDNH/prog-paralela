package com.programacion.paralela;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static com.programacion.paralela.FractalParam.*;

public class FractalCpu {


  public static int[] pixelBuffer;

  private int cores;

  private ExecutorService executor;

  public FractalCpu() {
    this.pixelBuffer = new int[WIDTH * HEIGHT];
    this.cores = Runtime.getRuntime().availableProcessors();
    this.executor = Executors.newFixedThreadPool(this.cores);
    System.out.println("FractalCpu inicializado con " + this.cores + " hilos.");
  }

  int acotado_2(double x, double y) {
    int iter = 1;
    double zr = x;
    double zi = y;

    while (iter < FractalParam.maxIteraciones && (zr * zr + zi * zi) < 4.0) {
      double dr = zr * zr - zi * zi + cReal;
      double di = 2.0 * zr * zi + cImag;

      zr = dr;
      zi = di;

      iter++;
    }
    if (iter < FractalParam.maxIteraciones) {
      int index = iter % PALETTE_SIZE;
      return colorRamp[index];
    }
    return 0xFF000000;
  }

  int acotado_3(double x, double y) {
    int iter = 1;
    double zr = x;
    double zi = y;

    while (iter < FractalParam.maxIteraciones && (zr * zr + zi * zi) < 4.0) {
      double dr = zr * zr - zi * zi + cReal;
      double di = 2.0 * zr * zi + cImag;

      zr = dr;
      zi = di;

      iter++;
    }
    if (iter < FractalParam.maxIteraciones) {
      int index = iter % PALETTE_SIZE;
      return colorRamp2[index];
    }
    return 0xFF000000;
  }

  void julia_serial_2(double xMin, double yMin, double xMax, double yMax, int width, int height) {
    double dx = (xMax - xMin) / width;
    double dy = (yMax - yMin) / height;
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        double x = xMin + i * dx;
        double y = yMax - j * dy;
        var color = acotado_2(x, y);
        pixelBuffer[j * width + i] = color;
      }
    }
  }

  void julia_threads_2(double xMin, double yMin, double xMax, double yMax, int width, int height) {
    double dx = (xMax - xMin) / width;
    double dy = (yMax - yMin) / height;

    int chunkSize = height / cores;

    List<Callable<Void>> tasks = new ArrayList<>();

    for (int ceil = 0; ceil < cores; ceil++) {
      final int startY = ceil * chunkSize;
      final int endY = (ceil == cores - 1) ? height : (ceil + 1) * chunkSize;

      tasks.add(() -> {
        for (int j = startY; j < endY; j++) {
          for (int i = 0; i < width; i++) {
            double x = xMin + i * dx;
            double y = yMax - j * dy;
            pixelBuffer[j * width + i] = acotado_3(x, y);
          }
        }
        return null;
      });
    }

    try {
      executor.invokeAll(tasks);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  }

  public void shutdown() {
    if (executor != null) {
      executor.shutdown();
    }
  }
}