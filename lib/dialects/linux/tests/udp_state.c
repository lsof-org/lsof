/* Keep connected/unconnected UDP and listening/connected TCP sockets in one PID.
 * Print their descriptors only once all sockets are ready for inspection.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int bound_socket(int family, int type, struct sockaddr_storage *address,
                        socklen_t *length) {
    int fd = socket(family, type, 0);
    if (fd < 0)
        return -1;
    memset(address, 0, sizeof(*address));
    address->ss_family = family;
    if (family == AF_INET) {
        struct sockaddr_in *v4 = (struct sockaddr_in *)address;
        v4->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        *length = sizeof(*v4);
    } else {
        struct sockaddr_in6 *v6 = (struct sockaddr_in6 *)address;
        v6->sin6_addr = in6addr_loopback;
        *length = sizeof(*v6);
    }
    if (bind(fd, (struct sockaddr *)address, *length) < 0 ||
        getsockname(fd, (struct sockaddr *)address, length) < 0) {
        int saved_errno = errno;
        close(fd);
        errno = saved_errno;
        return -1;
    }
    return fd;
}

int main(int argc, char **argv) {
    struct sockaddr_storage udp_address, tcp_address;
    socklen_t udp_length, tcp_length;
    int family, udp_idle, udp_connected, tcp_listener, tcp_client, tcp_server;

    if (argc != 2 || (strcmp(argv[1], "4") && strcmp(argv[1], "6")))
        return 1;
    family = !strcmp(argv[1], "4") ? AF_INET : AF_INET6;
    udp_idle = bound_socket(family, SOCK_DGRAM, &udp_address, &udp_length);
    if (udp_idle < 0) {
        int unavailable = family == AF_INET6 &&
                          (errno == EAFNOSUPPORT || errno == EADDRNOTAVAIL ||
                           errno == EPROTONOSUPPORT);
        perror("UDP bind");
        return unavailable ? 77 : 1;
    }
    udp_connected = socket(family, SOCK_DGRAM, 0);
    if (udp_connected < 0 ||
        connect(udp_connected, (struct sockaddr *)&udp_address, udp_length) < 0)
        goto error;
    tcp_listener = bound_socket(family, SOCK_STREAM, &tcp_address, &tcp_length);
    if (tcp_listener < 0 || listen(tcp_listener, 1) < 0)
        goto error;
    tcp_client = socket(family, SOCK_STREAM, 0);
    if (tcp_client < 0 ||
        connect(tcp_client, (struct sockaddr *)&tcp_address, tcp_length) < 0)
        goto error;
    tcp_server = accept(tcp_listener, NULL, NULL);
    if (tcp_server < 0)
        goto error;

    printf("%d %d %d %d %d\n", udp_idle, udp_connected, tcp_listener,
           tcp_client, tcp_server);
    fflush(stdout);
    for (;;)
        pause();
error:
    perror("socket setup");
    return 1;
}
