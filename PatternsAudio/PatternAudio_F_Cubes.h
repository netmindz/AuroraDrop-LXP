/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from Noel Bundy's work: https://github.com/TwystNeko/Object3d
 * Copyright (c) 2014 Noel Bundy
 *
 * Portions of this code are adapted from the Petty library: https://code.google.com/p/peggy/
 * Copyright (c) 2008 Windell H Oskay.  All right reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PatternAudioCubes_H
#define PatternAudioCubes_H

class PatternAudioCubes : public Drawable {
  private:

    const static uint8_t cubeCount = 2;  // 4 is good

    class CubeObject {
      public:  
        float focal = 30; // Focal of the camera
        int cubeWidth = 56; // Cube size, originaly 28
        float Angx = 20.0, AngxSpeed = 0.05; // rotation (angle+speed) around X-axis
        float Angy = 10.0, AngySpeed = 0.05; // rotation (angle+speed) around Y-axis
        float Ox = ((MATRIX_WIDTH-1)/2), Oy = ((MATRIX_HEIGHT-1)/2); // position (x,y) of the frame center
        int zCamera = 110; // distance from cube to the eye of the camera

        // Local vertices
        Vertex  local[8];
        // Camera aligned vertices
        Vertex  aligned[8];
        // On-screen projected vertices
        Point   screen[8];
        // Faces
        squareFace face[6];
        // Edges
        EdgePoint edge[12];
        int nbEdges;
        // ModelView matrix
        float m00, m01, m02, m10, m11, m12, m20, m21, m22;

        // individualy defined now
        byte hue = 0;
        int step = 0;


    // constructs the cube
    void make(int w)
    {
      nbEdges = 0;

      local[0].set(-w, w, w);
      local[1].set(w, w, w);
      local[2].set(w, -w, w);
      local[3].set(-w, -w, w);
      local[4].set(-w, w, -w);
      local[5].set(w, w, -w);
      local[6].set(w, -w, -w);
      local[7].set(-w, -w, -w);

      face[0].set(1, 0, 3, 2);
      face[1].set(0, 4, 7, 3);
      face[2].set(4, 0, 1, 5);
      face[3].set(4, 5, 6, 7);
      face[4].set(1, 2, 6, 5);
      face[5].set(2, 3, 7, 6);

      int f, i;
      for (f = 0; f < 6; f++)
      {
        for (i = 0; i < face[f].length; i++)
        {
          face[f].ed[i] = this->findEdge(face[f].sommets[i], face[f].sommets[i ? i - 1 : face[f].length - 1]);
        }
      }
    }

    // finds edges from faces
    int findEdge(int a, int b)
    {
      int i;
      for (i = 0; i < nbEdges; i++)
        if ((edge[i].x == a && edge[i].y == b) || (edge[i].x == b && edge[i].y == a))
          return i;
      edge[nbEdges++].set(a, b);
      return i;
    }

    // rotates according to angle x&y
    void rotate(float angx, float angy)
    {
      int i;
      float cx = cos(angx);
      float sx = sin(angx);
      float cy = cos(angy);
      float sy = sin(angy);

      m00 = cy;
      m01 = 0;
      m02 = -sy;
      m10 = sx * sy;
      m11 = cx;
      m12 = sx * cy;
      m20 = cx * sy;
      m21 = -sx;
      m22 = cx * cy;

      for (i = 0; i < 8; i++)
      {
        aligned[i].x = m00 * local[i].x + m01 * local[i].y + m02 * local[i].z;
        aligned[i].y = m10 * local[i].x + m11 * local[i].y + m12 * local[i].z;
        aligned[i].z = m20 * local[i].x + m21 * local[i].y + m22 * local[i].z + zCamera;

        screen[i].x = floor((Ox + focal * aligned[i].x / aligned[i].z));
        screen[i].y = floor((Oy - focal * aligned[i].y / aligned[i].z));
      }

      for (i = 0; i < 12; i++)
        edge[i].visible = false;

      Point *pa, *pb, *pc;
      for (i = 0; i < 6; i++)
      {
        pa = screen + face[i].sommets[0];
        pb = screen + face[i].sommets[1];
        pc = screen + face[i].sommets[2];

        boolean back = ((pb->x - pa->x) * (pc->y - pa->y) - (pb->y - pa->y) * (pc->x - pa->x)) < 0;
        if (!back)
        {
          int j;
          for (j = 0; j < 4; j++)
          {
            edge[face[i].ed[j]].visible = true;
          }
        }
      }
    }
      private:  

    };


  public:
    PatternAudioCubes() {
      name = (char *)"Cubes";
      id = "C";
      enabled = true;

      //cube = (CubeObject*)malloc(cubeCount * sizeof(CubeObject));
      //CubeObject** cube = new CubeObject*[cubeCount];

    }

    CubeObject cube[cubeCount];
    //CubeObject *cube;


    // ------------------ start -------------------
    void start(uint8_t _pattern) {


    for (int c=0; c<cubeCount; c++) {
      //cube[c].make(cubeWidth);   
      cube[c].make(random8(60,80));    // cube[c].make(random8(22,60)); 

      // randomise stuff
      //AngxSpeed = 0.05; 
      //AngySpeed = 0.05; 
      cube[c].Angx = random8(10,21);
      cube[c].Angy = random8(10,21);
      cube[c].hue = random8();

      //canvasH = (CRGB *)malloc(NUM_LEDS * sizeof(CRGB) / 4);
    }


    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

// leave blurring to effects


        uint8_t blurAmount = beatsin8(2, 10, 255);

#if FASTLED_VERSION >= 3001000
      blur2d(effects.leds, MATRIX_WIDTH, MATRIX_HEIGHT, blurAmount);
#else
      effects.DimAll(blurAmount); effects.ShowFrame();
#endif



    for (int c=0; c<cubeCount; c++) {

      cube[c].zCamera = beatsin8(2, 100, 140);
      cube[c].AngxSpeed = beatsin8(3, 1, 10) / 100.0f;
      cube[c].AngySpeed = beatcos8(5, 1, 10) / 100.0f;

      // AuroraDrop
      byte audio = fftData.specData8[c];
      if (audio > 127) audio = 127;
      cube[c].zCamera = 255 - audio;


      // Update values
      cube[c].Angx += cube[c].AngxSpeed;
      cube[c].Angy += cube[c].AngySpeed;
      if (cube[c].Angx >= TWO_PI)
        cube[c].Angx -= TWO_PI;
      if (cube[c].Angy >= TWO_PI)
        cube[c].Angy -= TWO_PI;

      cube[c].rotate(cube[c].Angx, cube[c].Angy);

      // Draw cube
      if (!fftData.noAudio) {      
  
        int i;

        CRGB color = effects.ColorFromCurrentPalette(cube[c].hue, 128);

        uint8_t backgroundBrightness = 128;
        uint8_t foregroundBrightness = 255;

        effects.ClearCanvas(1);

        // Backface
        EdgePoint *e;
        for (i = 0; i < 12; i++)
        {
          e = cube[c].edge + i;
          if (!e->visible) {
            effects.BresLine(cube[c].screen[e->x].x, cube[c].screen[e->x].y, cube[c].screen[e->y].x, cube[c].screen[e->y].y, cube[c].hue, backgroundBrightness);
            effects.BresLineCanvasH(effects.canvasH, cube[c].screen[e->x].x/2, cube[c].screen[e->x].y/2, cube[c].screen[e->y].x/2, cube[c].screen[e->y].y/2, cube[c].hue, backgroundBrightness / 2);
            //effects. matrix.drawLine(screen[e->x].x, screen[e->x].y, screen[e->y].x, screen[e->y].y, color);
          }
        }

        color = effects.ColorFromCurrentPalette(cube[c].hue, 255);


        // Frontface
        for (i = 0; i < 12; i++)
        {
          e = cube[c].edge + i;
          if (e->visible)
          {
            effects.BresLine(cube[c].screen[e->x].x, cube[c].screen[e->x].y, cube[c].screen[e->y].x, cube[c].screen[e->y].y, cube[c].hue, foregroundBrightness);
            effects.BresLineCanvasH(effects.canvasH, cube[c].screen[e->x].x/2, cube[c].screen[e->x].y/2, cube[c].screen[e->y].x/2, cube[c].screen[e->y].y/2, cube[c].hue, foregroundBrightness / 2);
            //matrix.drawLine(screen[e->x].x, screen[e->x].y, screen[e->y].x, screen[e->y].y, color);
          }
        }


        effects.ApplyCanvasH(effects.canvasH, 0, 0);
        effects.ApplyCanvasH(effects.canvasH, MATRIX_CENTER_X, 0);        // mirror/flip these?
        effects.ApplyCanvasH(effects.canvasH, 0, MATRIX_CENTER_Y);
        effects.ApplyCanvasH(effects.canvasH, MATRIX_CENTER_X, MATRIX_CENTER_Y);

      }



      // step on
      cube[c].step++;
      if (cube[c].step == 8) {
        cube[c].step = 0;
        cube[c].hue++;
      }


    }







      //effects.ShowFrame();

      //return 20;

      return 0;
    }


};

#endif
