#include "myHeader.h"




void Schedule(void){
  static Bool solarPanelInserted=FALSE;
  xTaskHandle solarPanelHandle=NULL, keypadHandle=NULL;
  xTaskCreate( vOLEDTask, ( signed portCHAR * ) "OLED", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
  xTaskCreate(DisplayManage, "DisplayManage", 100,&(myDisplay), 6,NULL);
  xTaskCreate(PowerManage, "PowerManage", 100,&(myPower), 1,NULL);
  xTaskCreate(ThrusterManage, "ThrusterManage", 100,&(myThruster), 2,NULL);
  xTaskCreate(ComsManage, "ComsManage", 100,&(myComs), 3,NULL);
  xTaskCreate(WarningManage, "WarningManage", 100,&(myWarning), 4,NULL);
  xTaskCreate(VComsManage, "VComsManage", 100,&(myVehComs), 5,NULL);
  xTaskCreate(Distance, "Distance", 100,&(myVehComs), 9,NULL);
  while(1){
    if(g_ulGPIOb){
      if(!solarPanelInserted){
        xTaskCreate(SolarPanel, "SolarPanel", 100,&(mySolarPanel), 7,solarPanelHandle);
        xTaskCreate(Keypad,"KeyPad",100,&(myKeypad),8,keypadHandle);
      } else{
        xTaskDelete(solarPanelHandle);
        xTaskDelete(keypadHandle);
      }
      g_ulGPIOb=0;
    }
    vTaskDelay(50);
  }
}

void Distance(void* Data)
{
  vehComs *data=(vehComs*)Data;
  static int s=0,f=0;
  while(1)
  {
    if(StartTimer && s==0)
    { s=globalCounter;
    StopTimer=FALSE;
    f=0;
    }
    if(StopTimer && f==0)
    {StartTimer=FALSE;
      f=globalCounter;
      *(data->dist)=(f-s)/100;
      f=0;
      
      
    }
    vTaskDelay(50);
  }
  
}

void DisplayManage(void *vParameters)
{
  
  display *mydisp = (display*)vParameters;
  //static portCHAR cMessage[ mainMAX_MSG_LEN ];
  xOLEDMessage xMessage;
  unsigned int x=0;
  unsigned int y=0;
  unsigned int xI=100;
  unsigned int yI=0;
  xMessage.X_Locs=&x;
  xMessage.Y_Locs=&y;
  xMessage.X_LocI=&xI;
  xMessage.Y_LocI=&yI;
  static int i=0;
  const char *T1Text = "DisplayManage is running\n\r";
  //sprintf(cMessage,"%u",GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6));
     // vOLEDStringDraw( cMessage, 0, 80, mainFULL_SCALE );

  //long button=0;
  while(1)
  {
      //RIT128x96x4StringDraw("dsp   ", 10, 80, 15); 
   // button =GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1);
    
    if (!g_ulFlags){i=0;vTaskDelay(50);g_ulFlags=TRUE; Clear=TRUE;}
      // goto exit;
    switch (mode){
      case TRUE:
        xMessage.msg=1;
          switch (i){
            
          case 0:
              xMessage.pIMessage=(mydisp->batteryLevel);
              x=0;      y=0; xI=105;    yI=0;
              xMessage.pcMessage="Battery  ";
              i++;
              goto exit;
            break;
          case 1:
                 
               xMessage.pIMessage=(mydisp->fuelLevel);
              x=0;      y=15; xI=105;    yI=15;
              xMessage.pcMessage="Fuel  ";
               i++;
               goto exit;
            break;
          case 2:
               xMessage.pIMessage=(mydisp->powerConsumption);
              x=0;      y=30; xI=105;    yI=30;
              xMessage.pcMessage="Power Consumption";
              i++;
              goto exit;
            break;
          case 3:
            
               xMessage.pIMessage=(mydisp->powerGeneration);
              x=0;      y=45; xI=105;    yI=45;
              xMessage.pcMessage="Power Generation";
              i++;
              goto exit;
            break;
            case 4:
               xMessage.pIMessage=(mydisp->solarPanelState);
              x=0;      y=60; xI=105;    yI=60;
              xMessage.pcMessage="Solar Panel";
              i=0;
              GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);  
              goto exit;
            break;   
          }
        break;
    case FALSE:
      xMessage.msg=0;
      switch(i){
        
          case 0:
              xMessage.pIMessage=(mydisp->batteryLevel);
              x=0;      y=0; xI=105;    yI=0;
              xMessage.pcMessage="Battery  ";
              i++;
              goto exit;
            break;
          case 1:
                 
               xMessage.pIMessage=(mydisp->fuelLevel);
              x=0;      y=15; xI=105;    yI=15;
              xMessage.pcMessage="Fuel  ";
               i++;
               goto exit;
            break;
          case 2:
               xMessage.pIMessage=(mydisp->batteryLow);
              x=0;      y=30; xI=105;    yI=30;
              if(*(mydisp->batteryLow)){
              xMessage.pcMessage="Battery Low      ";
              }else{xMessage.pcMessage="                        "; }
              i++;
              goto exit;
            break;
          case 3:
            
               xMessage.pIMessage=(mydisp->fuelLow);
              x=0;      y=45; xI=105;    yI=45;
              if(*(mydisp->fuelLow)){
              xMessage.pcMessage="Fuel Low      ";
              }else{xMessage.pcMessage="                    "; }
              i=0;
              goto exit;
            break;
            }
      }        
    exit:
      
      xQueueSend( xOLEDQueue, &xMessage, 0 );
      vTaskDelay(50);
    }
  }


void PowerManage(void *vParameters)
{
  power * myPower = (power*) vParameters;	// creates a pointer to void
//					// to the myPower data struct
  static int funcCount = 0;   // keeps track of the function calls
  static Bool incMethod = FALSE; // determines how powerConsumption is  
                                  // incremented
  unsigned long ulADC0_Value[1];
  unsigned long ulTemp_ValueC;
  
  
  while(1)
  {
        //RIT128x96x4StringDraw("Pwr    ", 10, 80, 15);          
      if(*(myPower -> solarPanelState))           // increments powerGeneration based on the
                                 // current
      {
        if(*(myPower->batteryLevel) > 31)	// switches the solarPanelState if battery above 95
        {							// resets the powerGeneration
          if(!(*myPower->solarPanelRetract)){
            g_ulGPIOb=1;
          }

          (*(myPower->solarPanelRetract)) = TRUE;
          (*(myPower->powerGeneration)) = 0;
        } 
        else 
        {

          (*(myPower->powerGeneration)) += 2;// increments powerGeneration based on current
            if(funcCount % 2)		// function count and batteryLevel
            {
              (*(myPower->powerGeneration))--;
                if(*(myPower->batteryLevel) > 18)
                  {
                    (*(myPower->powerGeneration))--;
                  }
            }
        }
      } 
      else 
      {
        if((*(myPower->batteryLevel)) <= 4)	// switches solarPanelState if battery below 11
        {
          if(!*(myPower -> solarPanelState))
          {
            if(!(*myPower->solarPanelDeploy))
            {
              g_ulGPIOb=1;
              
            }
            (*(myPower->solarPanelDeploy)) = TRUE;
          }
          (*(myPower -> powerGeneration)) = 13;// when solar panel is deployed powerGeneration
                                                                  // goes to 13 to ensure battery does not hit 0
        } 
      }
      
                                
                  
      if(*(myPower->powerConsumption) < 5)
      {
        incMethod = FALSE;     // switches the method of incrementation when
                               // powerConsumption goes below 5
      }
      else if(*(myPower->powerConsumption) > 10)
      {
        incMethod = TRUE;      // switches the method of incrementation when
                               // powerConsumption goes above 10
      }
      if(incMethod)            // increments powerConsumption based on the
                               // current increment method and funcCount
      {
        if(funcCount % 2)
          {
            (*(myPower->powerConsumption))++;
          } 
          else
          {
            (*(myPower->powerConsumption)) -= 2;
          }
      }
      else
      {
        if(funcCount % 2)
          {
            (*(myPower->powerConsumption))--;
          } 
          else 
          {
            (*(myPower->powerConsumption)) += 2;
          }
      }   
      funcCount++;
      if ((*(myPower->batteryLevel))<10)
      {
        *myPower->batteryLow = TRUE;
      } else {*myPower->batteryLow = FALSE;}
      
      char temp[20];

      if (Measure){
//          ADCSequenceDataGet(ADC_BASE, 0, g_ulIndex);
//          *(myPower -> batteryLevel) = g_ulIndex;
//        delay(5);
      ADCProcessorTrigger(ADC_BASE, 3);
      while(!ADCIntStatus(ADC_BASE, 3, false)) { }
      ADCIntClear(ADC_BASE, 3);
      ADCSequenceDataGet(ADC_BASE, 3, ulADC0_Value);
      *myPower -> batteryLevel =  32*ulADC0_Value[0]/(0x3FF);
      Measure=FALSE;
     // *myPower -> batteryLevel =  0;
      } else { *myPower -> batteryLevel= *myPower -> batteryLevel;}
      
      //DEBUG
      //sprintf(temp,"%u",*myPower -> batteryLevel );
      //RIT128x96x4StringDraw("         ", 10,80, 15);

      //RIT128x96x4StringDraw(temp, 10,80, 15);
      
      //DEBUG
    vTaskDelay(100);
  }
}
  
void ThrusterManage(void *vParameters)
{
  unsigned long period = 125000;
  static unsigned long dutyCycle = 62500;
 
  thruster * myThruster = (thruster*) vParameters;	// creates a pointer to void
                                                // to the myThruster data struct
  int duration;
  int magnitude;
 
  
  while(1)
  { 
    duration = *(myThruster -> thrusterCommand) & 0xff00;
    magnitude = *(myThruster -> thrusterCommand) & 240;
    
    
    dutyCycle = magnitude * period / 240;
    
    if(dutyCycle >= 0.99 * period)
    {
      dutyCycle = 0.99 * period;
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    
    GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, GPIO_PIN_1);
    
    GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_1, 0);
    
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);
    
    GPIOPadConfigSet(GPIO_PORTG_BASE, GPIO_PIN_1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
    
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, period);
    
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, dutyCycle);
    
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, 1); 
    
     // RIT128x96x4StringDraw("thrust   ", 10, 80, 15); 
    if(globalCounter % 500 == 0) 
      {

        
        if ((magnitude != 0) && (duration != 0) && (*(myThruster -> fuelLevel) != 0))
        {

          
          if (duration >= 1280)
          {
            int limit = *(myThruster -> fuelLevel) - (100 / (21600 / magnitude) * 5);
            if (limit <= 0)   // if fuel level will drop below 0 in the next 5 seconds
            {
              *(myThruster -> fuelLevel) = 0;	
              *(myThruster -> thrusterCommand) = 0x0000;	// thrusterCommand set to zeros to avoid unncesscary iteration of function
            }
            else
            {
              //(*fuelLevel) -= 100 / (37324800 / magnitude) * 5; // document specified fuelLevel decrease rate
              *(myThruster -> fuelLevel) -= 100 / (21600 / magnitude) * 5;    // fuelLevel decrease rate depends purely on magnitude
                                                                // 21600 = (5% * 1800 seconds) / 240 * 100
                                                                // fuel lasts for 1800 seconds at 5% (for demo purposes),
                                                                // is normalized by (/240) and the percentage magnitude is multiplied by 100
                                                                // similar calculations are used to derive the document
                                                                // specific fuelLevel consumption
              *(myThruster -> thrusterCommand) -= (256 * 5);                  //subtracts 5 seconds of remaining runtime for major cycle looping
            }
          }
          else
          {
            int limit = *(myThruster -> fuelLevel) - (100 / (2160 / magnitude) * (duration / 256));
            if (limit <= 0)   // if the fuel level will drop below 0 in the remaining duration
            {
                    *(myThruster -> fuelLevel) = 0;
                    *(myThruster -> thrusterCommand) = 0x0000;	// thrusterCommand set to zeros to avoid unncesscary iteration of function
            }
            else
            {
              //(*fuelLevel) -= 100 / ((37324800 / magnitude) * (duration / 256));
              *(myThruster -> fuelLevel) -= 100 / ((2160 / magnitude) * (duration / 256));  // decreases fuelLevel by magnitude and remaining duration
              *(myThruster -> thrusterCommand) -= duration;	// subtracts the remaining duration from thrusterCommand
            }
          }

          if (*(myThruster -> fuelLevel) <= 10)			// sets fuelLow to TRUE when fuel is below or equal to 10
          {
            *(myThruster -> fuelLow) = TRUE;
          }
          else							// sets fuelLow to FALSE when fuel is greater than 10
          {
            *(myThruster -> fuelLow) = FALSE;
          }
        } 
        else 
        {
          PWMGenDisable(PWM0_BASE, PWM_GEN_0);
          
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, 0);
        }
      }
    vTaskDelay(1000);
  }
}

void vOLEDTask( void *pvParameters )
{
   
    xOLEDMessage xMessage;
    unsigned portLONG ulY, ulMaxY;
    static portCHAR cMessage[ mainMAX_MSG_LEN ];
    extern volatile unsigned portLONG ulMaxJitter;
    unsigned portBASE_TYPE uxUnusedStackOnEntry;
    const unsigned portCHAR *pucImage;
    static int zz=0;
// Functions to access the OLED. 

    void ( *vOLEDInit )( unsigned portLONG ) = NULL;
    void ( *vOLEDStringDraw )( const portCHAR *, unsigned portLONG, unsigned portLONG, unsigned portCHAR ) = NULL;
    void ( *vOLEDImageDraw )( const unsigned portCHAR *, unsigned portLONG, unsigned portLONG, unsigned portLONG, unsigned portLONG ) = NULL;
    void ( *vOLEDClear )( void ) = NULL;
  
  
    vOLEDInit = RIT128x96x4Init;
    vOLEDStringDraw = RIT128x96x4StringDraw;
    vOLEDImageDraw = RIT128x96x4ImageDraw;
    vOLEDClear = RIT128x96x4Clear;
    ulMaxY = mainMAX_ROWS_96;
    pucImage = pucBasicBitmap;
              
// Just for demo purposes.
    uxUnusedStackOnEntry = uxTaskGetStackHighWaterMark( NULL );
  
    ulY = ulMaxY;
    
    /* Initialise the OLED  */
    vOLEDInit( ulSSI_FREQUENCY );	
    long button=0;
    xMessage.msg=1;
    while( 1 )
    {
       // RIT128x96x4StringDraw("Oled   ", 10, 80, 15); 
      if (Clear){
        vOLEDClear();Clear=FALSE;}
      
     // Wait for a message to arrive that requires displaying.
      
      xQueueReceive( xOLEDQueue, &xMessage, portMAX_DELAY );
  
      // Write the message on the next available row. 
      
//      ulY += mainCHARACTER_HEIGHT;
//      button=GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1);
//      if( button ==0)
//      {
//        if (zz==0){  //ulY = mainCHARACTER_HEIGHT;
//          vOLEDClear();zz++;
//        }else if (zz==50){zz=0;} else {zz++;}
//      } 
      sprintf(cMessage,"%u",GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1));
      vOLEDStringDraw( cMessage, 0, 80, mainFULL_SCALE );
  
      // Display the message  
     // vOLEDClear();                
      //if (xMessage.msg==1) 
      //{
       sprintf( cMessage, "%u", *(xMessage.pIMessage));
        vOLEDStringDraw( "        ", *xMessage.X_LocI, *xMessage.Y_LocI, mainFULL_SCALE );
        vOLEDStringDraw( cMessage, *xMessage.X_LocI, *xMessage.Y_LocI, mainFULL_SCALE );
        sprintf( cMessage, "%s", xMessage.pcMessage);      
        vOLEDStringDraw( cMessage, *xMessage.X_Locs, *xMessage.Y_Locs, mainFULL_SCALE );
      //}
      //else {
//        sprintf( cMessage, "%u", *(xMessage.pIMessage));
//        vOLEDStringDraw( "        ", *xMessage.X_LocI, *xMessage.Y_LocI, mainFULL_SCALE );
      //}
        
//        break;
      
        
      
      
      
  }
}
/*-----------------------------------------------------------*/

unsigned int receive(){
  return randomInteger(0, COMMAX);
}

void ComsManage(void *vParameters)
{
  coms *myComs=(coms*)vParameters;
  char temp;
  while(1)
  {
//    //send();
//    // receive message per major cycle, frequency 5/100
    if (globalCounter%1000==0&&randomInteger(0, 1000)<3) // 750
    {
      *(myComs->thrusterCommand)=receive();
      switch(receive()%6){
        case 0: 
          temp='F';
          break;
        case 1:
          temp='B';
          break;
        case 2:
          temp='L';
          break;
        case 3:
          temp='R';
          break;
        case 4:
          temp='D';
          break;
        default:
          temp='H';
          break;
      }
      if(*(myComs->response)!=temp){
        resVar=!resVar;
        *(myComs->response)=temp;
      }
    }
    vTaskDelay(1000);
  }
}

void VComsManage(void *vParameters)
{
  vehComs *myVehComs=(vehComs*)vParameters;
  while(1)
  {
    
    if(resVar){
      switch(*myVehComs->response){
        case 'F':
          UARTSend("A Forward", 9);
          break;
        case 'B':
          UARTSend("A Back", 6);
          break;
        case 'L':
          UARTSend("A Left", 6);
          break;
        case 'R':
          UARTSend("A Right", 7);
          break;
        case 'D':
          UARTSend("A Start", 7);
          break;
        case 'H':
          UARTSend("A Stop", 6);
      }
      resVar=!resVar;
    }
    vTaskDelay(1000);
  }
}

void SolarPanel(void *vParameters)
{
  
  static unsigned short motorDrive = 0;
  unsigned long period = 125000;
  static Bool Flag = FALSE;
  static unsigned long dutyCycle = 62500;
  static int remainingDeployment = 1000;
  
  while(1)
  {
    solarPanel * myPanel = (solarPanel*) vParameters;
  // DEREREFWEJFIOEAWN DEREFERENCE POINTERS
  
  //IntEnable(INT_GPIOB);

//  SysCtlPWMClockSet(SYSCTL_PWMDIV_32);
//  if (*(myPanel->solarPanelRetract))
//  {  
//    remainingDeployment = 0;
//  }
//  else{ 
//    remainingDeployment = 1000;
  // 100% motor drive * 10 seconds
  
//  
//  char temp[20];
//  sprintf(temp,"%u",*(myPanel->solarPanelDeploy));
//  RIT128x96x4StringDraw(temp, 40, 80, 15);
//  sprintf(temp,"%u",*myPanel->solarPanelRetract);
//  RIT128x96x4StringDraw(temp, 60, 80, 15);
//    sprintf(temp,"%u",remainingDeployment);
//  RIT128x96x4StringDraw(temp, 80, 80, 15);
  

  if(!((*(myPanel -> solarPanelDeploy) && 0 == remainingDeployment) && 
       (*(myPanel -> solarPanelRetract) && 1000 == remainingDeployment)))
    {
    if(*myPanel->solarPanelDeploy || *myPanel->solarPanelRetract)
    {
    
      SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);

      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
     
      GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
      
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);
      
      GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
      
      GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
      
      PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
      
      PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, period);

      PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, dutyCycle);

      PWMGenEnable(PWM0_BASE, PWM_GEN_0);
      
      PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, 1);
    }
    if(*(myPanel -> driveMotorSpeedInc))
      {
        if((dutyCycle + (0.05 * period)) < period)     // ensures duty cycle does not exceed 100%
        {
          dutyCycle += (period * 0.05); // Increments motor speed by 5%
        } 
        (*(myPanel -> driveMotorSpeedInc)) = FALSE;
      }
    else if(*(myPanel -> driveMotorSpeedDec))
      {
        int limit = dutyCycle - (0.05 * period);
        if(limit > 0)          // ensures duty cycle does not drop below 0%
        {
          dutyCycle -= (period * 0.05); // Decrements motor speed by 5%
        }
        (*(myPanel -> driveMotorSpeedDec)) = FALSE;
      }
    
    motorDrive = dutyCycle * 100 / period; // (dutyCycle / period) * 100% / 10
                                          // iterations per second 
                                          // solar panels are set up such that
                                          // 10 seconds of 100% motorDrive will 
                                          // fully retract or detract a solar panel
                                          // from a full ON or OFF state respectively
    if(*(myPanel -> solarPanelDeploy))
      {
        if((remainingDeployment - motorDrive) < 0)
        {
          remainingDeployment = 0;
        }
        else 
        {
          remainingDeployment -= motorDrive;
        }
      } 
    else if(*(myPanel->solarPanelRetract))
      {
        if((remainingDeployment + motorDrive) > 1000)
        {
          remainingDeployment = 1000;
        }
        else
        {
          remainingDeployment += motorDrive;
        }
      }
    
    if(remainingDeployment == 0 && *(myPanel -> solarPanelDeploy))
      {
        *(myPanel -> solarPanelDeploy) = FALSE;
        *(myPanel -> solarPanelState) = TRUE;
        g_ulGPIOb=1;
        PWMGenDisable(PWM0_BASE, PWM_GEN_0);
        PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, 0);
      }
    else if(remainingDeployment == 1000)
    {
      if(*(myPanel -> solarPanelRetract))
      {
        *(myPanel -> solarPanelRetract) = FALSE;
        *(myPanel -> solarPanelState) = FALSE;
        g_ulGPIOb=1;
        PWMGenDisable(PWM0_BASE, PWM_GEN_0);
        PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, 0);
      }
    }
    }
    vTaskDelay(500);
  }
}
void Keypad(void*data)
{
  while (1)
  {
          // RIT128x96x4StringDraw(" KKEYPAD   ", 20, 80, 15);
     
       keypad *myKey = (keypad*) data;
       if (UpFlag){
       *myKey->driveMotorSpeedInc=TRUE; UpFlag=FALSE;
       }
       if(globalCounter%500==0)
       {
          
          GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_2); 
          IntEnable(INT_GPIOE);
       }
       if(DownFlag){
       *myKey->driveMotorSpeedDec=TRUE;DownFlag=FALSE;
       }
       if(globalCounter%500==0)
       { 
         
          GPIOPinIntClear(GPIO_PORTD_BASE, GPIO_PIN_5);
        IntEnable(INT_GPIOD);
        }

    vTaskDelay(500);
  }
}
void WarningManage(void *vParameters)

{
  static short timer =0; // minor cycle is 1/6 a second
  static short timer2= 0; // timer 1 for 1 second int timer 2 for 2 sec inv
  static Bool t1 = FALSE;
  static Bool t2 = FALSE;
    while(1)
  {
      //RIT128x96x4StringDraw("warn   ", 10, 80, 15); 

  warning *mywarning= (warning*)vParameters;
//PORTC -  4 (PhA0) red, 5 (PC5) yellow, 6 (PhB0), 7(PC7)green 
  // 0xFF turn on 0x00 turn off
  //GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0xFF);
  
  
  if ((*(mywarning->fuelLevel) > 50) && (*(mywarning->batteryLevel) > 18))
    {       //RIT128x96x4StringDraw("Case 0", 00, 75, 15);   
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0xFF); // turn on green
             GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00); // turn off yellow
              GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // turn off red
            goto exit ;
    } // green
  
  if ((*(mywarning->fuelLevel) >50)&&(*(mywarning->batteryLevel) > 4))
  {     
       // RIT128x96x4StringDraw("Case 1", 00, 75, 15); 
  //fuel > 50 > battery > 10
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00); // turn off green
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // turn off red
    
        if (t1 && 0==timer) //  if its on turn it off at timer == 0
        { 
          GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00);
          t1=!t1;
        } else if ( !t1 && 0 == timer) { // if its off turn it onn at timer 0
          GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF);
          t1=!t1;
        }
        goto exit;
    } // yellow @ 1 sec 
  
   //fuel > 50  battery <10 
  if ((*(mywarning->fuelLevel) >50)&&(*(mywarning->batteryLevel) <5))
  {     
   // RIT128x96x4StringDraw("Case 2", 00, 75, 15);
      // fuel > 50 || 10 > battery > 0
      GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00); // turn off green
      GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00); // turn off yellow

     if (t1 && 0==timer) //  if its on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // red
        t1=!t1;
      } else if ( !t1 && 0 == timer) { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF); // red
        t1=!t1;
      }

    goto exit;
  } // red @ 1 sec
  // 50>fuel | battery > 50
  if ((*(mywarning->fuelLevel) > 10) && (*(mywarning->batteryLevel) > 18))
    {     
      GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00); // turn off green
      GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // turn off Red
     // RIT128x96x4StringDraw("Case 3", 00, 75, 15);    
       if (t2 && 0==timer2) //  if its on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00); // yellow
        t2=!t2;
      } 
      else if ( !t2 && 0 == timer2) 
      { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF); // yellow
        t2=!t2;
      }        
      
      
      if (t1 && 0==timer) //  if its on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00);
        t1=!t1;
      } 
      else if ( !t1 && 0 == timer) { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF);
        t1=!t1;
      }       
      goto exit;
    } // yellow @ 2 sec
   // 50>fuel |50 > battery > 10
  if ((*(mywarning->fuelLevel) >10)&&(*(mywarning->batteryLevel) > 4))
  { 
 
    //RIT128x96x4StringDraw("Case 4", 00, 75, 15); 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00); // turn off green
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // turn off red
    
      if (t2 && 0==timer2) //  if its on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00); // yellow
        t2=!t2;
      } 
      else if ( !t2 && 0 == timer2) 
      { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF); // yellow
        t2=!t2;
      }        
      
      
      if (t1 && 0==timer) //  if its on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00);
        t1=!t1;
      } 
      else if ( !t1 && 0 == timer) { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF);
        t1=!t1;
      }    
      
      goto exit;
  } // yellow @ 2 sec  && yellow @ 1 sec
  // 50 > fuel | 10 > battery
  if ((*(mywarning->fuelLevel) >10)&&(*(mywarning->batteryLevel) <5))
  { 
    //RIT128x96x4StringDraw("Case 5", 00, 75, 15);
       GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00); // turn off green
    
      if (t1 && 0==timer) //  if Red on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // red
        t1=!t1;
      } 
      else if ( !t1 && 0 == timer) { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF); // red
        t1=!t1;
      }
         
       if (t2 && 0==timer2) //  if yellow on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00); // yellow
        t2=!t2;
      } 
       else if ( !t2 && 0 == timer2) 
      { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF); // yellow
        t2=!t2;
      }   
      goto exit;
  } // yellow @ 2 sec && red @ 1 sec
  // 10> fuel | battery>50
    if ((*(mywarning->fuelLevel) < 11) && (*(mywarning->batteryLevel) > 18))
    {    
    
  //    RIT128x96x4StringDraw("Case 6", 00, 75, 15);  
       GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00); // turn off green
     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00); // turn off yellow
     
     
      if (t2 && 0==timer2) //  if Red on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // red
        t2=!t2;
      } 
      else if ( !t2 && 0 == timer2) { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF); // red
        t2=!t2;
      }
      
      goto exit;
    } // red @ 1 sec 
  // 10>fuel | 50 > battery
  if ((*(mywarning->fuelLevel) <11)&&(*(mywarning->batteryLevel) > 4))
  {    
    
    //RIT128x96x4StringDraw("Case 7", 00, 75, 15); 
       GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00);// turn off green
    
    
      if (t2 && 0==timer2) //  if Red on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // red
        t2=!t2;
      } 
      else if ( !t2 && 0 == timer2) { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF); // red
         t2=!t2;
      }
      
      
      if (t1 && 0==timer) //  if yellow on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00);
         t1=!t1;
      } 
      else if ( !t1 && 0 == timer) { // if yellow off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF);
         t1=!t1;
      }  
      goto exit;
  } // yellow @ 2 sec && red @ 1 sec
   // 10>fuel | 10 > battery 
  if ((*(mywarning->fuelLevel) <11)&&(*(mywarning->batteryLevel) <5))
  {    
    //RIT128x96x4StringDraw("Case 8", 00, 75, 15);  
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00);// turn off green
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00);// turn off yellow 
    
      if (t2 && 0==timer2) //  if Red on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // red
        t2=!t2;
      } 
      else if ( !t2 && 0 == timer2) { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF); // red
        t2=!t2;
      }
      
      if (t1 && 0==timer) //  if Red on turn it off at timer == 0
      { 
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // red
        t1=!t1;
      } 
      else if ( !t1 && 0 == timer) { // if its off turn it onn at timer 0
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF); // redt
        t1=!t1;
      }
      goto exit;
  
  } // red @ 2 sec && red @ 1 sec


exit :
   if (5 == timer)
   { timer = 0; }
   else { timer ++;}
  
  if ( 10 == timer2)
  {timer2=0;}
  else { timer2++;}
    
    
    vTaskDelay(100);
  }
}
