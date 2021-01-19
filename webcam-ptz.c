#include "libuvc/libuvc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


void handleError(uvc_error_t errorCode, char *message)
{
	if (errorCode < 0)
	{
		fprintf(stderr, "%s", message);
		exit(errorCode);
	}
}


int main(int argc, char **argv)
{
	if (argc != 3 || (strcmp(argv[1], "pan") && strcmp(argv[1], "tilt") && strcmp(argv[1], "zoom")))
	{
		fprintf(stderr, "Usage: %s pan|tilt|zoom min|max|middle|<number-of-steps-to-change>\n", argv[0]);
		exit(1);
	}

	char *command = argv[1];
	char *param = argv[2];

	uvc_context_t *ctx;
	uvc_device_t *dev;
	uvc_device_handle_t *devh;
	uvc_stream_ctrl_t ctrl;
	uvc_error_t errorCode;

	/* Initialize a UVC service context. Libuvc will set up its own libusb
	* context. Replace NULL with a libusb_context pointer to run libuvc
	* from an existing libusb context. */
	errorCode = uvc_init(&ctx, NULL);

	if (errorCode < 0)
	{
		uvc_perror(errorCode, "uvc_init");
		return errorCode;
	}

	//puts("UVC initialized");

	/* Locates the first attached UVC device, stores in dev */
	errorCode = uvc_find_device(ctx, &dev, 0, 0, NULL); /* filter devices: vendor_id, product_id, "serial_num" */
	if (errorCode < 0)
	{
		fprintf(stderr, "No camera found\n");
	}
	else
	{
		// Try to open the device: requires exclusive access
		errorCode = uvc_open(dev, &devh);
		if (errorCode < 0)
		{
			fprintf(stderr, "Unable to access camera - possibly another program is using it?\n");
		}
		else
		{
			if (strcmp(command, "zoom") == 0)
			{
				uint16_t zoomCurrent;
				uint16_t zoomStep;
				uint16_t zoomMin;
				uint16_t zoomMax;

				// get current value
				errorCode = uvc_get_zoom_abs(devh, &zoomCurrent, UVC_GET_CUR);
				handleError(errorCode, "Failed to read zoom setting - possibly unsupported by this camera?\n");

				// get steps
				errorCode = uvc_get_zoom_abs(devh, &zoomStep, UVC_GET_RES);
				handleError(errorCode, "Failed to read zoom setting - possibly unsupported by this camera?\n");

				// get min
				errorCode = uvc_get_zoom_abs(devh, &zoomMin, UVC_GET_MIN);
				handleError(errorCode, "Failed to read zoom setting - possibly unsupported by this camera?\n");

				// get max
				errorCode = uvc_get_zoom_abs(devh, &zoomMax, UVC_GET_MAX);
				handleError(errorCode, "Failed to read zoom setting - possibly unsupported by this camera?\n");

				// signed to allow overflow before we clamp the value to max/min!
				int zoom = zoomCurrent;

				if (strcmp(param, "min") == 0)
				{
					zoom = zoomMin;
				}
				else if (strcmp(param, "max") == 0)
				{
					zoom = zoomMax;
				}
				else if (strcmp(param, "middle") == 0)
				{
					zoom = (zoomMin + zoomMax) / 2;
				}
				else
				{
					zoom += atoi(param) * zoomStep;

					if (zoom < zoomMin)
					{
						zoom = zoomMin;
					}

					if (zoom > zoomMax)
					{
						zoom = zoomMax;
					}
				}

				errorCode = uvc_set_zoom_abs(devh, zoom);
				handleError(errorCode, "Failed to set zoom - possibly unsupported by this camera?\n");
			}
			else
			{
				// pan or tilt, so we need to read both values regardless
				int32_t pan;
				int32_t panStep;
				int32_t panMin;
				int32_t panMax;
				int32_t tilt;
				int32_t tiltStep;
				int32_t tiltMin;
				int32_t tiltMax;

				// get current value
                                errorCode = uvc_get_pantilt_abs(devh, &pan, &tilt, UVC_GET_CUR);
				handleError(errorCode, "Failed to read pan/tilt settings - possibly unsupported by this camera?\n");

				// get steps
                                errorCode = uvc_get_pantilt_abs(devh, &panStep, &tiltStep, UVC_GET_RES);
				handleError(errorCode, "Failed to read pan/tilt settings - possibly unsupported by this camera?\n");

				// get min
                                errorCode = uvc_get_pantilt_abs(devh, &panMin, &tiltMin, UVC_GET_MIN);
				handleError(errorCode, "Failed to read pan/tilt settings - possibly unsupported by this camera?\n");

				// get max
                                errorCode = uvc_get_pantilt_abs(devh, &panMax, &tiltMax, UVC_GET_MAX);
				handleError(errorCode, "Failed to read pan/tilt settings - possibly unsupported by this camera?\n");

				if (strcmp(command, "pan") == 0)
				{
					if (strcmp(param, "min") == 0)
					{
						pan = panMin;
					}
					else if (strcmp(param, "max") == 0)
					{
						pan = panMax;
					}
					else if (strcmp(param, "middle") == 0)
					{
						pan = (panMin + panMax) / 2;
					}
					else
					{
						pan += atoi(param) * panStep;

						if (pan < panMin)
						{
							pan = panMin;
						}

						if (pan > panMax)
						{
							pan = panMax;
						}
					}
				}
				else
				{
					if (strcmp(param, "min") == 0)
					{
						tilt = tiltMin;
					}
					else if (strcmp(param, "max") == 0)
					{
						tilt = tiltMax;
					}
					else if (strcmp(param, "middle") == 0)
					{
						tilt = (tiltMin + tiltMax) / 2;
					}
					else
					{
						tilt += atoi(param) * tiltStep;

						if (tilt < tiltMin)
						{
							tilt = tiltMin;
						}

						if (tilt > tiltMax)
						{
							tilt = tiltMax;
						}
					}
				}

				errorCode = uvc_set_pantilt_abs(devh, pan, tilt);
				handleError(errorCode, "Failed to set pan/tilt - possibly unsupported by this camera?\n");
			}

			// Release our handle on the device
			uvc_close(devh);
		}
		// Release the device descriptor
		uvc_unref_device(dev);
	}

	// Close the UVC context. This closes and cleans up any existing device handles,
	// and it closes the libusb context if one was not provided.
	uvc_exit(ctx);

	return 0;
}
