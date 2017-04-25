/*******************************************************************************
  File Name:
    app.h
 
  Summary:
    This Module Transfer CAN DATA to Ethernet(TCP-UDP).

  Description:
    This Module Transfer CAN DATA to Ethernet Supporting both(TCP-UDP)Connection
    When New Data Arrives from CAN,Interrupt Raises Flag CANNewData then Data 
    will be sent to many Destination.
 
  Author: Mostafa Bassem Heiba - ARL
 ******************************************************************************/

#ifndef _APP_H
#define _APP_H

/*******************************************************************************
/ Section: Included Files 
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "tcpip/tcpip.h"

/*Provide C++ Compatibility*/
#ifdef __cplusplus  

extern "C" {

#endif

/*******************************************************************************
/ Section: Global Data Definitions
*******************************************************************************/
#define CANbuff_MAX         8
#define ENETbuff_MAX        8
#define MAX_TCP_CLIENTS  5
typedef struct
{
    uint32_t ui32CAN_ID;
    uint8_t ui8len;
    uint8_t ui8CAN_Data[8];    
}msg;

/*******************************************************************************
/ Section: Application States
********************************************************************************

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*******************************************************************************/

typedef enum
{
	/* Application's state machine's initial state. */
	APP_STATE_INIT=0,
    APP_TCPIP_WAIT_FOR_IP,
    APP_TCPIP_ERROR,
    APP_TCPIP_OPEN_SOCKETS,
    MAIN,
    APP_TCPIP_WAIT_FOR_CONNECTION,
	/* TODO: Define states used by the application state machine. */

} APP_STATES;


/*******************************************************************************
/ Section: Application Data
********************************************************************************

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 ******************************************************************************/

typedef struct
{
    /* The application's current state */
    APP_STATES state;
    /* Add here required Client Sockets , as array of Client Sockets*/
    TCP_SOCKET Client_Sockets[MAX_TCP_CLIENTS];
    /*Add here required PORTS for Clients as an array*/
    int Client_PORTS[MAX_TCP_CLIENTS];
    /*Add here required IPs for Clients as an array*/
    char * IP_Client[MAX_TCP_CLIENTS];
    /* TODO: Define any additional data used by the application. */


} APP_DATA;



/*******************************************************************************
/  Function:
/    void APP_Initialize ( void )
/
/ Summary:
/     MPLAB Harmony application initialization routine.
/
/  Description:
/    This function initializes the Harmony application.  It places the 
/    application in its initial state and prepares it to run so that its 
/    APP_Tasks function can be called.
/
/  Precondition:
/    All other system initialization routines should be called before calling
/    this routine (in "SYS_Initialize").
/
/  Parameters:
/    None.
/
/  Returns:
/    None.
/
/  Example:
/    <code>
/    APP_Initialize();
/    </code>
/
/  Remarks:
/    This routine must be called from the SYS_Initialize function.
*******************************************************************************/

void APP_Initialize ( void );


/*******************************************************************************
/  Function:
/   void APP_Tasks ( void )
/
/  Summary:
/    MPLAB Harmony Demo application tasks function
/
/  Description:
/    This routine is the Harmony Demo application's tasks function.  It
/    defines the application's state machine and core logic.
/
/  Precondition:
/    The system and application initialization ("SYS_Initialize") should be
/    called before calling this.
/
/  Parameters:
/    None.
/
/  Returns:
/   None.
/
/  Example:
/    <code>
/    APP_Tasks();
/    </code>
/
/  Remarks:
/    This routine must be called from SYS_Tasks() routine.
*******************************************************************************/

void APP_Tasks( void );


#endif /* _APP_H */

#ifdef __cplusplus
}
#endif

/*******************************************************************************
/ End of file.
*******************************************************************************/

