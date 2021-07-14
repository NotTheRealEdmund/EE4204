# EE4204

Develop a UDP-based client-server socket program for transferring a large message.
Here, the message transmitted from the client to server is read from a large file. The
message is split into short data-units (DUs) which are sent and acknowledged in
batches of size 1, 2, and 3 DUs. The sender sends one DU, waits for an
acknowledgment (ACK); sends two DUs, waits for an ACK; sends three DUs, waits
for an ACK; and repeats the above procedure until the entire file is sent. The receiver
sends an ACK after receiving a DU; sends next ACK after receiving two DUs; sends
next ACK after receiving three DUs; and repeats the above procedure. <br><br>
Verify if the file has been sent completely and correctly by comparing the received
file with the original file. Measure the message transfer time and throughput for
various sizes of data-units and compare it with the stop-and-wait protocol where the
batch size is always fixed to be 1. Choose appropriate values for “data unit size” and
measure the performance. Repeat the experiment several times and plot the average
values in a report with a brief description of results, assumptions made, etc.<br>
Include the following performance figures in your report:
1) Transfer time vs data unit size ( one curve for this problem and one curve for
stop-and-wait )
2) Throughput vs data unit size ( one curve for this problem and one curve for
stop-and-wait )
