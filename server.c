/*****************************************************************************
 * Name : Prakhar Keshari
 * Username : pkeshari
 * Course : CS3411 Fall 2017
 * Description : Program 5, 
 				server program that uses a select call to handle multiple connections
 ******************************************************************************
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void error(char *msg){
	perror(msg);
	exit(-1);
}


// void printsin(sin s1, s2)
// struct sockaddr_in *s_in; char *s1, *s2; 
// {
// 	printf("Program %s\n%s", s1, s2);
// 	printf("(%d, %d)\n", s_in->sin_addr.s_addr, s_in->sin_port);
// }
int main(int argc, char *argv[]){

	int listener, conn, client_status[30], i;
	int count_open = 3; 
	socklen_t length;
	// char ch, c;
	char *who[100];
	char buf[1024] = "";
	char msg[1024] = "";
	char msgto[1024] = "";
	char to[100] = "";
	int maxfd = 0;
	int j;
	int k;


	fd_set rfds;
    // struct timeval tv = {5, 1};
    int retval;
    int readval = 0;


	struct sockaddr_in s1, s2;

	listener = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char *)&s1, sizeof(s1));

	//filling in serv_addr values with the family of IP values from AF_INET
	s1.sin_family = AF_INET;
	//open to connection from anyone
	s1.sin_addr.s_addr = INADDR_ANY;
	//Get TCP/IP to provide any unused port
	s1.sin_port = htons(0);
	bind(listener, (struct sockaddr *)&s1, sizeof(s1));
	length = sizeof(s1);
	getsockname(listener, (struct sockaddr *)&s1, &length);
	printf("\n\nRSTREAM:: assigned port number %d\n\n", ntohs(s1.sin_port));
	listen(listener, 3);
	length = sizeof(s2);

	//initializing client sockets to not connected, 0 = not connected; 1 = requested nickname; 2 = connected
	for(i = 0; i < 30; i++){
		client_status[i] = 0;
	}


	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	FD_SET(listener, &rfds);
	maxfd = listener;


	while(1){

		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		FD_SET(listener, &rfds);
		// FD_SET(maxfd, &rfds);
		for(i = 4; i <= maxfd; i++){

			if(client_status[i] > 0){
				FD_SET(i, &rfds);
			}
		}

		retval = select(maxfd+1, &rfds, NULL, NULL, NULL);

		if(retval == -1){
			error("select failed");
		}

		if(FD_ISSET(listener, &rfds)){
			conn = accept(listener, (struct sockaddr*)&s2, &length);
			if(conn > maxfd){
				maxfd = conn;
			}
			// maxfd = conn;
			count_open++;
			write(conn, "Nickname? :\n", 10);
			// printf("Client with file descriptor %d is being asked for their nickname and count open is:%d\n\n", conn, count_open);
			client_status[conn] = 1;
			FD_SET(conn, &rfds);
		}

		else if(FD_ISSET(0, &rfds)){
			readval = read(0, buf, 1024);


			buf[readval] = '\0';
			sprintf(msg, "Olympus: %s%c", buf, '\n');

			for(i = 4; i <= maxfd; i++){
				if(client_status[i] == 2){
					write(i, msg, strlen(msg));
				}	
			}
			
		}

		else{
			//there is data to be read from a client
			for(i = 4; i <= maxfd; i++){
				if(FD_ISSET(i, &rfds)){
					// memset(buf, '\0', 1024);
					readval = read(i, buf, 1024);
					///////////////////
					if(readval == 0) {
						printf("client %s closed\n\n", who[i]);
						close(i);
						FD_CLR(i, &rfds);
						count_open--;
						client_status[i] = 0;
						// client_status[i] = 0;
					}
					buf[readval] = '\0';
					// write(1, &buf, readval-2);
					// printf("value of readval is: %d and it contains %s\n", readval, buf);

					if(client_status[i] == 2){

						if(buf[0] != '?'){
							sprintf(msg, "%s: %s%c", who[i], buf, '\n');

							for(j = 4; j <= maxfd; j++){
								if(j == i) continue;

								if(client_status[j] == 2){
									write(j, msg, strlen(msg));
								}
							}	
						}
						
						if(strncmp("?who", buf, 4) == 0){
							for(k = 4; k <= maxfd; k++){
								if(client_status[k] == 0){
									continue;
								}
									write(i, who[k], strlen(who[k]));
							}
						}

						if(strncmp("?bye", buf, 4) == 0){
							// maxfd = maxfd-1;
							close(i);
							FD_CLR(i, &rfds);
							count_open--;
							client_status[i] = 0;
							// printf("command is %s client %d %s closed the connection and their status is %d maxfd = %d\n", buf, i, who[i], client_status[i], maxfd);
						}

						if(strncmp("?to", buf, 3) == 0){

							int t = 5;
							int k = 0;
							while(buf[t] != '>'){
								// if(buf[t] == '>') break;
								k++;
								// to[k] = buf[t];
								t++;
								if(t == 20) break;
							}
							// printf("broke out of while with t=%d k=%d\n", t, k);
							strncpy(to, buf+5, k);
							to[k] = '\0';

							// printf("Message is being sent to %s\n", to);

							strncpy(msgto, buf+(8+k), strlen(buf) - (4+k));
							sprintf(msg, "%s: %s%c", who[i], msgto, '\n');
							printf("%s\n", msg);

							for(j = 4; j <= maxfd; j++){

								if(strncmp(to, who[j], 7) == 0){
									// printf("Message to is trying to be sent to = %s %lu who = %s %lu\n", to, strlen(to), who[i], strlen(who[i]));
									if(client_status[j] == 0){
										sprintf(msg, "%s %s%c", who[j], " is offline", '\n');

										write(i, msg, strlen(msg));
									}
									else{
										write(j, msg, strlen(msg));

									}
								}
							}
						}
					}

					if(client_status[i] == 1){
						// buf[readval] = '\0';

						who[i] = (char*) malloc((strlen(buf)+1) * sizeof(char));
						strcat(who[i], buf);
						client_status[i] = 2;
						// printf("Client %d has connection status %d and nickname: %s\n", i, client_status[i], who[i]);
						// write(i, who[i], strlen(who[i])+1);

					}
				}
			}
		}
	}
	return 0;
}






