/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from "Flocking" in "The Nature of Code" by Daniel Shiffman: http://natureofcode.com/
 * Copyright (c) 2014 Daniel Shiffman
 * http://www.shiffman.net
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

// Flocking
// Daniel Shiffman <http://www.shiffman.net>
// The Nature of Code, Spring 2009

// Demonstration of Craig Reynolds' "Flocking" behavior
// See: http://www.red3d.com/cwr/
// Rules: Cohesion, Separation, Alignment

#ifndef PatternFlock_H
#define PatternFlock_H

class PatternFlock : public Drawable {
  public:
    PatternFlock() {
      name = (char *)"Flock";
      id = "L";
      enabled = true;
    }

    static const int boidCount = 10;
    Boid predator;

    PVector wind;
    byte hue = 0;
    bool predatorPresent = true;

    // ------------------------ START ------------------------
    void start(uint8_t _pattern) {
      for (int i = 0; i < boidCount; i++) {
        staticBoids[_pattern][i] = Boid(15, 15);
        staticBoids[_pattern][i].maxspeed = 0.380;
        staticBoids[_pattern][i].maxforce = 0.015;
      }

      predatorPresent = random(0, 2) >= 1;
      if (predatorPresent) {
        predator = Boid(31, 31);
        predatorPresent = true;
        predator.maxspeed = 0.385;
        predator.maxforce = 0.020;
        predator.neighbordist = 16.0;
        predator.desiredseparation = 0.0;
      }
    }

    // -------------------------- DRAW FRAME -------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
      effects.DimAll(250);  // WAS 230
      
      
      
      //////effects.ShowFrame();



      bool applyWind = random(0, 255) > 250;
      if (applyWind) {
        wind.x = Boid::randomf() * .015;
        wind.y = Boid::randomf() * .015;
      }

      CRGB color = effects.ColorFromCurrentPalette(hue);

      for (int i = 0; i < boidCount; i++) {
        Boid * boid = &staticBoids[_pattern][i];

        if (predatorPresent) {
          // flee from predator
          boid->repelForce(predator.location, 10);
        }

        boid->run(staticBoids[_pattern], boidCount);
        boid->wrapAroundBorders();
        PVector location = boid->location;
        // PVector velocity = boid->velocity;
        // backgroundLayer.drawLine(location.x, location.y, location.x - velocity.x, location.y - velocity.y, color);
        // effects.leds[XY(location.x, location.y)] += color;
        effects.drawBackgroundFastLEDPixelCRGB(location.x, location.y, color);

        // test draw canvas
        //effects.ApplyCanvasH(location.x - 8, location.y - 8, 1);

        if (applyWind) {
          boid->applyForce(wind);
          applyWind = false;
        }
      }

      if (predatorPresent) {
        predator.run(staticBoids[_pattern], boidCount);
        predator.wrapAroundBorders();
        color = effects.ColorFromCurrentPalette(hue + 128);
        PVector location = predator.location;
        // PVector velocity = predator.velocity;
        // backgroundLayer.drawLine(location.x, location.y, location.x - velocity.x, location.y - velocity.y, color);
        // effects.leds[XY(location.x, location.y)] += color;        
        effects.drawBackgroundFastLEDPixelCRGB(location.x, location.y, color);
      }

      EVERY_N_MILLIS(200) {
        hue++;
      }
      
      EVERY_N_SECONDS(30) {
        predatorPresent = !predatorPresent;
      }

      return 200;
    }
};

#endif
