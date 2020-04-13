using System;
using System.IO.Ports; 
using System.Diagnostics; 


public class SerialTest 
{
    public static void Main() 
    {
        /*
        FPGA Read Write Process
            - load bytes into local bytes array 
            - stream to FPGA using port.write()
            - read in serial input buffer using ReadAvailable()
        */

        // Variables for loading in the input byte stream
        string input_bytes_path = "C:/Users/hkhaj/Desktop/fpga_shit/echo/uart_input_bytes.txt";
        string[] hexBytes = System.IO.File.ReadAllText(input_bytes_path).Split('\n');
        Byte[] fpgaStream = new Byte [hexBytes.Length];
        int i = 0; 

        //Variables for the serial port reading/writing
        string fpgaBytesRead; 
        string portName = "COM4";
        int    baudRate = 6000000;
        int    dataBits = 8;
        int    readInterval = 1; 
        var    stopBit  = System.IO.Ports.StopBits.One;  
        var    parity   = System.IO.Ports.Parity.None;

        SerialPort fpgaComPort = new SerialPort(portName,baudRate,parity,dataBits,stopBit); 

        // Helper Variables for serial r/w byte counting
        int numOutputsExpected = 518*518*2; 
        int numOutputsRecieved = 0; 
        int bytesRemaining = 0; 

        string outputFilePath  = @"C:/Users/hkhaj/Desktop/fpga_shit/echo/fpgaOut_PS.txt";
        System.IO.StreamWriter outputFile = new System.IO.StreamWriter(outputFilePath);
        
        /* Load all FPGA Input Bytes into a Byte[] struct */ 
        int numFPGAStreamElements = 0; 

        for (i = 0; i < hexBytes.Length; i++)
        {
            Byte hexValue = Convert.ToByte(hexBytes[i],16);
            fpgaStream[i] = hexValue; 
            numFPGAStreamElements++;
        }

        Console.WriteLine("Loaded Input Bytes!");
        /* Send/Recieve Bytes from FPGA COM Ports */
        fpgaComPort.Open();
        Stopwatch timer = new Stopwatch(); 
        timer.Start();
        foreach(Byte outputByte in fpgaStream)
        {
            Byte[] writeByte = new byte [1];
            writeByte[0] = outputByte; 

            fpgaComPort.Write(writeByte,0,1);

            /* Allow 1ms to let Serial Input buffer fill with data */
            //System.Threading.Thread.Sleep(readInterval);

            Byte[] bufData = new byte [fpgaComPort.BytesToRead];
            fpgaComPort.Read(bufData,0, bufData.Length);

            Console.WriteLine("Recieved FPGA Data (HEX):  {0}", BitConverter.ToString(bufData).Replace("-","") );
            Console.WriteLine("Number of Bytes Recieved: {0}", bufData.Length);

            for (int j = 0; j < bufData.Length; j++)
            {
                outputFile.WriteLine(bufData[j]);
            }

            numOutputsRecieved += bufData.Length; 
            Console.WriteLine("Total FPGA Outputs Recieved = {0}", numOutputsRecieved);

        }
        
        /* Read the leftover FPGA Data */ 
        bytesRemaining = numOutputsExpected-numOutputsRecieved; 

        for (int p = 0; p < bytesRemaining; p++)
        {
            System.Threading.Thread.Sleep(readInterval);

            Byte[] bufData = new byte [fpgaComPort.BytesToRead];
            fpgaComPort.Read(bufData,0, bufData.Length);

            Console.WriteLine("Recieved FPGA Data (HEX):  {0}", BitConverter.ToString(bufData).Replace("-","") );
            Console.WriteLine("Number of Bytes Recieved: {0}", bufData.Length);

            for (int s = 0; s < bufData.Length; s++)
            {
                outputFile.WriteLine(bufData[s]);
            }

            numOutputsRecieved += bufData.Length; 
            Console.WriteLine("Total FPGA Outputs Recieved = {0}", numOutputsRecieved);
        }
        timer.Stop(); 
        Console.WriteLine("Execution Time: {0} ms", timer.ElapsedMilliseconds);

    }
}