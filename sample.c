#include <X11/Xlib.h>

int main() {
  Display *display = XOpenDisplay(NULL);
  if (display == NULL) {
    return 1;
  }

  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 300, 200, 0, BlackPixel(display, DefaultScreen(display)), WhitePixel(display, DefaultScreen(display)));

  XMapWindow(display, window);

  XEvent event;
  while (1) {
    XNextEvent(display, &event);
    if (event.type == ClientMessage && event.xclient.data.l[0] == XCloseWindow) {
      break;
    }
  }

  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
}