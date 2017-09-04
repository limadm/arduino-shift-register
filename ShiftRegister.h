/* Digital input/output with 74HC595.
 *
 * - Output mode uses pins as <DAT, SHCP/CLK, STCP/LATCH>
 *
 * - Input mode uses SH clock for both SHCP/CLK and STCP/LATCH
 *   (should be tied together) and ST as input pin, with push-
 *   buttons connected between outputs and ST input pin.
 *
 * MIT License © 2017 Daniel M. de Lima
 */
#pragma once

template<byte DAT, byte SH, byte ST>
class ShiftRegister {
private:
	uint8_t n;
	uint8_t *buffer;
public:
	ShiftRegister(int n)
		: n(n), buffer(new uint8_t[n])
	{
		pinMode(DAT, OUTPUT);
		pinMode(SH, OUTPUT);
		pinMode(ST, OUTPUT);
		digitalWrite(SH, LOW);
		digitalWrite(ST, LOW);
	}
	// set pin in internal buffer.
	void set(uint8_t pin, uint8_t value) {
		bitWrite(buffer[pin/8], pin%8, value);
	}
	// set pin then flush data to '595 outputs.
	void write(uint8_t pin, uint8_t value) {
		pinMode(ST, OUTPUT);
		set(pin, value);
		digitalWrite(SH, LOW);
		for (int i=0; i<n; i++) shiftOut(DO, SH, buffer[i]);
		digitalWrite(ST, HIGH);
		digitalWrite(ST, LOW);
	}
	// read current input value
	void read(uint8_t pin) {
		static uint8_t ready = 0;
		if (!ready) {
			read(); ready = 8*n-1;
		} else {
			ready--;
		}
		return buffer[pin/8][pin%8];
	}
	// update input buffer
	void read() {
		pinMode(ST, INPUT);
		digitalWrite(SH, LOW);
		digitalWrite(DAT, HIGH);
		digitalWrite(SH, HIGH);
		digitalWrite(DAT, LOW);
		for (int i=0; i<n; i++)
			for (int b=0; b<8; b++) {
				digitalWrite(SH, HIGH);
				digitalWrite(SH, LOW);
				bitWrite(buffer[i], b, digitalRead(ST));
			}
		digitalWrite(SH, HIGH);
		digitalWrite(SH, LOW);
	}

};
