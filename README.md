# webcam-ptz
Command-line tool to pan, tilt, and zoom a UVC-compatible USB webcam.

Tested on MacOS, should work on other UNIX-like operating systems.

I use it with "Better Touch Tool" on MacOS to pan, tilt, and zoom my webcam while on a video call by using keystrokes (mapped in BTT).

Tested with Logitech Brio and Logitech Webcam Pro 9000.  Should work with any other webcam that has settings that can be set over UVC.  

Dependencies: libuvc and libusb.

## Building

Requires these libraries to be built and installed:

        https://github.com/libuvc/libuvc

                git clone https://github.com/libuvc/libuvc
                cd libuvc/
                mkdir build
                cd build/
                cmake ..
                make
                sudo make install

        https://libusb.info/

                Download and extract:
                        https://github.com/libusb/libusb/releases/download/v1.0.24/libusb-1.0.24.tar.bz2

                cd libusb-1.0.24/
                ./configure && make && make install

Build with:

        cc -O3 -l uvc -o webcam-ptz webcam-ptz.c
        
Static build on MacOS:

        cc -O3 -framework CoreFoundation -framework IOKit /usr/local/lib/libuvc.a /usr/local/lib/libusb-1.0.a -o webcam-ptz webcam-ptz.c
        
## Usage
        ./webcam-ptz pan|tilt|zoom min|max|middle|<number-of-steps-to-change>
        
## Examples

        ./webcam-ptz pan middle; ./webcam-ptz tilt middle   # centre the webcam
        ./webcam-ptz zoom min                               # zoom all the way out
        ./webcam-ptz zoom max                               # zoom all the way in
        ./webcam-ptz zoom 1                                 # zoom in one step (the smallest amount the camera allows) 
        ./webcam-ptz zoom -5                                # zoom out five steps (5 x the smallest amount the camera allows) 
        ./webcam-ptz pan 1                                  # pan sideways one step (the smallest amount the camera allows) 
        ./webcam-ptz tilt -10                               # tilt ten steps (10 x the smallest amount the camera allows) 



