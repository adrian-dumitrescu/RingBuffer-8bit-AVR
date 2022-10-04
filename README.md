# Ring Buffer Implementation For 8-bit AVR Devices

A Linear buffer is a powerful tool that proves to be extremely useful when receiving data asynchronously. This routine stacks up incoming data so that it can be processed later without forcing other processes to delay operation. One process can read data from the front of the buffer while the other adds data to the back without overwriting each other.

The only drawback is that linear buffers only reset the writing position to the first array element when the array is entirely read out, making it useful for situation where you read all the data at once, however when the reading and writing are interleaved, you can run out of space in the buffer before it’s cleared out, or you can end up having “empty” space in the front of the buffer where data has already been read out.

A ring buffer is a solution to this problem, in which data wraps around to the beginning each time the array's end is reached, eliminating the need to reset the writing position. The main benefit is that the already processed array space can be reused to write new data, resulting in a more fluid operation.

A visual representation of a ring buffer can be observed in Figure 1:

![Picture1](https://user-images.githubusercontent.com/50441692/193831822-6fa77966-799e-47eb-a12e-2c812110b18a.png)
Figure 1

To accomplish a better performance compared to the linear buffer, the ring buffer must keep track of the buffer's "Head" and "Tail" index positions, as well as some rules to prevent existing data from being overwritten. The Head index will increment with every element added into the buffer and will always point to the next “empty” slot in the array, similarly the Tail index will increment when data is read from the buffer. In Figure 2, data is going into the ring buffer and the Head index accordingly.
![Picture2](https://user-images.githubusercontent.com/50441692/193799933-51bf3707-d5f7-4ef3-833f-0e0f86bdd9d3.png)
Figure 2

Since this representation is harder to visualize, consider Figure 3 where the data is arranged in a linear formation while keeping the functionalities of the ring buffer.
![Picture3](https://user-images.githubusercontent.com/50441692/193799936-29ad564f-39bb-49c4-8bf0-6aeb171a863c.png)
Figure 3

When the two indexes (Head and Tail) are equal, the buffer can either be considered full or empty. This situation needs to be handled by setting up a rule to differentiate between the two cases. The simplest solution to this problem is to always leave one slot empty in the array. This way, the buffer will be considered full when the Head index it’s one slot behind the Tail, and consequently the buffer status will be empty when the Tail is equal to the Head index.

![Picture4](https://user-images.githubusercontent.com/50441692/193799938-1651168b-20c2-4e71-80d9-d1947b865d6c.png)
![Picture5](https://user-images.githubusercontent.com/50441692/193799939-bc36c079-9f06-4012-8875-88031587f12c.png)
Figure 4

In Figure 4, the microcontroller receives another 5 bytes of data but saving all of them into the buffer would overwrite the previous message which is still unprocessed. As a result, the data is stored only partially, and the status is changed to FULL. This indicates that no more data is allowed into the buffer until some of the data is processed.

After partially reading the data, the buffer can continue to receive more bytes. It is important to note that reading from the buffer, does not actually erase the data, but only increments the Tail index. This allows the Head index to overwrite the processed data in the buffer. (Figure 5).


When all the bytes have been read from the buffer, the Head and the Tail indexes will match, resulting in a buffer EMPTY status (Figure 6).

![Picture6](https://user-images.githubusercontent.com/50441692/193799941-0e872180-cba9-4b62-85b3-147a27905462.png)
Figure 5


![Picture7](https://user-images.githubusercontent.com/50441692/193799944-4b66ca08-fdcf-4ce2-ae1b-45930e2cad11.png)
Figure 6

Buffers are generally used when the microcontroller cannot immediately process the series of commands or strings received. The messages can have unexpected lengths and therefore, a separation mechanism needs to be implemented to process the data when the processor is free. The commonly accepted solution to this problem is to use a delimiting character to separate the sequence of bytes into separate messages. A delimiter is a character that can be attached at the beginning or the end of a sequence of bytes and is not interpreted as part of the message, therefore, any data preceding or following the delimiting character will be processed as an individual command or message. Figure 7 illustrates an example of using a delimiter:

![Picture8](https://user-images.githubusercontent.com/50441692/193799946-6a4ab72f-17f2-4442-84c8-dce23455cbc4.png)
Figure 7

The software routine will search for the delimiting character by comparing each incoming byte with the chosen separator. When a match occurs, all unread bytes prior to the delimiter are extracted from the buffer as one individual message. 

This can either be handled with an interrupt routine or in polling mode depending on project requirements and preference. The most efficient way is to use an interrupt routine and process the command immediately after the delimiter is found.  This is almost always not possible as the microcontroller might be busy with other important processes.

An elegant solution to this case is to use a secondary ring buffer that holds the indexes to the end of each individual message in the primary buffer. In other words, whenever a delimiter is detected, instead of saving it into the Data Buffer, the index representing the end of the message will be saved in a separate buffer that can be processed at any time (Figure 8 and 9).


![Picture9](https://user-images.githubusercontent.com/50441692/193799948-ebd81825-05f7-4e17-b162-a58ba3a7c861.png)
Figure 8

![Picture10](https://user-images.githubusercontent.com/50441692/193799950-d6ea8f98-ae4e-4dd7-bc6a-e8c6c1d03abe.png)
Figure 9

Figure 9 illustrates how both the data buffer, and the delimiter buffer are being populated when two separate messages are received. Normally, the first separator would have been written on index 3 of the data buffer, but with this implementation, the index value is held in the delimiter buffer. The microcontroller occasionally checks if there is any value written in the secondary buffer and will immediately read the command from the data buffer if any (Figure 10 and 11).


![Picture11](https://user-images.githubusercontent.com/50441692/193799951-84cadf23-dbe1-4da3-b2d4-5d92172c47f1.png)
Figure 10

![Picture12](https://user-images.githubusercontent.com/50441692/193799954-899beea5-f79a-49fb-b042-94424448d26a.png)
Figure 11

As a conclusion, when using this implementation, you can choose to process the buffer data at any point, allowing for a fluid manipulation of the data in the buffer.

## License
[MIT](https://choosealicense.com/licenses/mit/)
