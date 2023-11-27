#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>

#define MY_PORT "4950"   // the port users will be connecting to
#define BUFFER_SIZE 500  // BUFFER_SIZE as per required for each segment (e.g: 500)


//struct for data packets
struct packet {
    int sequence_no;
    int packet_size;
    char data[BUFFER_SIZE];
    uint16_t checksum;
};


// Checksum function
uint16_t calculateChecksum(const char* data, size_t length) {
    uint32_t sum = 0;
    for (size_t i = 0; i < length; i++) {
        sum += data[i];
        if (sum & 0xFFFF0000) {
            sum &= 0xFFFF;
            sum++;
        }
    }
    return (uint16_t)(~sum);
}

// Set the checksum for a packet
void setChecksum(struct packet* pkt) {
    pkt->checksum = 0; // Reset the checksum field
    pkt->checksum = calculateChecksum((char*)pkt, sizeof(struct packet));
}

// Verify the checksum of a received packet
int verifyChecksum(const struct packet* pkt) {
    uint16_t originalChecksum = pkt->checksum;
    struct packet temp = *pkt;
    temp.checksum = 0;
    uint16_t calculatedChecksum = calculateChecksum((char*)&temp, sizeof(struct packet));
    return originalChecksum == calculatedChecksum;
}


// variables for socket
int socket_fd; // listen on socket_fd
struct addrinfo serv_addr, *serv_info, *ptr; // SERVER's address information
struct sockaddr_storage cli_addr; // client's address information
socklen_t cli_addr_len = sizeof (struct sockaddr_storage);
char ip_addr[INET6_ADDRSTRLEN];
int rv;

// variables for video file
int no_of_bytes = 0;
int out;
int file_size;
int remaining = 0;
int received = 0;

// variables for transferring data paackets and acks
int no_of_packets = 5; // window size is 5
struct packet temp_packet;
struct packet packets[5];
int no_of_acks;
int acks[5];
int temp_ack;


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


// Function to receive packets
void* receivePackets(void *vargp) {
    // Receive 5 packets
    for (int i = 0; i < no_of_packets; i++) {
        RECEIVE:
        if ((no_of_bytes = recvfrom(socket_fd, &temp_packet, sizeof(struct packet), 0, (struct sockaddr *)&cli_addr, &cli_addr_len)) < 0) {
            perror("UDP SERVER: recvfrom");
            exit(1);
        }

        if (verifyChecksum(&temp_packet)) {
            temp_ack = temp_packet.sequence_no;
            // In case of duplicate packet
            if (packets[temp_ack].packet_size != 0) {
                // Reallocate the array
                packets[temp_ack] = temp_packet;
                // Create an ack
                acks[temp_ack] = 1;
                // Send the ack
                if (sendto(socket_fd, &temp_ack, sizeof(int), 0, (struct sockaddr *)&cli_addr, cli_addr_len) < 0) {
                    perror("UDP SERVER: sendto");
                    exit(1);
                }
                printf("Duplicate Ack Sent: %d\n", temp_ack);
                // Receive packet again until a unique packet is sent
                goto RECEIVE;
            }
			

            // In case of last packet
            if (temp_packet.packet_size == -1) {
                printf("Last packet found\n");
                // Decrementing the counter of the remaining loops
                no_of_packets = temp_packet.sequence_no + 1;
            }

            // In case of unique packet
            if (no_of_bytes > 0) {
                printf("Packet Received: %d\n", temp_packet.sequence_no);
                // Keep the correct order of packets by index of the array
                packets[temp_ack] = temp_packet;
            }
        }
    }
    return NULL;
}


// main function
int main(void) {

	memset(&serv_addr, 0, sizeof serv_addr); // ensure the struct is empty
	serv_addr.ai_family = AF_UNSPEC;         // set to AF_INET to force IPv4
	serv_addr.ai_socktype = SOCK_DGRAM;      //UDP socket datagrams
	serv_addr.ai_flags = AI_PASSIVE;         // fill in my IP

	if ((rv = getaddrinfo(NULL, MY_PORT, &serv_addr, &serv_info)) != 0) {
		fprintf(stderr, "UDP SERVER: getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results in the linked list and bind to the first we can
	for(ptr = serv_info; ptr != NULL; ptr = ptr->ai_next) {
		if ((socket_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) {
			perror("UDP SERVER: socket");
			continue;
		}

		// bind socket
		if (bind(socket_fd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
			close(socket_fd);
			perror("UDP SERVER: bind");
			continue;
		}

		break;
	}

	if (ptr == NULL) {
		fprintf(stderr, "UDP SERVER: Failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(serv_info); // all done with this structure

	printf("UDP SERVER: Waiting for recieve datagrams...\n");
    
	pthread_t thread_id; // create thread ID
	
	// time delay variables
	struct timespec time1, time2;
	time1.tv_sec = 0;
	time1.tv_nsec = 3000000000L;  // 0.03 seconds

	FILE * out = fopen("recieved/recieve.mp4","wb"); // open the video file in write mode		
	
	// receiving the size of the video file from the client
	if ((no_of_bytes = recvfrom(socket_fd, &file_size, sizeof(off_t), 0, (struct sockaddr *)&cli_addr, &cli_addr_len)) < 0) {
		perror("UDP SERVER: recvfrom");
		exit(1);
	}
	printf("Size of Video File to be received: %d bytes\n", file_size);

	no_of_bytes = 1;
	remaining = file_size;

	while (remaining > 0 || (no_of_packets == 5)) {

		// reinitialize the arrays
		
		memset(packets, 0, sizeof(packets));
        	for (int i = 0; i < 5; i++) {
        		packets[i].packet_size = 0; 
        	}

        	for (int i = 0; i < 5; i++) {
        		acks[i] = 0; 
        	}
               
        	// SERVER starts receiving packets i.e thread execution starts
		pthread_create(&thread_id, NULL, receivePackets, NULL);

        	// wait for packets to be received i.e the code sleeps for 0.03 seconds
        	nanosleep(&time1, &time2);

		no_of_acks = 0;

		// send acks for the packets received only
		RESEND_ACK:
		for (int i = 0; i < no_of_packets; i++) {
			temp_ack = packets[i].sequence_no;
			// if the ack has not been sent before
			if (acks[temp_ack] != 1) {
				// create acks for the packets received ONLY
				if (packets[i].packet_size != 0) {
					acks[temp_ack] = 1;

					// send acks to the client
					if(sendto(socket_fd, &temp_ack, sizeof(int), 0, (struct sockaddr *)&cli_addr, cli_addr_len) > 0) {
						no_of_acks++;
						printf("Ack sent: %d\n", temp_ack);
					}
				}
			}
		}

		// stop n wait
		// wait for acks to be sent and processed by the client
		nanosleep(&time1, &time2);
		nanosleep(&time1, &time2);

		// if all packets were not received
		if (no_of_acks < no_of_packets) {
			goto RESEND_ACK;
		}
                
		// 5 packets have been received i.e. the thread executes successfully
		pthread_join(thread_id, NULL);
                 
		// write packets to output file
		for (int i = 0; i < no_of_packets; i++) {
			// data is present in the packets and its not the last packet
			if (packets[i].packet_size != 0 && packets[i].packet_size != -1) {
				printf("Writing packet: %d\n", packets[i].sequence_no);
				fwrite(packets[i].data, 1, packets[i].packet_size, out);
				remaining = remaining - packets[i].packet_size;
				received = received + packets[i].packet_size;
			}
		}

		printf("Received data:  %d bytes\nRemaining data: %d bytes\n", received, remaining);
		
		// repeat process for the next 5 packets
	}
	
	printf("\nUDP SERVER: Recieved video file from client %s\n", inet_ntop(cli_addr.ss_family, get_in_addr((struct sockaddr *)&cli_addr), ip_addr, sizeof ip_addr));
	printf("File recieve completed successfully!\n");
    	close(socket_fd); // close SERVER socket
    	return 0;
}