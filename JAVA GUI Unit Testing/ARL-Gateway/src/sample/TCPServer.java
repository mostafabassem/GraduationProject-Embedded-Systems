package sample;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;


public class TCPServer
{

    public static void main(String[] args) {
       /*Surround Code by try catch block*/
        try {
            /*Create Server Socket*/
            int socket = 1234;
            ServerSocket ServerSocket = new ServerSocket(socket);
            /*Server is always On*/
            while (true) {
                /*Listen for Clients */
                Socket c;
                c = ServerSocket.accept();
                /*indicate the arrival of a new client from the kit*/
                System.out.println("New Client Arrived");
                /*Create IO Streams*/
                /* for this application no need for DataOutputStream for now */
                //DataOutputStream dos = new DataOutputStream(c.getOutputStream());
                DataInputStream dis = new DataInputStream(c.getInputStream());
                /*Perform IO Operations with the client */
                while (true) {
                    /* available stream to be read */
                    int length = dis.available();
                    /* create buffer */
                    byte[] buf = new byte[length];

                    /* read the full data into the buffer*/
                    dis.readFully(buf);

                    /* for each byte in the buffer*/
                    for (byte b:buf)
                    {
                        /* Cast- convert byte to char */
                        char ByteChar = (char)b;

                        /* prints character */
                        System.out.print(ByteChar);
                    }
                    /*Close/release resources*/
                    dis.close();
                    //dos.close();
                    c.close();

                }

            }

        } catch (Exception e) {
            System.out.println(e.getMessage());
        }


    }

}

