# -- PARAMETERS --
$PORT = 'COM4'
$BAUDRATE = 6000000
$Parity = [System.IO.Ports.Parity]::None # System.IO.Ports.Parity
$dataBits = 8
$StopBits = [System.IO.Ports.StopBits]::one # System.IO.Ports.StopBits
$ReadInterval = 1
# END PARAMETERS ---------------------------------

#Opens file for data to write
$writestreamreader = New-Object System.IO.StreamReader("C:\Users\Paul\Documents\Senior Project\SeniorProject\input.txt")
$lineerror = 0

#Opens up COM port
$port = new-Object System.IO.Ports.SerialPort $PORT, $BAUDRATE, $Parity, $dataBits, $StopBits
$port.Open()

#Used to translate the values read into ints
$encoder = new-object System.Text.UTF8Encoding

#Reads over COM port
while(($linewrite = $writestreamreader.ReadLine()) -ne $null)
{
    C:\Users\Paul\Documents\Senior Project\SeniorProject\output.txt
    
    #Reads values over COM
    Start-Sleep -Milliseconds $ReadInterval
    $readdata = $port.ReadExisting()
    
    #Converts values read into byte integers
    $bytes = $encoder.Getbytes($readdata)
    Write-Host ("Received " + $bytes)
}
$writestreamreader.Dispose()
$port.Close()