
#include <stdint.h>


#include "LCDAlarm.h"
#include "ST7735.h"


const int32_t CircleXbuf[180] = { 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70, 0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999
};
const int32_t CircleYbuf[180] = {0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999, 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70
};



/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
#define TITLEHEIGHT 0
#define SCREENHEIGHT 160
#define GRAPHICWIDTH 118
#define GRAPHICHEIGHT 124

int32_t static dimensionalData[4]; // Used to create a surface which will be mapped to the ST7735
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_FillScreen(ST7735_BLACK); //BLACK 
	dimensionalData[0] = minX;
	dimensionalData[1] = maxX - minX; // width
	dimensionalData[2] = maxY;
	dimensionalData[3] = minY - maxY; // height
}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	uint32_t counter = 0;
	while(counter < num){
		// Normalized Coordinate between 0 and 1 and multiplied by pixel width/height
		uint8_t mappedX = GRAPHICWIDTH*(bufX[counter]-dimensionalData[0])/(dimensionalData[1]);
		uint8_t mappedY = (GRAPHICHEIGHT*(bufY[counter]-dimensionalData[2])/(dimensionalData[3]))+TITLEHEIGHT;
		// 2 x 2 Pixel Thickness Plotter
		ST7735_DrawPixel(mappedX+3, mappedY+3, ST7735_WHITE);		/**/	ST7735_DrawPixel(mappedX+4, mappedY+3, ST7735_WHITE);
		ST7735_DrawPixel(mappedX+3, mappedY+2, ST7735_WHITE);	/**/	ST7735_DrawPixel(mappedX+4, mappedY+2, ST7735_WHITE);
		counter ++;
	}
	
}


void ST7735_drawline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){  //function that is used to draw a line with a slope on the LCD
	float slope = ((y1 - y2)/(x2 - x1));
	for(int x = x1; x < x2; x++){
		ST7735_DrawPixel(x, (int)(slope*(x1 - x)+ y1), ST7735_WHITE);
	}
}

void LCDAlarmInit(void){
	
	ST7735_XYplotInit("Circle",-2500, 2500, -2500, 2500);
  ST7735_XYplot(180,(int32_t *)CircleXbuf,(int32_t *)CircleYbuf);
	ST7735_SetCursor(10, 0);
	ST7735_OutUDec(12);
	ST7735_SetCursor(15, 1);
	ST7735_OutUDec(1);
	ST7735_SetCursor(18, 3);
	ST7735_OutUDec(2);
	ST7735_SetCursor(19, 6);
	ST7735_OutUDec(3);
	ST7735_SetCursor(18, 9);
	ST7735_OutUDec(4);
	ST7735_SetCursor(15, 11);
	ST7735_OutUDec(5);
	ST7735_SetCursor(10, 12);
	ST7735_OutUDec(6);
	ST7735_SetCursor(5, 11);
	ST7735_OutUDec(7);
	ST7735_SetCursor(2, 9);
	ST7735_OutUDec(8);
	ST7735_SetCursor(1, 6);
	ST7735_OutUDec(9);
	ST7735_SetCursor(1, 3);
	ST7735_OutUDec(10);
	ST7735_SetCursor(5, 1);
	ST7735_OutUDec(11);

}
