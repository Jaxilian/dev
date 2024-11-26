#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>

int main()
{
    int fbfd = 0;  // Framebuffer file descriptor
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    uint8_t *fbp = 0;  // Framebuffer pointer

    // Open the framebuffer device file
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: Cannot open framebuffer device");
        exit(1);
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error: Reading fixed information");
        close(fbfd);
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error: Reading variable information");
        close(fbfd);
        exit(3);
    }

    // Calculate the size of the screen in bytes
    screensize = vinfo.yres_virtual * finfo.line_length;

    // Map the framebuffer device to memory
    fbp = (uint8_t *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((intptr_t)fbp == -1) {
        perror("Error: Failed to map framebuffer device to memory");
        close(fbfd);
        exit(4);
    }

    // Clear the screen with a solid color (e.g., blue)
    memset(fbp, 0x00, screensize);  // Clear screen (black)

    // Define the color (in this case, blue)
    uint32_t color = 0x000000FF;  // ARGB format (assuming 32bpp)

    // Draw a rectangle covering the entire screen
    for (int y = 0; y < vinfo.yres; y++) {
        for (int x = 0; x < vinfo.xres; x++) {
            long int location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                                (y + vinfo.yoffset) * finfo.line_length;

            if (vinfo.bits_per_pixel == 32) {
                *(uint32_t *)(fbp + location) = color;
            } else if (vinfo.bits_per_pixel == 16) {
                uint16_t b = (color & 0x000000FF) >> 3;
                uint16_t g = (color & 0x0000FF00) >> 10;
                uint16_t r = (color & 0x00FF0000) >> 19;
                uint16_t pixel = (r << 11) | (g << 5) | b;
                *(uint16_t *)(fbp + location) = pixel;
            } else {
                // Handle other bit depths if necessary
            }
        }
    }

    while(getchar() != 'c'){
    
    }

    // Unmap and close the framebuffer device
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
