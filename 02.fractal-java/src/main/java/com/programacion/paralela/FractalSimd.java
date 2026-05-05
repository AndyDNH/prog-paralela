package com.programacion.paralela;

import java.nio.ByteBuffer;
import static com.programacion.paralela.FractalParam.*;

import static com.programacion.paralela.FractalParam.HEIGHT;
import static com.programacion.paralela.FractalParam.WIDTH;

public class FractalSimd {
  ByteBuffer pixelBuffer;

  public FractalSimd() {
    this.pixelBuffer = ByteBuffer.allocate(WIDTH*HEIGHT*4);
  }

  public void juliaSimd(){
    FractalDll.INSTANCE.julia_simd(xMin,yMin,xMax,yMax,WIDTH,HEIGHT,maxIteraciones,pixelBuffer);
  }
}
