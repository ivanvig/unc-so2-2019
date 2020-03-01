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

#define CONN_BW 100 // Mbits/s
/* #define CONN_RTT 20 // ms */
#define CONN_RTT 1 // ms
#define BWD_PROD (1000 * CONN_BW * CONN_RTT / 8)
#define OPT_SOCK_BUF (4 * BWD_PROD / (3 * 2))

#define TCP_HDR 20
#define IP_HDR 20
#define MTU 1500
#define MSS MTU - IP_HDR - TCP_HDR

#define SV_PORT 6020
#define SV_UDPPORT 6021
#define SV_IP "192.168.0.100"
/* #define SV_IP "192.168.2.107" */

struct telemetria {
	char id[MAX_NAME_SIZE];
	long uptime;
	char ver[VER_BUF_SIZE];
	unsigned long load;
	unsigned long ram_usage;
};

#endif // __REMOTE_H_
