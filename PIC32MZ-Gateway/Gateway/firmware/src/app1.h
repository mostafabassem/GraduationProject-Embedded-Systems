/*******************************************************************************
  File Name:
    app1.h

  Summary:
    This Module Transfer Ethernet (TCP Connection) DATA to CAN.

  Description:
    This Module Transfer Ethernet DATA to CAN, Server Listens on PORT "9761"
    When New Data Arrives from Ethernet, it Raises Flag ENETNewData then Data 
    will be sent to CAN.
 
  Author: Mostafa Bassem Heiba - ARL
 ******************************************************************************/

#ifndef _APP1_H
#define _APP1_H

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
    /* In this state, the application waits for the initialization of the TCP/IP stack
     * to complete. */
    APP1_TCPIP_WAIT_INIT,

    /* In this state, the application waits for a IP Address */
    APP1_TCPIP_WAIT_FOR_IP,

    APP1_TCPIP_OPENING_SERVER,

    APP1_TCPIP_WAIT_FOR_CONNECTION,

    APP1_TCPIP_SERVING_CONNECTION,

    APP1_TCPIP_CLOSING_CONNECTION,

    APP1_TCPIP_ERROR,
} APP1_STATES;


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
    APP1_STATES state;

    TCP_SOCKET              socket;
} APP1_DATA;


/*******************************************************************************
/  Function:
/    void APP1_Initialize ( void )
/
/ Summary:
/     MPLAB Harmony application initialization routine.
/
/  Description:
/    This function initializes the Harmony application.  It places the 
/    application in its initial state and prepares it to run so that its 
/    APP1_Tasks function can be called.
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
/    APP1_Initialize();
/    </code>
/
/  Remarks:
/    This routine must be called from the SYS_Initialize function.
*******************************************************************************/

void APP1_Initialize ( void );


/*******************************************************************************
/  Function:
/   void APP1_Tasks ( void )
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
/    APP1_Tasks();
/    </code>
/
/  Remarks:
/    This routine must be called from SYS_Tasks() routine.
*******************************************************************************/

void APP1_Tasks( void );


#endif /* _APP1_H */

#ifdef __cplusplus
}
#endif

/*******************************************************************************
/ End of file.
*******************************************************************************/
