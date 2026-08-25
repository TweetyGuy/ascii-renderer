#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

int R1 = 2, R2 = 5;
int LENGTH = 10;
int distance = 30;

typedef struct {
  float x;
  float y;
  float z;
} Vec3;

typedef struct {
  float x;
  float y;
} Vec2;

char* light_vals = ".:;=a+&$@#";

void pixel(int x, int y, char c, int s_width, int s_height)
{
  if (x < 0 || x >= s_width || y < 0 || y >= s_height)
    return;
  mvaddch(y, x, c);
}

Vec3 calculate_vec3(float u, float v, float A, float B, float R1, float R2, int d)
{
  float x = sin(v) * cos(A) * (R1 * sin(u) + R2) - R1 * cos(u) * sin(A);
  float y = cos(v) * cos(B) * (R1 * sin(u) + R2) + sin(B) * (sin(v) * sin(A) * (R1 * sin(u) + R2) + R1 * cos(u) * cos(A));
  float z = cos(B) * (sin(v) * sin(A) * (R1 * sin(u) + R2) + R1 * cos(u) * cos(A)) - cos(v) * sin(B) * (R1 * sin(u) + R2);
  
  return (Vec3){x, y, z};
}

float dot_product(Vec3 v1, Vec3 v2)
{
  Vec3 v = {
    .x = v1.x * v2.x,
    .y = v1.y * v2.y,
    .z = v1.z * v2.z
  };

  return v.x + v.y + v.z;
}

Vec3 normalize(Vec3 v)
{
  float s = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
  return (Vec3){
    .x = v.x/s,
    .y = v.y/s,
    .z = v.z/s
  };
}

Vec2 project(Vec3 v3, int s_width, int s_height)
{
  float FOV = 45.0f;
  float rFOV = 45.0f * M_PI / 180.0f;
  float ooz = 1.0f/(v3.z + distance);
  float y_p = (s_height / 2.0f) / tan(rFOV / 2.0f);
  float h_aspect = (float)s_width / (s_height * 2);
  Vec2 v2 = {
    .x = s_width/2.0f + v3.x * ooz * y_p * h_aspect,
    .y = s_height/2.0f - v3.y * ooz * y_p
  };
  return v2;
}

void draw_coordinate_grid(float A, float B, int s_width, int s_height, int length)
{
  for (float K = 0; K < length; K += 0.2)
  {
    Vec3 x_axis = (Vec3){K * cos(A), K * sin(A) * sin(B), K * sin(A) * cos(B)};
    Vec2 x_line = project(x_axis, s_width, s_height);
    if (x_line.x < 0 || x_line.x >= s_width || x_line.y < 0 || x_line.y >= s_height) continue;
    pixel(x_line.x, x_line.y, 'X', s_width, s_height); // X line

    Vec3 y_axis = (Vec3){0, K * cos(B), -K * sin(B)};
    Vec2 y_line = project(y_axis, s_width, s_height);
    if (y_line.x < 0 || y_line.x >= s_width || y_line.y < 0 || y_line.y >= s_height) continue;
    pixel(y_line.x, y_line.y, 'Y', s_width, s_height); // Y line

    Vec3 z_axis = (Vec3){-K * sin(A), K * cos(A) * sin(B), K * cos(A) * cos(B)};
    Vec2 z_line = project(z_axis, s_width, s_height);
    if (z_line.x < 0 || z_line.x >= s_width || z_line.y < 0 || z_line.y >= s_height) continue;
    pixel(z_line.x, z_line.y, 'Z', s_width, s_height); // Z line

  }
}

void render_frame(float A, float rY, float B, float rX, float* z_buffer, int s_width, int s_height, Vec3 light)
{
  // todo draw line
  // todo seperate into functions of draw line and draw torus
  draw_coordinate_grid(A+rY, B+rX, s_width, s_height, LENGTH);

  for (float u = 0; u < 2 * M_PI; u += (M_PI / 157))
  {
    for (float v = 0; v < 2 * M_PI; v += (M_PI / 157))
    {
      Vec3 vert = calculate_vec3(u, v, A+rY, B+rX, R1, R2, distance);
      float denom = vert.z + distance;
      float ooz = 1/denom;
      
      Vec2 p = project(vert, s_width, s_height);

      if (p.x < 0 || p.x >= s_width || p.y < 0 || p.y >= s_height) continue;

      Vec3 normal = calculate_vec3(u, v, A+rY, B+rX, 1, 0, 0);
      float light_angle = dot_product(normalize(light), normal);
      
      if (light_angle > 0)
      {
        if (ooz > z_buffer[(int)(p.y * s_width + p.x)])
        {
          z_buffer[(int)((int)(p.y * s_width) + (int)p.x)] = ooz;
          int light_val = (int)(light_angle * 10);
          pixel(p.x, p.y, light_vals[light_val], s_width, s_height);
        }
      }
    }
  }
}

void init_curses()
{
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  curs_set(0);
  keypad(stdscr, TRUE);
}

void end_curses()
{
  echo();
  endwin();
}

int main()
{
  init_curses();  

  int screen_width = 160, screen_height = 50;
  getmaxyx(stdscr, screen_height, screen_width);

  float* z_buffer = (float*)malloc(screen_width * screen_height * sizeof(float));
  memset(z_buffer, 0, screen_width * screen_height * sizeof(float));
  
  float rotationY = 0, rotationX = 0;

  Vec3 light = {
    .x = 0,
    .y = 0,
    .z = -1
  };

  int rotate = 1;
  for (float rotation = 0;;)
  {
    if (rotate == 1)
      rotation += (M_PI / 157);
    memset(z_buffer, 0, screen_width * screen_height * sizeof(float));
    
    int ch = getch();
    if (ch == '+' || ch == '=')
      distance--;
    if (ch == '-' || ch == '_')
      distance++;
    if (ch == 'a')
      rotationY += 3.14/8;
    if (ch == 'd')
      rotationY -= 3.14/8;
    if (ch == 'w')
      rotationX += 3.14/8;
    if (ch == 's')
      rotationX -= 3.14/8;
    if (ch == 'q')
      break;
    if (ch == ' ')
    {
      if (rotate == 1) rotate = 0;
      else rotate = 1;
    }
    if (ch == KEY_LEFT) light.x -= 1.0f/8.0f;
    else if (ch == KEY_RIGHT) light.x += 1.0f/8.0f;
    else if (ch == KEY_UP) light.y += 1.0f/8.0f;
    else if (ch == KEY_DOWN) light.y -= 1.0f/8.0f;
    if (ch == 'r')
    {
      flushinp();
      sleep(1);
      ch = getch();
      if (ch >= '0' && ch <= '9') R1 = ch - '0';
    }
    else if (ch == 't')
    {
      flushinp();
      sleep(1);
      ch = getch();
      if (ch >= '0' && ch <= '9') R2 = ch - '0';
    }
    else if (ch == 'l')
    {
      flushinp();
      sleep(1);
      ch = getch();
      if (ch >= '0' && ch <= '9') LENGTH = 2 * (ch - '0');
    }
    flushinp();

    erase();
    render_frame(rotation, rotationY, rotation, rotationX, z_buffer, screen_width, screen_height, light);
    refresh();
  }
  free(z_buffer);
  end_curses();
  return 0;
}