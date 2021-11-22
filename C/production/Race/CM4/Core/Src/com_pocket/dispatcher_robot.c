/*
 * dispatcher_robot.c
 *
 *  Created on: 21 oct. 2021
 *      Author: martin
 */


#include <com_pocket/dispatcher_robot.h>
#include "robot/robot.h"



/*********************************************************************************/
/**									Private prototypes							**/
/*********************************************************************************/

static void dispatcher_robot_decode_msg(Pilot_message_r msg);


static void dispatcher_robot_dispatch_msg(Pilot_message_r msg);

/*********************************************************************************/
/**									Public functions							**/
/*********************************************************************************/

extern void dispatcher_robot_set_msg(Pilot_message_r msg){
	printf("je suis dans le set message\n");
	dispatcher_robot_dispatch_msg(msg);
}





/*********************************************************************************/
/**									Private functions							**/
/*********************************************************************************/
static void dispatcher_robot_dispatch_msg(Pilot_message_r msg){

	Robot_mq_t messageToRobot;

	switch (msg.order_r) {
		case ASK_FOR_LEFT_DIRECTION:
			messageToRobot.directionToPerform = LEFT;
			messageToRobot.event = E_TRY_DIR;
			robot_run(messageToRobot);
			break;
		case ASK_FOR_RIGHT_DIRECTION:
			messageToRobot.directionToPerform = RIGHT;
			messageToRobot.event = E_TRY_DIR;
			robot_run(messageToRobot);
			break;
		case ASK_FOR_FORWARD_DIRECTION:
			messageToRobot.directionToPerform = FORWARD;
			messageToRobot.event = E_TRY_DIR;
			robot_run(messageToRobot);
			break;
		case ASK_FOR_BACKWARD_DIRECTION:
			messageToRobot.directionToPerform = BACKWARD;
			messageToRobot.event = E_TRY_DIR;
			robot_run(messageToRobot);
			break;
		case ASK_FOR_STOP:
			messageToRobot.directionToPerform = STOP;
			messageToRobot.event = E_TRY_DIR;
			robot_run(messageToRobot);
			break;

		default:
			break;
	}

}

static void dispatcher_robot_decode_msg(Pilot_message_r msg){
	printf("CMD:%d\n",msg.order_r);
	printf("je suis dans dispatcher robot\n");



	switch(msg.order_r){

	case ASK_FOR_LEFT_DIRECTION:
		printf("left\n");
		MOTORS_turn_left();
		break;

	case ASK_FOR_RIGHT_DIRECTION:
		printf("right\n");
		MOTORS_turn_right();
		break;

	case ASK_FOR_FORWARD_DIRECTION:
		printf("forward\n");
		MOTORS_forward();
		break;

	case ASK_FOR_BACKWARD_DIRECTION:
		printf("backward\n");
		MOTORS_backward();
		break;

	case ASK_FOR_STOP:
		printf("stop\n");
		MOTORS_stop();
		break;

	case ASK_FOR_URGENT_BREAK:
		printf("urgentstop\n");
		MOTORS_stop();
		break;

	case ASK_FOR_DEFAULT_ORDER_S:
		//NOTHING TO DO

	default:
		break;
	}
}



