// ***** 0. Documentation Section *****
// elevator_main.cpp
// Runs on Linux
// The program emulates the movement of the elevator. 
// Andrey Spitsyn
// September 10, 2017

// ***** 1. Pre-processor Directives Section *****
#include <iostream>
#include <ctime>
#include <list>
#include <set>
#include <map>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>

#define INPUT_LIMIT 3	 // max three digit decimal number
#define FLOORS_LIMIT 100 // suppose there is no buildings above FLOORS_LIMIT

using namespace std; // just to siplify code

// ***** 2. Subroutines Section *****
void enable_raw_mode() // Disable echo as well
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

bool kbhit() // detect key press
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

// ***** 2. Classes Section *****
// Elevetor class
class Elevator {
    enum class elev_direction { UP, DOWN, IDLE };
    int elevatorLevel; // Elevator current level
    elev_direction elev_move; // Elevator current direction
    set<int> levelsToReach; // Set of levels to reach

  public:
    Elevator() = delete; // no constructor without parameters
    Elevator(int level) : elevatorLevel{level}, elev_move{elev_direction::IDLE} {}

    int getCurrentLevel() {
        return elevatorLevel;
    }

    elev_direction getDirection() {
        return elev_move;
    }

    void setDirection(elev_direction elevDirection){
    	elev_move = elevDirection;
    }

    bool isDirectionGood(int level_req) {
    	bool ret_value = false;
    	if( level_req <  elevatorLevel  && elev_move == elev_direction::DOWN ){
    		ret_value = true;
    	} else if( level_req > elevatorLevel  && elev_move == elev_direction::UP ){
    		ret_value = true;
    	} else if( elev_move == elev_direction::IDLE ){
    		ret_value = true;
    	}
        return ret_value;

    }

    void addNewRequest(int level_req) {
    	if( level_req < elevatorLevel ){
    		setDirection(elev_direction::DOWN);
    	} else {
    		setDirection(elev_direction::UP);
    	}
        levelsToReach.insert(level_req);
    }

    bool move() { // emulate elevator movement
    	bool ret_value = false;
        if ( !levelsToReach.empty() ) { // if current set of requests is not empty move elevator
            auto level_to_reach = levelsToReach.begin();
            if( elevatorLevel == *level_to_reach ){ // if elevator reach max floor request
            	cout << "Cabin on floor " << elevatorLevel << " Stopped. Doors opened." << endl;
            	levelsToReach.erase(elevatorLevel);
            	ret_value = true;
            } else if( elevatorLevel == *levelsToReach.lower_bound(elevatorLevel) ){ // if elevator reach concomitant level
            	cout << "Cabin on floor " << elevatorLevel << " Stopped. Doors opened." << endl;
            	levelsToReach.erase( elevatorLevel );
            	ret_value = true;
            }
            if ( elevatorLevel < *level_to_reach ){
            	cout << "Cabin on floor " << this->getCurrentLevel() << " Going UP!" << endl;
            	++elevatorLevel;
            } else if(elevatorLevel > *level_to_reach){
            	cout << "Cabin on floor " << this->getCurrentLevel() << " Going DOWN!" << endl;
            	--elevatorLevel;
            } else {

            }
        } else { // all requests handled
        	cout << "Cabin on floor " << elevatorLevel << " Stopped. Doors opened." << endl;
        	setDirection( elev_direction::IDLE );
        	ret_value = false;
        }
        return ret_value;
    }

}; // Class Elevator


// Elevator control class
class ElevatorControl {
    Elevator elevator{1}; // elevator initialisation
    set<int> requests; // set to store requests from floors
    int numLevels; // max num floors in the building
  public:
    ElevatorControl(int maxLevel) : numLevels{maxLevel} {}

    bool step(){ // handle requests and move elevator
    	set<int> temp;
        for (auto level_to_reach : requests) {
        	if( elevator.isDirectionGood(level_to_reach) ){ // if request from concomitant level handle it
        		elevator.addNewRequest(level_to_reach);
        		temp.insert(level_to_reach);
            }
        }
        for (auto level_to_reach : temp) { // remove requests which will handle by elevator 
        	requests.erase(level_to_reach);
        }
        // here move elevator
        return elevator.move();
    }

    void elevCall(int floor) { // call elevator
        requests.insert(floor);
    }

    bool isThereElevCall() { // check if request set is not empty
    	return !requests.empty();
    }


    void printStatus() { // help function to visualise elevator movement
        for (int level = numLevels; level >= 1; --level) {
        	if (elevator.getCurrentLevel() == level)
        		cout << "#";
        	else
            		cout << ".";
        }
        cout << endl;
    }

}; // Class Elevator control



int main()
{
	char input[INPUT_LIMIT]; // store user input
	uint32_t max_floors_num = 0;
    	uint32_t request_from_floor = 0;
	cout << "To close the program type \"CTRL+c\"\nPlease enter MAX number of floors in your building <= " << FLOORS_LIMIT << " : " << endl;
	while ( cin.getline(input, INPUT_LIMIT ) ){ // get max number floors, suppose there is no buildings above FLOORS_LIMIT
		if( isdigit(input[0]) ){
			max_floors_num = atoi(input);
			break;
		} else {
			cout << "Wrong input!!!\n";
					cout << "To close the program type \"CTRL+c\"\nPlease enter MAX number of floors in your building <= " << FLOORS_LIMIT << " :" << endl;
		}

	}
    	if ( max_floors_num < 1 || max_floors_num > FLOORS_LIMIT ){ // if max_floors above limit end program
    		cout << "ENTERED WRONG FLOORS NUMBER!!!" << endl;
    		return 0;
    	}
    	cout << "Enter floor which you want to reach : " << endl;
	while ( cin.get(input, INPUT_LIMIT) ){
		if( isdigit(input[0]) ){
			request_from_floor = atoi(input);
			break;
		} else {
			cout << "Wrong input!!!\n";
			cout << "Enter floor which you want to reach:" << endl;
		}
	}
    	while( request_from_floor < 1 || request_from_floor > max_floors_num ){ // check requested floor
    		cout << "Please enter right floor number: " << endl;
    		while ( cin.getline(input, INPUT_LIMIT ) ){
    			if( isdigit(input[0]) ){
    			request_from_floor = atoi(input);
    			break;
    		} else {
    			cout << "Wrong input!!!\nType \"CTRL+c\" to stop!\n";
    			cout << "Enter floor which you want to reach:" << endl;
    		}

    	}
    }
    // Create elevator control object
    ElevatorControl ec(max_floors_num);
    // add request from a floor
    ec.elevCall(request_from_floor);
    // main loop for elevator movement
    while( 1 ){
    	ec.printStatus(); // visualise elevator movement 
    	enable_raw_mode();
    	while( !kbhit() && !ec.step() ){ // if no keyboard press or elevator reach floor
    		sleep(1);
    		cout << "<Press any key to enter new command>";
    		ec.printStatus();
    	}
    	disable_raw_mode();
    	tcflush(0, TCIFLUSH); // Clear stdin to prevent characters appearing on prompt
    	if( cin.get(input, INPUT_LIMIT) && isdigit(input[0]) ){ // when elevator reach a floor ask for new request
    		request_from_floor = atoi(input);
    	} else { // if something wrong with input, clear input stream and continue
    		request_from_floor = 0;
    		cin.clear();
    		cin.ignore(100, '\n');
    		if( cin.get(input, INPUT_LIMIT) && isdigit(input[0]) ){
    			request_from_floor = atoi(input);
    		}
    	}
        if( request_from_floor >= 1 && request_from_floor <= max_floors_num ){ // check floor number and add to elevator call set
        	ec.elevCall(request_from_floor);
        } else { // continue
        	request_from_floor = 0;
        }
    }
}
