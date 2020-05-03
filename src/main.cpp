#include "main.h"

#define FWPORT 15
pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Motor fw(15);

double motorPower = 0.0; //Flywheel Power Output Variable
double lastError = 0.0; //Previous Error
double tbh = 0.0; //Calculates TBH Output
float FlywheelTarget = 0.0001; //Current Flywheel target

double gain = 0.5; //Flywheel control gain, tune this to adjust recovery speed.

double FlywheelCalculate(double target) {
    double error = target - fw.get_actual_velocity();
    motorPower += gain * error;
    motorPower = motorPower/fabs(motorPower) * ((fabs(motorPower) > 127)? 127: motorPower);
  //If the error has changed in sign since the last processing
  if ((lastError > 0) != (error>0)) {
      motorPower = 0.5 * (motorPower + tbh);
      tbh = motorPower;
      lastError = error;
  }
  return motorPower;
}

void SetNewFlywheelTarget(float newRpm) {
    if(FlywheelTarget < newRpm) {
        lastError = 1;
    }
    else if(FlywheelTarget > newRpm) {
        lastError = -1;
    }
    tbh = (2 * (newRpm / 200)) - 1;
    FlywheelTarget = newRpm;
}

void initialize() {
	pros::lcd::initialize();
}

void disabled() {}
void competition_initialize() {}
void autonomous() {}

void opcontrol() {
	while (1){
		if (master.get_digital(DIGITAL_A)) {
			SetNewFlywheelTarget(230);
		}
		if (master.get_digital(DIGITAL_B)) {
			SetNewFlywheelTarget(100);
		}
		if (master.get_digital(DIGITAL_X)) {
			SetNewFlywheelTarget(50);
		}
		if (master.get_digital(DIGITAL_Y)) {
			SetNewFlywheelTarget(0);
		}
		fw.move(FlywheelCalculate(FlywheelTarget));
		pros::lcd::print(2,"%lf", fw.get_actual_velocity());
		pros::delay(20);
	}
}
