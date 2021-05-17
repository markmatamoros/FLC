byte piezoPins[19] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A12, 
                  A13, A14, A15, A16, A17, A18, A19, A20}; 

//hold piezo values
float currentRead[19];
float priorRead[19];
float furthestRead[19];

//hold MIDI values - converted piezo readings and notes
float midiVals[19];

int midiNotes[19] = {48, 52, 55, 59, 62, 53, 57, 60, 62, 
                     72, 64, 67, 71, 74, 65, 69, 84, 76, 79};

int waitFlag = 0;       //wait to pop notes... need to check
int noteOnFlag = 0;     //turn on notes

void setup() 
{
  Serial.begin(9600);

  for (int i = 0; i < 19; i++)
  {
    currentRead[i] = 0;
    priorRead[i] = 0;
    furthestRead[i] = 0;
  }
}

void loop() 
{
  //hold for 4 seconds 
  if (waitFlag == 0)
  {
    delay(4000);
    waitFlag = 1;
  }

  //trigger notes with maximum velocity
  if (noteOnFlag == 0)
  {
    for (int i = 0; i < 19; i++)
    {
    usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
    delay(20);
    noteOnFlag = 1;
    }
  }
  
  //read piezos and smooth readings for midi values
  for (int i = 0; i < 19; i++)
  {
    //read piezo sensors
    currentRead[i] = analogRead(piezoPins[i]);

    //cap values
    if (currentRead[i] > 15.0)
    {
      currentRead[i] = 15.0;
    }
    if (currentRead[i] < 1.1)
    {
      currentRead[i] = 0;
    }

    //low pass smoother
    currentRead[i] = (currentRead[i] + priorRead[i] + furthestRead[i])/3.0;

    //janky smoother
    if (priorRead[i] < currentRead[i])
    {
      currentRead[i] = currentRead[i] - ((currentRead[i] - priorRead[i]) / 1.5);
    }

    //low pass smoother... again
    currentRead[i] = (currentRead[i] + priorRead[i] + furthestRead[i])/3.0;

    //map values to midi range... with help
    midiVals[i] = map(currentRead[i], 0.0, 15.0, 0.0, 127.0) * 1.5; // * 3.5;

    //cap range if exceeded
    if (midiVals[i] > 127)
    {
      midiVals[i] = 127;
    }

    //set values for low pass smoother
    furthestRead[i] = priorRead[i];
    priorRead[i] = currentRead[i];

    usbMIDI.sendAfterTouchPoly(midiNotes[i], midiVals[i], 1);

    //print midi values
    /*Serial.print((int)midiVals[i]);
    Serial.print(", ");
    
    if (i == 18)
    {
      Serial.println();
    }*/
  }
  delay(5);
}
