/* This provides a crude manner of testing the performance of a broker in messages/s. */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <mosquitto.h>

#define MESSAGE_COUNT 20000

static bool run = true;
static int message_count = 0;
static struct timeval start, stop;

void my_connect_callback(void *obj, int rc)
{
	printf("rc: %d\n", rc);
	gettimeofday(&start, NULL);
}

void my_disconnect_callback(void *obj)
{
	run = false;
}

void my_publish_callback(void *obj, uint16_t mid)
{
	message_count++;
	//printf("%d ", message_count);
	if(message_count == MESSAGE_COUNT){
		gettimeofday(&stop, NULL);
		mosquitto_disconnect((struct mosquitto *)obj);
	}
}

int create_data(void)
{
	int i;
	FILE *fptr, *rnd;
	int rc = 0;
	char buf[100];

	fptr = fopen("msgsps_pub.dat", "rb");
	if(fptr){
		fseek(fptr, 0, SEEK_END);
		if(ftell(fptr) == 100*MESSAGE_COUNT){
			fclose(fptr);
			return 0;
		}
		fclose(fptr);
	}

	fptr = fopen("msgsps_pub.dat", "wb");
	if(!fptr) return 1;
	rnd = fopen("/dev/urandom", "rb");
	if(!rnd){
		fclose(fptr);
		return 1;
	}

	for(i=0; i<MESSAGE_COUNT; i++){
		if(fread(buf, sizeof(char), 100, rnd) != 100){
			rc = 1;
			break;
		}
		if(fwrite(buf, sizeof(char), 100, fptr) != 100){
			rc = 1;
			break;
		}
	}
	fclose(rnd);
	fclose(fptr);

	return rc;
}

int main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int i;
	double dstart, dstop, diff;
	FILE *fptr;
	uint8_t buf[100];

	start.tv_sec = 0;
	start.tv_usec = 0;
	stop.tv_sec = 0;
	stop.tv_usec = 0;

	if(create_data()){
		printf("Error: Unable to create random input data.\n");
		return 1;
	}
	fptr = fopen("msgsps_pub.dat", "rb");
	if(!fptr){
		printf("Error: Unable to open random input data.\n");
		return 1;
	}
	mosquitto_lib_init();

	mosq = mosquitto_new("perftest", NULL);
	mosquitto_connect_callback_set(mosq, my_connect_callback);
	mosquitto_disconnect_callback_set(mosq, my_disconnect_callback);
	mosquitto_publish_callback_set(mosq, my_publish_callback);

	mosquitto_connect(mosq, "127.0.0.1", 1885, 600, true);
	for(i=0; i<MESSAGE_COUNT; i++){
		fread(buf, sizeof(uint8_t), 100, fptr);
		mosquitto_publish(mosq, NULL, "perf/test", 100, buf, 0, false);
	}

	while(!mosquitto_loop(mosq, 1) && run){
	}
	dstart = (double)start.tv_sec*1.0e6 + (double)start.tv_usec;
	dstop = (double)stop.tv_sec*1.0e6 + (double)stop.tv_usec;
	diff = (dstop-dstart)/1.0e6;

	printf("Start: %g\nStop: %g\nDiff: %g\nMessages/s: %g\n", dstart, dstop, diff, (double)MESSAGE_COUNT/diff);

	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	fclose(fptr);

	return 0;
}