/*
 * Project doorSensor
 * Description:
 * Author:
 * Date:
 */

 //The average open-close entry period.
 #define AverageEntryTime 7
 //The door open alert period in seconds.
 #define OpenAlertTime 300

 int reed = A0;

 long int triggerTime = 0;
 long int openPeriod = 0;
 long int lastTimeNotificationIndex = 0;
 bool doorOpen = false;
 bool doorOpenNotification = false;
 int AlertIntervals = 0;
 int DoorOpenNotificationTime = 0;
 //Converting the openAlertTime to minutes.
 int openAlertPeriod = OpenAlertTime/60;

 void setup() {
     //Setting reed(a0) to input for taking in
     pinMode(reed, INPUT);
     //Exposing the doorOpen flag to the cloud for debugging
     Particle.variable("DoorOpen", doorOpen);
     Particle.variable("TriggerTime", triggerTime);
     Particle.variable("DoorOpenNotificationTime", DoorOpenNotificationTime);
     //Initial state detection.
     if(analogRead(reed) > 4000){
       doorOpen = true;
       //TODO [added 5/30/2020 10:37am] - add tiggerTime value.
     }
 }

 void loop() {

      while(!doorOpen){
        if(analogRead(reed) > 4000){
            //Setting the doorOpen flag to true to reflect the door being open.
            doorOpen = true;
            //Setting trigger time to the current time for notification tracking.
            triggerTime = Time.now();
            //Publishing an event to recognize door state change to open.
            Particle.publish("Door_open", NULL, 0, PRIVATE);
        }
      }

      while(doorOpen){
        if((analogRead(reed) < 4000)){
            //Setting doorOpen flag to flase to reflect the door being closed.
            doorOpen = false;
            //Saving the time the door was open for.
            openPeriod = doorOpenPeriod(Time.now());
            //If the door was closed within the OpenAlertTime a "Your door WAS opened" notification is sent.
            if(openPeriod <= AverageEntryTime){
                //Publishing a quick open-close event with the openPeriod value.
                Particle.publish("Your_door_was_opened", String::format("%ld", openPeriod), 0, PRIVATE);
            }
            else{
                //Publishing an extended period door close event with the openPeriod value.
                Particle.publish("Your_door_is_now_closed", String::format("%ld", openPeriod), 0, PRIVATE);
            }
            //Resetting lastTimeNotification vaiable for propper notification behavior.
            lastTimeNotificationIndex = 0;
            //Resetting doorOpenNotification flag.
            doorOpenNotification = false;
        }
        else if((doorOpenPeriod(Time.now()) / OpenAlertTime) > lastTimeNotificationIndex){
            //Saving the interval calculation for ease of code reading.
            AlertIntervals = doorOpenPeriod(Time.now()) / OpenAlertTime;
            //Set DoorOpenNotificationTime for IFTTT notification data pull.
            DoorOpenNotificationTime = AlertIntervals * openAlertPeriod;
            //Particle event publish your door has been openfor x minutes.
            Particle.publish("Extended_open_period", String::format("%d", AlertIntervals * openAlertPeriod), 0, PRIVATE);
            //Increment last time notification.
            lastTimeNotificationIndex++;
        }
        else if(!doorOpenNotification && (doorOpenPeriod(Time.now()) > AverageEntryTime)){
            Particle.publish("Your_door_is_open", NULL, 0, PRIVATE);
            //Setting doorOpen Notification to true to prevent duplicate notifications.
            doorOpenNotification = true;
        }
      }



    //  //Door open conditional
    //  if((analogRead(reed) > 4000) && !doorOpen){
    //      Particle.publish("Door_open", NULL, 0, PRIVATE);
    //      //Setting the doorOpen flag to true to reflect the door being open.
    //      doorOpen = true;
    //      //Setting trigger time to the current time for notification tracking.
    //      triggerTime = Time.now();
    //  }
    //  //Door closed conditional
    //  else if((analogRead(reed) < 4000) && doorOpen){
    //      //Setting doorOpen flag to flase to reflect the door being closed.
    //      doorOpen = false;
    //      //Saving the time the door was open for.
    //      openPeriod = doorOpenPeriod(Time.now());
    //      //If the door was closed within the OpenAlertTime a "Your door WAS opened" notification is sent.
    //      if(openPeriod <= AverageEntryTime){
    //          //Publishing a quick open-close event with the openPeriod value.
    //          Particle.publish("Your_door_was_opened", String::format("%ld", openPeriod), 0, PRIVATE);
    //      }
    //      else{
    //          //Publishing an extended period door close event with the openPeriod value.
    //          Particle.publish("Your_door_is_now_closed", String::format("%ld", openPeriod), 0, PRIVATE);
    //      }
    //      //Resetting lastTimeNotification vaiable for propper notification behavior.
    //      lastTimeNotificationIndex = 0;
    //      //Resetting doorOpenNotification flag.
    //      doorOpenNotification = false;
    //  }
    //  //Otherwise Notification conditional.
    //  else if(((doorOpenPeriod(Time.now()) / OpenAlertTime) > lastTimeNotificationIndex) && doorOpen){
    //      //Saving the interval calculation for ease of code reading.
    //      AlertIntervals = doorOpenPeriod(Time.now()) / OpenAlertTime;
    //      //Set DoorOpenNotificationTime for IFTTT notification data pull.
    //      DoorOpenNotificationTime = AlertIntervals * openAlertPeriod;
    //      //Particle event publish your door has been openfor x minutes.
    //      Particle.publish(String::format("Your_door_has_been_open_for_%d_minutes", AlertIntervals * openAlertPeriod), "Extended_open_period", 0, PRIVATE);
    //      //Increment last time notification.
    //      lastTimeNotificationIndex++;
    //  }
    //  else{
    //      //If the door has been open for 7 seconds and a door open notification has not been sent one will be sent.
    //      if(doorOpen && (doorOpenPeriod(Time.now()) > AverageEntryTime) && !doorOpenNotification){
    //          Particle.publish("Your_door_is_open", NULL, 0, PRIVATE);
    //          doorOpenNotification = true;
    //      }
    //  }
 }

 long int doorOpenPeriod(long int time){
     return (time - triggerTime);
 }
