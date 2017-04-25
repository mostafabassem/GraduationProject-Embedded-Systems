/*******************************************************************************
  File Name:
    app.c
 
  Summary:
    This Module Transfer CAN DATA to Ethernet(TCP-UDP).

  Description:
    This Module Transfer CAN DATA to Ethernet Supporting both(TCP-UDP)Connection
    When New Data Arrives from CAN,Interrupt Raises Flag CANNewData then Data 
    will be sent to many Destination.
 
  Author: Mostafa Bassem Heiba - ARL
 ******************************************************************************/


/*******************************************************************************
/ Section: Included Files 
*******************************************************************************/

#include "app.h"

/*******************************************************************************
/ Section: Global Data Definitions
*******************************************************************************/

#define UDP 0
#define TWO_CLIENTS 1
#define TestDebug 0

#define SERVER_PORT1 1234
#define SERVER_PORT2 1244
#define CLIENT_IP1 "192.168.1.2"
#define CLIENT_IP2 "192.168.1.3"

#define RPM_Reading 0x280
#define Coolant_Reading 0x289
#define Ignition_Reading 0x5F1
#define AirPressure_Reading 0x5F2
#define AirTemperature_Reading 0x5F2
#define Throttle_Reading 0x5F3
#define Battery_Reading 0x5F3

volatile msg CANbuffRX[CANbuff_MAX];/*buffer to be send using ethernet*/
volatile msg ENETbuffRX[ENETbuff_MAX];/*buffer to save data recive by ethernet*/
volatile uint8_t CANbuffEndRX=0;/*index of can buffer recive*/
volatile uint8_t ENETbuffEndRX=0;/*index of can buffer that save date recive by ethernet*/
volatile bool CANNewData=false;
volatile bool ENETNewData=false;

unsigned char value[4];
unsigned char buffer_RPM[5];
unsigned char buffer_Coolant[5];
unsigned char buffer_Ignition[5];
unsigned char buffer_AP[5];
unsigned char buffer_AT[5];
unsigned char buffer_TR[5];
unsigned char buffer_Battery[5];

unsigned char buffer[255];

/* Application Data,This structure holds the application's data. */

APP_DATA appData;

/*******************************************************************************
/  Function:
/    unsigned int reverse_nibbles(unsigned int x)
/
/  Remarks:
/    See prototype in app.h.
/******************************************************************************/

unsigned int reverse_nibbles(unsigned int x)
{
  unsigned int out = 0, i;
  for(i = 0; i < 4; ++i)
  {
    const unsigned int byte = (x >> 8 * i) & 0xff;
    out |= byte << (24 - 8 * i);
  }
  return out;
}


/*******************************************************************************
/  Function:
/    void changeFormat(short x)
/
/  Remarks:
/    See prototype in app.h.
/******************************************************************************/

void changeFormat(short x)
{

	unsigned char xThousand = x / 1000;
	unsigned char xHundred = (x - (xThousand * 1000)) / 100;
	unsigned char xTens = (x - (xThousand * 1000 + xHundred *100)) / 10;
	unsigned char xDigit = (x - (xThousand * 1000 + xHundred * 100 + xTens * 10));
	
	value[0] = xThousand + 48 ;
	value[1] = xHundred + 48;
	value[2] = xTens + 48;
	value[3] = xDigit + 48;

	//for (int i = 0; i < 4; i++){
	//	printf("%c", value[i]);
	//}

//	printf("\n");



}


/*******************************************************************************
/  Function:
/    void APP_Initialize ( void )
/
/  Remarks:
/    See prototype in app.h.
/******************************************************************************/



void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

}


/******************************************************************************
/  Function:
/    void APP_Tasks ( void )
/
/  Remarks:
/    See prototype in app.h.
/******************************************************************************/


void APP_Tasks ( void )
{
    /*Ethernet Things*/
    SYS_STATUS          tcpipStat;
    const char          *netName, *netBiosName;
    static IPV4_ADDR    dwLastIP[2] = { {-1}, {-1} };
    IPV4_ADDR           ipAddr;
    TCPIP_NET_HANDLE    netH;
    int                 i, nNets;
    /*End Ethernet Things*/
   
     uint64_t ui64i=0;
    /* Check the application's current state. */
    LATH=0x3333;
    TRISHCLR=0xffff;
    LATHSET=0xFFFE;
  
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if(tcpipStat < 0)
            {   /* Error occurred */
                appData.state = APP_TCPIP_ERROR;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                /* now that the stack is ready we can check the available interfaces */
                nNets = TCPIP_STACK_NumberOfNetworksGet();
                for(i = 0; i < nNets; i++)
                {

                    netH = TCPIP_STACK_IndexToNet(i);
                    netName = TCPIP_STACK_NetNameGet(netH);
                    netBiosName = TCPIP_STACK_NetBIOSName(netH);
                    
                }
                appData.state = APP_TCPIP_WAIT_FOR_IP;

            }
            
            break;
        }
        case APP_TCPIP_WAIT_FOR_IP:

            nNets = TCPIP_STACK_NumberOfNetworksGet();

            for (i = 0; i < nNets; i++)
            {
                netH = TCPIP_STACK_IndexToNet(i);
                ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                if(dwLastIP[i].Val != ipAddr.Val)
                {
                    dwLastIP[i].Val = ipAddr.Val;

                    if (ipAddr.v[0] != 0 && ipAddr.v[0] != 169) // Wait for a Valid IP
                    {
                        appData.state = APP_TCPIP_OPEN_SOCKETS;
                    }
                }
            }
            break;
        case APP_TCPIP_OPEN_SOCKETS:
        {
        
                /*Server Listening Ports*/
                appData.Client_PORTS[0] = SERVER_PORT1;
#if TWO_CLIENTS
                appData.Client_PORTS[1] = SERVER_PORT2;
#endif 
                
                /*Open Client Sockets*/
#if UDP
               appData.Client_Sockets[0] = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, SERVER_PORT1, 0);
#else
                 /*Open Server Socket*/
                appData.Client_Sockets[0] = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, SERVER_PORT1, 0);
#endif
                
#if TWO_CLIENTS
#if UDP
                appData.Client_Socket[1] = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, SERVER_PORT2, 0);
#else
                appData.Client_Sockets[1] = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, SERVER_PORT2, 0);
#endif
#endif
                if (appData.Client_Sockets[0] == INVALID_SOCKET)
                {
                    appData.state = APP_TCPIP_OPEN_SOCKETS;
                }
#if TWO_CLIENTS
                if(appData.Client_Sockets[1] == INVALID_SOCKET)
                {
                    appData.state = APP_TCPIP_OPEN_SOCKETS;
                }
#endif
                
                appData.state = MAIN;
   
            
        }
        break;
        case APP_TCPIP_WAIT_FOR_CONNECTION:
        {
            /*Check for Connection , stay in the same state if no connection*/
            if (!TCPIP_TCP_IsConnected(appData.Client_Sockets[0]))
            {
                return;
            }
            else if (!TCPIP_TCP_IsConnected(appData.Client_Sockets[1]))
            {
                return;
            }
            else
            {
                /*New Connection Arrived ! */
                appData.state = MAIN;
            }
            break;
        }

        case MAIN:
        {
            /*Wait in MAIN state if connection lost*/
#if UDP
            if (!TCPIP_UDP_IsConnected(appData.Client_Sockets[0]))
            {
                break;
            }
#else
            if (!TCPIP_TCP_IsConnected(appData.Client_Sockets[0]))
            {
                break;
            }
#endif
            ui64i++;
            if((ui64i & 0x1FFFFFF)==0)
            {
                LATHINV=0x0003;
            }
            
            if(CANNewData)
            {
                /*Ethernet read buffer and send here*/              
                
                /*Check if connection is still available*/
#if UDP
                if (!TCPIP_UDP_IsConnected(appData.Client_Sockets[0]))
                {
                    break;
                }
#else
                if (!TCPIP_TCP_IsConnected(appData.Client_Sockets[0]))
                {
                    break;
                }
#endif
#if UDP
                if(TCPIP_UDP_PutIsReady(appData.Client_Sockets[0]) == 0)
                {
                    break;
                }
#else
                if(TCPIP_TCP_PutIsReady(appData.Client_Sockets[0]) == 0)
                {
                    break;
                } 
#endif
                uint8_t counter=0;
                while(CANbuffEndRX > 0)
                {
                    if(CANbuffRX[counter].ui32CAN_ID==RPM_Reading)
                    {
                        /*Incoming CAN Data from RPM, update RPM Buffer*/
                        uint16_t temp = CANbuffRX[counter].ui8CAN_Data[3] * 256 + CANbuffRX[counter].ui8CAN_Data[2];
                        changeFormat(temp);
                        sprintf(buffer_RPM,"%c%c%c%c%c",value[0],value[1],value[2],value[3],',');
                    }
                    else if(CANbuffRX[counter].ui32CAN_ID==Coolant_Reading)
                    {
                        /*Incoming CAN Data from Coolant, update Coolant Buffer*/
                        uint16_t temp = CANbuffRX[counter].ui8CAN_Data[3] * 256 + CANbuffRX[counter].ui8CAN_Data[2];
                        changeFormat(temp);
                        sprintf(buffer_Coolant,"%c%c%c%c%c",value[0],value[1],value[2],value[3],',');
                    }
                    else if(CANbuffRX[counter].ui32CAN_ID==Ignition_Reading)
                    {
                        /*Incoming CAN Data from Ignition, update Ignition Buffer*/
                        uint16_t temp = CANbuffRX[counter].ui8CAN_Data[0] * 256 + CANbuffRX[counter].ui8CAN_Data[1];
                        changeFormat(temp);
                        sprintf(buffer_Ignition,"%c%c%c%c%c",value[0],value[1],value[2],value[3],',');
                    }
                    else if(CANbuffRX[counter].ui32CAN_ID==AirPressure_Reading)
                    {
                        /*Incoming CAN Data from Air Pressure, update Air Pressure Buffer*/
                        uint16_t temp = CANbuffRX[counter].ui8CAN_Data[2] * 256 + CANbuffRX[counter].ui8CAN_Data[3];
                        changeFormat(temp);
                        sprintf(buffer_AP,"%c%c%c%c%c",value[0],value[1],value[2],value[3],',');
                    }
                    else if(CANbuffRX[counter].ui32CAN_ID==AirTemperature_Reading)
                    {
                        /*Incoming CAN Data from Air Temperature, update Air Temperature Buffer*/
                        uint16_t temp = CANbuffRX[counter].ui8CAN_Data[4] * 256 + CANbuffRX[counter].ui8CAN_Data[5];
                        changeFormat(temp);
                        sprintf(buffer_AT,"%c%c%c%c%c",value[0],value[1],value[2],value[3],',');
                    }
                    else if(CANbuffRX[counter].ui32CAN_ID==Throttle_Reading)
                    {
                        /*Incoming CAN Data from Throttle, update Throttle Buffer*/
                        uint16_t temp = CANbuffRX[counter].ui8CAN_Data[0] * 256 + CANbuffRX[counter].ui8CAN_Data[1];
                        changeFormat(temp);
                        sprintf(buffer_TR,"%c%c%c%c%c",value[0],value[1],value[2],value[3],',');
                    }
                    else if(CANbuffRX[counter].ui32CAN_ID==Battery_Reading)
                    {
                        /*Incoming CAN Data from Battery, update Battery Buffer*/
                        uint16_t temp = CANbuffRX[counter].ui8CAN_Data[2] * 256 + CANbuffRX[counter].ui8CAN_Data[3];
                        changeFormat(temp);
                        sprintf(buffer_Battery,"%c%c%c%c%c",value[0],value[1],value[2],value[3],'?');
                    }
                    
                    /*Put all Readings Buffer in the Main Buffer*/
                    sprintf(buffer,buffer_RPM);
                    strcat(buffer,buffer_Coolant);
                    strcat(buffer,buffer_Ignition);
                    strcat(buffer,buffer_AP);
                    strcat(buffer,buffer_AT);
                    strcat(buffer,buffer_TR);
                    strcat(buffer,buffer_Battery);
                    
                    /*Send Data to Ethernet*/
#if TWO_CLIENTS
#if UDP
                    TCPIP_UDP_ArrayPut(appData.Client_Sockets[0], (uint8_t*)buffer,strlen(buffer) );
                    TCPIP_UDP_Flush(appData.Client_Sockets[0]);
                    TCPIP_UDP_ArrayPut(appData.Client_Sockets[1], (uint8_t*)buffer,strlen(buffer) );
                    TCPIP_UDP_Flush(appData.Client_Sockets[1]);
#else
                    TCPIP_TCP_ArrayPut(appData.Client_Sockets[0], (uint8_t*)buffer,strlen(buffer) );
                    TCPIP_TCP_ArrayPut(appData.Client_Sockets[1], (uint8_t*)buffer,strlen(buffer) );
#endif
#else
#if UDP
                    TCPIP_UDP_ArrayPut(appData.Client_Sockets[0], (uint8_t*)buffer,strlen(buffer) );
                    TCPIP_UDP_Flush(appData.Client_Sockets[0]);
#else
                    TCPIP_TCP_ArrayPut(appData.Client_Sockets[0], (uint8_t*)buffer,strlen(buffer) );
#endif     
#endif
                    
#if TestDebug
                    DRV_CAN0_ChannelMessageTransmit(CAN_CHANNEL0,50,CANbuffRX[counter].ui8len,((uint8_t *)&CANbuffRX[counter].ui8CAN_Data[0]));
                    DRV_CAN0_ChannelMessageTransmit(CAN_CHANNEL0,60,4,((uint8_t *)&CANbuffRX[counter].ui32CAN_ID));
#endif
                    /*Update Counter & CanBufferEndRX*/   
                    counter++;
                    CANbuffEndRX--;
                }
#if UDP
                TCPIP_UDP_Discard(appData.Client_Sockets[0]);
#endif
                /*Reset CANNewData flag*/
                CANNewData=false;
                /*Check Connection Again */
#if UDP
                if (!TCPIP_UDP_IsConnected(appData.Client_Sockets[0]))
                {
                    break;
                }
#else
                if (!TCPIP_TCP_IsConnected(appData.Client_Sockets[0]))
                {
                    break;
                } 
#endif
            }
            if(ENETNewData)
            {
                uint8_t counter=0;
                /*CAN read buffer and send here*/
                while(ENETbuffEndRX > 0)
                {
                    DRV_CAN0_ChannelMessageTransmit(CAN_CHANNEL0,
                            ENETbuffRX[counter].ui32CAN_ID,ENETbuffRX[counter].ui8len,
                            ((uint8_t *)&(ENETbuffRX[counter].ui8CAN_Data[0])));
                    counter++;
                    ENETbuffEndRX--;
                }
                ENETNewData=false;
                /*Check Connection Again */
#if UDP
                if (!TCPIP_UDP_IsConnected(appData.Client_Sockets[0]))
                {
                    break;
                }
#else
                if (!TCPIP_TCP_IsConnected(appData.Client_Sockets[0]))
                {
                    break;
                } 
#endif
            }
            
            
        break;
    
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
 
/*******************************************************************************
/ End of file.
*******************************************************************************/




        
       