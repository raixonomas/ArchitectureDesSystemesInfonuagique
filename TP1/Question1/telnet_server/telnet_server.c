#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>

#define PORT 23 //TELNET PORT

int send_all(int socket_fd, const char *msg){
    size_t total_sent = 0;
    size_t msg_len = strlen(msg);

    while(total_sent < msg_len) {
        ssize_t sent = send(socket_fd, msg + total_sent, msg_len - total_sent, 0);
        if(sent==0){return -1;}
        if(sent < 0){
            if(errno == EINTR){
                continue;
            }
            return -1;
        }
        total_sent += (size_t)sent;
    }
    return 0;
}


#define IAC 255
#define DONT 254
#define DO 253
#define WONT 252
#define WILL 251

ssize_t recv_telnet(int socket_fd, char *buffer, size_t size){
    size_t used = 0;
    while (used < size -1){
        unsigned char byte;
        ssize_t received = recv(socket_fd, &byte, 1, 0);
        if(received <= 0){return received;}
        if (byte == IAC){
            unsigned char cmd;
            unsigned char opt;
            if(recv(socket_fd, &cmd, 1, 0) <= 0){return -1;}
            if(cmd==IAC){continue;}
            if(cmd==DO || cmd==DONT || cmd==WILL || cmd==WONT){
                if(recv(socket_fd, &opt, 1, 0) <= 0){return -1;}
                unsigned char res[3];
                if(cmd==DO || cmd==DONT){
                    res[0]= IAC;
                    res[1]= WONT;
                    res[2]= opt;
                } else {
                    res[0]= IAC;
                    res[1]= DONT;
                    res[2]= opt;
                }
                send(socket_fd, res, sizeof(res), MSG_NOSIGNAL);
            }
            continue;
        }
        if(byte == '\n'){break;}
        if(byte != '\r'){buffer[used++]=(char)byte;}
    }
    buffer[used] = '\0';
    return (ssize_t)used;
}

void *calculate_imc(void *arg){
    int conn_sock = (int)(intptr_t)arg;

    const char* conn_msg =
        "+----------------------------+ \r\n"
        "|                            | \r\n"
        "|    Calculate your IMC !    | \r\n"
        "|                            | \r\n"
        "+----------------------------+ \r\n";
    send_all(conn_sock, conn_msg);

    const char* err_msg = "Please enter a valid number.\r\n";

    const char* weight_msg = "Enter your weight in kg: ";
    send_all(conn_sock, weight_msg);

    char weight[32];
    ssize_t weight_received = recv_telnet(conn_sock, weight, sizeof(weight));
    int weight_i;
    if(weight_received <= 0){
        close(conn_sock);
        return NULL;
    }
    if(weight_received > 0){
        weight[weight_received] = '\0';
        weight_i = atoi(weight);
        if(weight_i <= 0){
            send_all(conn_sock, err_msg);
            close(conn_sock);
            return NULL;
        }
    }
    printf("client answer on weight: %d \r\n", weight_i);

    const char* height_msg = "Enter your height in centimeters: ";
    send_all(conn_sock, height_msg);

    char height[32];
    ssize_t height_received = recv_telnet(conn_sock, height, sizeof(height));
    float height_i;
    if(height_received <= 0){
        close(conn_sock);
        return NULL;
    }
    if(height_received > 0){
        height[height_received] = '\0';
        int height_m = atoi(height);
        if(height_m <= 0){
            send_all(conn_sock, err_msg);
            close(conn_sock);
            return NULL;
        }
        height_i = height_m / 100.0f;
    }
    printf("client answer on height: %.2f \r\n", height_i);

    float client_imc = (float)weight_i /(height_i*height_i);
    
	char cat_imc_msg[32];
    if(client_imc < 16.0){
        snprintf(cat_imc_msg, sizeof(cat_imc_msg), "You are severely underweight !\n\r");
    }
    else if(16.0 <= client_imc <= 16.9){
        snprintf(cat_imc_msg, sizeof(cat_imc_msg), "You are moderately underweight !\n\r");
    }
    else if(17.0 <= client_imc <= 18.4){
        snprintf(cat_imc_msg, sizeof(cat_imc_msg), "You are slightly underweight !\n\r");
    }
    else if(18.5 <= client_imc < 24.9){
        snprintf(cat_imc_msg, sizeof(cat_imc_msg), "You have a normal weight !\n\r");
    }
    else if(25.0 <= client_imc <= 29.9){
        snprintf(cat_imc_msg, sizeof(cat_imc_msg), "You are overweight !\n\r");
    }
    else if(client_imc >= 30.0){
        snprintf(cat_imc_msg, sizeof(cat_imc_msg), "You are obese !\n\r");
    }
    printf("client IMC: %.2f \r\n", client_imc);
    char imc_msg[32];
    snprintf(imc_msg, sizeof(imc_msg), "Your IMC is %.2f\n\r",client_imc);
    send_all(conn_sock, imc_msg);
    send_all(conn_sock, cat_imc_msg);
   
    char end[8] = "\n\r";
    send_all(conn_sock, end);

    // shutdown and close client socket
    shutdown(conn_sock, SHUT_RDWR);
    close(conn_sock);

    //terminating thread
    return NULL;
}

int main(int argc, char **argv)
{
    // -------- SOCKET
    //
    // create listen socket
    // -- AF_INET = IPv4 Internet protocols
    // -- SOCK_STREAM = sequenced, reliable, two-way, connection-based byte streams use for TCP
    // -- See 'man socket' for further information
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Listen socket created successfully ! \r\n");

    int opt = 1;
    // help manipulating opt for socket, and reuse address and port
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(socket_fd, SOL_SOCKET,SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    addr.sin_port = htons(PORT); // PORT formatting

    //bind socket to telnet port
    if(bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket bind to telnet port successfully ! \r\n");


    listen(socket_fd, 3);
    printf("Listening to port %d... \r\n", PORT);

    for(;;){
        // --------- CLIENT SOCKET
        //
        // Create socket for new client when a client is connected
        socklen_t addrlen = sizeof(addr);
        int conn_sock = accept(socket_fd, (struct sockaddr*)&addr, &addrlen);
        if(conn_sock < 0){
            perror("connection socket accept failed");
            exit(EXIT_FAILURE);
        }
        printf("new client connected: %s\n",inet_ntoa(addr.sin_addr));

        // create thread for each connection
        pthread_t th;

        int res_thread = pthread_create(&th, NULL, calculate_imc, (void *)(intptr_t)conn_sock);

        if (res_thread != 0) {
            fprintf(stderr, "pthread_create failed\n");
                close(conn_sock);
        } else {
            pthread_detach(th);
        }
    }

    // -- At end, close all socket
    close(socket_fd);

    return 0;
}
