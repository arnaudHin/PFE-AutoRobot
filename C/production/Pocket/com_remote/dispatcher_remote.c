/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/**
 * @file  dispatcher_remote.c
 * @author Montet Julien & Marbeuf Pierre
 * @version 3.0
 * @date 11/06/2020
 * @brief Dispatcher network
 *
 */
#include "dispatcher_remote.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "../util.h"
#include <unistd.h>
#include <arpa/inet.h>

#include "postman_remote.h"
#include "../commun.h"
#include "remote_protocol.h"
#include "../pilot/pilot.h"

#define SIZE_MSG_CMD (1)
#define SIZE_MSG_SIZE (3)
#define SIZE_COORD (2)


static pthread_t pthread;
static Message_from_jump_t myMessageFromJump;
static uint8_t myBufferFromJump[BUFF_SIZE_TO_RECEIVE];
static uint8_t ask_quit = 0;





static void * dispatcher_remote_run();
static void dispatcher_remote_check_for_message();



/**
 *  \fn static void dispatcher_remote_start()
 *
 *  \brief Function dedicated to start the dispatcher and postman and pilot.
 */
extern void dispatcher_remote_start()
{
        postman_remote_start();
        
		int returnError = pthread_create(&pthread,NULL,&dispatcher_remote_run,NULL);
        if (returnError == -1)
        {
                perror("Error : creation of thread incorrect in remoteUI start");
                exit(EXIT_FAILURE);
        }
}


/**
 *  \fn void dispatcher_remote_stop()
 *
 *  \brief Function dedicated stop the dispatcher
 *
 */
extern void dispatcher_remote_stop()
{
        TRACE("DISPATCHER STOP \r\n");

        int returnCode = pthread_join(pthread,NULL);
        
		TRACE("AFTER DISPATCHER STOP \r\n");
        if (returnCode == -1)
        {
                perror("Error : join thread incorrect");
                exit(EXIT_FAILURE);
        }

		postman_remote_stop();

}


/**
 *  \fn void interpret_24_bit_as_int32()
 *
 *  \brief Function dedicated convert an hexa-string of 3 bytes in int
 *
 *  \param byte_array (char *) : byte_array of 3 bytes to convert
 *
 *  \return int : value converted
 *
 */
static int interpret_24_bit_as_int32(unsigned char * byte_array) {
    return (
        (byte_array[0] << 24) |
        (byte_array[1] << 16) |
        (byte_array[2] << 8)) >> 8;
}







/**
 *  \fn static void dispatcher_remote_run()
 *
 *  \brief Function dedicated to constantly read the socket
 *             Once the message is received, it decodes it and requests a dispatch
 */
static void * dispatcher_remote_run(){

	while (ask_quit != 1){
		fprintf(stderr, "Attente msg from jump...\n");
		dispatcher_remote_check_for_message(); //function block !! 

		CMD_from_jump_e commandMsg = myMessageFromJump.command;

		switch (commandMsg)
		{
			case ASK_TRY_DIR:
				
				pilot_set_direction(myMessageFromJump.data.direction);
				//fprintf(stderr, "%s:%d:%s(): "__FILE__, __LINE__, __func__);
				fprintf(stderr, "dispatcher_remote_run\n");

				break;
			case ASK_QUIT:
				ask_quit = 1;

			case ASK_SET_MODE:

				break;
			default:
				break;
		}
	}
}


static void dispatcher_remote_check_for_message(){

	ssize_t resultRead = 0;
    ssize_t byteToRead = CMD_SIZE_BYTE + 2;

	resultRead = postman_remote_receive(myBufferFromJump, byteToRead);
	if(resultRead == -1 || resultRead == 0){
		fprintf(stderr, "ERROR POSTMAN RECEIVE 0 : %d\n", resultRead);
	}

	//DECODE myBufferFromJump -> myMessageFromJump (CMD + SIZEdata)
	remote_protocol_decode(myBufferFromJump, &myMessageFromJump, byteToRead);

	if(myMessageFromJump.sizeData != 0){
		byteToRead = myMessageFromJump.sizeData;
		uint8_t myTempBuffer[byteToRead];
		memset(myTempBuffer, 0x00, sizeof(myTempBuffer) );

		resultRead = postman_remote_receive(myTempBuffer, byteToRead);
		if(resultRead == -1){
			fprintf(stderr, "ERROR POSTMAN RECEIVE DATA : %d\n", resultRead);
		}
		
		//DECODE myTempBuffer -> myMessageFromJump (DATA)
		remote_protocol_decode(myTempBuffer, &myMessageFromJump, byteToRead);

	}

	fprintf(stderr, "\nCMD : %d | ", myMessageFromJump.command);
	fprintf(stderr, "Size : %d | ", myMessageFromJump.sizeData);
	fprintf(stderr, "Data dir : %d \n", myMessageFromJump.data.direction);

}







// static void dispatcher_decod(unsigned char * buffer, Network_msg * network_msg)
// {

// 	network_msg->cmd = NOP;
// 	network_msg->size = 0;
// 	// network_msg->coord.x = 0;
// 	// network_msg->coord.y = 0;

// 	unsigned char size[SIZE_MSG_SIZE];

// 	// Get value of CMD
// 	memcpy(&network_msg->cmd, &buffer[0], SIZE_MSG_CMD);

// 	// Get value of SIZE
// 	for(int j=0;j<sizeof(size);j++){
// 		strcpy((char *)&size[j],(char *)&buffer[SIZE_MSG_CMD+j]);
// 	}
// 	network_msg->size = interpret_24_bit_as_int32(size);

// 	#ifdef DEBUG
// 	//Show more details
// 	printf("ALL array: ");
// 	for(int i=0;i<SIZE_MSG_CMD+SIZE_MSG_SIZE;i++){
// 		printf("%02X ",buffer[i]);
// 	}printf("\n");

// 	TRACE("CMD : %d\n",network_msg->cmd);
// 	TRACE("Size : %d\n",network_msg->size);

// 	#endif

// 	// If size > 0, data has been sent
// 	if (network_msg->size > 0)
// 	{
// 		// Continue to read the msg
// 		unsigned char buffer_data[network_msg->size];
// 		memset(buffer_data,0x00,sizeof(buffer_data));

// 		// Continue to read the socket
// 		postman_remote_receive_msg(buffer_data,network_msg->size);

// 		#ifdef DEBUG
// 		TRACE("DATA : ");
// 		for(int i=0;i<sizeof(buffer_data);i++){
// 			printf("%02X ",buffer_data[i]);
// 		}printf("\n");
// 		#endif


// 	}
// }