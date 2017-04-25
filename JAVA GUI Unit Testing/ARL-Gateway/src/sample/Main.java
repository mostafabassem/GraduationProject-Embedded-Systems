package sample;

import javafx.application.Application;
import javafx.collections.FXCollections;
import javafx.event.ActionEvent;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.chart.PieChart;
import javafx.scene.control.*;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import java.net.*;
import java.io.*;
import java.util.List;
import java.util.Scanner;
import javax.sound.sampled.Port;
import javax.swing.JOptionPane;





public class Main extends Application
{
    /**********************Application Tabs***************************/
    public static Tab TCPClient_TAB;
    public static Tab TCPServer_TAB;
    public static Tab UDPClient_TAB;
    public static Tab UDPServer_TAB;
    /*****************************************************************/

    /**********************Network Stack Elements*********************/
    public  TextField IPBox_TCPClient;
    public  TextArea SndBox_TCPClient;
    public  TextField PortBox_TCPClient;
    public Button SendBtn_TCPClient;
    public  TextField IPBox_UDPClient;
    public  TextArea SndBox_UDPClient;
    public  TextField PortBox_UDPClient;
    public Button SendBtn_UDPClient;
    public  TextArea RcvBox_UDPServer;
    public  TextField PortBox_UDPServer;
    public Button StartBtn_UDPServer;
    public  TextArea RcvBox_TCPServer;
    public  TextField PortBox_TCPServer;
    public Button StartBtn_TCPServer;
    /*****************************************************************/


    public void Send_TCPClient(ActionEvent actionEvent)
    {
        /* Send Button Pressed - Act as Client TCP */
        String IP = IPBox_TCPClient.getText().toString();
        String Port =  PortBox_TCPClient.getText().toString();
        try
        {
            /*Create Client Socket and connect to the server with given IP & Port No.*/
            Socket client = new Socket(IP, Integer.parseInt(Port));
            /*If accepted create IO streams*/
            DataOutputStream dos = new DataOutputStream(client.getOutputStream());
            /*DataInputStream dis = new DataInputStream(client.getInputStream());*/
            /*Create a Scanner to read inputs from the user*/
            Scanner sc = new Scanner(SndBox_TCPClient.getText());
            String userInput;
            /*Perform IO operations with the server - DataInputStream Disabled no need for now in this application */
            /*read from the user*/
            userInput = sc.nextLine();
            dos.writeBytes(userInput);
            /*String response;*/
            /*read the response from the server*/
            /*response = dis.readUTF();*/
            /*Print response*/
            System.out.println(userInput);
            /*RcvBox_TCPClient.setText(response);*/
            /*Close/release resources*/
            /*dis.close();*/
            dos.close();
            client.close();

        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
        }
    }

    public void Send_UDPClient(ActionEvent actionEvent)
    {
        /* Send Button Pressed - Act as Client TCP */
        String IP = IPBox_UDPClient.getText().toString();
        String Port =  PortBox_UDPClient.getText().toString();
        try
        {
            /*Create Client Socket and connect to the server with given IP & Port No.*/
            DatagramSocket clientSocket = new DatagramSocket();
            InetAddress IPAddress = InetAddress.getByName(IP);
            byte[] sendData = new byte[1024];
            String Data = SndBox_UDPClient.getText().toString();
            sendData = Data.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, Integer.parseInt(Port));
            clientSocket.send(sendPacket);
            //DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
            // clientSocket.receive(receivePacket);
            //String modifiedSentence = new String(receivePacket.getData());
            //System.out.println("FROM SERVER:" + modifiedSentence);
            clientSocket.close();

        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
        }
    }



    public void Start_UDPServer(ActionEvent actionEvent)
    {
        class UDPServer extends Thread
        {
            private String Port_Number ;
            UDPServer(String Pn)
            {
                Port_Number=Pn;
            }
            public void run()
            {
                try
                {
                    DatagramSocket serverSocket = new DatagramSocket(Integer.parseInt(Port_Number));
                    byte[] receiveData = new byte[1024];
                    byte[] sendData = new byte[1024];
                    while(true)
                    {
                        DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
                        serverSocket.receive(receivePacket);
                        String sentence = new String(receivePacket.getData());
                        RcvBox_UDPServer.setText("Received: " + sentence);
                        System.out.println("RECEIVED: " + sentence);
                        InetAddress IPAddress = receivePacket.getAddress();
                        int port = receivePacket.getPort();
                        String capitalizedSentence = sentence.toUpperCase();
                        sendData = capitalizedSentence.getBytes();
                        DatagramPacket sendPacket =
                                new DatagramPacket(sendData, sendData.length, IPAddress, port);
                        serverSocket.send(sendPacket);
                    }
                }
                catch (Exception e)
                {

                }
            }

        }
        String Port =  PortBox_UDPServer.getText().toString();
        UDPServer t1=new UDPServer(Port);
        t1.start();
    }

    public void Stop_UDPServer(ActionEvent actionEvent)
    {

    }


    public void Start_TCPServer(ActionEvent actionEvent)
    {
        class TCPServer extends Thread
        {
            private volatile boolean isRunning = true;
            private String Port_Number ;
            TCPServer(String Pn)
            {
                Port_Number=Pn;
            }
            public void run()
            {
                try
                {

                    String clientSentence;
                    String capitalizedSentence;
                    ServerSocket welcomeSocket = new ServerSocket(1234);

                    while(true)
                    {
                        Socket connectionSocket = welcomeSocket.accept();
                        BufferedReader inFromClient =
                                new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
                        DataOutputStream outToClient = new DataOutputStream(connectionSocket.getOutputStream());
                        clientSentence = inFromClient.readLine();
                        RcvBox_TCPServer.setText("Received: "+clientSentence);
                        System.out.println("Received: " + clientSentence);
                        capitalizedSentence = clientSentence.toUpperCase() + '\n';
                        outToClient.writeBytes(capitalizedSentence);
                    }
                }
                catch (Exception e)
                {

                }
            }

        }
        String Port =  PortBox_TCPServer.getText().toString();
        TCPServer t1=new TCPServer(Port);
        t1.start();
    }


    public void Stop_TCPServer(ActionEvent actionEvent)
    {

    }

    @Override
    public void start(Stage primaryStage) throws Exception
    {
        Parent root = FXMLLoader.load(getClass().getResource("ClientFX.fxml"));
        primaryStage.setTitle("ARL-Gateawy Ethernet Test");
        primaryStage.setScene(new Scene(root, 400, 400));
        primaryStage.show();

    }



    public static void main(String[] args)
    {
        launch(args);
    }
}
