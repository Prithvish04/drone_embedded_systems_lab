#include "joystick.h"
#include <stdio.h>


#include <fcntl.h>

#include "js_data_read.h"


js_data_type js_get_current_values() {

	int fd;
	struct js_event js;
	js_data_type js_data;

	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		perror("Joystick");
		exit(1);
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);


	while (read(fd, &js, sizeof(struct js_event)) == 
					sizeof(struct js_event))  {

		switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				break;
		}
	}

    js_data.roll = axis[0]; 		// roll
    js_data.pitch = axis[1]; 		// pitch
    js_data.yaw = axis[2];			// yaw
    js_data.lift = axis[3];			// Slider/throttle
    js_data.abort = button[0];		// Fire button

	close(fd);

	return js_data;
}