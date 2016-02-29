/*
 ROT 3.0 (ROT 2.2 ported to DJGPP using the Allegro game library)
 */

#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>
#include <conio.h>
#include <math.h>
#include <dos.h>

BITMAP *memory_bitmap;
BITMAP *video_buffer;
BITMAP *save_screen;
//RLE_SPRITE *jball;
BITMAP *jdrive_bmp;
PALETTE pallete;
PALETTE jpallete;
RGB temp_rgb;

// RGB black = {0, 0, 0};

float x[600],y[600], z[600];
float clr[600];
float spin[600];
float x_rot, y_rot, z_rot;
int num_things;
int maxx, maxy, maxz, maxc;
int i, a, b, c;
float theta;
float temp, temp2, f;
float tx, ty, tz, camx, camz, camy;
float dircamx, dircamz, dircamy;
int flag=0;
int cx, cy, cz;
int dx=0;
int dy=0;
int dz=0;

float djx=13.75;
float jx=100;
int jy=100;

int jd_w = 219;
int jd_h = 200;

int z_adj = 7500;
int fov = 50;
float viewdist_x, viewdist_y;
int spindir = 1;
char pressed;

float pi = 3.14159;
float view_dist;

void rotpoints(void);
void initxys(void);
void drawpoints(void);
void bounce_center(void);
void jdrive(void);

void init_palette(void);

struct vtx
{
 int x, y, z;
} vtx;

struct vtx scr[400];
struct vtx tvtx;

int main() {
int xit_flag =0;

   allegro_init();
   install_keyboard();
   set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0);

 cx = SCREEN_W / 2;
 cy = SCREEN_H / 2;
 maxc = 256;

 initxys();
 set_color_depth(32);
 init_palette();
 set_pallete(pallete);

 while(!xit_flag) {
//  set_pallete(jpallete);
//  jdrive();
  drawpoints();
  rotpoints();
  if(keypressed()) {
   pressed = readkey() & 0xff;
   if(pressed == 'q') xit_flag = 1;

   if(z_adj > 5 && z_adj < 100000)
    {
    if(pressed == 'w')  z_adj *= 1.1;
    if(pressed == 's')  z_adj /= 1.1;
    }
   if(pressed == '-')
    for(i=0; i<num_things; i++)
     spin[i] /= 1.1;
   if(pressed == '+')
    for(i=0; i<num_things; i++)
     spin[i] *= 1.1;
   if(pressed == '1') {
   save_screen = create_sub_bitmap(screen, 0, 0, SCREEN_W, SCREEN_H);
   save_bitmap("jball.pcx", save_screen, pallete);
   destroy_bitmap(save_screen);
   }
  }

  temp_rgb = pallete[255];
  for (c=255; c>1; c--)
   pallete[c] = pallete[c-1];
  pallete[1] = temp_rgb;
  set_pallete(pallete);

  blit(video_buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  clear_to_color(video_buffer, 0);
 }

   /* clean up */
   fade_out(1);
   destroy_bitmap(memory_bitmap);
   destroy_bitmap(video_buffer);
//   destroy_bitmap(jdrive_bmp);
//   readkey();
   allegro_exit();
   clrscr();
   printf("Rotten - (c) 1998 by Joshua Silver\n");
   return 0;
}

void initxys(void)
{
   video_buffer = create_bitmap(SCREEN_W, SCREEN_H);
   clear_to_color(video_buffer, 0);

   memory_bitmap = create_bitmap(20, 20);
   clear_to_color(memory_bitmap, 0);

//   jdrive_bmp = load_bmp("jdrive.bmp", jpallete);
//   circle(memory_bitmap, 10, 10, 10, 1);
//   jdrive = get_rle_sprite(memory_bitmap);

   cx = SCREEN_W / 2;
   cy = SCREEN_H / 2;

   viewdist_x = cx / tan(pi * fov / 180);
   viewdist_y = cy / tan(pi * fov / 180);

 view_dist=16;
 num_things=254;
 for(i=0; i<num_things; i++)
 {
  x[i]=(i * 10 - num_things * 5);
  y[i]=i * 50;
  z[i]=i * 150 - num_things * 25;
  clr[i]=i;
  spin[i]=(i / 75.01) - 2;
 }
 camx=10;
 dircamx=1.3;

 camz=10;
 dircamz=.75;

 camy=0;
 dircamy=1.26;
}

void drawpoints(void)
{
if(camx>360) camx-=360;
camx+=dircamx;

if(camz>360) camz-=360;
camz+=dircamz;

if(camy>360) camy-=360;
camy+=dircamy;

for(i=0; i<num_things; i++)
{
tx=x[i]; ty=y[i]; tz=z[i];

temp=tx; tx=tz; tz=temp;
theta= pi * (camx) / 180;
x_rot = tx * cos(theta) - ty * sin(theta);
y_rot = tx * sin(theta) + ty * cos(theta);
tx = x_rot;
ty = y_rot;
temp=tx; tx=tz; tz=temp;

theta= pi * (camz) / 180;
x_rot = tx * cos(theta) - ty * sin(theta);
y_rot = tx * sin(theta) + ty * cos(theta);
tx = x_rot;
ty = y_rot;

temp=tx; tx=tz; tz=ty; ty=temp;
theta= pi * camy / 180;
x_rot = tx * cos(theta) - ty * sin(theta);
y_rot = tx * sin(theta) + ty * cos(theta);
tx = x_rot;
ty = y_rot;
temp=tz; tz=tx; tx=ty; ty=temp;

 if(z_adj + tz > 1) {
  tx = (viewdist_x * tx / (tz + z_adj)) + cx;
  ty = (viewdist_y * ty / (tz + z_adj)) + cy;
  scr[i].x = tx;
  scr[i].y = ty;
 }
 scr[i].z = tz;
// draw_rle_sprite(video_buffer, jball, tx, ty);

 }

 for(i=0; i<num_things; i++)
  {
  for(a = i+1; a < num_things; a++)
   {
   if(scr[i].z < scr[a].z)
    {
    tvtx.x = scr[a].x;
    tvtx.y = scr[a].y;
    tvtx.z = scr[a].z;
//    c = clr[a];

    scr[a].x = scr[i].x;
    scr[a].y = scr[i].y;
    scr[a].z = scr[i].z;
//    clr[a] = clr[i];

    scr[i].x = tvtx.x;
    scr[i].y = tvtx.y;
    scr[i].z = tvtx.z;
//    clr[i] = c;
    }
   }
  if(z_adj + scr[i].z > 1) {
   circlefill(video_buffer, scr[i].x, scr[i].y, ((i+1)/10), clr[i]);
  }
//    masked_blit(jball, video_buffer, 1, 1, scr[i].x, scr[i].y, 20, 20);
 }

 /*
  for(i = 0; i < num_things; i++)
   {
    for(a = i; a < num_things; a++)
     {
     line(video_buffer, scr[i].x,scr[i].y, scr[a].x, scr[a].y, (i % 254)+1);
     }
   }
*/
}

void rotpoints(void)
{
 for(a=0; a<num_things; a++)
 {
    temp=x[a]; x[a]=z[a]; z[a]=y[a]; y[a]=temp;
    theta= pi * spin[a] / 180;
    x_rot = x[a] * cos(theta) - y[a] * sin(theta);
    y_rot = x[a] * sin(theta) + y[a] * cos(theta);
    x[a] = x_rot;
    y[a] = y_rot;
    temp=z[a]; z[a]=x[a]; x[a]=y[a]; y[a]=temp;
 }
}

void init_palette(void) {

   for (c=1; c<64; c++) {
      pallete[c].r = c;
      pallete[c].g = 10;
      pallete[c].b = 10;
   }
   pallete[0].r = 0;
   pallete[0].g = 0;
   pallete[0].b = 0;
   for (c=64; c<128; c++) {
      pallete[c].r = 127-c;
      pallete[c].g = c-64;
      pallete[c].b = 10;
   }
   for (c=128; c<192; c++) {
      pallete[c].r = 10;
      pallete[c].g = 191-c;
      pallete[c].b = c-128;
   }
   for (c=192; c<256; c++) {
      pallete[c].r = 10;
      pallete[c].g = 10;
      pallete[c].b = 255-c;
   }

}

void jdrive(void) {
 if((int)jx > SCREEN_W) {
  jx = -jd_w;
  jy *= 3.07;
  jy %= SCREEN_H;
 }
 if(jx < SCREEN_W) {
  blit(jdrive_bmp, video_buffer, 0, 0, jx, jy, jd_w, jd_h);
 }
 jx += djx;
}



