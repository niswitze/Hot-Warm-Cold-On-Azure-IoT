void initSerial()
{
    // Start serial and initialize stdout
    Serial.begin(DATA_RATE);
    while (! Serial);
    Serial.println("Serial successfully inited.");
}

/* Read a string whose length should in (0, lengthLimit) from Serial and save it into buf.
 *
 *        prompt   - the interact message and buf should be allocaled already and return true.
 *        buf      - a part of memory which is already allocated to save string read from serial
 *        maxLen   - the buf's len, which should be upper bound of the read-in string's length, this should > 0
 *        timeout  - If after timeout(ms), return false with nothing saved to buf.
 *                   If no timeout <= 0, this function will not return until there is something read.
 */
bool readFromSerial(String prompt, char * buf, int maxLen, int timeout)
{
    int timer = 0, delayTime = 1000;
    String input = "";
    if(maxLen <= 0)
    {
        // nothing can be read
        return false;
    }

    Serial.println(prompt);
    while(1)
    {
        input = Serial.readString();

        if(input.endsWith("\n"))
        {
          Serial.println("Removing trailing new line character.");
          input.replace("\n","");
        }
        
        int len = input.length();
        
        if(len > maxLen)
        {
            Serial.printf("Your input should less than %d character(s), your input was %d characters.\r\n", maxLen, len);
        }
        else if (len > 0)
        {
            // save the input into the buf
            sprintf(buf, "%s", input.c_str());
            return true;
        }

        // if timeout, return false directly
        timer += delayTime;
        if(timeout > 0 && timer >= timeout)
        {
            Serial.println("You entered no input, moving on...");
            return false;
        }
        // delay a time before next read
        delay(delayTime);
    }
}
