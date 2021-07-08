#include "Arial28x28.h"
#include "ILI9486.h"
#include "map_constrain.h"
#include "mbed.h"
#include <vector>

const int DisplayResolution_x = 320;
const int DisplayResolution_y = 480;

const int SnakeSize = 10;
const float SnakeMovingSpeed = 2.0f;

const int OrbRadius = 5;
const int OrbScore = 10;

const unsigned short BackgroundColor = Cyan;
const unsigned short SnakeColor = Yellow;
const unsigned short OrbColor = Orange;

PinName buspins[8] = {D8, D9, D2, D3, D4, D5, D6, D7};
ILI9486 TFTLCD(BUS_8, buspins, A3, A4, A2, A1, A0, "TFTLCD");

DigitalIn UpButton(PC_2, PullDown), DownButton(PC_3, PullDown),
    LeftButton(PH_0, PullDown), RightButton(PH_1, PullDown);

// I need c++20+

namespace snake_game {
enum SnakeHeadMovingDirections { Up, Down, Left, Right };

class GameObject {
public:
  virtual void appear(int _x, int _y) {}

  virtual void move(int next_x, int next_y) {}

  int getScore() { return score; }

  int getPositionX() { return x; }

  int getPositionY() { return y; }

protected:
  TFT *Display;
  int previous_x = 0;
  int previous_y = 0;
  int x = 0;
  int y = 0;
  int score = 0;
  int size = 0;
  unsigned short color = 0; // Dislpay's lib color
  unsigned short background_colour = 0;
};

class Orb : public GameObject {
public:
  Orb(TFT *_Display, int _color, int _background_colour, int _score,
      int _radius) {

    color = _color;
    background_colour = _background_colour;
    score = _score;
    radius = _radius;
    Display = _Display;
  }

  int getRadius() { return radius; }

  void appear(int orb_x, int orb_y) override {
    previous_x = x;
    previous_y = y;
    Display->fillcircle(previous_x, previous_y, radius, background_colour);
    x = orb_x;
    y = orb_y;
    Display->fillcircle(x, y, radius, color);
  }

private:
  int radius = 0;
};

class Snake : public GameObject {
public:
  Snake(TFT *_Display, int _color, int _body_segment_size,
        unsigned short _background_colour, float _speed, int _disp_res_x,
        int _disp_res_y,
        /*
        you may to not write this paramethers
        */
        int _segments_quantity = 4,
        const char _GameOverMessage[] = "Game over") {

    Display = _Display;
    background_colour = _background_colour;
    disp_res_x = _disp_res_x;
    disp_res_y = _disp_res_y;

    GameOverMessage = _GameOverMessage;

    speed = _speed;
    color = _color;

    segments_quantity = _segments_quantity;

    Segment seg;
    seg.setSegmentSize(_body_segment_size);
    seg.setBackgroundColour(_background_colour);
    seg.setDisplay(Display);

    for (int n = 0; n < 1024; n++) { // You never reach this size!
      segments.push_back(seg);
    }
  }
  void appear(int _x, int _y) override {
    x = _x;
    y = _y;
    int temp_draw_x = x;
    for (int n = 0; n < segments_quantity; n++) {
      temp_draw_x -= segments[n].getSegmentSize();
      segments[n].draw_segment(temp_draw_x, y, color);
    }
  }

  bool caughtFood(Orb &food) {
    int _size = segments[0].getSegmentSize();

    bool x_is_orb_x =
        bool((x + size <= food.getPositionX() + food.getRadius() &&
              x + size >= food.getPositionX() - food.getRadius()) ||
             (x - size <= food.getPositionX() + food.getRadius() &&
              x - size >= food.getPositionX() - food.getRadius()));

    bool y_is_orb_y =
        bool((y + size <= food.getPositionY() + food.getRadius() &&
              y + size >= food.getPositionY() - food.getRadius()) ||
             (y - size <= food.getPositionY() + food.getRadius() &&
              y - size >= food.getPositionY() - food.getRadius()));

    if (x_is_orb_x && y_is_orb_y) {
      score += food.getScore();
      segments_quantity += 1;
      return true;
    } else {
      return false;
    }
  }

  void move(int direction_x, int direction_y) override { // in process
    int seg_size = segments[0].getSegmentSize();

    // checking for crocing borders
    if (x >= disp_res_x) {
      x = 0;
    } else if (x < 0) {
      x = disp_res_x;
    }
    if (y >= disp_res_y) {
      y = 0;
    } else if (y < 0) {
      y = disp_res_y;
    }
    int size = segments[0].getSegmentSize();

    x += direction_x * seg_size;
    y += direction_y * seg_size;

    segments[0].draw_segment(x, y, color);
    CheckingHeadBodyCollisions();

    for (int segment_number = 1; segment_number < segments_quantity;
         segment_number++) {
      segments[segment_number].draw_segment(
          segments[segment_number - 1].getPreviousPositionX(),
          segments[segment_number - 1].getPreviousPositionY(), color);
    }
    segments[segments_quantity].draw_segment(
        segments[segments_quantity - 1].getPreviousPositionX(),
        segments[segments_quantity - 1].getPreviousPositionY(),
        background_colour);

    thread_sleep_for(int((1 / speed) * 1000));
  }

protected:
  class Segment : public GameObject {
  public:
    Segment() {}
    void draw_segment(int _x, int _y, unsigned short _color) {
      int half_body_segment_size = body_segment_size / 2;
      previous_x = x;
      previous_y = y;
      Display->fillrect(x - half_body_segment_size, y - half_body_segment_size,
                        x + half_body_segment_size, y + half_body_segment_size,
                        background_colour);
      x = _x;
      y = _y;

      Display->fillrect(x - half_body_segment_size, y - half_body_segment_size,
                        x + half_body_segment_size, y + half_body_segment_size,
                        _color);
    }

    void setSegmentSize(int _body_segment_size) {
      body_segment_size = _body_segment_size;
    }

    void setBackgroundColour(unsigned short colour) {
      background_colour = colour;
    }

    void setDisplay(TFT *_Display) { Display = _Display; }

    int getSegmentSize() { return body_segment_size; }

    int getPreviousPositionX() { return previous_x; }

    int getPreviousPositionY() { return previous_y; }

  private:
    int body_segment_size = 10; // by deffault
  };

private:
  void GameOver() {
    Display->cls();
    Display->set_font((unsigned char *)Arial28x28);

    // I tired to align this
    Display->locate(disp_res_x / 4 - sizeof(GameOverMessage), disp_res_y / 4);
    Display->printf("%s", GameOverMessage);
    Display->locate(disp_res_x / 2 - 84, disp_res_y / 2.5);
    Display->printf("%s", "Your score is");
    Display->locate(disp_res_x / 2 - score, disp_res_y / 2);
    Display->printf("%d", score);

    for (;;) { // Watining for reset
    }
  }

  void CheckingHeadBodyCollisions() {
    for (int n = 1; n < segments_quantity; n++) {
      if (x == segments[n].getPositionX() && y == segments[n].getPositionY()) {
        GameOver();
      } else {
      }
    }
  }
  vector<Segment> segments;
  int segments_quantity = 0;
  float speed = 0;
  int disp_res_x, disp_res_y;

  const char *GameOverMessage;
};

} // namespace snake_game

snake_game::Snake MyFuckingSnake(&TFTLCD, SnakeColor, SnakeSize,
                                 BackgroundColor, SnakeMovingSpeed,
                                 DisplayResolution_x, DisplayResolution_y);
snake_game::Orb MyFuckingOrb(&TFTLCD, OrbColor, BackgroundColor, OrbScore,
                             OrbRadius);

int main() {
  const int orb_x = DisplayResolution_x + OrbRadius * 2;
  const int orb_y = DisplayResolution_x + OrbRadius * 2;

  int direction = snake_game::Right;
  srand(time(NULL));
  TFTLCD.background(BackgroundColor);
  TFTLCD.cls();

  while (true) {

    if (MyFuckingSnake.caughtFood(MyFuckingOrb)) {
      MyFuckingOrb.appear(rand() % orb_x, rand() % orb_y);
    } else {
    }

    // this construction need for  holding button pressing
    if (UpButton.read() == 1) {
      direction = snake_game::Up;
    } else if (DownButton.read() == 1) {
      direction = snake_game::Down;
    } else if (LeftButton.read() == 1) {
      direction = snake_game::Left;
    } else if (RightButton.read() == 1) {
      direction = snake_game::Right;
    } else {
    }

    switch (direction) {
    case snake_game::Up:
      MyFuckingSnake.move(0, -1);
      break;
    case snake_game::Down:
      MyFuckingSnake.move(0, 1);
      break;
    case snake_game::Right:
      MyFuckingSnake.move(1, 0);
      break;
    case snake_game::Left:
      MyFuckingSnake.move(-1, 0);
      break;
    default:
      break;
    }
  }
}
