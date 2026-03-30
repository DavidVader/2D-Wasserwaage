#include <avr/io.h>
#include <math.h>
#include "i2clib.h"
#include "OLEDLib.h"

#define PCAL6408A_ADDR 0x40
#define FXLS_ADDR 0x30

int16_t x, y, z;

void wait(void)
{
	for(volatile uint32_t i = 0; i < 60000; i++) {}
}

void pcal_write(uint8_t r,uint8_t v)
{
	i2c_start();
	i2c_write(PCAL6408A_ADDR);
	i2c_write(r);
	i2c_write(v);
	i2c_stop();
}

void fxls_write(uint8_t r,uint8_t v)
{
	i2c_start();
	i2c_write(FXLS_ADDR);
	i2c_write(r);
	i2c_write(v);
	i2c_stop();
}

uint8_t fxls_read(uint8_t r)
{
	uint8_t v;
	i2c_start();
	i2c_write(FXLS_ADDR);
	i2c_write(r);
	i2c_stop();
	i2c_start();
	i2c_write(FXLS_ADDR | 1);
	v = i2c_read();
	i2c_m_nack();
	i2c_stop();
	return v;
}

int main(void)
{
	wait();

	// Nr.1
	pcal_write(0x03,0x00);
	pcal_write(0x01,0x01);

	// Nr.2
	fxls_write(0x15,0x00);
	fxls_write(0x16,0b01010000);
	fxls_write(0x17,0x77);
	fxls_write(0x18,0x84);
	fxls_write(0x15,1);

	// Nr.3
	TCCR1A = 0x00;
	TCCR1B = 0x08;
	OCR1A = 11125;
	OCR1B = 3;

	// Nr.4
	DDRD  = DDRD | 0x04; // PD2 = Ausgang
	PORTD = PORTD & 0xFB; // PD2 = 0
	
	// Nr. 10
	int16_t sensor4, sensor5, sensor6, sensor7, sensor8, sensor9;
	
	//Nr.12
	old_init();
	
	uint8_t led_state = 0;

	while(1)
	{
		// Nr.5
		TCCR1B = 0x0B; // CS1 = 011 (0x08 + 0x03 = 0x0B)
		PORTD = PORTD | 0x04;

		// Nr.6
		while((TIFR1 & 0x04) == 0){}

		// Nr.7
		PORTD = PORTD & 0xFB;

		// Nr.8
		while((TIFR1 & 0x02) == 0){}

		// Nr.9
		TCCR1B = 0x08;
		TIFR1 = TIFR1;

		// Nr.10
		sensor4 = fxls_read(4);
		sensor5 = fxls_read(5);
		sensor6 = fxls_read(6);
		sensor7 = fxls_read(7);
		sensor8 = fxls_read(8);
		sensor9 = fxls_read(9);
		
		// Nr.11
		x = sensor4 + sensor5 * 256;
		y = sensor6 + sensor7 * 256;
		z = sensor8 + sensor9 * 256;
		
		// Nr.12
		old_clear(0);
		//old_intdez2(2, 0, x, 4);
		//old_intdez2(2, 3, y, 4);
		//old_intdez2(2, 6, z, 4);
		
		/* Aufgabe 7
		Berechne aus den x, y, z Werte den Neigungswinkel und gib ihn im Display aus.
		Tipp:
		benutze die Funktion float atan2(float y, float x)
		Das Ergebnis wird im Bogenmaß zurückgegeben, Bereich -PI bis +PI.
		Rechne es in Grad um.
		Tipp2:
		Für den Wert in der x-y-Ebene benutze Pythagoras.
		*/
		float xf = x;
		float yf = y;
		float zf = z;

		float xy = sqrt(xf*xf + yf*yf);
		float winkel = atan2(zf, xy);
		float grad = winkel * 57.2958;
		old_float2(2, 0, grad, 2);


		// Aufgabe LED wie eine Luftblase von einer Wasserwaage
		float richtung = atan2(yf, xf) * 57.2958;  // Richtung als Winkel
		if (richtung < 0) richtung += 360;  // behält alles in 0-360° Bereich (keine negative zahl)
		
		led_state = 0x01;
		
		if (richtung >= 300 || richtung < 60) {
			led_state = led_state | 0x20; // P5 = 1 (LED aus) 0x20
		}
		if (richtung >= 30 && richtung < 150) {
			led_state = led_state | 0x10; // P4 = 1 (LED aus) 0x10
		}
		if (richtung >= 120 && richtung < 240) {
			led_state = led_state | 0x04; // P2 = 1 (LED aus) 0x04
		}
		if (richtung >= 210 && richtung < 330) {
			led_state = led_state | 0x08; // P3 = 1 (LED aus) 0x08
		}
		
		led_state = ~led_state; // bits inventieren
		led_state = led_state | 0x01; // OE 
		pcal_write(0x01, led_state);
	}
}
