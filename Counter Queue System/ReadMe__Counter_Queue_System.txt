Program's Goal:
To use dynamic memory allocation to realise a system of counters, buffers and a common queue.
To use the concepts of static and global variables for data sharing and data retention.

Instructions for compilation:-
1.Compile using the GCC compiler with std=c11 and execute the .out file
2.Input is from the service_request.txt file which contains the number of counters,buffer size of each counter and a 
list of counters in that order.
3.Output is printed in allocation.txt and qms-log.txt
4.No output is printed on terminal in case of a successful termination of program
5.allocation.txt contains the following information for every request that is processed
<request_id, counter_number, entrytime at counter, exittime at counter, time spent in queue, time spent in counterbuffer>
6.qms-log.txt contains the following information for all time instants
Line 1: <Time instant, queue size, buffer size1, .., buffer size n>
Line 2: <Time instant, List of requests despatched from queue to buffers>
Line 3: < Time instant,  List of requests despatched from buffer to counter>
Line 4: < Time instant, List of jobs finished at counter and sent out of system>

Sample Input(In service-request.txt):-
A.
2
2
1
5
6
2
3
9
10
4

B.
4
4
1
5
6
2
3
9
10
4
5
2
1
4
5
3
1
5
6
2
1
5
4
5
6
7

Corresponding Sample Outputs(in allocation.txt(i) and qms-log.txt(ii)):-
A.
(i).
<R1	Counter-1	0	1	0	0>
<R2	Counter-2	0	5	0	0>
<R3	Counter-1	1	7	0	1>
<R4	Counter-2	5	7	0	5>
<R5	Counter-1	7	10	0	7>
<R6	Counter-2	7	16	0	7>
<R7	Counter-1	10	20	1	9>
<R8	Counter-2	16	20	5	11>
(ii).
<0	2	2	2	>
<0	R3	R5	R4	R6	>
<0	>
<0	>
<1	1	2	2	>
<1	R7	>
<1	R3	>
<1	R1	>
<2	1	2	2	>
<2	>
<2	>
<2	>
<3	1	2	2	>
<3	>
<3	>
<3	>
<4	1	2	2	>
<4	>
<4	>
<4	>
<5	0	2	2	>
<5	R8	>
<5	R4	>
<5	R2	>
<6	0	2	2	>
<6	>
<6	>
<6	>
<7	0	1	1	>
<7	>
<7	R5	R6	>
<7	R3	R4	>
<8	0	1	1	>
<8	>
<8	>
<8	>
<9	0	1	1	>
<9	>
<9	>
<9	>
<10	0	0	1	>
<10	>
<10	R7	>
<10	R5	>
<11	0	0	1	>
<11	>
<11	>
<11	>
<12	0	0	1	>
<12	>
<12	>
<12	>
<13	0	0	1	>
<13	>
<13	>
<13	>
<14	0	0	1	>
<14	>
<14	>
<14	>
<15	0	0	1	>
<15	>
<15	>
<15	>
<16	0	0	0	>
<16	>
<16	R8	>
<16	R6	>
<17	0	0	0	>
<17	>
<17	>
<17	>
<18	0	0	0	>
<18	>
<18	>
<18	>
<19	0	0	0	>
<19	>
<19	>
<19	>
<20	0	0	0	>
<20	>
<20	>
<20	R7	R8	>

B.
(i).
<R1	Counter-1	0	1	0	0>
<R4	Counter-4	0	2	0	0>
<R5	Counter-1	1	4	0	1>
<R2	Counter-2	0	5	0	0>
<R3	Counter-3	0	6	0	0>
<R8	Counter-4	2	6	0	2>
<R9	Counter-1	4	9	0	4>
<R12	Counter-4	6	10	0	6>
<R13	Counter-1	9	14	0	9>
<R6	Counter-2	5	14	0	5>
<R16	Counter-4	10	15	0	10>
<R10	Counter-2	14	16	0	14>
<R7	Counter-3	6	16	0	6>
<R11	Counter-3	16	17	0	16>
<R15	Counter-3	17	18	0	17>
<R14	Counter-2	16	19	0	16>
<R19	Counter-3	18	19	0	18>
<R17	Counter-1	14	20	0	14>
<R20	Counter-4	15	20	0	15>
<R18	Counter-2	19	21	0	19>
<R21	Counter-1	20	24	1	19>
<R22	Counter-4	20	25	2	18>
<R24	Counter-2	21	28	5	16>
<R23	Counter-1	24	30	4	20>

(ii).
<0	4	4	4	4	4	>
<0	R5	R9	R13	R17	R6	R10	R14	R18	R7	R11	R15	R19	R8	R12	R16	R20	>
<0	>
<0	>
<1	3	4	4	4	4	>
<1	R21	>
<1	R5	>
<1	R1	>
<2	2	4	4	4	4	>
<2	R22	>
<2	R8	>
<2	R4	>
<3	2	4	4	4	4	>
<3	>
<3	>
<3	>
<4	1	4	4	4	4	>
<4	R23	>
<4	R9	>
<4	R5	>
<5	0	4	4	4	4	>
<5	R24	>
<5	R6	>
<5	R2	>
<6	0	4	4	3	3	>
<6	>
<6	R7	R12	>
<6	R3	R8	>
<7	0	4	4	3	3	>
<7	>
<7	>
<7	>
<8	0	4	4	3	3	>
<8	>
<8	>
<8	>
<9	0	3	4	3	3	>
<9	>
<9	R13	>
<9	R9	>
<10	0	3	4	3	2	>
<10	>
<10	R16	>
<10	R12	>
<11	0	3	4	3	2	>
<11	>
<11	>
<11	>
<12	0	3	4	3	2	>
<12	>
<12	>
<12	>
<13	0	3	4	3	2	>
<13	>
<13	>
<13	>
<14	0	2	3	3	2	>
<14	>
<14	R17	R10	>
<14	R13	R6	>
<15	0	2	3	3	1	>
<15	>
<15	R20	>
<15	R16	>
<16	0	2	2	2	1	>
<16	>
<16	R14	R11	>
<16	R10	R7	>
<17	0	2	2	1	1	>
<17	>
<17	R15	>
<17	R11	>
<18	0	2	2	0	1	>
<18	>
<18	R19	>
<18	R15	>
<19	0	2	1	0	1	>
<19	>
<19	R18	>
<19	R14	R19	>
<20	0	1	1	0	0	>
<20	>
<20	R21	R22	>
<20	R17	R20	>
<21	0	1	0	0	0	>
<21	>
<21	R24	>
<21	R18	>
<22	0	1	0	0	0	>
<22	>
<22	>
<22	>
<23	0	1	0	0	0	>
<23	>
<23	>
<23	>
<24	0	0	0	0	0	>
<24	>
<24	R23	>
<24	R21	>
<25	0	0	0	0	0	>
<25	>
<25	>
<25	R22	>
<26	0	0	0	0	0	>
<26	>
<26	>
<26	>
<27	0	0	0	0	0	>
<27	>
<27	>
<27	>
<28	0	0	0	0	0	>
<28	>
<28	>
<28	R24	>
<29	0	0	0	0	0	>
<29	>
<29	>
<29	>
<30	0	0	0	0	0	>
<30	>
<30	>
<30	R23	>

