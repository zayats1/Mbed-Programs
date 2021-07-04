#include "ILI9486.h"
#include "map_constrain.h"
#include "mbed.h"
#include <vector>
const int DisplayResolution_x = 320;
const int DisplayResolution_y = 480;

PinName buspins[8] = {D8, D9, D2, D3, D4, D5, D6, D7};
ILI9486 TFTLCD(BUS_8, buspins, A3, A4, A2, A1, A0, "TFTLCD");

DigitalIn UpButton(PC_2, PullDown), DownButton(PC_3, PullDown),
    LeftButton(PH_0, PullDown), RightButton(PH_1, PullDown);

unsigned short BackgroundColor = Cyan;
unsigned short SnakeColor = Yellow;

enum SnakeHeadMovingDirections { Up, Down, Left, Right };

class GameObject {
public:
  virtual void appear(int _x, int _y) {}

  virtual void move(int next_x, int next_y) {}

  virtual int getScore() { return score; }

  virtual int getPositionX() { return x; }

  virtual int getPositionY() { return y; }

protected:
  int x = 0;
  int y = 0;
  int score = 0;
  int size = 0;
  unsigned short color = 0; // Dislpay's lib color
};

class Snake : public GameObject {
public:
  Snake(int _color, int _body_segment_size, unsigned short _background_colour,
        int _speed) {
    color = _color;
    background_colour = _background_colour;
    Segment seg;
    for (int n = 0; n <= segments_quantity; n++) {
      segments.push_back(seg);
    }
  }
  void appear(int _x, int _y) override { // To reworks
    x = _x;
    y = _y;
    int temp_draw_x = x;
    for (int n = 0; n < segments_quantity; n++) {
      temp_draw_x -= segments[n].getSegmentSize();
      segments[n].draw_segment(temp_draw_x, y, color, background_colour);
    }
  }

  void move(int direction_x, int direction_y) override { // in process
    int seg_size = segments[0].getSegmentSize();
    // checking for crocing borders
    if (x >= DisplayResolution_x) {
      x = 0;
    } else if (x < 0) {
      x = DisplayResolution_x;
    }
    if (y >= DisplayResolution_y) {
      y = 0;
    } else if (y < 0) {
      y = DisplayResolution_y;
    }
    int size = segments[0].getSegmentSize();

    x += direction_x * seg_size;
    y += direction_y * seg_size;

    segments[0].draw_segment(x, y, color, background_colour);

    for (int segment_number = 1; segment_number < segments_quantity;
         segment_number++) {
      int previous_segment =
          previous_element(segment_number, segments_quantity);
      segments[segment_number].draw_segment(
          segments[previous_segment].getPreviousPositionX(), // bug
          segments[previous_segment].getPreviousPositionY(), color,
          background_colour);
    }
    thread_sleep_for(int((1 / speed) * 1000));
  }

protected:
  class Segment : public GameObject {
  public:
    Segment() {}
    void draw_segment(int _x, int _y, unsigned short _color,
                      unsigned short _background_color) {
      int half_body_segment_size = body_segment_size / 2;
      previous_x = x;
      previous_y = y;
      TFTLCD.fillrect(x - half_body_segment_size, y - half_body_segment_size,
                      x + half_body_segment_size, y + half_body_segment_size,
                      _background_color);
      x = _x;
      y = _y;

      TFTLCD.fillrect(x - half_body_segment_size, y - half_body_segment_size,
                      x + half_body_segment_size, y + half_body_segment_size,
                      _color);
    }

    void setSegmentSize(int _body_segment_size) {
      body_segment_size = _body_segment_size;
    }

    int getSegmentSize() { return body_segment_size; }

    int getPreviousPositionX() { return previous_x; }

    int getPreviousPositionY() { return previous_y; }

  private:
    int body_segment_size = 10; // by deffault
    int previous_x;
    int previous_y;
  };

  vector<Segment> segments;

private:
  unsigned short background_colour = 0;
  int segments_quantity = 6;
  int speed = 1;
};

class Orb : public GameObject {
  Orb(int _color, int _score, int _size) {
    color = _color;
    score = _score;
    size = _size;
  }
  void appear(int _x, int _y) override {}
};

Snake MyFuckingSnake(SnakeColor, 10, BackgroundColor, 2);
int main() {
  TFTLCD.background(BackgroundColor);
  TFTLCD.cls();
  MyFuckingSnake.appear(160, 240);
  int dir_x, dir_y;
  int direction = Right;

  while (true) {
    if (UpButton.read() == 1) {
      direction = Up;
    } else if (DownButton.read() == 1) {
      direction = Down;
    } else if (LeftButton.read() == 1) {
      direction = Left;
    } else if (RightButton.read() == 1) {
      direction = Right;
    } else {
    }

    switch (direction) {
    case Up:
      MyFuckingSnake.move(0, -1);
      break;
    case Down:
      MyFuckingSnake.move(0, 1);
      break;
    case Right:
      MyFuckingSnake.move(1, 0);
      break;
    case Left:
      MyFuckingSnake.move(-1, 0);
      break;
    default:
      break;
    }
  }
}
