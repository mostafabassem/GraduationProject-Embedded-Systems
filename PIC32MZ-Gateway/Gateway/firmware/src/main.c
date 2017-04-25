/*******************************************************************************
  File Name:
    main.c

  Summary:
    main module.

  Description:
    main module where initialization and app tasks take turns.
  
  Author: Mostafa Bassem Heiba - ARL
 ******************************************************************************/

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "system/common/sys_module.h"   // SYS function prototypes


/******************************************************************************
/ Section: main entry point
******************************************************************************/

int main ( void )
{
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );


    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );

    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
/ End of file.
*******************************************************************************/

