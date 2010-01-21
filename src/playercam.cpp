#include <iostream>
#include <fstream>
#include <libplayerc++/playerc++.h>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
using namespace std;
using namespace PlayerCc;

PlayerClient    robot;
Position2dProxy pp(&robot, 0);
CameraProxy 	cp(&robot);
GtkWidget *window = NULL;
GtkWidget *vbox;
GtkWidget *drawing_area = NULL;
int32_t g_window_width  = 256;
int32_t g_window_height = 190;

uint16_t    g_width = 0;
uint16_t   g_height = 0;
GdkPixbuf* g_pixbuf = NULL;
size_t allocated_size = 0;
guchar *g_img = NULL;

int usize;

void player_update() {
	robot.Read();
	robot.Read();
	int image_count = cp.GetImageSize();
	int image_width = cp.GetWidth();
	int image_height = cp.GetHeight();
	usize = image_count;
	uint8_t imgBuffer[image_count]; // Image buffer with the size of the image (# of bytes)
 	cp.GetImage(imgBuffer);
  	g_img =  (guchar *) realloc(g_img, usize);
 	memcpy(g_img, imgBuffer, image_count);
    g_width  = image_width;
    g_height = image_height;
}

gint
render_camera(gpointer data)
{
  GdkPixbuf *pixbuf = NULL;
  GdkPixbuf *blobbuf= NULL;
  GdkGC         *gc = NULL;
  GtkWidget *drawing_area = GTK_WIDGET(data);
  uint16_t i;

  gc = GTK_WIDGET(drawing_area)->style->fg_gc[GTK_WIDGET_STATE(GTK_WIDGET(drawing_area))];

  player_update();

  gdk_draw_pixbuf(GTK_WIDGET(drawing_area)->window, gc,
                    g_pixbuf, 0, 0, 0, 0, g_width, g_height,
                    GDK_RGB_DITHER_NONE, 0, 0);
  return TRUE;
}

gint
resize(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    GtkAllocation *size = (GtkAllocation*)event;
    g_window_width  = size->width;
    g_window_height = size->height;
    return TRUE;
}

static
gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  gdk_pixbuf_unref(g_pixbuf);
  return FALSE;
}

static
void destroy(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}

void startGtk(int argc, char **argv) {
  gtk_init(&argc, &argv);
  player_update();

  g_img =  (guchar *) realloc(g_img, usize);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "PlayerCam");

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add (GTK_CONTAINER(window), vbox);
  gtk_widget_show(vbox);

  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(GTK_WIDGET (drawing_area), g_width, g_height);
  gtk_box_pack_start(GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);
  gtk_widget_show(drawing_area);

  gtk_widget_show_all(window);

  gtk_widget_add_events(GTK_WIDGET(drawing_area), GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT (window), "delete_event",
                    G_CALLBACK (delete_event), NULL);
  g_signal_connect(G_OBJECT (window), "destroy",
                    G_CALLBACK (destroy), NULL);
  g_signal_connect(GTK_OBJECT(drawing_area), "size-allocate",
                    G_CALLBACK(resize), NULL);

  g_pixbuf = gdk_pixbuf_new_from_data(g_img, GDK_COLORSPACE_RGB,
                                      FALSE, 8, g_width, g_height,
                                      g_width * 3, NULL, NULL);

  gtk_idle_add(render_camera, drawing_area);

  gtk_main();
}

int main(int argc, char **argv)
{
	startGtk(argc, argv);
	return 0;
}