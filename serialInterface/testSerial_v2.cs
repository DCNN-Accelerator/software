using System; 
using System.IO; 
using System.IO.Ports;
using System.Diagnostics; 


public class SerialTest_v2 
{
    public static void Main()
    {
        // Variables for loading in the input byte stream
        string input_bytes_path = "C:/Users/hkhaj/Desktop/fpga_shit/echo/uart_input_bytes.txt";
        string[] hexBytes = System.IO.File.ReadAllText(input_bytes_path).Split('\n');
        Byte[] fpgaStream = new Byte [hexBytes.Length];
        int i = 0; 

        string outputFilePath  = @"C:/Users/hkhaj/Desktop/fpga_shit/echo/fpgaOut_PSII.txt";
        System.IO.StreamWriter outputFile = new System.IO.StreamWriter(outputFilePath);

        Stopwatch timer = new Stopwatch(); 

        //Variables for the serial port reading/writing
        string fpgaBytesRead; 
        string portName = "COM4";
        int    baudRate = 6000000;
        int    dataBits = 8;
        int    readInterval = 10; 
        var    stopBit  = System.IO.Ports.StopBits.One;  
        var    parity   = System.IO.Ports.Parity.None;
        
        int numOutputBytes = (518*518) + 50; 
        int numInputBytes = 518*518*2; 
        Byte [] fpgaOutputStream = new byte[numInputBytes];

        /* Configure Serial Port Buffer depth settings */
        SerialPort fpgaComPort = new SerialPort(portName,baudRate,parity,dataBits,stopBit); 
        fpgaComPort.ReadBufferSize  = numInputBytes;
        fpgaComPort.WriteBufferSize = numOutputBytes;
        fpgaComPort.Handshake       = System.IO.Ports.Handshake.RequestToSend;

        /* Load output bytes into Byte[] struct */
        for (i = 0; i < hexBytes.Length; i++)
        {
            Byte hexValue = Convert.ToByte(hexBytes[i],16);
            fpgaStream[i] = hexValue; 
        }

        /* Start sending and receiving data, ostensibly by creating a buffer wide enough and letting the Serial Port do
        it's job */ 
        Console.WriteLine("Beginning FPGA Convolution Test");

        fpgaComPort.Open(); 
        
        /* Clean up the buffers */
        fpgaComPort.DiscardInBuffer(); 
        fpgaComPort.DiscardOutBuffer(); 

        timer.Start();
        fpgaComPort.Write(fpgaStream,0,fpgaStream.Length);

        /* Fairly liberal FPGA latency of 5ms */ 
        System.Threading.Thread.Sleep(readInterval);

        fpgaComPort.Read(fpgaOutputStream,0,numOutputBytes);
        timer.Stop(); 

        Console.WriteLine("Execution time {0} ms",timer.ElapsedMilliseconds);

        /* Write FPGA Output bytes to file */ 

        foreach (byte val in fpgaOutputStream)
        {
            outputFile.WriteLine(val);
        }

        Console.WriteLine("Completed FPGA Convolution Test");




    }
}