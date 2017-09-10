//Title of this code

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

#define INPUT_LIMIT 3
#define FLOORS_LIMIT 100

using namespace std;

enum class elev_direction { UP, DOWN, IDLE };

void enable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

class Elevator {
    int elevatorLevel;
    elev_direction elev_move;
    set<int> levelsToReach;

  public:

    Elevator() = delete;
    Elevator(int level) : elevatorLevel{level}, elev_move{elev_direction::IDLE} {}

    int getCurrentLevel() {
        return elevatorLevel;
    }

    elev_direction getDirection() {
        //  -1,0,1
        return elev_move;
    }

    void setDirection(elev_direction elevDirection){
    	elev_move = elevDirection;
    }

    bool isDirectionGood(int level_req) {
        // depends on requester level and requested level
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

        // not good need to be ordered , no duplicates
    	if( level_req < elevatorLevel ){
    		setDirection(elev_direction::DOWN);
    	} else {
    		setDirection(elev_direction::UP);
    	}

        levelsToReach.insert(level_req);
    }

    bool move() {
    	bool ret_value = false;
//    	cout << "Enter move\nLevels to reach is: " << levelsToReach.size() << endl;
        if ( !levelsToReach.empty() ) {
            auto level_to_reach = levelsToReach.begin();
//            cout << "Level to reach: " << *level_to_reach << endl;
            if( elevatorLevel == *level_to_reach ){
            	cout << "Cabin on floor " << elevatorLevel << " Stopped. Doors opened." << endl;
//            	cout << "Equal range for " << elevatorLevel << " " << *levelsToReach.equal_range(elevatorLevel).first << endl;
            	levelsToReach.erase(elevatorLevel);
//            	setDirection(elev_direction::IDLE);
            	ret_value = true;
            } else if( elevatorLevel == *levelsToReach.lower_bound(elevatorLevel) ){
            	cout << "Cabin on floor " << elevatorLevel << " Stopped. Doors opened." << endl;
            	levelsToReach.erase( elevatorLevel );
            	ret_value = true;
            }
            if ( elevatorLevel < *level_to_reach ){
            	cout << "Cabin on floor " << this->getCurrentLevel() << " Going UP!" << endl;
//            	cout << "Lower bound: " << *levelsToReach.lower_bound(elevatorLevel) << endl;
            	++elevatorLevel;
            } else if(elevatorLevel > *level_to_reach){
            	cout << "Cabin on floor " << this->getCurrentLevel() << " Going DOWN!" << endl;
//            	cout << "Lower bound: " << *levelsToReach.lower_bound(elevatorLevel) << endl;
            	--elevatorLevel;
            } else {

            }
        } else {
        	cout << "Cabin on floor " << elevatorLevel << " Stopped. Doors opened." << endl;
        	setDirection( elev_direction::IDLE );
        	ret_value = false;
        }
        return ret_value;
    }

};



class ElevatorControl {

    Elevator elevator{1};
    set<int> requests;
    int numLevels;

  public:

    ElevatorControl(int maxLevel) : numLevels{maxLevel} {}

    bool step(){
    	set<int> temp;
 //   	cout << "Enter step!" << endl;
        for (auto level_to_reach : requests) {
 //       	cout << level_to_reach << endl;
        	if( elevator.isDirectionGood(level_to_reach) ){
        		elevator.addNewRequest(level_to_reach);
        		temp.insert(level_to_reach);
            }
        }
        for (auto level_to_reach : temp) {
        	requests.erase(level_to_reach);
        }
        // here do something smart thing with elevators
        return elevator.move();
    }


    void elevCall(int floor) {
        requests.insert(floor);
    }

    bool isThereElevCall() {
    	return !requests.empty();
    }


    void printStatus() {

        for (int level = numLevels; level >= 1; --level) {
        	if (elevator.getCurrentLevel() == level)
        		cout << "#";
            else
            	cout << ".";
        }
        cout << endl;
    }

};



int main()
{
	char input[INPUT_LIMIT];
	uint32_t max_floors_num = 0;
    uint32_t request_from_floor = 0;
	cout << "To close the program type \"CTRL+c\"\nPlease enter MAX number of floors in your building <= " << FLOORS_LIMIT << " : " << endl;
	while ( cin.getline(input, INPUT_LIMIT ) ){
		if( isdigit(input[0]) ){
			max_floors_num = atoi(input);
			break;
		} else {
			cout << "Wrong input!!!\n";
					cout << "To close the program type \"CTRL+c\"\nPlease enter MAX number of floors in your building <= " << FLOORS_LIMIT << " :" << endl;
		}

	}
    if ( max_floors_num < 1 || max_floors_num > FLOORS_LIMIT ){
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
    while( request_from_floor < 1 || request_from_floor > max_floors_num ){
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
	ElevatorControl ec(max_floors_num);
    ec.elevCall(request_from_floor);
    while( 1 ){
    	ec.printStatus();
    	enable_raw_mode();
    	while( !kbhit() && !ec.step() ){
    		sleep(1);
    		cout << "<Press any key to enter new command>";
    		ec.printStatus();
    	}
    	disable_raw_mode();
    	tcflush(0, TCIFLUSH); // Clear stdin to prevent characters appearing on prompt
    	if( cin.get(input, INPUT_LIMIT) && isdigit(input[0]) ){
    		request_from_floor = atoi(input);
//    		cout << "Got floor number!" << endl;
    	} else {
    		request_from_floor = 0;
//    		cout << "input: " << input << " cin cleared!" << endl;
    		cin.clear();
    		cin.ignore(100, '\n');
    		if( cin.get(input, INPUT_LIMIT) && isdigit(input[0]) ){
    			request_from_floor = atoi(input);
//    		    cout << "Got floor number!" << endl;
    		}
    	}
        if( request_from_floor >= 1 && request_from_floor <= max_floors_num ){
        	ec.elevCall(request_from_floor);
        } else {
        	request_from_floor = 0;
        }
    }
}
