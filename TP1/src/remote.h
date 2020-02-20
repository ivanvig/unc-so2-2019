#ifndef __REMOTE_H_
#define __REMOTE_H_

#define MAX_NAME_SIZE 50

#define SAT_GETTEL 0x1

struct telemetria {
	char id[MAX_NAME_SIZE];
	int uptime; //TODO: poner tipo correcto
	int version; //TODO: poner tipo correcto
	int usage; //TODO: poner tipo correcto
};

#endif // __REMOTE_H_
