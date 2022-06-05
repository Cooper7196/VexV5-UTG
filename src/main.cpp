#include "main.h"
#include "autoSelect/selection.h"
#include <fstream>

/* Build with
pros make -- FLAGS=-DREPLAY
to replay, will record otherwise

*/

// Change this comment to reupload

// #define REPLAY

#ifndef REPLAY
#define RECORD
#endif

double xPos = 0;
double yPos = 0;
double angle = 0;
int selectedAuto = 0;
bool hasAutoStarted = false;
bool startReplay = false;

pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::IMU gyro(3);
pros::Rotation leftEncoder(11);
pros::Rotation rightEncoder(20);
pros::Rotation sideEncoder(7);

pros::Motor front_right_mtr(6);
pros::Motor front_left_mtr(5);
pros::Motor back_right_mtr(16);
pros::Motor back_left_mtr(15);
pros::Motor arm_mtr_left(1);
pros::Motor arm_mtr_right(10);
pros::Motor claw_mtr(8);
pros::Motor back_claw_mtr(9);

pros::Motor spinner(4);

Arm arm(1, 10, 1);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

void autoSelector()
{
	std::string autos[5] = {"Left Win Point", "Right Win Point", "Left Center Goal", "Right Center Goal", "Skills Autonomous"};
	selectedAuto = 0;
	std::string autoString = autos[selectedAuto];
	controller.clear();
	pros::delay(50);
	controller.set_text(0, ceil((19 - autoString.length()) / 2), autoString);
	while (!hasAutoStarted)
	{
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) && selectedAuto > 0)
		{
			selectedAuto--;
			controller.clear();
			autoString = autos[selectedAuto];
			pros::delay(50);
			controller.set_text(0, ceil((20 - autoString.length()) / 2), autoString);
			pros::delay(50);
		}
		else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT) && selectedAuto < 4)
		{
			selectedAuto++;
			controller.clear();
			autoString = autos[selectedAuto];
			pros::delay(50);
			controller.set_text(0, ceil((20 - autoString.length()) / 2), autoString);
			pros::delay(50);
		}
		pros::delay(20);
		// printf("%d\n", selectedAuto);
	}
}

void initialize()
{
	pros::Task odometry_task(odometry);
	pros::Task selector_task(autoSelector);

#ifdef RECORD
	pros::Task replay_task(savePos);
#endif

#ifdef REPLAY
	pros::Task replay_task(replayPos);
#endif

	selector::init();
	arm_mtr_left.set_reversed(true);
	claw_mtr.set_reversed(true);
	back_claw_mtr.set_reversed(true);
	pros::delay(2000);
}

void savePos()
{
	std::ofstream myfile;
	myfile.open("/usd/PosData.txt");
	myfile.close();
	std::uint32_t now = pros::millis();
	int curTime = pros::millis();
	int lastTime = pros::millis();
	// pros::controller_print(CONTROLLER_MASTER, 0, 6, "Recording");
	while (true)
	{
		if ((abs(xPos) > 0.1 || yPos > 0.1 || angle * 180 / M_PI > 0.5))
		{
			lastTime = curTime;
			myfile.open("/usd/PosData.txt", std::ios_base::app);
			curTime = pros::micros();
			myfile
				<< xPos << " " << yPos << " " << angle * 180 / M_PI << " " << curTime - lastTime << std::endl;
			myfile.close();
		}
		pros::Task::delay_until(&now, 20);
	}
}
void replayPos()
{
	printf(pros::usd::is_installed() ? "true" : "false");
	printf("Opening File\n\n");
	std::ifstream infile("/usd/PosData.txt");
	printf("File Opened\n\n");
	pros::delay(2000);
	controller.set_text(0, 0, "Starting Replay");
	printf("Replay Starting\n\n");
	double tempX, tempY, tempAngle, tempTime;
	startReplay = true;
	std::uint32_t now = pros::millis();
	while (infile >> tempX >> tempY >> tempAngle >> tempTime)
	{
		targetX = tempX;
		targetY = tempY;
		targetAngleGlobal = tempAngle;
		printf("%f\n", tempTime);
		pros::delay(20);
		// pros::Task::delay_until(&now, tempTime / 1000.0);
	}
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector

 * starts.
 */
void competition_initialize()
{
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

/*
Our auton, who art in c++,
	called by thy name;
  thy operating system run;
thy program run correctly
  as it did in testing.
Give us this day our daily miracle.
And forgive us our sketchy autonomous,
	as we forgive Cooper
  who writes broken code.
And lead us not into defeat;
  but deliver us from coding at the tournament.
	For thine is the awp,
the odometry, and the tracking,
  leading us, our robot,
	to victory.
	  Return;
*/
void autonomous()
{
	leftEncoder.reset();
	rightEncoder.reset();
	sideEncoder.reset();
	leftEncoder.reset_position();
	rightEncoder.reset_position();
	sideEncoder.reset_position();

	// selectedAuto = 0;
	hasAutoStarted = true;
	if (selectedAuto == 0)
	{

		driveToPoint(0, 22, 0, 60, 850);
		move_relative_blocking(claw_mtr, 2400, 100, 1000);
		driveToPoint(0, 16, 5, 60, 300);
		move_relative_blocking(claw_mtr, -2400, 100, 1000);
		driveToPoint(0, 8, 5, 60, 150);
		driveToPoint(49, 22, -85, 80, 2000);
		move_relative_blocking(claw_mtr, 2400, 100, 1000);
		arm.asyncMoveToAngle(50, 60);
		driveToPoint(20, 20, -90, 100);
	}
	if (selectedAuto == 1)
	{

		driveToPoint(0, 18, 0, 60, 750);
		move_relative_blocking(claw_mtr, 2400, 100, 1000);
		driveToPoint(14, 8, 90, 60, 500);
		move_relative_blocking(claw_mtr, -2400, 100, 1000);
		driveToPoint(23, 8, 90, 80, 600);
		driveToPoint(23, 48, 0, 80, 1000);
		driveToPoint(23, 50, 0, 80, 250);
		move_relative_blocking(claw_mtr, 2400, 127, 1000);
		driveToPoint(23, 15, 0, 80, 1000);
		move_relative_blocking(claw_mtr, -2400, 100, 1000);
	}

	if (selectedAuto == 2)
	{

		driveToPoint(-22, 46, 5, 127, 1500);
		driveToPoint(-22, 52, 5, 127, 100);
		move_relative_blocking(claw_mtr, 2400, 127, 1000);
		arm.asyncMoveToAngle(50, 60);
		driveToPoint(-22, 20, 0, 100, 2000);
		arm.moveToAngle(0, 60);

		move_relative_blocking(claw_mtr, -2400, 100, 3000);
	}
	if (selectedAuto == 3)
	{
		driveToPoint(22, 10, 0, 127, 100);

		driveToPoint(22, 44, 0, 100, 750);
		driveToPoint(22, 53, 0, 127, 100);
		move_relative_blocking(claw_mtr, 2400, 127, 1000);

		driveToPoint(22, 20, 0, 100, 1000);
	}

	if (selectedAuto == 4)
	{
		// grab neutral goal
		driveToPoint(-22, 46, 5, 127, 1500);
		driveToPoint(-22, 52, 5, 127, 100);
		move_relative_blocking(claw_mtr, 2400, 127, 1000);
		arm.asyncMoveToAngle(110, 127);
		// Put neutral goal on platform
		driveToPoint(-50, 105, 10, 100, 2000);
		// driveToPoint(-56, 21, 170, 100, 2000);
		arm.moveToAngle(75, 40);
		pros::delay(500);
		move_relative_blocking(claw_mtr, -2400, 100, 3000);

		// get WP red goal
		driveToPoint(-43, 95, 10, 100, 500);
		arm.asyncMoveToAngle(0, 60);
		driveToPoint(-12, 94, -90, 100, 2000);
		driveToPoint(-6, 94, -90, 100, 800);
		move_relative_blocking(claw_mtr, 2400, 127, 1000);
		arm.asyncMoveToAngle(110, 100);
		// Put WP red goal on platform
		driveToPoint(-56, 21, 170, 100, 3000);
		arm.moveToAngle(75, 40);
		move_relative_blocking(claw_mtr, -2400, 100, 3000);
		driveToPoint(-56, 26, 170, 100, 500);
		// Get WP blue goal
		arm.asyncMoveToAngle(0, 60);
		driveToPoint(-97, 30, 90, 100, 3000);
		driveToPoint(-105, 30, 90, 100, 500);
		move_relative_blocking(claw_mtr, 2400, 127, 1000);
		// Score WP blue goal
		arm.asyncMoveToAngle(60, 127);

		driveToPoint(-100, 46, 175, 127, 1500);
		driveToPoint(-100, 52, 175, 127, 500);
		move_relative_blocking(back_claw_mtr, 2400, 200, 3000);

		// driveToPoint(-30, 105, 10, 100, 2000);
	}
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol()
{
	// pros::delay(5000);
	// autonomous();
	bool fieldCentric = false;
	double moveSpeed = 0;
	double rotSpeed = 0;

	leftEncoder.reset();
	rightEncoder.reset();
	sideEncoder.reset();
	leftEncoder.reset_position();
	rightEncoder.reset_position();
	sideEncoder.reset_position();

	xPos = 0;
	yPos = 0;
	angle = 0;

	arm.setMode(false);
	arm_mtr_left.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	arm_mtr_right.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	claw_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	back_claw_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

	spinner.move(127);

	while (true)
	{
		float joystickCh1 = controller.get_analog(ANALOG_RIGHT_X) / 127.0 * 200.0;
		float joystickCh2 = controller.get_analog(ANALOG_RIGHT_Y) / 127.0 * 200.0;
		float joystickCh3 = controller.get_analog(ANALOG_LEFT_Y) / 127.0 * 200.0;
		float joystickCh4 = controller.get_analog(ANALOG_LEFT_X) / 127.0 * 200.0;

		if (!fieldCentric)
		{
			front_right_mtr.move_velocity(-joystickCh3 + joystickCh1 + joystickCh4);
			front_left_mtr.move_velocity(joystickCh3 + joystickCh1 + joystickCh4);
			back_right_mtr.move_velocity(-joystickCh3 + joystickCh1 - joystickCh4);
			back_left_mtr.move_velocity(joystickCh3 + joystickCh1 - joystickCh4);
		}
		else
		{
			// driveToPoint(0, 0, 3600, 127);
			double targetAngle = (atan2(-joystickCh4, joystickCh3) + M_PI / 2 + angle);

			double xRatio = -cos(targetAngle + (M_PI / 4));
			double yRatio = sin(targetAngle + (M_PI / 4));

			// Normalize values to maximum of 1
			double maxRatio = std::max(abs(xRatio), abs(yRatio));
			double xPowerPercentage = (xRatio / maxRatio);
			double yPowerPercentage = (yRatio / maxRatio);

			double joystickMag = hypot(joystickCh4, joystickCh3);

			moveSpeed = slew(joystickMag, moveSpeed, 20);
			rotSpeed = slew(joystickCh1, rotSpeed, 20);

			// Move at angle while rotating
			front_right_mtr.move(-xPowerPercentage * moveSpeed + rotSpeed);
			front_left_mtr.move(yPowerPercentage * moveSpeed + rotSpeed);
			back_right_mtr.move(-yPowerPercentage * moveSpeed + rotSpeed);
			back_left_mtr.move(xPowerPercentage * moveSpeed + rotSpeed);
		}
		pros::delay(20);
	}
}