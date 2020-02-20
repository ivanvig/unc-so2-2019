#ifndef __REMOTE_H_
#define __REMOTE_H_

#define MAX_NAME_SIZE 50

#define SAT_GETTEL 0x1
#define SAT_UPDATE 0x2
#define SAT_SCAN 0x3

#define SV_PORT 6020
/* #define SV_IP "172.0.0.1" */
#define SV_IP "192.168.0.10"

struct telemetria {
	char id[MAX_NAME_SIZE];
	int uptime; //TODO: poner tipo correcto
	int version; //TODO: poner tipo correcto
	int usage; //TODO: poner tipo correcto
};

#endif // __REMOTE_H_
