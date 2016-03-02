//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define COMMAX 65535
#define NULL 0
#include "C:\StellarisWare\inc\hw_types.h"
#include "C:\StellarisWare\driverlib\debug.h"
#include "C:\StellarisWare\driverlib\sysctl.h"
#include "C:\StellarisWare\boards\ek-lm3s8962\drivers\rit128x96x4.h"

#include "C:\StellarisWare\inc\hw_ints.h"
#include "C:\StellarisWare\inc\hw_memmap.h"
#include "C:\StellarisWare\inc\hw_nvic.h"
#include "C:\StellarisWare\driverlib\gpio.h"
#include "C:\StellarisWare\driverlib\interrupt.h"
#include "C:\StellarisWare\driverlib\systick.h"
#include "C:\StellarisWare\driverlib\timer.h"
#include "C:\StellarisWare\driverlib\pwm.h"
#include "C:\StellarisWare\driverlib\adc.h"
#include "C:\StellarisWare\driverlib\uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
extern volatile unsigned long g_ulGPIOb;


typedef enum{FALSE, TRUE} Bool;

extern int globalCounter;
extern Bool mode, addSolarPanel,Measure, startCapture, finishCapture;

typedef struct{
	int *real;
	int *complex;
} complex;

typedef struct mystructTCB{
  void (*myTask)(void*);
  void *taskDataPtr;
  struct mystructTCB *next;
  struct mystructTCB *prev;
} TCB;

typedef struct{
  Bool *solarPanelState;
  Bool *solarPanelDeploy;
  Bool *solarPanelRetract;
  Bool *batteryLow;
  unsigned short *batteryLevel;
  unsigned int *batteryLevelPtr;
  unsigned short *powerConsumption;
  unsigned short *powerGeneration;
  unsigned short *fuelLevel;
  unsigned short *battTemp;
  unsigned short *secondTemp;
  Bool *battOverTemp;
  unsigned int *battTempPtr;
} power;

typedef struct{
  Bool *solarPanelState;
  Bool *solarPanelDeploy;
  Bool *solarPanelRetract;
  Bool *driveMotorSpeedInc;
  Bool *driveMotorSpeedDec;
} solarPanel;

typedef struct{
  Bool *driveMotorSpeedInc;
  Bool *driveMotorSpeedDec;
} keypad;

typedef struct{
  unsigned int *thrusterCommand;
  unsigned short *fuelLevel;
  Bool *fuelLow;
} thruster;

typedef struct{
  Bool *fuelLow;
  Bool *batteryLow;
  Bool *solarPanelState;
  Bool *TransmitFlag;
  unsigned short *batteryLevel;
  unsigned short *fuelLevel;
  unsigned short *powerConsumption;
  unsigned short *powerGeneration;
  unsigned int *thrusterCommand;
  unsigned short *battTemp;
  unsigned short *secondTemp;
  char *response;
  volatile unsigned long *dist;
  unsigned short *peakFreq;
  char *cType;
  char *Payload;
  char *Response;
} coms;

typedef struct{
  char *command;
  char *response;
  volatile unsigned long *dist;
} vehComs;

typedef struct{
  Bool *fuelLow;
  Bool *batteryLow;
  Bool *solarPanelState;
  unsigned short *batteryLevel;
  unsigned short *fuelLevel;
  unsigned short *powerConsumption;
  unsigned short *powerGeneration;
  volatile unsigned long *VehDist;
  unsigned short *TempKelvin;

  unsigned short *battTemp;
  unsigned short *secondTemp;
  Bool *battOverTemp;
  unsigned short *peakFreq;
} display;

typedef struct{
  Bool *fuelLow;
  Bool *batteryLow;
  unsigned short *batteryLevel;
  unsigned short *fuelLevel;
  Bool *battOverTemp;
} warning;

typedef struct{
	signed int *imageDataRawPtr;
	signed int *imageDataPtr;
	unsigned short *peakFreq;
        signed int *imageDataPending;
} image;

typedef struct{
  char *cType;
  char *Payload;
  char *Response;
  unsigned int *thrusterCommand;
  Bool *TransmitFlag;
  unsigned short *batteryLevel;
  unsigned short *fuelLevel;
  
}Command;

extern xTaskHandle commHandle;
extern coms myComs;
extern char *chav, *pcText;

void delay(unsigned long aValue);
void schedule(TCB** head, TCB** tail, TCB* solarPanelSys, TCB* keypadSys);
void thrusterManage(void* myThruster);
void powerManage(void* myPower);
void displayManage(void* myDisplay);
void comsManage(void* myComs);
void warningManage(void* myWarning);
void vehComsManage(void* myVehComsManage);
void keypadManage(void* myKeypad);
void solarPanelManage(void* mySolarPanel);
int randomInteger(int low, int high);
// interrupt routine for push button
void IntGPIOa(void);
void ThrustUpKey(void);
// TIMER
void Timer1IntHandler(void);
void UARTIntHandler(void);
void solarpanelINT(void);
void measureINT(void);
void Distance(void* Data);
void CommandManage(void*mydata);
extern void ENET(void);

