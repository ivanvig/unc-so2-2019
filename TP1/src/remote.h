#ifndef __REMOTE_H_
#define __REMOTE_H_

#define MAX_NAME_SIZE 32
#define VER_BUF_SIZE 8

#define SAT_GETTEL 0x01
#define SAT_UPDATE 0x02
#define SAT_SCAN 0x03
#define SAT_ERR 0xEE
#define SAT_INV 0xEA
#define SAT_OK 0xAA

#define SV_PORT 6020
/* #define SV_IP "172.0.0.1" */
#define SV_IP "192.168.0.10"

struct telemetria {
	char id[MAX_NAME_SIZE];
	long uptime;
	char ver[VER_BUF_SIZE];
	unsigned long load;
	unsigned long ram_usage;
};

#endif // __REMOTE_H_
