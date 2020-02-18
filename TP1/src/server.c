#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define SERV_PORT 6020
#define N_MAX_CONN 10
#define SNAME
#define DPROMPT ">> "


struct connect_descriptor {
        int sockfd;
        uint8_t valid;
        char name[50];
};

void start_remote_conection(int sockfd_local);
void start_cli(int sockfd_local);
void list_connections(struct connect_descriptor *conn_table);
int get_conn_fd(char* name, struct connect_descriptor *conn_table);
void sat_connect(char* name, struct connect_descriptor *conn_table, int *sockfd, char *prompt, size_t prompt_size);
void listen_setup(int *sockfd, struct sockaddr_in *serv_addr);

int main(int argc, char const *argv[])
{
        int sv[2];
        int pid;


        if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) < 0) {
                perror("[!] ERROR: No se pudo crear socket local para CLI");
                exit(EXIT_FAILURE);
        }

        switch ((pid = fork())) {
        case 0:
                close(sv[1]);
                start_remote_conection(sv[0]);
                break;
        case -1:
                perror("[!] ERROR: Un error ha ocurrido durante la creacion de la CLI");
                exit(EXIT_FAILURE);
        default:
                close(sv[0]);
                start_cli(sv[1]);
                break;
        }

        return EXIT_SUCCESS;
}

void start_cli(int sockfd_local)
{
        ssize_t nchr_read;
        char* line = NULL;
        char* op;
        char prompt[32];
        size_t lsize = 0;
        int sockfd;

        strncpy(prompt, DPROMPT, sizeof(prompt));

        struct connect_descriptor *connection_table;

        if ((connection_table = (struct connect_descriptor *) calloc(N_MAX_CONN, sizeof(struct connect_descriptor)))) {
                perror("[!] ERROR: No se pudo allocar memoria para tabla de conecciones");
                exit(EXIT_FAILURE);
        }

        while (1) {
                printf("%s", prompt);
                if ((nchr_read = getline(&line, &lsize, stdin)) < 0) {
                        perror("[!] ERROR: CLI input error");
                }

                if (!nchr_read) continue;

                line[nchr_read - 1] = '\0';
                op = strtok(line, " ");

                if (!strcmp(op, "list"))
                        list_connections(connection_table);
                else if (!strcmp(op, "connect"))
                        sat_connect(strtok(NULL, " "), connection_table, &sockfd, prompt, sizeof(prompt));
                else
                        write(sockfd, "kawabonga\n", 10);

        }


}

void list_connections(struct connect_descriptor *conn_table)
{
        for (int i = 0; i < N_MAX_CONN; i++) {
                if (conn_table[i].valid) {
                        printf("%s", conn_table[i].name);
                }
        }
}

int get_conn_fd(char* name, struct connect_descriptor *conn_table)
{
        for (int i = 0; i < N_MAX_CONN; i++) {
                if (conn_table[i].valid && !strcmp(name, conn_table[i].name)) {
                        return conn_table[i].sockfd;
                }
        }
        return -1;
}

void sat_connect(char* name, struct connect_descriptor *conn_table, int *sockfd, char* prompt, size_t prompt_size)
{
        int fd_aux = 0;

        if ((fd_aux = get_conn_fd(name, conn_table)) < 0) {
                printf("[!] Nombre invalido '%s'", name);
                return;
        }

        *sockfd = fd_aux;

        strncpy(prompt, name, prompt_size);
        strncat(prompt, DPROMPT, prompt_size-strlen(prompt)-1);
}

void listen_setup(int *sockfd, struct sockaddr_in *serv_addr)
{

        if (!(*sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
                perror("[!] ERROR: El socket no pudo ser creado.\n");
                exit(EXIT_FAILURE);
        }

        memset(serv_addr, 0, sizeof(*serv_addr));
        serv_addr->sin_family = AF_INET;
        serv_addr->sin_addr.s_addr = INADDR_ANY;
        serv_addr->sin_port = htons(SERV_PORT);

        if (bind(*sockfd, (struct sockaddr*) serv_addr, sizeof(*serv_addr))) {
                perror("[!] ERROR: No se pudo hacer el bind del socket");
                exit(EXIT_FAILURE);
        }

        if (listen(*sockfd, N_MAX_CONN)){
                perror("[!] ERROR: No se pudo poner el socket a la escucha");
                exit(EXIT_FAILURE);
        }

        printf("[*] Servidor listo a la escucha en puerto %d\n", ntohs(serv_addr->sin_port));
}

ssize_t write_sockfd(int sockfd_local, int sockfd_remote)
{
        struct msghdr msg = { 0 };
        struct cmsghdr *cmsg;

        char iobuf[1];
        struct iovec io = {
                .iov_base = iobuf,
                .iov_len = sizeof(iobuf)
        };
        union {
                char buf[CMSG_SPACE(sizeof(sockfd_remote))];
                struct cmsghdr align;
        } u;

        msg.msg_iov = &io;
        msg.msg_iovlen = 1;
        msg.msg_control = u.buf;
        msg.msg_controllen = sizeof(u.buf);
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(sockfd_remote));
        memcpy(CMSG_DATA(cmsg), &sockfd_remote, sizeof(sockfd_remote));

        return sendmsg(sockfd_local, &msg, 0);
}

ssize_t read_sockfd(int sockfd_local, int *sockfd_remote)
{
        ssize_t recv_size;

        struct msghdr msg = { 0 };
        struct cmsghdr *cmsg;

        char iobuf[1];
        struct iovec io = {
                .iov_base = iobuf,
                .iov_len = sizeof(iobuf)
        };
        union {
                char buf[CMSG_SPACE(sizeof(*sockfd_remote))];
                struct cmsghdr align;
        } u;

        msg.msg_iov = &io;
        msg.msg_iovlen = 1;
        msg.msg_control = u.buf;
        msg.msg_controllen = sizeof(u.buf);

        if ((recv_size = recvmsg(sockfd_local, &msg, 0) < 0)) {
                goto finish;
        }

        cmsg = CMSG_FIRSTHDR(&msg);

        if (
                cmsg
                && cmsg->cmsg_len == CMSG_LEN(sizeof(*sockfd_remote))
                && cmsg->cmsg_level == SOL_SOCKET
                && cmsg->cmsg_type == SCM_RIGHTS
                ) {

                memcpy(sockfd_remote, CMSG_DATA(cmsg), sizeof(*sockfd_remote));

        } else {
                *sockfd_remote = -1;
        }

finish:
        return recv_size;
}

void load_connection(char* name, int sockfd, struct connect_descriptor *connection_table, int nentries)
{
        for (int i = 0; i < nentries; i++) {
                if (!connection_table[i].valid) {
                        strncpy(connection_table[i].name, name, sizeof(connection_table[i].name));
                        connection_table[i].sockfd = sockfd;
                        connection_table[i].valid = 1;
                }
        }
}

void start_remote_connection(int sockfd_local)
{
        int sockfd, connect_sockfd;
        struct sockaddr_in connect_addr, serv_addr;
        socklen_t connect_addr_len = sizeof(connect_addr);

        struct connect_descriptor *connection_table;

        struct pollfd fds[N_MAX_CONN+2];
        int ret, nfd;

        if ((connection_table = (struct connect_descriptor *) calloc(N_MAX_CONN, sizeof(struct connect_descriptor)))) {
                perror("[!] ERROR: No se pudo allocar memoria para tabla de conecciones");
                exit(EXIT_FAILURE);
        }

        listen_setup(&sockfd, &serv_addr);

        fds[0].fd = sockfd;
        fds[0].events = POLLIN;

        fds[1].fd = sockfd_local;
        fds[1].events = POLLIN;

        nfd = 2;

        while (1) {

                if ((ret = poll(fds, nfd, -1)) <= 0) {
                        perror("[!] ERROR: Un error ocurrio haciendo polling");
                        exit(EXIT_FAILURE);
                }

                for (int i = 0; i < nfd; i++) {
                        if (fds[i].revents & POLLIN){
                                if (i == 0) {
                                        if ((connect_sockfd = accept(fds[i].fd, (struct sockaddr*) &connect_addr, &connect_addr_len)) < 0) {
                                                perror("[!] ERROR: La conexion no se pudo establecer");
                                                exit(EXIT_FAILURE);
                                        }
                                        load_connection("asd", connect_sockfd, connection_table, N_MAX_CONN);
                                } else if (i == 1) {
                                        command_handler(fds[i].fd);
                                }
                        } else if (fds[i].revents & POLLHUP) {
                                printf("Algo se cerro che");
                        }
                }
        }
        write_sockfd(sockfd_local, connect_sockfd);
        close(connect_sockfd);
}
