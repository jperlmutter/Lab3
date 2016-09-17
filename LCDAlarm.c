
#include <stdint.h>

#include <math.h>
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
#define GRAPHICWIDTH 120
#define GRAPHICHEIGHT 120

int32_t static dimensionalData[4]; // Used to create a surface which will be mapped to the ST7735

int8_t secondX[60];
int8_t secondY[60];

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
	counter = 0;
	
	
}


void ST7735_drawline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){  //function that is used to draw a line with a slope on the LCD
	float slope;
	float dy = (y1 - y2); float dx = (x2 - x1);
	if(dx > 0 && dy != 0){
		slope = dy/dx;
		for(int x = x1; x < x2; x++){
			ST7735_DrawPixel(x, (int)(slope*(x1 - x)+ y1), ST7735_WHITE);
		}
		for(int y = y2; y < y1; y++){
			ST7735_DrawPixel((int)((y1-y)/slope)+ x1, y, ST7735_WHITE);
		}
	}
	if(dx < 0 && dy != 0){
		slope = dy/dx;
		for(int x = x2; x < x1; x++){
			ST7735_DrawPixel(x, (int)(slope*(x2 - x)+ y2), ST7735_WHITE);
		}
		for(int y = y1; y < y2; y++){
			ST7735_DrawPixel((int)((y2-y)/slope)+ x2, y, ST7735_WHITE);
		}
	}
	if(dx == 0 && dy != 0){
		if(y1 > y2){
			for(int y = y2; y < y1; y++){
			ST7735_DrawPixel(x1, y, ST7735_WHITE);
			}
		}
		if(y2 > y1){
			for(int y = y1; y < y2; y++){
			ST7735_DrawPixel(x1, y, ST7735_WHITE);
			}
		}
	}
	if(dx != 0 && dy == 0){
		if(x1 > x2){
			for(int x = x2; x < y1; x++){
			ST7735_DrawPixel(x, y1, ST7735_WHITE);
			}
		}
		if(x2 > x1){
			for(int x = x1; x < x2; x++){
			ST7735_DrawPixel(x, y1, ST7735_WHITE);
			}
		}
	}
}

void clearLCD(void){
	ST7735_FillRect(30, 30, 65, 65, ST7735_BLACK);
	ST7735_FillRect(45, 22, 37, 80, ST7735_BLACK);
	ST7735_FillRect(22, 44, 83, 39, ST7735_BLACK);
}

void drawSecondHand(int sec)
{
	ST7735_drawline(63, 63, secondX[sec], secondY[sec]);
}
void initClockHands(void){
	
	for(int x = 0; x < 60; x++)
	{
		secondX[x] = (int)(35*sin(2*3.14159*x/60)) + 63;
		secondY[x] = (int)(-35*cos(2*3.14159*x/60)) + 63;
	}
	ST7735_drawline(63, 63, secondX[40], secondY[40]);
}

void initLCDAlarm(void){
	//Circle
	ST7735_XYplotInit("Circle",-2500, 2500, -2500, 2500);
  ST7735_XYplot(180,(int32_t *)CircleXbuf,(int32_t *)CircleYbuf);
	ST7735_XYplot(180,(int32_t *)CircleXbuf,(int32_t *)CircleYbuf);
	//Background: sacrificing readability and initialization time for implementation complexity and memory reduction (bmp cost too high)
	ST7735_FillRect(0, 0, 128, 16, ST7735_WHITE);
	ST7735_FillRect(0, 0, 16, 128, ST7735_WHITE);
	ST7735_FillRect(0, 109, 128, 20, ST7735_WHITE);
	ST7735_FillRect(110, 0, 18, 128, ST7735_WHITE);
	ST7735_FillRect(16, 16, 14, 14, ST7735_WHITE);
	ST7735_FillRect(96, 16, 14, 13, ST7735_WHITE);
	ST7735_FillRect(16, 96, 14, 14, ST7735_WHITE);
	ST7735_FillRect(96, 96, 14, 14, ST7735_WHITE);
	ST7735_FillRect(30, 16, 7, 7, ST7735_WHITE);
	ST7735_FillRect(16, 30, 8, 7, ST7735_WHITE); 
	ST7735_FillRect(37, 16, 10, 2, ST7735_WHITE);	ST7735_FillRect(15, 36, 4, 12, ST7735_WHITE);
	ST7735_FillRect(23, 29, 4, 4, ST7735_WHITE); ST7735_FillRect(18, 37, 4, 5, ST7735_WHITE);
	ST7735_FillRect(37, 17, 5, 4, ST7735_WHITE); ST7735_FillRect(30, 22, 4, 4, ST7735_WHITE);
	//part 2
	ST7735_FillRect(84, 16, 12, 4, ST7735_WHITE); ST7735_FillRect(80, 16, 4, 3, ST7735_WHITE);
	ST7735_FillRect(89, 20, 10, 4, ST7735_WHITE); ST7735_FillRect(94, 24, 2, 2, ST7735_WHITE);
	ST7735_FillRect(100, 28, 10, 6, ST7735_WHITE); ST7735_FillRect(107, 34 , 4, 11, ST7735_WHITE);
	ST7735_FillRect(103, 34, 4, 5, ST7735_WHITE);
	//part 3
	ST7735_FillRect(11, 77, 8, 20, ST7735_WHITE); ST7735_FillRect(19, 85, 4, 11, ST7735_WHITE);
	ST7735_FillRect(23, 90, 3, 6, ST7735_WHITE); ST7735_FillRect(25, 94, 2, 2, ST7735_WHITE);
	ST7735_FillRect(30, 99, 5, 10, ST7735_WHITE); ST7735_FillRect(35, 107, 11, 10, ST7735_WHITE);
	ST7735_FillRect(35, 105, 10, 2, ST7735_WHITE); ST7735_FillRect(35, 102, 5, 3, ST7735_WHITE);
	//part 4
	ST7735_FillRect(100, 91, 10, 5, ST7735_WHITE); ST7735_FillRect(107, 80, 5, 11, ST7735_WHITE);
	ST7735_FillRect(103, 86, 10, 10, ST7735_WHITE); ST7735_FillRect(81, 106, 15, 5, ST7735_WHITE);
	ST7735_FillRect(92, 99, 10, 10, ST7735_WHITE); ST7735_FillRect(87, 103, 5, 5, ST7735_WHITE);
	ST7735_FillRect(90, 101, 2, 2, ST7735_WHITE);
	
	//Lines (center: 63, 63)
	ST7735_drawline(86, 25, 90, 20);  //1
	ST7735_drawline(85, 26, 89, 21); 
	ST7735_drawline(101, 42, 105, 40); // 2
	ST7735_drawline(100, 42, 105, 41); 
	ST7735_DrawFastHLine(106,63,4, ST7735_WHITE); //3
	ST7735_drawline( 101, 85, 105, 87); // 4
	ST7735_drawline( 100, 84, 105, 88); //
	ST7735_drawline( 84, 97, 88, 105); // 5
	ST7735_drawline( 85, 98, 88, 106); // 
	ST7735_DrawFastVLine(63,105,4, ST7735_WHITE); //6
	ST7735_drawline(40, 101, 43, 97); // 7
	ST7735_drawline(39, 101, 42, 97);
	ST7735_DrawPixel(40,100, ST7735_WHITE); 
	ST7735_drawline(23, 86, 27, 84); // 8 
	ST7735_drawline(23, 85, 27, 84); 
	ST7735_DrawFastHLine(17,63,4, ST7735_WHITE); //9
	ST7735_drawline( 22, 39, 27, 41); //10
	ST7735_drawline( 22, 38, 27, 41); //
	ST7735_drawline( 39, 20, 43, 27); // 11
	ST7735_drawline( 39, 21, 42, 26); // 
	ST7735_DrawFastVLine(63,16,4, ST7735_WHITE); //12
	
	//center; used in debugging clear function
	ST7735_DrawPixel(63,63, ST7735_WHITE);
	
	//Numbers
	//12
	ST7735_DrawFastVLine(60,6,9, ST7735_BLACK);
	ST7735_DrawFastHLine(58,14,5, ST7735_BLACK);
	ST7735_DrawPixel(59,7, ST7735_BLACK);
	ST7735_DrawPixel(58,8, ST7735_BLACK);
	
	ST7735_DrawFastHLine(64,6,5, ST7735_BLACK);
	ST7735_DrawFastVLine(68,7,3, ST7735_BLACK);
	ST7735_DrawFastHLine(64,10,5, ST7735_BLACK);
	ST7735_DrawFastVLine(64,11,4, ST7735_BLACK);
	ST7735_DrawFastHLine(64,14,5, ST7735_BLACK);

	//1
	ST7735_DrawFastVLine(91,14,9, ST7735_BLACK);
	ST7735_DrawFastHLine(89,22,5, ST7735_BLACK);
	ST7735_DrawPixel(90,15, ST7735_BLACK);
	ST7735_DrawPixel(89,16, ST7735_BLACK);
	
	//2
	ST7735_DrawFastHLine(107,34,5, ST7735_BLACK);
	ST7735_DrawFastVLine(111,35,3, ST7735_BLACK);
	ST7735_DrawFastHLine(107,38,5, ST7735_BLACK);
	ST7735_DrawFastVLine(107,39,4, ST7735_BLACK);
	ST7735_DrawFastHLine(107,42,5, ST7735_BLACK);
	
	//3
	ST7735_DrawFastHLine(111,58,5, ST7735_BLACK);
	ST7735_DrawFastVLine(115,59,7, ST7735_BLACK);
	ST7735_DrawFastHLine(113,62,3, ST7735_BLACK);
	ST7735_DrawFastHLine(111,66,5, ST7735_BLACK);
	
	//4
	ST7735_DrawFastVLine(107,83,4, ST7735_BLACK);
	ST7735_DrawFastVLine(111,83,9, ST7735_BLACK);
	ST7735_DrawFastHLine(107,87,5, ST7735_BLACK);
	
	//5
	ST7735_DrawFastHLine(87,104,5, ST7735_BLACK);
	ST7735_DrawFastVLine(87,105,3, ST7735_BLACK);
	ST7735_DrawFastHLine(87,108,5, ST7735_BLACK);
	ST7735_DrawFastVLine(91,109,4, ST7735_BLACK);
	ST7735_DrawFastHLine(87,112,5, ST7735_BLACK);
	
	//6
	ST7735_DrawFastHLine(61,110,5, ST7735_BLACK);
	ST7735_DrawFastVLine(61,111,8, ST7735_BLACK);
	ST7735_DrawFastHLine(61,114,5, ST7735_BLACK);
	ST7735_DrawFastVLine(65,115,4, ST7735_BLACK);
	ST7735_DrawFastHLine(61,118,5, ST7735_BLACK);
	
	//7
	ST7735_DrawFastHLine(35,104,5, ST7735_BLACK);
	ST7735_DrawPixel(39,105, ST7735_BLACK);
	ST7735_DrawPixel(38,106, ST7735_BLACK);
	ST7735_DrawFastVLine(37,107,6, ST7735_BLACK);
	
	//8
	ST7735_DrawFastHLine(17,86,5, ST7735_BLACK);
	ST7735_DrawFastVLine(17,87,8, ST7735_BLACK);
	ST7735_DrawFastHLine(17,90,5, ST7735_BLACK);
	ST7735_DrawFastVLine(21,87,8, ST7735_BLACK);
	ST7735_DrawFastHLine(17,94,5, ST7735_BLACK);
	
	//9
	ST7735_DrawFastHLine(11,59,5, ST7735_BLACK);
	ST7735_DrawFastVLine(11,60,4, ST7735_BLACK);
	ST7735_DrawFastHLine(11,63,5, ST7735_BLACK);
	ST7735_DrawFastVLine(15,60,8, ST7735_BLACK);
	ST7735_DrawFastHLine(11,67,5, ST7735_BLACK);
	
	//10
	ST7735_DrawFastVLine(13,31,9, ST7735_BLACK);
	ST7735_DrawFastHLine(11,39,5, ST7735_BLACK);
	ST7735_DrawPixel(12,32, ST7735_BLACK);
	ST7735_DrawPixel(11,33, ST7735_BLACK);
	
	ST7735_DrawFastHLine(17,31,5, ST7735_BLACK);
	ST7735_DrawFastVLine(17,32,8, ST7735_BLACK);
	ST7735_DrawFastVLine(21,32,8, ST7735_BLACK);
	ST7735_DrawFastHLine(17,39,5, ST7735_BLACK);
	
	//11
	ST7735_DrawFastVLine(41,10,9, ST7735_BLACK);
	ST7735_DrawFastHLine(39,18,5, ST7735_BLACK);
	ST7735_DrawPixel(40,11, ST7735_BLACK);
	ST7735_DrawPixel(39,12, ST7735_BLACK);
	
	ST7735_DrawFastVLine(35,10,9, ST7735_BLACK);
	ST7735_DrawFastHLine(33,18,5, ST7735_BLACK);
	ST7735_DrawPixel(34,11, ST7735_BLACK);
	ST7735_DrawPixel(33,12, ST7735_BLACK);
	
	initClockHands();
}
