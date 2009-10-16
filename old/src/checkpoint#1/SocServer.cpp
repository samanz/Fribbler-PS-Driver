#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKNAME	"local_socket"

void	die(char const message[])
{
	perror(message);
}
void copyData(int from, int to)
{
	char	buf[1024];
	int	amount;

	while ((amount = read(from, buf, sizeof(buf))) > 0)
	{
		if (write(to, buf, amount) != amount)
		{
			die("write");
			return;
		}
	}
	if (amount < 0)
		die("read");
}

int main(int argc, char *argv[])
{
	int	soc;
	struct sockaddr_un sa;
	size_t	addrLen;


	unlink(SOCKNAME);


	soc = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (soc < 0)
	{
		printf("Bad Sock\n");
		exit(1);
	}

	unlink(SOCKNAME);

	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, SOCKNAME);

	addrLen = sizeof(sa.sun_family) + strlen(sa.sun_path);

	int	ret;

	ret = bind(soc, (struct sockaddr *)&sa, addrLen);
	if (ret < 0)
	{
		printf("Bad Bind\n");
		exit(1);
	}

	ret = listen(soc, 5);
	if (ret < 0)
	{
		printf("Bad Listen\n");
		exit(1);
	}



	int fd;

	fd = accept(soc, (struct sockaddr *)&sa, &addrLen);
	if (fd < 0)
	{
		printf("Bad accept\n");
		exit(1);
	}


	copyData(fd, 1);

	unlink(SOCKNAME);

	return 0;
}	
