#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>    

void move_mouse(Display* display, int x, int y){
    XTestFakeRelativeMotionEvent(display, x, y, 0);
    XFlush(display);
}
void set_mouse(Display* display, int x, int y){
    XTestFakeMotionEvent(display, 0, x, y, 0);
    XFlush(display);
}
void button_make(Display* display, unsigned int button){
    XTestFakeButtonEvent(display, button, True, 0);
    XFlush(display);
}
void button_break(Display* display, unsigned int button){
    XTestFakeButtonEvent(display, button, False, 0);
    XFlush(display);
}


/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

enum Keys {
    ArrowLeft = 65361,
    ArrowUp = 65362,
    ArrowRight = 65363,
    ArrowDown = 65364
};

int main(int argc, char **argv){
    Display *display = XOpenDisplay(NULL);
    // set_mouse(display, 0, 0);
    // button_make(display, 1);
    // button_break(display, 1);

	Window root = DefaultRootWindow(display);
	long mask = KeyPressMask | KeyReleaseMask | FocusChangeMask;

    Window focus;
    int revert;
    XComposeStatus comp;

    char keyStr[16];
    KeySym keySym;
    int len;
    int isCaptured = 0;

    XWindowAttributes winAttrs;
    XGetWindowAttributes(display, root, &winAttrs);

    // CTRL + ALT + P
    int shortc[3] = {};
    int arrows[4] = {};
    float velocity[4] = {};
    int location[2] = {winAttrs.width / 2, winAttrs.height / 2};
    int buttons[3] = {};
    int scroll[4] = {};
    int ctrl = 0;
    int shift = 0;
    int alt = 0;

    
	XGetInputFocus(display, &focus, &revert);
	XSelectInput(display, focus, mask);
    
	XEvent event;
	while (1) {

        if (XCheckWindowEvent(display, focus, mask, &event)) {
            // XNextEvent(display, &event);
            // printf("x: %d, y: %d\n", event.xkey.x, event.xkey.y);
            // printf("button: %u\n", event.xbutton.button);
            switch (event.type) {
                case ButtonPress:
                    printf("button: %u\n", event.xbutton.button);
                    break;
                // Change follow focused window
                case FocusOut:
                    // Get rid of event masks
                    if (focus != root)
                        XSelectInput(display, focus, 0);
                    // Fetch newly focused window
                    XGetInputFocus (display, &focus, &revert);
                    if (focus == PointerRoot)
                        focus = root;
                    // Set new event listener
                    XSelectInput(display, focus, mask);
                    break;
                case KeyPress:
                    len = XLookupString(&event.xkey, keyStr, 16, &keySym, &comp);
                    printf("PRESS: %s (%lu)\n", keyStr, keySym);
                    if (keySym == 65507) shortc[0] = 1;
                    if (keySym == 65513) shortc[1] = 1;
                    if (keySym == 112 || keySym == 80) shortc[2] = 1;
                    // Handle arrows
                    if (keySym == ArrowLeft) arrows[0] = 1;
                    if (keySym == ArrowUp) arrows[1] = 1;
                    if (keySym == ArrowRight) arrows[2] = 1;
                    if (keySym == ArrowDown) arrows[3] = 1;
                    if (keySym == 65507) ctrl = 1;
                    if (keySym == 65505) shift = 1;
                    if (keySym == 65513) alt = 1;
                    // Handle buttons
                    if (keySym == 113) buttons[0] = 1;
                    if (keySym == 101) buttons[1] = 1;
                    if (keySym == 114) buttons[2] = 1;
                    // Handle scrolls
                    if (keySym == 119) scroll[0] = 1;
                    if (keySym == 115) scroll[1] = 1;
                    if (keySym == 97) scroll[2] = 1;
                    if (keySym == 100) scroll[3] = 1;

                    // if CTRL + ALT + P was hit
                    if (shortc[0] == 1 && shortc[1] == 1 && shortc[2] == 1) {
                        printf("Toggle capturing\n");
                        isCaptured = !isCaptured;
                        shortc[2] = 0;
                        if (!isCaptured) {
                            location[0] = winAttrs.width / 2;
                            location[1] = winAttrs.height / 2;
                            for (int i = 0; i < 4; i++)
                                velocity[i] = 0;
                            for (int i = 0; i < 4; i++)
                                buttons[i] = 0;
                            for (int i = 0; i < 4; i++)
                                scroll[i] = 0;
                            ctrl = 0;
                            shift = 0;
                            alt = 0;
                        }
                    }
                    
                    break;
                case KeyRelease:
                    len = XLookupString(&event.xkey, keyStr, 16, &keySym, &comp);
                    // printf("RELEASE: %s (%lu)\n", keyStr, keySym);
                    if (keySym == 65507) shortc[0] = 0;
                    if (keySym == 65513) shortc[1] = 0;
                    if (keySym == 112 || keySym == 80) shortc[2] = 0;
                    // Handle arrows
                    if (keySym == ArrowLeft) arrows[0] = 0;
                    if (keySym == ArrowUp) arrows[1] = 0;
                    if (keySym == ArrowRight) arrows[2] = 0;
                    if (keySym == ArrowDown) arrows[3] = 0;
                    if (keySym == 65507) ctrl = 0;
                    if (keySym == 65505) shift = 0;
                    if (keySym == 65513) alt = 0;
                    // Handle buttons
                    if (keySym == 113) buttons[0] = 0;
                    if (keySym == 101) buttons[1] = 0;
                    if (keySym == 114) buttons[2] = 0;
                    // Handle scrolls
                    if (keySym == 119) scroll[0] = 0;
                    if (keySym == 115) scroll[1] = 0;
                    if (keySym == 97) scroll[2] = 0;
                    if (keySym == 100) scroll[3] = 0;
                    break;

            }
            continue;
        }

        if (isCaptured) {
            int speed = 10; 
            const float fraction = 0.7; 

            if (alt)
                speed /= 2;
            if (ctrl)
                speed /= 4;
            if (shift)
                speed *= 2;

            if (arrows[0]) velocity[0] = 1;
            if (arrows[1]) velocity[1] = 1;
            if (arrows[2]) velocity[2] = 1;
            if (arrows[3]) velocity[3] = 1;

            location[0] -= (int)(velocity[0] * speed);
            location[1] -= (int)(velocity[1] * speed);
            location[0] += (int)(velocity[2] * speed);
            location[1] += (int)(velocity[3] * speed);

            // for (int i = 0; i < 2; i++) {
            //     printf("%d, ", location[i]);
            // }
            // printf("\n");

            set_mouse(display, location[0], location[1]);

            if (buttons[0]) {
                button_make(display, 1);
                button_break(display, 1);
                buttons[0] = 0;
            }
            if (buttons[1]) {
                button_make(display, 3);
                button_break(display, 3);
                buttons[1] = 0;
            }
            if (buttons[2]) {
                button_make(display, 2);
                button_break(display, 2);
                buttons[2] = 0;
            }

            if (scroll[0]) {
                button_make(display, 4);
                button_break(display, 4);
            }
            if (scroll[1]) {
                button_make(display, 5);
                button_break(display, 5);
            }

            for (int i = 0; i < 4; i++)
                velocity[i] *= fraction;
        }
        

        msleep(10);

	}
	XCloseDisplay(display);
    return 0;
}
