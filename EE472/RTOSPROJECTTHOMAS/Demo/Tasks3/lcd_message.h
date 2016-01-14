#ifndef LCD_MESSAGE_H
#define LCD_MESSAGE_H

typedef struct
{
	signed char *pcMessage;
        unsigned short *pIMessage;
        unsigned int *X_Locs;
        unsigned int *Y_Locs;
        unsigned int *X_LocI;
        unsigned int *Y_LocI;
        unsigned int msg;
        
} xOLEDMessage;

#endif /* LCD_MESSAGE_H */
