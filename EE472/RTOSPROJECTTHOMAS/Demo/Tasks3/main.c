#define mainINCLUDE_WEB_SERVER		0


/* Standard includes. */
#include <stdio.h>
#include <math.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Project Files
#include "myHeader.h"

/* Hardware library includes. */
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_sysctl.h"
#include "debug.h"
#include "hw_ints.h"
#include "hw_nvic.h"
#include "gpio.h"
#include "interrupt.h"
#include "systick.h"
#include "timer.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"
#include "sysctl.h"
#include "gpio.h"
#include "grlib.h"
#include "rit128x96x4.h"
#include "osram128x64x4.h"
#include "formike128x128x16.h"

// Button interrupt FLAGS
Bool g_ulFlags=TRUE, mode=TRUE;


/* Demo app includes. */

#include "lcd_message.h"
#include "bitmap.h"

#define mainCHECK_DELAY	( ( portTickType ) 5000 / portTICK_RATE_MS )

// Size of the stack allocated to the uIP task.
#define mainBASIC_WEB_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 3 )

// The OLED task uses the sprintf function so requires a little more stack too.
#define mainOLED_TASK_STACK_SIZE	    ( configMINIMAL_STACK_SIZE + 50 )

//  Task priorities.
#define mainQUEUE_POLL_PRIORITY		    ( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		    ( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY		    ( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY		    ( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY	    ( tskIDLE_PRIORITY )


//  The maximum number of messages that can be waiting for display at any one time.
  #define mainOLED_QUEUE_SIZE					( 10 )

// Dimensions the buffer into which the jitter time is written. 
  #define mainMAX_MSG_LEN						25

/* 
  The period of the system clock in nano seconds.  This is used to calculate
  the jitter time in nano seconds. 
*/

#define mainNS_PER_CLOCK ( ( unsigned portLONG ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )


// Constants used when writing strings to the display.

#define mainCHARACTER_HEIGHT		    ( 9 )
#define mainMAX_ROWS_128		    ( mainCHARACTER_HEIGHT * 14 )
#define mainMAX_ROWS_96			    ( mainCHARACTER_HEIGHT * 10 )
#define mainMAX_ROWS_64			    ( mainCHARACTER_HEIGHT * 7 )
#define mainFULL_SCALE			    ( 15 )
#define ulSSI_FREQUENCY			    ( 3500000UL )


#define ABS(x)  (((x)<0)?(-(x)):(x))
#define CEILING(x) (((x)>511)?511:(x))
/*-----------------------------------------------------------*/

/*
 * The task that handles the uIP stack.  All TCP/IP processing is performed in
 * this task.
 */
extern void vuIP_Task( void *pvParameters );

/*
 * The display is written two by more than one task so is controlled by a
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the display directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 */

static void vOLEDTask( void *pvParameters );

/*
 * Configure the hardware .
 */
static void prvSetupHardware( void );

/*
 * Configures the high frequency timers - those used to measure the timing
 * jitter while the real time kernel is executing.
 */
extern void vSetupHighFrequencyTimer( void );

/*
 * Hook functions that can get called by the kernel.
 */
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
void vApplicationTickHook( void );

/*
  three dummy tasks of different priorities that simply run, announce 
  themselves, then sleep
*/

void DisplayManage(void *vParameters);
void PowerManage(void *vParameters);
void ThrusterManage(void *vParameters);
void ComsManage(void *vParameters);
void SolarPanel(void *vParameters);
void WarningManage(void *vParameters);
void VComsManage(void *vParameters);
void keypadManage(void*data);
void Schedule(void);
void Distance(void* Data);
void Pirate(void);
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
void ImageCapture(void *vParameters);
signed int optfft(signed int x[256], signed int y[256]);
void PirateManage(void * Pirates);
void delay(unsigned long aValue);
/*-----------------------------------------------------------*/

/* 
  The queue used to send messages to the OLED task. 
*/
xQueueHandle xOLEDQueue;

/*-----------------------------------------------------------*/
//initialize data structs//
void InitData( void );


//Global Variables
    int globalCounter=0;

    unsigned short batteryLevel=32, fuelLevel=100, powerConsumption=1, powerGeneration=0, BattTempKelvin=200;
    unsigned int batteryBuffer[16];
    unsigned int thrusterCommand=65418, *batteryLevelPtr=batteryBuffer;
    Bool solarPanelState=FALSE, solarPanelDeploy=FALSE, solarPanelRetract=FALSE,
            fuelLow=FALSE, batteryLow=FALSE, driveMotorSpeedInc=FALSE,
            driveMotorSpeedDec=FALSE, Measure=FALSE, Clear=FALSE, resVar=FALSE;
    Bool UpFlag=FALSE,DownFlag=FALSE,StartTimer=FALSE,StopTimer=FALSE, distFLAG=FALSE,
              TFLAG=FALSE, BTempAck=FALSE, Pirates = FALSE, startCapture=FALSE, finishCapture=FALSE;
    char command=NULL, response=NULL, Ctype=NULL;
    volatile unsigned long g_ulGPIOb , distance=100;
    char payload[20],Response[30];
    static Bool Trigger=TRUE;
    power myPower;
    solarPanel mySolarPanel;
    keypad myKeypad;
    thruster myThruster;
    coms myComs;
    vehComs myVehComs;
    display myDisplay;
    warning myWarning; 
    Command myCommand;
    image myImage;

//	Bool BattOTemp = TRUE

    Bool battOverTemp = TRUE;
    unsigned short battTemp = 300, secondTemp = 300;
    signed int rawDataBuffer[256];
    signed int imageBuffer[256];
    signed int *imageDataRawPtr = rawDataBuffer;
    signed int *imageDataPtr = imageBuffer;
    signed int pendingBuffer[16];
    signed int *imageDataPending = pendingBuffer;
    unsigned short peakFreq = 0;
    unsigned int battTempBuffer[16];
    unsigned int *battTempPtr=battTempBuffer;
    //init display data
    
    xTaskHandle commHandle=NULL;


int main( void )
{
    InitData();
    prvSetupHardware();

    /*  
        Create the queue used by the OLED task.  Messages for display on the OLED
        are received via this queue. 
    */
    
    xOLEDQueue = xQueueCreate( mainOLED_QUEUE_SIZE, sizeof( xOLEDMessage ) );
    
    xTaskCreate(Schedule,"Schedule",100,NULL,10,NULL);
    
    vSetupHighFrequencyTimer();
    vTaskStartScheduler();

    return 0;
}

void Schedule(void){
  static Bool solarPanelInserted=FALSE;
  xTaskHandle solarPanelHandle=NULL, keypadHandle=NULL, imageHandle=NULL;
  xTaskCreate( vOLEDTask, ( signed portCHAR * ) "OLED", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
  xTaskCreate(DisplayManage, "DisplayManage", 75,&(myDisplay), 5,NULL);
  xTaskCreate(PowerManage, "PowerManage", 75,&(myPower), 1,NULL);
  xTaskCreate(ThrusterManage, "ThrusterManage", 75,&(myThruster), 2,NULL);
  if( SysCtlPeripheralPresent( SYSCTL_PERIPH_ETH ) )
  xTaskCreate(ComsManage, "ComsManage", 400,&(myComs), 10,NULL);
  xTaskCreate(WarningManage, "WarningManage", 100,&(myWarning), 3,NULL);
  xTaskCreate(VComsManage, "VComsManage", 50,&(myVehComs), 4,NULL);
  xTaskCreate(Distance, "Distance", 50,&(myVehComs), 8,NULL);
  xTaskCreate(CommandManage,"Commandmanage",75,&(myCommand),9,&commHandle);  
  xTaskCreate(ImageCapture, "ImageCapture", 600, &(myImage), 11, NULL);
  int z=0;
  while(1){
    if(g_ulGPIOb){
      if(!solarPanelInserted){
        if(solarPanelHandle==NULL){
          xTaskCreate(SolarPanel, "SolarPanel", 75,&(mySolarPanel), 6,&solarPanelHandle);
          xTaskCreate(keypadManage,"KeyPad",50,&(myKeypad),7,&keypadHandle);
        } else{
          vTaskResume(solarPanelHandle);
          vTaskResume(keypadHandle);
        }
        solarPanelInserted=!solarPanelInserted;
      } else{
        vTaskSuspend(solarPanelHandle);
        vTaskSuspend(keypadHandle);
      }
      g_ulGPIOb=0;
    }
    if(startCapture){
      if(imageHandle==NULL)
        xTaskCreate(ImageCapture, "ImageCapture", 600, &(myImage), 11, &imageHandle);
      else 
        vTaskResume(imageHandle);
    } else if(imageHandle!=NULL)
        vTaskSuspend(imageHandle);
    startCapture=FALSE;
    z=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_2);
      if (z==4)
      {
          GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_5, 0xFF);
     }else if (z==0){
          GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_5, GPIO_LOW_LEVEL);//ENABLE MANAGE TASK
      }

    globalCounter++;

    
    vTaskDelay(50);

}
}

void CommandManage(void*mydata)
{
  Command *PCDat=(Command*)mydata;
static Bool OLED=TRUE;
  while(1)
  {
    
    PCDat=(Command*)mydata;
    switch (*(PCDat->cType))
    {
    case 'P':
      sprintf(Response,"A stop",*PCDat->batteryLevel);
      IntDisable(INT_GPIOC|INT_GPIOD|INT_GPIOE|INT_GPIOF|INT_GPIOG);
        break;
    case 'S':
      sprintf(Response,"A start",*PCDat->batteryLevel);
      IntEnable(INT_GPIOC|INT_GPIOD|INT_GPIOE|INT_GPIOF|INT_GPIOG);
      break;
    case 'D':
      sprintf(Response,"A OLED",*PCDat->batteryLevel);
         switch (OLED)
         {
         case TRUE:
          RIT128x96x4Disable();
          OLED=FALSE;
          break;
         case FALSE:      
           RIT128x96x4Init(ulSSI_FREQUENCY)	;
            OLED=TRUE;
            break;
         default:
           break;
         }
      break;
    case 'T':
      sprintf(Response,"A thruster command",*PCDat->batteryLevel);
      *(PCDat->thrusterCommand)=atoi(PCDat->Payload);
      break;
    case 'M':
      *(PCDat->TransmitFlag)=TRUE;
      switch(*(PCDat->Payload))
      {
      case 'B':
        sprintf(Response,"BatteryLevel:.%u",*PCDat->batteryLevel);
       break;
      case 'F':
        sprintf(Response,"FuelLevel:.%u",*PCDat->fuelLevel);
        break;
      default:
        sprintf(Response,"B.%u.F.%u",*PCDat->batteryLevel,*PCDat->fuelLevel);
        break;        
      }
      
      break;
//    case 'A':
//      *(PCDat->TransmitFlag)=TRUE;
//      sprintf(Response,"%c.%u.ACK",*PCDat->cType,*PCDat->Payload);
//      break;
    default:
      *(PCDat->TransmitFlag)=TRUE;
      (PCDat->Response)="ERROR";
      break;
      
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
    switch (mode)
    {
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
              x=0;      y=12; xI=105;    yI=12;
              xMessage.pcMessage="Fuel  ";
               i++;
               goto exit;
            break;
          case 2:
               xMessage.pIMessage=(mydisp->powerConsumption);
              x=0;      y=24; xI=105;    yI=24;
              xMessage.pcMessage="Power Consumption";
              i++;
              goto exit;
            break;
          case 3:
            
               xMessage.pIMessage=(mydisp->powerGeneration);
              x=0;      y=36; xI=105;    yI=36;
              xMessage.pcMessage="Power Generation";
              i++;
              goto exit;
            break;
            case 4:
               xMessage.pIMessage=(mydisp->solarPanelState);
              x=0;      y=48; xI=105;    yI=48;
              xMessage.pcMessage="Solar Panel";
              i=5;
              goto exit;
            break;  
            case 5:
              xMessage.pIMessage=(mydisp->VehDist);
              x=0;      y=60; xI=105;    yI=60;
              xMessage.pcMessage="Veh Distance";
              i=6;
              goto exit;
            break;  
            case 6:
              xMessage.pIMessage=(mydisp->battTemp);
              x=0;      y=72; xI=105;    yI=72;
              xMessage.pcMessage="Battery Temp 1";
              i=7; 
              goto exit;          
//            case 7:
//              xMessage.pIMessage = (mydisp->secondTemp);
//              x=0;      y=84; xI=105;    yI=84;
//              xMessage.pcMessage = "Battery Temp 2 (K)";
//              i=0;
//              GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);
//              goto exit;
            case 7:
              xMessage.pIMessage = (mydisp->peakFreq);
              x=0;      y=84; xI=70;    yI=84;
              xMessage.pcMessage = "PeakFreq";
              i=0;
              GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);
              goto exit;
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
              i=4;
              goto exit;
            break;
            case 4:
              xMessage.pIMessage=(mydisp->battTemp);
              x=0;      y=60; xI=105;    yI=60;
              xMessage.pcMessage="Battery Temp 1";
              i=5; 
              goto exit;              
            case 5:
              xMessage.pIMessage = (mydisp->secondTemp);
              x = 0;      y = 75; xI = 105;    yI = 75;
              xMessage.pcMessage = "Battery Temp 2";
              i=0;
              GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);
              goto exit;
            }
      exit:
      
      xQueueSend( xOLEDQueue, &xMessage, 0 );
      vTaskDelay(50);
    }
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
  
  int prevMax = 373;
  int currentMax = 0;
  int index = 0, k = 0;

  unsigned long ulADC1_Value[1];
  unsigned long ulADC2_Value[1];

  while(1)
  {
    HWREG(NVIC_SW_TRIG)=INT_GPIOG-16;
    //char temp[20];

    if (Measure)
	{
        //          ADCSequenceDataGet(ADC_BASE, 0, g_ulIndex);
        //          *(myPower -> batteryLevel) = g_ulIndex;
          delay(5);
          if(index > 15) {index = 0;}
          ADCProcessorTrigger(ADC_BASE, 3);
          while(!ADCIntStatus(ADC_BASE, 3, false)) { }
          ADCIntClear(ADC_BASE, 3);
          ADCSequenceDataGet(ADC_BASE, 3, ulADC0_Value);
          *myPower -> batteryLevel = 32*ulADC0_Value[0]/(0x3FF);
          (myPower -> batteryLevelPtr)[index] = *myPower -> batteryLevel;
          Measure=FALSE;
        
        // *myPower -> batteryLevel =  0;
        } 
	else 
	{ 
          *myPower -> batteryLevel= *myPower -> batteryLevel;
          (myPower -> batteryLevelPtr)[index] = *myPower -> batteryLevel;
	}
	if (*(myPower->solarPanelState))
	{
            delay(4);
            if(k > 15) {k = 0;}
            ADCProcessorTrigger(ADC_BASE, 2);
            while (!ADCIntStatus(ADC_BASE, 2, false)) {}
            ADCIntClear(ADC_BASE, 2);
            ADCSequenceDataGet(ADC_BASE, 2, ulADC1_Value);
            *(myPower->battTemp) = (13 * ulADC1_Value[0] / (0x3FF) / 4) * 32 + 33 + 273;
            (myPower->battTempPtr)[k] = 325*ulADC1_Value[0] / (0x3FF);
            k++;
            if (*(myPower->battTemp) > currentMax)
            {
              currentMax = (myPower->battTemp)[0];
            }
            if ((*(myPower->battTemp) * 6 / 5) > prevMax)
            {
              *(myPower->battOverTemp) = TRUE;
            }

            if(k > 15) {k = 0;}
            ADCProcessorTrigger(ADC_BASE, 1);
            while (!ADCIntStatus(ADC_BASE, 1, false)) {}
            ADCIntClear(ADC_BASE, 1);
            ADCSequenceDataGet(ADC_BASE, 1, ulADC2_Value);

            *(myPower->secondTemp) = (13 * ulADC2_Value[0] / (0x3FF) / 4) * 32 + 33 + 273;
            (myPower->battTempPtr)[k] = 325*ulADC2_Value[0] / (0x3FF);
            k++;
            if (*(myPower->secondTemp) > currentMax)
            {
              currentMax = *(myPower->secondTemp);
            }
              if ((*(myPower->secondTemp) * 6 / 5) > prevMax)
            {
              *(myPower->battOverTemp) = TRUE;
            }

            prevMax = currentMax;
            currentMax = 0;
          
	}

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
    if(globalCounter % 50 == 0) 
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
//      sprintf(cMessage,"%u",GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1));
      //vOLEDStringDraw( cMessage, 0, 80, mainFULL_SCALE );
  
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

        sprintf( cMessage, "%u", GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2));
        vOLEDStringDraw( cMessage, 50, 0, mainFULL_SCALE );
        
          
  
        
      
      
      
  }
}
/*-----------------------------------------------------------*/

unsigned int receive(){
  return randomInteger(0, COMMAX);
}

//void ComsManage(void *vParameters)
//{
//  coms *myComs=(coms*)vParameters;
//  char temp;
//  static Bool capture=FALSE;
//  while(1)
//  {
////    //send();
////    // receive message per major cycle, frequency 5/100
//    if (globalCounter%1000==0&&randomInteger(0, 1000)<3) // 750
//    {
//      *(myComs->thrusterCommand)=receive();
//      switch(receive()%7){
//        case 0: 
//          temp='F';
//          break;
//        case 1:
//          temp='B';
//          break;
//        case 2:
//          temp='L';
//          break;
//        case 3:
//          temp='R';
//          break;
//        case 4:
//          temp='D';
//          break;
//        case 5:
//          temp='H';
//          break;
//        default:
//          if(capture)
//          {
//            temp = 'I';
//          }
//          else
//          {
//            temp = 'S';
//          }
//          capture = (!capture);
//          break;
//      }
//      if(*(myComs->response)!=temp){
//        resVar=!resVar;
//        *(myComs->response)=temp;
//      }
//    }
//    vTaskDelay(1000);
//  }
//}

void VComsManage(void *vParameters)
{
  vehComs *myVehComs=(vehComs*)vParameters;
  while(1)
  {
    if(finishCapture){
      UARTCharPutNonBlocking(UART0_BASE, 'W');
      finishCapture=FALSE;
    }
//    if(resVar){
//      switch(*myVehComs->response){
//        case 'F':
//          UARTSend("A Forward", 9);
//          break;
//        case 'B':
//          UARTSend("A Back", 6);
//          break;
//        case 'L':
//          UARTSend("A Left", 6);
//          break;
//        case 'R':
//          UARTSend("A Right", 7);
//          break;
//        case 'D':
//          UARTSend("A Start", 7);
//          break;
//        case 'H':
//          UARTSend("A Stop", 6);
//        case 'S':
//          UARTSend("A Start capture", 15);
//          ADCSequenceEnable(ADC_BASE, 0);
//          break;
//        case 'I':
//          UARTSend("A Image", 7);
//          ADCSequenceDisable(ADC_BASE, 0);
//          break;
//      }
//      resVar=!resVar;
//    }
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
//        HWREG(NVIC_SW_TRIG)=INT_GPIOE-16;
      IntDisable(INT_GPIOG); 

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
//  char xyz[20];
//  sprintf(xyz,"%u",dutyCycle);
//  RIT128x96x4StringDraw(xyz, 20, 80, 15);
    SysCtlDelay(600);
    IntEnable(INT_GPIOG);//wait 600 ms before turning on measure interrupts
    vTaskDelay(500);
  }
}

void keypadManage(void * data)
{
          ///Keypad interrupts up
//      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//     
//      GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3);
//      GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3,GPIO_DIR_MODE_IN );
//      GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 
//      GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3  , GPIO_FALLING_EDGE);
//      IntEnable(INT_GPIOE);
  while (1)
  {
          // RIT128x96x4StringDraw(" KKEYPAD   ", 20, 80, 15);
    
       keypad *myKey = (keypad*) data;
       if (UpFlag){
       *myKey->driveMotorSpeedInc=TRUE; UpFlag=FALSE;
      // RIT128x96x4StringDraw("        ", 20, 80, 15);
       }
       if(DownFlag){
       *myKey->driveMotorSpeedDec=TRUE;DownFlag=FALSE;
      // RIT128x96x4StringDraw("         ", 80, 80, 15);
       }

         
//          GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_3);
        IntEnable(INT_GPIOE);


    vTaskDelay(200);
  }
}


void WarningManage(void *vParameters)
{
  static short timer =0; // minor cycle is 1/6 a second
  static short timer2= 0; // timer 1 for 1 second int timer 2 for 2 sec inv
  static short timer3= 0;
  static Bool t1 = FALSE;
  static Bool t2 = FALSE;
  static Bool t3= FALSE;
  static Bool t4 = FALSE;
  static short Btimer=0,timer4=0;
  
    while(1)
  {
      //RIT128x96x4StringDraw("warn   ", 10, 80, 15); 

  warning *mywarning= (warning*)vParameters;
//PORTC -  4 (PhA0) red, 5 (PC5) yellow, 6 (PhB0), 7(PC7)green 
  // 0xFF turn on 0x00 turn off
  //GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0xFF);
  
   if (Btimer>500)//if battery temp runs past 500 battery warning lights take precedence
    {
       GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00);// turn off green
       if(timer4==0 && t4){
            if (t3 && 0==timer3) //  if Red on turn it off at timer == 0
            { 
              GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // red
              GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF);// turn ON yellow 
              t3=!t3;
            } 
            else if ( !t3 && 0 == timer3) { // if its off turn it onn at timer 0
              GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF); // red
              GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00);// turn OFF yellow 
              t3=!t3;
            }
       } else if ( !t4 && timer4==0){
         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x00);// turn OFF yellow 
         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00); // red
       }
      
      goto exit;
    }
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
   if (5 == timer)//1sec
   { timer = 0; }
   else { timer ++;}
  
  if ( 10 == timer2)//2 sec
  {timer2=0;}
  else { timer2++;}
  if (1==timer3)//10Hz
  {
    timer3=0;
  }else {timer3++;}
 if (25==timer4)//5sec
 {
   timer4=0;
 }else{timer4++;}
//Battery
  if (*(mywarning->battOverTemp))
  {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
//    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
//    
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0);
//    
    GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_1);
    
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
//    
    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
//    
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, 156);
//    
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 78);
//        
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);
    
    PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, 1); 
    Btimer++;
//    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_2, 0xFF);//
  }
  if(BTempAck)
  {
	 PWMGenDisable(PWM0_BASE, PWM_GEN_2); Btimer=0;
         Btimer=0;
         battOverTemp=FALSE;
         BTempAck=FALSE;
  }
  else 
  {
	  Btimer++;
  }    
    vTaskDelay(100);
  }
}

void ImageCapture(void *vParameters)
{
	unsigned long ulADC_Value[1];

	image * myImage = (image*)vParameters;

	int index = 0, peakFreqIndex, period = 125000, dutyCycle = 62500;
	int m_index = 1;
        volatile double temp;
        unsigned long samplingRate;
        static int buffIndex = 0;
	volatile int i, samplFreq, maxAmp=0;
        
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
//
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//
//	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
//
//	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 0);
//
//	GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_0);
//
//	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
//
//	PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
//
//	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, period);
//
//	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, dutyCycle);
//
//	PWMGenEnable(PWM0_BASE, PWM_GEN_2);
//
//	PWMOutputState(PWM0_BASE, PWM_OUT_4_BIT, 1);

	while (1)
	{

          while (index < 256)
          {
//            SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);
            ADCProcessorTrigger(ADC_BASE, 0);
            while (!ADCIntStatus(ADC_BASE, 0, false)) {}
            ADCIntClear(ADC_BASE, 0);
            ADCSequenceDataGet(ADC_BASE, 0, ulADC_Value);
//                        samplingRate = SysCtlADCSpeedGet();
            (myImage->imageDataRawPtr)[index] = ulADC_Value[0] - 0.5;            // /(0x3FF)
            (myImage->imageDataPtr)[index] = 0;
            index++;
            
            //SysCtlDelay(430);
            
            delay(5); // delay 5, freq 1k, index 30, samplFreq 8533, incr 33
                      // delay 4, index 25, samplFreq 10240, incr 40
                      // delay 1, index 9, samplFreq 28444, incr 111
                      // delay 2, index 14, samplFreq 18286, incr 71
                      // delay 3, index 19, samplFreq 13474, incr 53

            // delay = 50, freq = 1k, index = 15, samplFreq = 17066.66, N = 256
                        // delay = 98, freq = 1k, index = 16, samplFreq = 16000
                              // delay = 100, freq = 1k, index = 26, sampl freq = 9846
                              // delay = 200, freq = 1k, index = 6, sampl freq = 42666.66
            //delay = 30, freq = 1k, index = 92,
            //delay = 30, freq = 2k, index = 72
          }
          index = 0;
          maxAmp = 0;
          peakFreqIndex = optfft(myImage->imageDataRawPtr, myImage->imageDataPtr);
//          for (i = 0; i < 129; i++)
//          {
//            temp = sqrt((((myImage->imageDataRawPtr)[i])*((myImage->imageDataRawPtr)[i]))
//                        + (((myImage->imageDataPtr)[i])*((myImage->imageDataPtr)[i])));
//            if (temp > maxAmp)
//            {
//              maxAmp = temp;
//              m_index = i;
//            }
//          }

//		samplFreq = 500000;
//          samplFreq = (myImage->imageDataRawPtr)[peakFreqIndex];
          samplFreq = 8533;     //8533
          *(myImage -> peakFreq) = samplFreq * peakFreqIndex / 256;
//          *(myImage -> peakFreq) = samplFreq * m_index / 256;
//                *(myImage->peakFreq) = samplingRate;
          if(buffIndex > 15)
          {
            buffIndex = 0;
          }
          finishCapture = TRUE;
          vTaskDelay(500);
	}
}

/*******************************************************************************/
/* optfft.c                                                                    */
/*                                                                             */
/* An optimized version of the fft function using only 16-bit integer math.    */
/*                                                                             */
/* Optimized by Brent Plump                                                    */
/* Based heavily on code by Jinhun Joung                                       */
/*                                                                             */
/* - Works only for input arrays of 256 length.                                */
/* - Requires two arrays of 16-bit ints.  The first contains the samples, the  */
/*   second contains all zeros.  The samples range from -31 to 32              */
/* - Returns the index of the peak frequency                                   */
/*******************************************************************************/
//#include "optfft.h"

#define ABS(x)  (((x)<0)?(-(x)):(x))
#define CEILING(x) (((x)>511)?511:(x))

signed int optfft(signed int real[256], signed int imag[256]) {

	signed int i, i1, j, l, l1, l2, t1, t2, u;
	/* 256 bytes */
	const signed char u1[] = {
		32,
		32,
		0,
		32,
		22,
		0,
		-22,
		32,
		29,
		22,
		12,
		0,
		-12,
		-22,
		-29,
		32,
		31,
		29,
		26,
		22,
		17,
		12,
		6,
		0,
		-6,
		-12,
		-17,
		-22,
		-26,
		-29,
		-31,
		32,
		31,
		31,
		30,
		29,
		28,
		26,
		24,
		22,
		20,
		17,
		15,
		12,
		9,
		6,
		3,
		0,
		-3,
		-6,
		-9,
		-12,
		-15,
		-17,
		-20,
		-22,
		-24,
		-26,
		-28,
		-29,
		-30,
		-31,
		-31,
		32,
		31,
		31,
		31,
		31,
		31,
		30,
		30,
		29,
		28,
		28,
		27,
		26,
		25,
		24,
		23,
		22,
		21,
		20,
		19,
		17,
		16,
		15,
		13,
		12,
		10,
		9,
		7,
		6,
		4,
		3,
		1,
		0,
		-1,
		-3,
		-4,
		-6,
		-7,
		-9,
		-10,
		-12,
		-13,
		-15,
		-16,
		-17,
		-19,
		-20,
		-21,
		-22,
		-23,
		-24,
		-25,
		-26,
		-27,
		-28,
		-28,
		-29,
		-30,
		-30,
		-31,
		-31,
		-31,
		-31,
		-31,
		32,
		31,
		31,
		31,
		31,
		31,
		31,
		31,
		31,
		31,
		31,
		30,
		30,
		30,
		30,
		29,
		29,
		29,
		28,
		28,
		28,
		27,
		27,
		27,
		26,
		26,
		25,
		25,
		24,
		24,
		23,
		23,
		22,
		22,
		21,
		20,
		20,
		19,
		19,
		18,
		17,
		17,
		16,
		15,
		15,
		14,
		13,
		12,
		12,
		11,
		10,
		10,
		9,
		8,
		7,
		7,
		6,
		5,
		4,
		3,
		3,
		2,
		1,
		0,
		0,
		0,
		-1,
		-2,
		-3,
		-3,
		-4,
		-5,
		-6,
		-7,
		-7,
		-8,
		-9,
		-10,
		-10,
		-11,
		-12,
		-12,
		-13,
		-14,
		-15,
		-15,
		-16,
		-17,
		-17,
		-18,
		-19,
		-19,
		-20,
		-20,
		-21,
		-22,
		-22,
		-23,
		-23,
		-24,
		-24,
		-25,
		-25,
		-26,
		-26,
		-27,
		-27,
		-27,
		-28,
		-28,
		-28,
		-29,
		-29,
		-29,
		-30,
		-30,
		-30,
		-30,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31
	};

	/* 256 bytes */
	const signed char u2[] = {
		0,
		0,
		-32,
		0,
		-22,
		-31,
		-22,
		0,
		-12,
		-22,
		-29,
		-31,
		-29,
		-22,
		-12,
		0,
		-6,
		-12,
		-17,
		-22,
		-26,
		-29,
		-31,
		-31,
		-31,
		-29,
		-26,
		-22,
		-17,
		-12,
		-6,
		0,
		-3,
		-6,
		-9,
		-12,
		-15,
		-17,
		-20,
		-22,
		-24,
		-26,
		-28,
		-29,
		-30,
		-31,
		-31,
		-31,
		-31,
		-31,
		-30,
		-29,
		-28,
		-26,
		-24,
		-22,
		-20,
		-17,
		-15,
		-12,
		-9,
		-6,
		-3,
		0,
		-1,
		-3,
		-4,
		-6,
		-7,
		-9,
		-10,
		-12,
		-13,
		-15,
		-16,
		-17,
		-19,
		-20,
		-21,
		-22,
		-23,
		-24,
		-25,
		-26,
		-27,
		-28,
		-28,
		-29,
		-30,
		-30,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-30,
		-30,
		-29,
		-28,
		-28,
		-27,
		-26,
		-25,
		-24,
		-23,
		-22,
		-21,
		-20,
		-19,
		-17,
		-16,
		-15,
		-13,
		-12,
		-10,
		-9,
		-7,
		-6,
		-4,
		-3,
		-1,
		0,
		0,
		-1,
		-2,
		-3,
		-3,
		-4,
		-5,
		-6,
		-7,
		-7,
		-8,
		-9,
		-10,
		-10,
		-11,
		-12,
		-12,
		-13,
		-14,
		-15,
		-15,
		-16,
		-17,
		-17,
		-18,
		-19,
		-19,
		-20,
		-20,
		-21,
		-22,
		-22,
		-23,
		-23,
		-24,
		-24,
		-25,
		-25,
		-26,
		-26,
		-27,
		-27,
		-27,
		-28,
		-28,
		-28,
		-29,
		-29,
		-29,
		-30,
		-30,
		-30,
		-30,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-31,
		-30,
		-30,
		-30,
		-30,
		-29,
		-29,
		-29,
		-28,
		-28,
		-28,
		-27,
		-27,
		-27,
		-26,
		-26,
		-25,
		-25,
		-24,
		-24,
		-23,
		-23,
		-22,
		-22,
		-21,
		-20,
		-20,
		-19,
		-19,
		-18,
		-17,
		-17,
		-16,
		-15,
		-15,
		-14,
		-13,
		-12,
		-12,
		-11,
		-10,
		-10,
		-9,
		-8,
		-7,
		-7,
		-6,
		-5,
		-4,
		-3,
		-3,
		-2,
		-1,
		0
	};

	/* 512 bytes */
	const unsigned char square[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 6,
		6, 6, 6, 6, 6, 7, 7, 7,
		7, 7, 7, 8, 8, 8, 8, 8,
		9, 9, 9, 9, 9, 9, 10, 10,
		10, 10, 10, 11, 11, 11, 11, 12,
		12, 12, 12, 12, 13, 13, 13, 13,
		14, 14, 14, 14, 15, 15, 15, 15,
		16, 16, 16, 16, 17, 17, 17, 17,
		18, 18, 18, 18, 19, 19, 19, 19,
		20, 20, 20, 21, 21, 21, 21, 22,
		22, 22, 23, 23, 23, 24, 24, 24,
		25, 25, 25, 25, 26, 26, 26, 27,
		27, 27, 28, 28, 28, 29, 29, 29,
		30, 30, 30, 31, 31, 31, 32, 32,
		33, 33, 33, 34, 34, 34, 35, 35,
		36, 36, 36, 37, 37, 37, 38, 38,
		39, 39, 39, 40, 40, 41, 41, 41,
		42, 42, 43, 43, 43, 44, 44, 45,
		45, 45, 46, 46, 47, 47, 48, 48,
		49, 49, 49, 50, 50, 51, 51, 52,
		52, 53, 53, 53, 54, 54, 55, 55,
		56, 56, 57, 57, 58, 58, 59, 59,
		60, 60, 61, 61, 62, 62, 63, 63,
		64, 64, 65, 65, 66, 66, 67, 67,
		68, 68, 69, 69, 70, 70, 71, 71,
		72, 72, 73, 73, 74, 74, 75, 76,
		76, 77, 77, 78, 78, 79, 79, 80,
		81, 81, 82, 82, 83, 83, 84, 84,
		85, 86, 86, 87, 87, 88, 89, 89,
		90, 90, 91, 92, 92, 93, 93, 94,
		95, 95, 96, 96, 97, 98, 98, 99,
		100, 100, 101, 101, 102, 103, 103, 104,
		105, 105, 106, 106, 107, 108, 108, 109,
		110, 110, 111, 112, 112, 113, 114, 114,
		115, 116, 116, 117, 118, 118, 119, 120,
		121, 121, 122, 123, 123, 124, 125, 125,
		126, 127, 127, 128, 129, 130, 130, 131,
		132, 132, 133, 134, 135, 135, 136, 137,
		138, 138, 139, 140, 141, 141, 142, 143,
		144, 144, 145, 146, 147, 147, 148, 149,
		150, 150, 151, 152, 153, 153, 154, 155,
		156, 157, 157, 158, 159, 160, 160, 161,
		162, 163, 164, 164, 165, 166, 167, 168,
		169, 169, 170, 171, 172, 173, 173, 174,
		175, 176, 177, 178, 178, 179, 180, 181,
		182, 183, 183, 184, 185, 186, 187, 188,
		189, 189, 190, 191, 192, 193, 194, 195,
		196, 196, 197, 198, 199, 200, 201, 202,
		203, 203, 204, 205, 206, 207, 208, 209,
		210, 211, 212, 212, 213, 214, 215, 216,
		217, 218, 219, 220, 221, 222, 223, 224,
		225, 225, 226, 227, 228, 229, 230, 231,
		232, 233, 234, 235, 236, 237, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247,
		248, 249, 250, 251, 252, 253, 254, 255
	};
	//#include "tables.c"

	/* Bit reversal. */
	/*Do the bit reversal */
	l2 = 128;
	i = 0;
	for (l = 0; l<255; l++) {
		if (l < i) {
			j = real[l]; real[l] = real[i]; real[i] = j;
		}
		l1 = l2;
		while (l1 <= i){
			i -= l1;
			l1 >>= 1;
		}
		i += l1;
	}

	/* Compute the FFT */
	u = 0;
	l2 = 1;
	for (l = 0; l<8; l++){
		l1 = l2;
		l2 <<= 1;
		for (j = 0; j<l1; j++){
			for (i = j; i<256; i += l2){
				i1 = i + l1;
				t1 = (u1[u] * real[i1] - u2[u] * imag[i1]) / 32;
				t2 = (u1[u] * imag[i1] + u2[u] * real[i1]) / 32;
				real[i1] = real[i] - t1;
				imag[i1] = imag[i] - t2;
				real[i] += t1;
				imag[i] += t2;
			}
			u++;
		}
	}

	/* Find the highest amplitude value */
	/* start at index 1 because 0 can hold high values */
	j = 1;
	l = 0;
	for (i = 1; i<(128); i++) {
//		l1 = square[CEILING(ABS(real[i]))] + square[CEILING(ABS(imag[i]))];
                l1 = (ABS(real[i]))*(ABS(real[i])) + (ABS(imag[i]))*(ABS(imag[i]));
		if (l1 > l) {
			j = i;
			l = l1;
		}
	}
	return (j);
}

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
    ( void ) pxTask;
    ( void ) pcTaskName;
  
    while( 1 );
}

/*-----------------------------------------------------------*/

void prvSetupHardware( void )
{
    /* 
      If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
      a workaround to allow the PLL to operate reliably. 
    */
  
    if( DEVICE_IS_REVA2 )
    {
        SysCtlLDOSet( SYSCTL_LDO_2_75V );
    }
	
    // Set the clocking to run from the PLL at 50 MHz
    
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );

    // Button interrupt hardware setup
      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);//PUSHBUTTON
      GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);
      GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_1,GPIO_DIR_MODE_IN );
      GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 
      GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_1  , GPIO_FALLING_EDGE);
      GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_1); 
      IntEnable(INT_GPIOF);
     /// start with the int enabled
      
    SysCtlPWMClockSet(SYSCTL_PWMDIV_32);
      //Warning LED
      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // enable  PORTC -  4 (PhA0), 5 (PC5), 6 (PhB0), 7(PC7)
      GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7));
      //Warning Speaker
//      GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_2);
//      SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
////    
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//    
//    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
////    
//    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0);
////    
//    GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_1);
//    
//    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
////    
//    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
////    
//    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, 50);
////    
//    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 25);
////    

      
      //Battery Connect
      //solar panel triggers interrupt before moving
//      GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
//      GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_6,GPIO_DIR_MODE_IN );
//      GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 
//      GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_6  , GPIO_FALLING_EDGE);
//      GPIOPinIntEnable(GPIO_PORTC_BASE, GPIO_PIN_6);
      IntEnable(INT_GPIOG);
       
      //Pirate
      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
      GPIOPinTypeGPIOInput(GPIO_PORTD_BASE,GPIO_PIN_2);
      GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_5);
      GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_2 ,GPIO_DIR_MODE_IN );
      GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_5 ,GPIO_DIR_MODE_OUT );
      GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2 |GPIO_PIN_5, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU); 
      //GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_HIGH_LEVEL);
     // GPIOPinIntEnable(GPIO_PORTD_BASE, GPIO_PIN_2 );
     // IntEnable(INT_GPIOD);
      
      //distance sensor
      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
      GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
      GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_6,GPIO_DIR_MODE_IN );
      GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 
      GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_6  , GPIO_RISING_EDGE);
      GPIOPinIntEnable(GPIO_PORTC_BASE, GPIO_PIN_6);
      IntEnable(INT_GPIOC);

      //Timer
      SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
      IntMasterEnable();
      TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
      TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet());
      IntEnable(INT_TIMER1A);
      TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT/100);
      TimerEnable(TIMER1_BASE, TIMER_A);
              //ADC STUFF
      SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
      ADCSequenceStepConfigure(ADC_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                               ADC_CTL_END);
      ADCSequenceEnable(ADC_BASE, 3);
      ADCIntClear(ADC_BASE, 3);
      ADCIntEnable(ADC_BASE, 0);
      
      ADCSequenceStepConfigure(ADC_BASE, 2, 0, ADC_CTL_CH1 | ADC_CTL_IE |
              ADC_CTL_END);
      ADCSequenceEnable(ADC_BASE, 2);
      ADCIntClear(ADC_BASE, 2);
      ADCIntEnable(ADC_BASE, 2);

      ADCSequenceStepConfigure(ADC_BASE, 1, 0, ADC_CTL_CH2 | ADC_CTL_IE |
              ADC_CTL_END);
      ADCSequenceEnable(ADC_BASE, 1);
      ADCIntClear(ADC_BASE, 1);
      ADCIntEnable(ADC_BASE, 1);

      ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_CH3 | ADC_CTL_IE |
              ADC_CTL_END);
//      SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);
      ADCSequenceEnable(ADC_BASE, 0);
      ADCIntClear(ADC_BASE, 0);
      ADCIntEnable(ADC_BASE, 0);

//        ///Keypad interrupts up
           SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
     
      GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3);
      GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3,GPIO_DIR_MODE_IN );
      GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 
      GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3  , GPIO_LOW_LEVEL);
        GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_3); 
      IntEnable(INT_GPIOE);
    
    //// UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    /// Set GPIO A0 and A1 as UART pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    /// Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                 UART_CONFIG_PAR_NONE));
    /// Enable the UART interrupt.
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    
}
void PirateManage(void * Pirates)
{
  Bool * Pir=(Bool*)Pirates;
   if (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_2)!=0XFF)
  {
     GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2  , GPIO_LOW_LEVEL);
      GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0xFF);//ENABLE MANAGE TASK 
      GPIOPinIntEnable(GPIO_PORTD_BASE, GPIO_PIN_2); 
      IntEnable(INT_GPIOD);
    
  }else if (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_2)==0X00){
      GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0xFF);//ENABLE MANAGE TASK
      GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2  , GPIO_HIGH_LEVEL);
      GPIOPinIntEnable(GPIO_PORTD_BASE, GPIO_PIN_2); 
      IntEnable(INT_GPIOD);
  }
}
  


void Pirate(void)
{
   GPIOPinIntClear(GPIO_PORTD_BASE, GPIO_PIN_2); 

     Pirates=!Pirates;
     IntDisable(INT_GPIOD);
    
}

/*-----------------------------------------------------------*/
void INTDistance(void)
{ 
  GPIOPinIntClear(GPIO_PORTC_BASE, GPIO_PIN_6); 
  if(!distFLAG){
    distFLAG=TRUE;
  }
}

void Distance(void* Data)
{
  vehComs *data=(vehComs*)Data;
  static int s=0,f=0;
  StopTimer=FALSE;
  StartTimer=TRUE;
  static unsigned long distbuff[]={0,0,0,0,0,0,0,0};
const int MaxPeriod=100;
  while(1)
  {
    if (distFLAG){
      if(StartTimer && !StopTimer){
        StopTimer=FALSE;
        StartTimer=FALSE;
      }else if(StopTimer && !StartTimer)
      {
        StopTimer=FALSE;
        StartTimer=FALSE;
      }   
      distFLAG=FALSE;
    }
    if(!StartTimer && s==0 && !StopTimer)
    { 
        s=globalCounter;
        StopTimer=TRUE;
        StartTimer=FALSE;
    }else if(!StartTimer && !StopTimer && s!=0)
    {
      if ((globalCounter-s)>MaxPeriod)
      {
          distbuff[f]=2000;
      }else if((globalCounter-s)==6)
      {
      distbuff[f]=0;
      }else{
          distbuff[f]=((globalCounter-s))*20;}
    //  if((distance[f]-distance[f%7])>10)//if current dist is greater than 10% of last distance
     // {
        *(data->dist)=distbuff[f];
     // }
      f=0;
     StopTimer=FALSE;
      StartTimer=TRUE;
      s=0;
      f++;
    }
    if (7==f){f=0;}
    
    vTaskDelay(50);
  }
  
}

void vApplicationTickHook( void )
{
    static xOLEDMessage xMessage = { "PASS" };
    static unsigned portLONG ulTicksSinceLastDisplay = 0;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* 
      Called from every tick interrupt.  Have enough ticks passed to make it
      time to perform our health status check again? 
    */
    
    ulTicksSinceLastDisplay++;
    if( ulTicksSinceLastDisplay >= mainCHECK_DELAY )
    {
       ulTicksSinceLastDisplay = 0;
            
    }
}


void InitData( void )
{
      // initialize each structure
    myPower.solarPanelState=&solarPanelState;
    myPower.solarPanelDeploy=&solarPanelDeploy;
    myPower.solarPanelRetract=&solarPanelRetract;
    myPower.batteryLevelPtr=batteryLevelPtr;
    myPower.fuelLevel=&fuelLevel;
    myPower.powerConsumption=&powerConsumption;
    myPower.powerGeneration=&powerGeneration;
    myPower.batteryLow=&batteryLow;
    myPower.batteryLevel=&batteryLevel;
	myPower.battTemp = &battTemp;
	myPower.secondTemp = &secondTemp;
	myPower.battOverTemp = &battOverTemp;
        myPower.battTempPtr=battTempPtr;

    myCommand.cType=&Ctype;
    myCommand.Payload=payload;
    myCommand.thrusterCommand=&thrusterCommand;
    myCommand.TransmitFlag=&TFLAG;
    myCommand.batteryLevel=&batteryLevel;
    myCommand.fuelLevel=&fuelLevel;
    myCommand.Response=Response;
     
    mySolarPanel.driveMotorSpeedDec=&driveMotorSpeedDec;
    mySolarPanel.driveMotorSpeedInc=&driveMotorSpeedInc;
    mySolarPanel.solarPanelDeploy=&solarPanelDeploy;
    mySolarPanel.solarPanelRetract=&solarPanelRetract;
    mySolarPanel.solarPanelState=&solarPanelState;
    
    myKeypad.driveMotorSpeedDec=&driveMotorSpeedDec;
    myKeypad.driveMotorSpeedInc=&driveMotorSpeedInc;
    
    myThruster.fuelLevel=&fuelLevel;
    myThruster.thrusterCommand=&thrusterCommand;
    myThruster.fuelLow=&fuelLow;

    myComs.batteryLevel=&batteryLevel;
    myComs.batteryLow=&batteryLow;
    myComs.fuelLevel=&fuelLevel;
    myComs.fuelLow=&fuelLow;
    myComs.powerConsumption=&powerConsumption;
    myComs.powerGeneration=&powerGeneration;
    myComs.solarPanelState=&solarPanelState;
    myComs.thrusterCommand=&thrusterCommand;
    myComs.response=&response;
    myComs.battTemp=&battTemp;
    myComs.secondTemp=&secondTemp;
    myComs.dist=&distance;
    myComs.peakFreq=&peakFreq;
    myComs.cType=&Ctype;
    myComs.Payload=payload;
    myComs.Response=Response;
    myComs.TransmitFlag=&TFLAG;
  
    myVehComs.command=&command;
    myVehComs.response=&response;
    myVehComs.dist=&distance;
    
    myDisplay.batteryLevel=&batteryLevel;
    myDisplay.batteryLow=&batteryLow;
    myDisplay.fuelLevel=&fuelLevel;
    myDisplay.fuelLow=&fuelLow;
    myDisplay.powerConsumption=&powerConsumption;
    myDisplay.powerGeneration=&powerGeneration;
    myDisplay.solarPanelState=&solarPanelState;
    myDisplay.VehDist=&distance;
    //myDisplay.TempKelvin=&BattTempKelvin;
	myDisplay.battTemp = &battTemp;
	myDisplay.secondTemp = &secondTemp;
	myDisplay.battOverTemp = &battOverTemp;
        myDisplay.peakFreq=&peakFreq;
    
    myWarning.batteryLevel=&batteryLevel;
    myWarning.batteryLow=&batteryLow;
    myWarning.fuelLevel=&fuelLevel;
    myWarning.fuelLow=&fuelLow;
//    myWarning.BattOverTemp=&BattOTemp;
	myWarning.battOverTemp = &battOverTemp;
    
	myImage.imageDataRawPtr = imageDataRawPtr;
	myImage.imageDataPtr = imageDataPtr;
	myImage.peakFreq = &peakFreq;
        myImage.imageDataPending=imageDataPending;
    //GPIOPortIntRegister(GPIO_PORTF_BASE, &IntGPIOa);
}

void measureINT(void)
{
  if (!Measure){
   // RIT128x96x4Clear(); 
    Measure=TRUE;}
  GPIOPinIntClear(GPIO_PORTC_BASE, GPIO_PIN_6);  
  //RIT128x96x4StringDraw("measure int    ", 10, 80, 15); 

}

void Timer1IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
//RIT128x96x4StringDraw("TIMER    ", 10, 80, 15);  

    globalCounter++;
    //
    // Update the interrupt status on the display.
    // 
    //IntMasterDisable();

    //IntMasterEnable();
}

void ThrustUpKey(void)
{ 

//      char x[20];
//      sprintf(x,"%u",GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2));
//      RIT128x96x4StringDraw(x, 40, 80, 15);
          
    if(0==GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2))
        { 
//          RIT128x96x4StringDraw(" UP     ", 20, 80, 15);
          if(!UpFlag){UpFlag=TRUE;}
        GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_2);
        
        }
        //   GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_2);   
           //delay(4000);
    if (0==GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3))
        {
//          RIT128x96x4StringDraw(" DOWN    ", 80, 80, 15);
          if(!DownFlag){ DownFlag=TRUE; }
          GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_3);
        }
          
          IntDisable(INT_GPIOE);
         
}



void IntGPIOa(void)
{
  if (g_ulFlags)
  {mode=!mode; g_ulFlags=FALSE; BTempAck=TRUE; } 
  GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);
}

void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    // Loop while there are more characters to send.
    while(ulCount--)
    {
        // Write the next character to the UART.
        UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}

void UARTIntHandler(void){

    // Clear the asserted interrupts.
    UARTIntClear(UART0_BASE, UARTIntStatus(UART0_BASE, true));

    //*(myVehComs.response)=*(myVehComs.command);
    if(UARTCharsAvail(UART0_BASE))
    {
      *(myVehComs.command)=UARTCharGetNonBlocking(UART0_BASE);
      //*(myVehComs.response)=*(myVehComs.command);
      switch(*(myVehComs.command)){
        case 'T':
          UARTCharPutNonBlocking(UART0_BASE, 'K');
          break;
        case 'D':
          UARTCharPutNonBlocking(UART0_BASE, 'C');
          break;
        default:
          *(myVehComs.response)='N';
          UARTCharPutNonBlocking(UART0_BASE, 'N');
          break;
      }
    }
}

void delay(unsigned long aValue)
{
    volatile unsigned long i = 0;

    volatile unsigned int j = 0;
    
    for (i = aValue; i > 0; i--)
    {
        for (j = 0; j < 100; j++);
    }

    return;
}