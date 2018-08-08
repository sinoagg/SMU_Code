#ifndef __AD8253_H
#define __AD8253_H	 

enum AD8253_Scale
{
	SCALE1X=0,
	SCALE10X=1,
	SCALE100X=2
};

void ADC_VoltageScaling(enum AD8253_Scale scale);
void ADC_CurrentScaling(enum AD8253_Scale scale);

#endif

