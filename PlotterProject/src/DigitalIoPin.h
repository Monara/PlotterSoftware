/*
 * DigitalIoPin.h
 *
 *  Created on: 25 Aug 2020
 *      Author: Asus
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_

class DigitalIoPin {
public:
	enum pinMode {
		output,
		input,
		pullup,
		pulldown
	};
	DigitalIoPin(int port, int pin, pinMode mode, bool invert = false);
	~DigitalIoPin();
	bool read();
	void write(bool value);
private:
	int port;
	int pin;

};

#endif /* DIGITALIOPIN_H_ */
