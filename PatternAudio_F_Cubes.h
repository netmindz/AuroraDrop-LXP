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

    const static uint8_t cubeCount = 4;

    class AudioCubeObject {
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
      enabled = false;
    }

    AudioCubeObject audioCube[cubeCount];


    // ------------------ start -------------------
    void start(uint8_t _pattern) {

    for (int c=0; c<cubeCount; c++) {
      //cube[c].make(cubeWidth);   
      audioCube[c].make(random8(22,60));  

      // randomise stuff
      //AngxSpeed = 0.05; 
      //AngySpeed = 0.05; 
      audioCube[c].Angx = random8(10,21);
      audioCube[c].Angy = random8(10,21);
      audioCube[c].hue = random8();
    }


    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        uint8_t blurAmount = beatsin8(2, 10, 255);

#if FASTLED_VERSION >= 3001000
      blur2d(effects.leds, MATRIX_WIDTH, MATRIX_HEIGHT, blurAmount);
#else
      effects.DimAll(blurAmount); effects.ShowFrame();
#endif

    for (int c=0; c<cubeCount; c++) {
      audioCube[c].zCamera = beatsin8(2, 100, 140);
      audioCube[c].AngxSpeed = beatsin8(3, 1, 10) / 100.0f;
      audioCube[c].AngySpeed = beatcos8(5, 1, 10) / 100.0f;

      // Update values
      audioCube[c].Angx += audioCube[c].AngxSpeed;
      audioCube[c].Angy += audioCube[c].AngySpeed;
      if (audioCube[c].Angx >= TWO_PI)
        audioCube[c].Angx -= TWO_PI;
      if (audioCube[c].Angy >= TWO_PI)
        audioCube[c].Angy -= TWO_PI;

      audioCube[c].rotate(audioCube[c].Angx, audioCube[c].Angy);

      // Draw cube
      int i;

      CRGB color = effects.ColorFromCurrentPalette(audioCube[c].hue, 128);

      uint8_t backgroundBrightness = 128;
      uint8_t foregroundBrightness = 255;

      // Backface
      EdgePoint *e;
      for (i = 0; i < 12; i++)
      {
        e = audioCube[c].edge + i;
        if (!e->visible) {
          effects.BresLine(audioCube[c].screen[e->x].x, audioCube[c].screen[e->x].y, audioCube[c].screen[e->y].x, audioCube[c].screen[e->y].y, audioCube[c].hue, backgroundBrightness);
          //effects. matrix.drawLine(screen[e->x].x, screen[e->x].y, screen[e->y].x, screen[e->y].y, color);
        }
      }

      color = effects.ColorFromCurrentPalette(audioCube[c].hue, 255);

      // Frontface
      for (i = 0; i < 12; i++)
      {
        e = audioCube[c].edge + i;
        if (e->visible)
        {
          effects.BresLine(audioCube[c].screen[e->x].x, audioCube[c].screen[e->x].y, audioCube[c].screen[e->y].x, audioCube[c].screen[e->y].y, audioCube[c].hue, foregroundBrightness);
          //matrix.drawLine(screen[e->x].x, screen[e->x].y, screen[e->y].x, screen[e->y].y, color);
        }
      }

      // step on
      audioCube[c].step++;
      if (audioCube[c].step == 8) {
        audioCube[c].step = 0;
        audioCube[c].hue++;
      }


    }







      //effects.ShowFrame();

      //return 20;

      return 0;
    }


};

#endif
