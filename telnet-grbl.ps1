# Simple Telnet client for GRBL
param(
    [string]$Host = "192.168.4.1",
    [int]$Port = 23
)

try {
    Write-Host "Connecting to GRBL at ${Host}:${Port}..." -ForegroundColor Green
    Write-Host "Press Ctrl+C to exit" -ForegroundColor Yellow
    Write-Host ""
    
    $client = New-Object System.Net.Sockets.TcpClient($Host, $Port)
    $stream = $client.GetStream()
    $reader = New-Object System.IO.StreamReader($stream)
    $writer = New-Object System.IO.StreamWriter($stream)
    $writer.AutoFlush = $true
    
    Write-Host "Connected! You can now type GRBL commands." -ForegroundColor Green
    Write-Host ""
    
    # Background job to read from socket
    $readJob = Start-Job -ScriptBlock {
        param($stream)
        $reader = New-Object System.IO.StreamReader($stream)
        while ($true) {
            $line = $reader.ReadLine()
            if ($line) {
                Write-Output $line
            }
        }
    } -ArgumentList $stream
    
    # Main loop - read keyboard and send to socket
    while ($client.Connected) {
        if ([Console]::KeyAvailable) {
            $key = [Console]::ReadKey($true)
            
            if ($key.Key -eq "Enter") {
                $writer.WriteLine()
            }
            elseif ($key.Key -eq "Backspace") {
                Write-Host "`b `b" -NoNewline
            }
            else {
                $writer.Write($key.KeyChar)
                Write-Host $key.KeyChar -NoNewline
            }
        }
        
        # Display output from GRBL
        $jobOutput = Receive-Job -Job $readJob
        if ($jobOutput) {
            foreach ($line in $jobOutput) {
                Write-Host $line
            }
        }
        
        Start-Sleep -Milliseconds 50
    }
}
catch {
    Write-Host "Error: $_" -ForegroundColor Red
}
finally {
    if ($readJob) { Stop-Job -Job $readJob; Remove-Job -Job $readJob }
    if ($writer) { $writer.Close() }
    if ($reader) { $reader.Close() }
    if ($stream) { $stream.Close() }
    if ($client) { $client.Close() }
    Write-Host "`nDisconnected." -ForegroundColor Yellow
}
