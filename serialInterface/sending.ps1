# -- PARAMETERS --
$PORT = 'COM4'
$BAUDRATE = 6000000
$Parity = [System.IO.Ports.Parity]::None # System.IO.Ports.Parity
$dataBits = 8
$StopBits = [System.IO.Ports.StopBits]::one # System.IO.Ports.StopBits
$ReadInterval = 1
# END PARAMETERS ---------------------------------

#Opens file for data to write
$writestreamreader = New-Object System.IO.StreamReader("C:\Users\Paul\Documents\Senior Project\SeniorProject\output.txt")
$lineerror = 0

#Opens up COM port
$port = new-Object System.IO.Ports.SerialPort $PORT, $BAUDRATE, $Parity, $dataBits, $StopBits
$port.Open()

#Used to translate the values read into ints
$encoder = new-object System.Text.UTF8Encoding

#Writes over COM port
while(($linewrite = $writestreamreader.ReadLine()) -ne $null)
{
    #Splits the byte into two nibbles
    $tempMSN = $linewrite.Substring(0,1)
    $tempLSN = $linewrite.Substring(1,1)
    
    #Determines value of MSN
    if($tempMSN -ge 0 -AND $tempMSN -le 9)
    {
    	$tempMSNint = [int]$tempMSN
    }
    elseif($tempMSN -ge "A" -AND $tempMSN -le "F")
    {
    	switch ($tempLSN)
	{
	    "A" {$tempMSNint = 10}
	    "B" {$tempMSNint = 11}
	    "C" {$tempMSNint = 12}
	    "D" {$tempMSNint = 13}
	    "E" {$tempMSNint = 14}
	    "F" {$tempMSNint = 15}
	 	    
 	}
    }
    else
    {
    	Write-Host ("Error: Value from file out of bounds line " + $lineerror)
    }
    
    #Determines value of LSN
    if($tempLSN -ge 0 -AND $tempLSN -le 9)
    {
	$tempLSNint = [int]$tempLSN
    }
    elseif($tempLSN -ge "A" -AND $tempLSN -le "F")
    {
 	switch ($tempLSN)
 	{
 	    "A" {$tempLSNint = 10}
 	    "B" {$tempLSNint = 11}
 	    "C" {$tempLSNint = 12}
 	    "D" {$tempLSNint = 13}
 	    "E" {$tempLSNint = 14}
 	    "F" {$tempLSNint = 15}
 	    
 	}
    }
    else
    {
	Write-Host ("Error: Value from file out of bounds line " + $lineerror)
    }
    
    #Calculates value of the byte
    $writevalue = [int](([int]$tempMSNint) * 16 + ([int]$tempLSNint))
    Write-Host ("Writing " + $writevalue)
    
    #increase error line counter
    $lineerror++
    
    #write a byte value over COM
    $port.Write([byte]$writevalue, 0, 1)
}
$writestreamreader.Dispose()
$port.Close()