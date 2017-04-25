/*******************************************************************************
  File Name:
    app1.c

  Summary:
    This Module Transfer Ethernet (TCP Connection) DATA to CAN.

  Description:
    This Module Transfer Ethernet DATA to CAN, Server Listens on PORT "9761"
    When New Data Arrives from Ethernet, it Raises Flag ENETNewData then Data 
    will be sent to CAN.
 
  Author: Mostafa Bassem Heiba - ARL
 ******************************************************************************/

/******************************************************************************
/ Section: Included Files 
******************************************************************************/

#include "app1.h"
#include "app.h"
/******************************************************************************
/ Section: Global Data Definitions
******************************************************************************/


#define SERVER_PORT 9761

#define CAN_ID 0x640


/* Application Data,This structure holds the application's data. */

APP1_DATA app1Data;

/*******************************************************************************
/  Function:
/    void Format_CAN_Data(uint8_t* AppBuffer)
/
/  Remarks:void Format_CAN_Data(uint8_t* AppBuffer)
/    Format CAN frames before sending it to MegaSquirt.
/******************************************************************************/
inline void Format_CAN_Data(uint8_t* AppBuffer)
{
    uint16_t temp;
    temp = AppBuffer[7] + 100 * AppBuffer[6];
    if ((temp&0xff00)==0)
    {
        /*Do Nothing*/
    }
    else 
    {
        AppBuffer[7] = temp&0x00ff;
        AppBuffer[6] = temp>>8;
                    
    }
    temp = AppBuffer[5] + 100 * AppBuffer[4];
    if ((temp&0xff00)==0)
    {
        /*Do Nothing*/
    }
    else 
    {
        AppBuffer[5] = temp&0x00ff;
        AppBuffer[4] = temp>>8;
                    
    }
    temp = AppBuffer[3] + 100 * AppBuffer[2];
    if ((temp&0xff00)==0)
    {
        /*Do Nothing*/
    }
    else 
    {
        AppBuffer[3] = temp&0x00ff;
        AppBuffer[2] = temp>>8;
                    
    }
    temp = AppBuffer[1] + 100 * AppBuffer[0];
    if ((temp&0xff00)==0)
    {
        /*Do Nothing*/
    }
    else 
    {
        AppBuffer[1] = temp&0x00ff;
        AppBuffer[0] = temp>>8;
                    
    }
}

/*******************************************************************************
/  Function:
/    void APP1_Initialize ( void )
/
/  Remarks:
/    See prototype in app1.h.
/******************************************************************************/
void APP1_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app1Data.state = APP1_TCPIP_WAIT_INIT;
    
}

/******************************************************************************
/  Function:
/    void APP1_Tasks ( void )
/
/  Remarks:
/    See prototype in app1.h.
/******************************************************************************/

void APP1_Tasks ( void )
{
    SYS_STATUS          tcpipStat;
    const char          *netName, *netBiosName;
    static IPV4_ADDR    dwLastIP[2] = { {-1}, {-1} };
    IPV4_ADDR           ipAddr;
    int                 i, nNets;
    TCPIP_NET_HANDLE    netH;

    switch(app1Data.state)
    {
        case APP1_TCPIP_WAIT_INIT:
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if(tcpipStat < 0)
            {   
               /*Error Occurred*/
                app1Data.state = APP1_TCPIP_ERROR;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                /*now that the stack is ready we can check available interfaces*/
                nNets = TCPIP_STACK_NumberOfNetworksGet();
                for(i = 0; i < nNets; i++)
                {

                    netH = TCPIP_STACK_IndexToNet(i);
                    netName = TCPIP_STACK_NetNameGet(netH);
                    netBiosName = TCPIP_STACK_NetBIOSName(netH);

                }
                app1Data.state = APP1_TCPIP_WAIT_FOR_IP;

            }
            break;

        case APP1_TCPIP_WAIT_FOR_IP:

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
                        app1Data.state = APP1_TCPIP_OPENING_SERVER;
                    }
                }
            }
            break;
        case APP1_TCPIP_OPENING_SERVER:
        {
            /*Open Server Socket*/
            app1Data.socket = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, SERVER_PORT, 0);
            
            /*Check if Socket is invalid Stay in the same State and try Again*/
            if (app1Data.socket == INVALID_SOCKET)
            {
                break;
            }
            
            app1Data.state = APP1_TCPIP_WAIT_FOR_CONNECTION;
        }
        break;

        case APP1_TCPIP_WAIT_FOR_CONNECTION:
        {
            /*Check for Connection , stay in the same state if no connection*/
            if (!TCPIP_TCP_IsConnected(app1Data.socket))
            {
                return;
            }
            else
            {
                /*New Connection Arrived ! */
                app1Data.state = APP1_TCPIP_SERVING_CONNECTION;
            }
        }
        break;

        case APP1_TCPIP_SERVING_CONNECTION:
        {
            if (!TCPIP_TCP_IsConnected(app1Data.socket))
            {
                /*No effect if this line removed, leave it for now*/
                app1Data.state = APP1_TCPIP_CLOSING_CONNECTION;
                break;
            }
            int16_t wMaxGet, wMaxPut, wCurrentChunk;
            uint16_t w;
            uint8_t AppBuffer[32];
            
            /* Figure out how many bytes have been received and how many we can transmit*/
            /*Get TCP RX FIFO byte count*/
            wMaxGet = TCPIP_TCP_GetIsReady(app1Data.socket);	
            /*Get TCP TX FIFO free space*/
            wMaxPut = TCPIP_TCP_PutIsReady(app1Data.socket);	

            /*Make sure we don't take more bytes out of the RX FIFO than we can put into the TX FIFO*/
            if(wMaxPut < wMaxGet)
            {
                wMaxGet = wMaxPut;
            }
           

            /*Process all bytes that we can
            This is implemented as a loop, processing up to sizeof(AppBuffer) bytes at a time.
            This limits memory usage while maximizing performance.  Single byte Gets and Puts are a lot slower than multibyte GetArrays and PutArrays*/
            wCurrentChunk = sizeof(AppBuffer);
            for(w = 0; w < wMaxGet; w += sizeof(AppBuffer))
            {
                /*Make sure the last chunk, which will likely be smaller than sizeof(AppBuffer), is treated correctly*/
                if(w + sizeof(AppBuffer) > wMaxGet)
                    wCurrentChunk = wMaxGet - w;

                /*Transfer the data out of the TCP RX FIFO and into our local processing buffer*/
                TCPIP_TCP_ArrayGet(app1Data.socket, AppBuffer, wCurrentChunk);
                
                /*Transfer the data out of our local processing buffer and into the TCP TX FIFO*/
                TCPIP_TCP_ArrayPut(app1Data.socket, AppBuffer, wCurrentChunk);
                
                /*Function That Format the Frame to cope with MegaSquirt Standard Frame*/
                Format_CAN_Data(AppBuffer);
                
                /*Transfer Data to CAN with Desired CAN_ID*/
                DRV_CAN0_ChannelMessageTransmit(CAN_CHANNEL0,CAN_ID,wCurrentChunk,AppBuffer);
            }
        }
        break;
        case APP1_TCPIP_CLOSING_CONNECTION:
        {
            /*Close Socket Connection*/
            TCPIP_TCP_Close(app1Data.socket);
            
            /*Try Opening Again*/
            app1Data.state = APP1_TCPIP_OPENING_SERVER;

        }
        break;
        
        default:
            break;
    }
}
 
/*******************************************************************************
/ End of file.
*******************************************************************************/
