#include "main.h"

void odometry(void *odometryArgs)
{

    *((OdometryArgs *)odometryArgs)->x = 0;
    *((OdometryArgs *)odometryArgs)->y = 0;
    *((OdometryArgs *)odometryArgs)->theta = 0;

    pros::Rotation leftEncoder(((OdometryArgs *)odometryArgs)->leftEncoderPort);
    pros::Rotation rightEncoder(((OdometryArgs *)odometryArgs)->rightEncoderPort);

    // pros::Rotation sideEncoder(12);to_string

    // pros::lcd::print(7, "%p", (((OdometryArgs *)odometryArgs)->theta));

    long int currentLeft = 0;
    long int currentRight = 0;
    long int oldLeft = 0;
    long int oldRight = 0;

    long int deltaLeft = 0;
    long int deltaRight = 0;
    double deltaTheta = 0;

    leftEncoder.reset();
    rightEncoder.reset();
    leftEncoder.reset_position();
    rightEncoder.reset_position();
    while (true)
    {
        oldLeft = currentLeft;
        oldRight = currentRight;

        // Get the current encoder values in degrees
        currentLeft = -(leftEncoder.get_position() / 100.0);
        currentRight = (rightEncoder.get_position() / 100.0);

        // multiply by wheel diameter then divide by 360 degrees to get inches
        currentLeft *= 2.5 / 360.0;
        currentRight *= 2.5 / 360.0;

        // Calculate the change in distance
        deltaLeft = (currentLeft - oldLeft);
        deltaRight = (currentRight - oldRight);

        // Calculate the change in angle
        deltaTheta = (deltaLeft - deltaRight) / (((OdometryArgs *)odometryArgs)->leftWheelDistance + ((OdometryArgs *)odometryArgs)->rightWheelDistance);

        //Accumulate the change in angle
        *(((OdometryArgs *)odometryArgs)->theta) += (double)deltaTheta;

        
        pros::delay(10);
    }
}