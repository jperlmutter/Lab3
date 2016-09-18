
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
int8_t minuteX[60];
int8_t minuteY[60];
int8_t hourX[60];
int8_t hourY[60];

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


void ST7735_drawline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color){  //function that is used to draw a line with a slope on the LCD
	float slope;
	float dy = (y1 - y2); float dx = (x2 - x1);
	if(dx > 0 && dy != 0){
		slope = dy/dx;
		for(int x = x1; x < x2; x++){
			ST7735_DrawPixel(x, (int)(slope*(x1 - x)+ y1), color);
		}
		if(y1 > y2){//dy>0
			for(int y = y2; y < y1; y++){
				ST7735_DrawPixel((int)((y1-y)/slope)+ x1, y, color);
			}
		}
		if(y2 > y1){
			for(int y = y1; y < y2; y++){
				ST7735_DrawPixel((int)((y2-y)/slope)+ x2, y, color);
			}
		}
	}
	if(dx < 0 && dy != 0){
		slope = dy/dx;
		for(int x = x2; x < x1; x++){
			ST7735_DrawPixel(x, (int)(slope*(x2 - x)+ y2), color);
		}
		if(y1 > y2){//dy>0
			for(int y = y2; y < y1; y++){
				ST7735_DrawPixel((int)((y1-y)/slope)+ x1, y, color);
			}
		}
		if(y2 > y1){
			for(int y = y1; y < y2; y++){
				ST7735_DrawPixel((int)((y2-y)/slope)+ x2, y, color);
			}
		}
	}
	if(dx == 0 && dy != 0){
		if(y1 > y2){
			for(int y = y2; y < y1; y++){
			ST7735_DrawPixel(x1, y, color);
			}
		}
		if(y2 > y1){
			for(int y = y1; y < y2; y++){
			ST7735_DrawPixel(x1, y, color);
			}
		}
	}
	if(dx != 0 && dy == 0){
		if(x1 > x2){
			for(int x = x2; x < y1; x++){
			ST7735_DrawPixel(x, y1, color);
			}
		}
		if(x2 > x1){
			for(int x = x1; x < x2; x++){
			ST7735_DrawPixel(x, y1, color);
			}
		}
	}
}

void clearLCD(void){
	ST7735_FillRect(30, 30, 65, 65, ST7735_BLACK);
	ST7735_FillRect(45, 22, 37, 80, ST7735_BLACK);
	ST7735_FillRect(22, 44, 83, 39, ST7735_BLACK);
}

void drawDigital(int sec, int min, int hour)
{
	
	//min01
	int digit1 = sec%10;
	
	if(digit1 == 1 || digit1 == 2 || digit1 == 3 || digit1 == 4 || digit1 == 7 || digit1 == 8|| digit1 == 9 || digit1 == 0){
		ST7735_DrawFastVLine(108,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(107,134,9, ST7735_WHITE); //top right
		ST7735_DrawFastVLine(106,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(108,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(107,134,9, ST7735_BLACK); //top right
		ST7735_DrawFastVLine(106,135,7, ST7735_BLACK);
	}
	if(digit1 == 1 || digit1 == 3 || digit1 == 4 || digit1 == 5 || digit1 == 6 || digit1 == 7 || digit1 == 8 || digit1 == 9 || digit1 == 0 ){
		ST7735_DrawFastVLine(108,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(107,146,9, ST7735_WHITE);// bottom right
		ST7735_DrawFastVLine(106,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(108,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(107,146,9, ST7735_BLACK);// bottom right
		ST7735_DrawFastVLine(106,147,7, ST7735_BLACK);
	}
	if(digit1 == 2 || digit1 == 3 || digit1 == 4 || digit1 == 5 || digit1 == 6 || digit1 == 8 || digit1 == 9){
		ST7735_DrawFastHLine(97,143,9, ST7735_WHITE);
		ST7735_DrawFastHLine(96,144,11, ST7735_WHITE); //middle
		ST7735_DrawFastHLine(97,145,9, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(97,143,9, ST7735_BLACK);
		ST7735_DrawFastHLine(96,144,11, ST7735_BLACK); //middle
		ST7735_DrawFastHLine(97,145,9, ST7735_BLACK);
	}
	if(digit1 == 2 || digit1 == 3 || digit1 == 5 || digit1 == 6 || digit1 == 7|| digit1 == 8 || digit1 == 9 || digit1 == 0){
		ST7735_DrawFastHLine(96,132,11, ST7735_WHITE);
		ST7735_DrawFastHLine(97,133,9, ST7735_WHITE); //top
		ST7735_DrawFastHLine(98,134,7, ST7735_WHITE);	
	}
	else{
		ST7735_DrawFastHLine(96,132,11, ST7735_BLACK);
		ST7735_DrawFastHLine(97,133,9, ST7735_BLACK); //top
		ST7735_DrawFastHLine(98,134,7, ST7735_BLACK);
	}
	if(digit1 == 2 || digit1 == 3 || digit1 == 5 || digit1 == 6 || digit1 == 8 || digit1 == 9 || digit1 == 0){
		ST7735_DrawFastHLine(96,156,11, ST7735_WHITE);
		ST7735_DrawFastHLine(97,155,9, ST7735_WHITE); //bottom
		ST7735_DrawFastHLine(98,154,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(96,156,11, ST7735_BLACK);
		ST7735_DrawFastHLine(97,155,9, ST7735_BLACK); //bottom
		ST7735_DrawFastHLine(98,154,7, ST7735_BLACK);

	}
	if(digit1 == 4 || digit1 == 5 || digit1 == 6 || digit1 == 8 || digit1 == 9 || digit1 == 0){
		ST7735_DrawFastVLine(94,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(95,134,9, ST7735_WHITE); //top left
		ST7735_DrawFastVLine(96,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(94,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(95,134,9, ST7735_BLACK); //top left
		ST7735_DrawFastVLine(96,135,7, ST7735_BLACK);
	}
	if(digit1 == 2 || digit1 == 6 || digit1 == 8 || digit1 == 0){
		ST7735_DrawFastVLine(94,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(95,146,9, ST7735_WHITE); //bottom left
		ST7735_DrawFastVLine(96,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(94,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(95,146,9, ST7735_BLACK); //bottom left
		ST7735_DrawFastVLine(96,147,7, ST7735_BLACK);
	}		
	
	
	int digit2 = sec/10;
	if(digit2 == 1 || digit2 == 2 || digit2 == 3 || digit2 == 4 || digit2 == 0){
		ST7735_DrawFastVLine(91,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(90,134,9, ST7735_WHITE); //top right
		ST7735_DrawFastVLine(89,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(91,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(90,134,9, ST7735_BLACK); //top right
		ST7735_DrawFastVLine(89,135,7, ST7735_BLACK);
	}
	if(digit2 == 1 || digit2 == 3 || digit2 == 4 || digit2 == 5 || digit2 == 0 ){
		ST7735_DrawFastVLine(91,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(90,146,9, ST7735_WHITE);// bottom right
		ST7735_DrawFastVLine(89,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(91,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(90,146,9, ST7735_BLACK);// bottom right
		ST7735_DrawFastVLine(89,147,7, ST7735_BLACK);
	}
	if(digit2 == 2 || digit2 == 3 || digit2 == 4 || digit2 == 5){
		ST7735_DrawFastHLine(80,143,9, ST7735_WHITE);
		ST7735_DrawFastHLine(79,144,11, ST7735_WHITE); //middle
		ST7735_DrawFastHLine(80,145,9, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(80,143,9, ST7735_BLACK);
		ST7735_DrawFastHLine(79,144,11, ST7735_BLACK); //middle
		ST7735_DrawFastHLine(80,145,9, ST7735_BLACK);
	}
	if(digit2 == 2 || digit2 == 3 || digit2 == 5 || digit2 == 0){
		ST7735_DrawFastHLine(79,132,11, ST7735_WHITE);
		ST7735_DrawFastHLine(80,133,9, ST7735_WHITE); //top
		ST7735_DrawFastHLine(81,134,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(79,132,11, ST7735_BLACK);
		ST7735_DrawFastHLine(80,133,9, ST7735_BLACK); //top
		ST7735_DrawFastHLine(81,134,7, ST7735_BLACK);
	}
	if(digit2 == 2 || digit2 == 3 || digit2 == 5 || digit2 == 0){
		ST7735_DrawFastHLine(79,156,11, ST7735_WHITE);
		ST7735_DrawFastHLine(80,155,9, ST7735_WHITE); //bottom
		ST7735_DrawFastHLine(81,154,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(79,156,11, ST7735_BLACK);
		ST7735_DrawFastHLine(80,155,9, ST7735_BLACK); //bottom
		ST7735_DrawFastHLine(81,154,7, ST7735_BLACK);

	}
	if(digit2 == 4 || digit2 == 5 || digit2 == 0){
		ST7735_DrawFastVLine(77,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(78,134,9, ST7735_WHITE); //top left
		ST7735_DrawFastVLine(79,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(77,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(78,134,9, ST7735_BLACK); //top left
		ST7735_DrawFastVLine(79,135,7, ST7735_BLACK);
	}
	if(digit2 == 2 || digit2 == 0){
		ST7735_DrawFastVLine(77,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(78,146,9, ST7735_WHITE); //bottom left
		ST7735_DrawFastVLine(79,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(77,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(78,146,9, ST7735_BLACK); //bottom left
		ST7735_DrawFastVLine(79,147,7, ST7735_BLACK);
	}
	
	
	
	ST7735_FillRect(73, 138, 3, 3, ST7735_WHITE); //colon
	ST7735_FillRect(73, 148, 3, 3, ST7735_WHITE); //colon
	
	
	int digit3 = min%10;
	
	//hour 0x01
	if(digit3 == 1 || digit3 == 2 || digit3 == 3 || digit3 == 4 || digit3 == 7 || digit3 == 8|| digit3 == 9 || digit3 == 0){
		ST7735_DrawFastVLine(71,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(70,134,9, ST7735_WHITE); //top right
		ST7735_DrawFastVLine(69,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(71,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(70,134,9, ST7735_BLACK); //top right
		ST7735_DrawFastVLine(69,135,7, ST7735_BLACK);
	}
	if(digit3 == 1 || digit3 == 3 || digit3 == 4 || digit3 == 5 || digit3 == 6 || digit3 == 7 || digit3 == 8 || digit3 == 9 || digit3 == 0 ){
			ST7735_DrawFastVLine(71,145,11, ST7735_WHITE);
			ST7735_DrawFastVLine(70,146,9, ST7735_WHITE);// bottom right
			ST7735_DrawFastVLine(69,147,7, ST7735_WHITE);
	}
	else{
			ST7735_DrawFastVLine(71,145,11, ST7735_BLACK);
			ST7735_DrawFastVLine(70,146,9, ST7735_BLACK);// bottom right
			ST7735_DrawFastVLine(69,147,7, ST7735_BLACK);
	}
	if(digit3 == 2 || digit3 == 3 || digit3 == 4 || digit3 == 5 || digit3 == 6 || digit3 == 8 || digit3 == 9){
		ST7735_DrawFastHLine(60,143,9, ST7735_WHITE);
		ST7735_DrawFastHLine(59,144,11, ST7735_WHITE); //middle
		ST7735_DrawFastHLine(60,145,9, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(60,143,9, ST7735_BLACK);
		ST7735_DrawFastHLine(59,144,11, ST7735_BLACK); //middle
		ST7735_DrawFastHLine(60,145,9, ST7735_BLACK);
	}
	if(digit3 == 2 || digit3 == 3 || digit3 == 5 || digit3 == 6 || digit3 == 7|| digit3 == 8 || digit3 == 9 || digit3 == 0){
		ST7735_DrawFastHLine(59,132,11, ST7735_WHITE);
		ST7735_DrawFastHLine(60,133,9, ST7735_WHITE); //top
		ST7735_DrawFastHLine(61,134,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(59,132,11, ST7735_BLACK);
		ST7735_DrawFastHLine(60,133,9, ST7735_BLACK); //top
		ST7735_DrawFastHLine(61,134,7, ST7735_BLACK);
	}
	if(digit3 == 2 || digit3 == 3 || digit3 == 5 || digit3 == 6 || digit3 == 8 || digit3 == 9 || digit3 == 0){
		ST7735_DrawFastHLine(59,156,11, ST7735_WHITE);
		ST7735_DrawFastHLine(60,155,9, ST7735_WHITE); //bottom
		ST7735_DrawFastHLine(61,154,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(59,156,11, ST7735_BLACK);
		ST7735_DrawFastHLine(60,155,9, ST7735_BLACK); //bottom
		ST7735_DrawFastHLine(61,154,7, ST7735_BLACK);
	}
	if(digit3 == 4 || digit3 == 5 || digit3 == 6 || digit3 == 8 || digit3 == 9 || digit3 == 0){
		ST7735_DrawFastVLine(57,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(58,134,9, ST7735_WHITE); //top left
		ST7735_DrawFastVLine(59,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(57,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(58,134,9, ST7735_BLACK); //top left
		ST7735_DrawFastVLine(59,135,7, ST7735_BLACK);
	}
	if(digit3 == 2 || digit3 == 6 || digit3 == 8 || digit3 == 0){
		ST7735_DrawFastVLine(57,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(58,146,9, ST7735_WHITE); //bottom left
		ST7735_DrawFastVLine(59,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(57,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(58,146,9, ST7735_BLACK); //bottom left
		ST7735_DrawFastVLine(59,147,7, ST7735_BLACK);
	}		
	
	
	int digit4 = min/10;
	
	
	if(digit4 == 1 || digit4 == 2 || digit4 == 3 || digit4 == 4 || digit4 == 0){
		ST7735_DrawFastVLine(54,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(53,134,9, ST7735_WHITE); //top right
		ST7735_DrawFastVLine(52,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(54,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(53,134,9, ST7735_BLACK); //top right
		ST7735_DrawFastVLine(52,135,7, ST7735_BLACK);
	}
	if(digit4 == 1 || digit4 == 3 || digit4 == 4 || digit4 == 5 || digit4 == 0 ){
		ST7735_DrawFastVLine(54,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(53,146,9, ST7735_WHITE);// bottom right
		ST7735_DrawFastVLine(52,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(54,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(53,146,9, ST7735_BLACK);// bottom right
		ST7735_DrawFastVLine(52,147,7, ST7735_BLACK);
	}
	if(digit4 == 2 || digit4 == 3 || digit4 == 4 || digit4 == 5){
		ST7735_DrawFastHLine(43,143,9, ST7735_WHITE);
		ST7735_DrawFastHLine(42,144,11, ST7735_WHITE); //middle
		ST7735_DrawFastHLine(43,145,9, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(43,143,9, ST7735_BLACK);
		ST7735_DrawFastHLine(42,144,11, ST7735_BLACK); //middle
		ST7735_DrawFastHLine(43,145,9, ST7735_BLACK);
	}
	if(digit4 == 2 || digit4 == 3 || digit4 == 5 || digit4 == 0){
		ST7735_DrawFastHLine(42,132,11, ST7735_WHITE);
		ST7735_DrawFastHLine(43,133,9, ST7735_WHITE); //top
		ST7735_DrawFastHLine(44,134,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(42,132,11, ST7735_BLACK);
		ST7735_DrawFastHLine(43,133,9, ST7735_BLACK); //top
		ST7735_DrawFastHLine(44,134,7, ST7735_BLACK);
	}
	if(digit4 == 2 || digit4 == 3 || digit4 == 5 || digit4 == 0){
		ST7735_DrawFastHLine(42,156,11, ST7735_WHITE);
		ST7735_DrawFastHLine(43,155,9, ST7735_WHITE); //bottom
		ST7735_DrawFastHLine(44,154,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(42,156,11, ST7735_BLACK);
		ST7735_DrawFastHLine(43,155,9, ST7735_BLACK); //bottom
		ST7735_DrawFastHLine(44,154,7, ST7735_BLACK);
	}
	if(digit4 == 4 || digit4 == 5 || digit4 == 0){
		ST7735_DrawFastVLine(40,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(41,134,9, ST7735_WHITE); //top left
		ST7735_DrawFastVLine(42,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(40,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(41,134,9, ST7735_BLACK); //top left
		ST7735_DrawFastVLine(42,135,7, ST7735_BLACK);
	}
	if(digit4 == 2 || digit4 == 0){
		ST7735_DrawFastVLine(40,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(41,146,9, ST7735_WHITE); //bottom left
		ST7735_DrawFastVLine(42,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(40,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(41,146,9, ST7735_BLACK); //bottom left
		ST7735_DrawFastVLine(42,147,7, ST7735_BLACK);
	}
	ST7735_FillRect(36, 138, 3, 3, ST7735_WHITE); //colon
	ST7735_FillRect(36, 148, 3, 3, ST7735_WHITE); //colon
	
	int digit5 = (hour/5)%10;
	if(hour/5 == 0) {digit5 = 2;}
	//hour 0x01
	if(digit5 == 1 || digit5 == 2 || digit5 == 3 || digit5 == 4 || digit5 == 7 || digit5 == 8|| digit5 == 9 || digit5 == 0){
		ST7735_DrawFastVLine(34,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(33,134,9, ST7735_WHITE); //top right
		ST7735_DrawFastVLine(32,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(34,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(33,134,9, ST7735_BLACK); //top right
		ST7735_DrawFastVLine(32,135,7, ST7735_BLACK);
	}
	if(digit5 == 1 || digit5 == 3 || digit5 == 4 || digit5 == 5 || digit5 == 6 || digit5 == 7 || digit5 == 8 || digit5 == 9 || digit5 == 0 ){
			ST7735_DrawFastVLine(34,145,11, ST7735_WHITE);
			ST7735_DrawFastVLine(33,146,9, ST7735_WHITE);// bottom right
			ST7735_DrawFastVLine(32,147,7, ST7735_WHITE);
	}
	else{
			ST7735_DrawFastVLine(34,145,11, ST7735_BLACK);
			ST7735_DrawFastVLine(33,146,9, ST7735_BLACK);// bottom right
			ST7735_DrawFastVLine(32,147,7, ST7735_BLACK);
	}
	if(digit5 == 2 || digit5 == 3 || digit5 == 4 || digit5 == 5 || digit5 == 6 || digit5 == 8 || digit5 == 9){
		ST7735_DrawFastHLine(23,143,9, ST7735_WHITE);
		ST7735_DrawFastHLine(22,144,11, ST7735_WHITE); //middle
		ST7735_DrawFastHLine(23,145,9, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(23,143,9, ST7735_BLACK);
		ST7735_DrawFastHLine(22,144,11, ST7735_BLACK); //middle
		ST7735_DrawFastHLine(23,145,9, ST7735_BLACK);
	}
	if(digit5 == 2 || digit5 == 3 || digit5 == 5 || digit5 == 6 || digit5 == 7|| digit5 == 8 || digit5 == 9 || digit5 == 0){
		ST7735_DrawFastHLine(22,132,11, ST7735_WHITE);
		ST7735_DrawFastHLine(23,133,9, ST7735_WHITE); //top
		ST7735_DrawFastHLine(24,134,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(22,132,11, ST7735_BLACK);
		ST7735_DrawFastHLine(23,133,9, ST7735_BLACK); //top
		ST7735_DrawFastHLine(24,134,7, ST7735_BLACK);
	}
	if(digit5 == 2 || digit5 == 3 || digit5 == 5 || digit5 == 6 || digit5 == 8 || digit5 == 9 || digit5 == 0){
		ST7735_DrawFastHLine(22,156,11, ST7735_WHITE);
		ST7735_DrawFastHLine(23,155,9, ST7735_WHITE); //bottom
		ST7735_DrawFastHLine(24,154,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(22,156,11, ST7735_BLACK);
		ST7735_DrawFastHLine(23,155,9, ST7735_BLACK); //bottom
		ST7735_DrawFastHLine(24,154,7, ST7735_BLACK);
	}
	if(digit5 == 4 || digit5 == 5 || digit5 == 6 || digit5 == 8 || digit5 == 9 || digit5 == 0){
		ST7735_DrawFastVLine(20,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(21,134,9, ST7735_WHITE); //top left
		ST7735_DrawFastVLine(22,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(20,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(21,134,9, ST7735_BLACK); //top left
		ST7735_DrawFastVLine(22,135,7, ST7735_BLACK);
	}
	if(digit5 == 2 || digit5 == 6 || digit5 == 8 || digit5 == 0){
		ST7735_DrawFastVLine(20,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(21,146,9, ST7735_WHITE); //bottom left
		ST7735_DrawFastVLine(22,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(20,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(21,146,9, ST7735_BLACK); //bottom left
		ST7735_DrawFastVLine(22,147,7, ST7735_BLACK);
	}		
	
	
	int digit6 = hour/50;
	
	if(hour/5 == 0){digit6 = 1;}
	
	if(digit6 == 1 || digit6 == 0){
		ST7735_DrawFastVLine(17,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(16,134,9, ST7735_WHITE); //top right
		ST7735_DrawFastVLine(15,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(17,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(16,134,9, ST7735_BLACK); //top right
		ST7735_DrawFastVLine(15,135,7, ST7735_BLACK);
	}
	if(digit6 == 1 || digit6 == 0 ){
		ST7735_DrawFastVLine(17,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(16,146,9, ST7735_WHITE);// bottom right
		ST7735_DrawFastVLine(15,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(17,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(16,146,9, ST7735_BLACK);// bottom right
		ST7735_DrawFastVLine(15,147,7, ST7735_BLACK);
	}

	if(digit6 == 0){
		ST7735_DrawFastHLine(5,132,11, ST7735_WHITE);
		ST7735_DrawFastHLine(6,133,9, ST7735_WHITE); //top
		ST7735_DrawFastHLine(7,134,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(5,132,11, ST7735_BLACK);
		ST7735_DrawFastHLine(6,133,9, ST7735_BLACK); //top
		ST7735_DrawFastHLine(7,134,7, ST7735_BLACK);
	}
	if(digit6 == 0){
		ST7735_DrawFastHLine(5,156,11, ST7735_WHITE);
		ST7735_DrawFastHLine(6,155,9, ST7735_WHITE); //bottom
		ST7735_DrawFastHLine(7,154,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastHLine(5,156,11, ST7735_BLACK);
		ST7735_DrawFastHLine(6,155,9, ST7735_BLACK); //bottom
		ST7735_DrawFastHLine(7,154,7, ST7735_BLACK);
	}
	if(digit6 == 0){
		ST7735_DrawFastVLine(3,133,11, ST7735_WHITE);
		ST7735_DrawFastVLine(4,134,9, ST7735_WHITE); //top left
		ST7735_DrawFastVLine(5,135,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(3,133,11, ST7735_BLACK);
		ST7735_DrawFastVLine(4,134,9, ST7735_BLACK); //top left
		ST7735_DrawFastVLine(5,135,7, ST7735_BLACK);
	}
	if(digit6 == 0){
		ST7735_DrawFastVLine(3,145,11, ST7735_WHITE);
		ST7735_DrawFastVLine(4,146,9, ST7735_WHITE); //bottom left
		ST7735_DrawFastVLine(5,147,7, ST7735_WHITE);
	}
	else{
		ST7735_DrawFastVLine(3,145,11, ST7735_BLACK);
		ST7735_DrawFastVLine(4,146,9, ST7735_BLACK); //bottom left
		ST7735_DrawFastVLine(5,147,7, ST7735_BLACK);
	}
}


void drawHands(int sec, int min, int hour)
{
	ST7735_drawline(63, 63, hourX[hour], hourY[hour], ST7735_WHITE);
	
	ST7735_drawline(62, 63, hourX[hour]-1, hourY[hour], ST7735_WHITE);
	ST7735_drawline(64, 63, hourX[hour]+1, hourY[hour], ST7735_WHITE);
	ST7735_drawline(63, 62, hourX[hour], hourY[hour]-1, ST7735_WHITE);
	ST7735_drawline(63, 64, hourX[hour], hourY[hour]+1, ST7735_WHITE);
	
	ST7735_drawline(63, 63, secondX[sec], secondY[sec], ST7735_RED);
	ST7735_drawline(63, 63, minuteX[min], minuteY[min], ST7735_WHITE);
	
}

void initClockHands(void){
	
	for(int x = 0; x < 60; x++)
	{
		secondX[x] = (int)(35*sin(2*3.14159*x/60)) + 63;
		secondY[x] = (int)(-35*cos(2*3.14159*x/60)) + 63;
		minuteX[x] = (int)(34*sin(2*3.14159*x/60)) + 63;
		minuteY[x] = (int)(-34*cos(2*3.14159*x/60)) + 63;
		hourX[x] = (int)(22*sin(2*3.14159*x/60)) + 63;
		hourY[x] = (int)(-22*cos(2*3.14159*x/60)) + 63;
	}
	
	
}
uint8_t amOrpm = 1;
uint16_t grayColor;

void AMorPM(void){
	grayColor = ST7735_Color565(169, 169, 169);
	amOrpm = (amOrpm+1)%2;
	
	if(!amOrpm)
	{
		//AM
	ST7735_DrawFastVLine(112,135,8, ST7735_YELLOW);
	ST7735_DrawFastVLine(113,133,4, ST7735_YELLOW);
	ST7735_DrawFastVLine(114,132,2, ST7735_YELLOW);
	ST7735_DrawFastVLine(115,132,2, ST7735_YELLOW);
	ST7735_DrawFastVLine(116,133,4, ST7735_YELLOW);
	ST7735_DrawFastVLine(117,135,8, ST7735_YELLOW);
	ST7735_DrawFastHLine(113,137,4, ST7735_YELLOW);

	ST7735_DrawFastVLine(119,132,11, ST7735_YELLOW);
	ST7735_DrawFastVLine(120,132,5, ST7735_YELLOW);
	ST7735_DrawFastVLine(121,136,5, ST7735_YELLOW);
	ST7735_DrawFastVLine(122,139,4, ST7735_YELLOW);
	ST7735_DrawFastVLine(125,132,11, ST7735_YELLOW);
	ST7735_DrawFastVLine(124,132,5, ST7735_YELLOW);
	ST7735_DrawFastVLine(123,136,5, ST7735_YELLOW);
	
	//PM
	ST7735_DrawFastVLine(112,146,11, grayColor);
	ST7735_DrawFastHLine(113,146,4, grayColor);
	ST7735_DrawFastVLine(116,146,2, grayColor);
	ST7735_DrawFastVLine(117,147,4, grayColor);
	ST7735_DrawFastVLine(116,150,2, grayColor);
	ST7735_DrawFastHLine(113,151,4, grayColor);
	
	ST7735_DrawFastVLine(119,146,11, grayColor);
	ST7735_DrawFastVLine(120,146,5, grayColor);
	ST7735_DrawFastVLine(121,150,5, grayColor);
	ST7735_DrawFastVLine(122,153,4, grayColor);
	ST7735_DrawFastVLine(125,146,11, grayColor);
	ST7735_DrawFastVLine(124,146,5, grayColor);
	ST7735_DrawFastVLine(123,150,5, grayColor);
	}
	else
	{
		//AM
	ST7735_DrawFastVLine(112,135,8, grayColor);
	ST7735_DrawFastVLine(113,133,4, grayColor);
	ST7735_DrawFastVLine(114,132,2, grayColor);
	ST7735_DrawFastVLine(115,132,2, grayColor);
	ST7735_DrawFastVLine(116,133,4, grayColor);
	ST7735_DrawFastVLine(117,135,8, grayColor);
	ST7735_DrawFastHLine(113,137,4, grayColor);

	ST7735_DrawFastVLine(119,132,11, grayColor);
	ST7735_DrawFastVLine(120,132,5, grayColor);
	ST7735_DrawFastVLine(121,136,5, grayColor);
	ST7735_DrawFastVLine(122,139,4, grayColor);
	ST7735_DrawFastVLine(125,132,11, grayColor);
	ST7735_DrawFastVLine(124,132,5, grayColor);
	ST7735_DrawFastVLine(123,136,5, grayColor);
	
	//PM
	ST7735_DrawFastVLine(112,146,11, ST7735_YELLOW);
	ST7735_DrawFastHLine(113,146,4, ST7735_YELLOW);
	ST7735_DrawFastVLine(116,146,2, ST7735_YELLOW);
	ST7735_DrawFastVLine(117,147,4, ST7735_YELLOW);
	ST7735_DrawFastVLine(116,150,2, ST7735_YELLOW);
	ST7735_DrawFastHLine(113,151,4, ST7735_YELLOW);
	
	ST7735_DrawFastVLine(119,146,11, ST7735_YELLOW);
	ST7735_DrawFastVLine(120,146,5, ST7735_YELLOW);
	ST7735_DrawFastVLine(121,150,5, ST7735_YELLOW);
	ST7735_DrawFastVLine(122,153,4, ST7735_YELLOW);
	ST7735_DrawFastVLine(125,146,11, ST7735_YELLOW);
	ST7735_DrawFastVLine(124,146,5, ST7735_YELLOW);
	ST7735_DrawFastVLine(123,150,5, ST7735_YELLOW);
	
	}

	
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
	ST7735_drawline(86, 25, 90, 20, ST7735_WHITE);  //1
	ST7735_drawline(85, 26, 89, 21, ST7735_WHITE); 
	ST7735_drawline(101, 42, 105, 40, ST7735_WHITE); // 2
	ST7735_drawline(100, 42, 105, 41, ST7735_WHITE); 
	ST7735_DrawFastHLine(106,63,4, ST7735_WHITE); //3
	ST7735_drawline( 101, 85, 105, 87, ST7735_WHITE); // 4
	ST7735_drawline( 100, 84, 105, 88, ST7735_WHITE); //
	ST7735_drawline( 84, 97, 88, 105, ST7735_WHITE); // 5
	ST7735_drawline( 85, 98, 88, 106, ST7735_WHITE); // 
	ST7735_DrawFastVLine(63,105,4, ST7735_WHITE); //6
	ST7735_drawline(40, 101, 43, 97, ST7735_WHITE); // 7
	ST7735_drawline(39, 101, 42, 97, ST7735_WHITE);
	ST7735_DrawPixel(40,100, ST7735_WHITE); 
	ST7735_drawline(23, 86, 27, 84, ST7735_WHITE); // 8 
	ST7735_drawline(23, 85, 27, 84, ST7735_WHITE); 
	ST7735_DrawFastHLine(17,63,4, ST7735_WHITE); //9
	ST7735_drawline( 22, 39, 27, 41, ST7735_WHITE); //10
	ST7735_drawline( 22, 38, 27, 41, ST7735_WHITE); //
	ST7735_drawline( 39, 20, 43, 27, ST7735_WHITE); // 11
	ST7735_drawline( 39, 21, 42, 26, ST7735_WHITE); // 
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
	AMorPM();
}
