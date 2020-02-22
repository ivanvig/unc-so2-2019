#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>

#include "client_utils.h"
#include "remote.h"

int gettel(struct telemetria *tel)
{
	struct sysinfo info;

	if (gethostname(tel->id, sizeof(tel->id))) {
		return -1;
	}

	if (sysinfo(&info)) {
		return -2;
	}

	tel->uptime = info.uptime;
	tel->load = info.loads[1];
	tel->ram_usage = (info.freeram * info.mem_unit) / 1024;

	strncpy(tel->ver, SOFT_VER, sizeof(tel->ver));

	return 0;
}
