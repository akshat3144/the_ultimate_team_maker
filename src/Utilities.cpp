#include "../include/Utilities.h"
#include <iostream>
#include <iomanip>
using namespace std;

// Helper function to draw a line
void drawLine(int length)
{
    cout << setfill('-') << setw(length) << "-" << setfill(' ') << endl;
}

// Helper function to print a message with spacing
void printMessage(const string &message, int spacing)
{
    cout << setw(spacing) << "" << message << endl;
}