//============================================================================
// Name        : .cpp
// Author      : Dr. Sibt Ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game...
//============================================================================

#ifndef RushHour_CPP_
#define RushHour_CPP_
#include "util.h"
#include <iostream>
#include<string>
#include<string>
#include<cmath> // for basic math functions such as cos, sin, sqrt
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

const int cell_size = 54;   // 1080/20 = 54 
const int grid_width = 36;  //1920/54 = 35.5
const int grid_height = 20;  //1080/54 = 20
const int board_width = grid_width * cell_size;
const int board_height = grid_height * cell_size;
const int no_ofPassengers = 3;
const int delivery_pkg = 3;
const int fuel_stations = 3;
const int other_cars = 3;



// seed the random numbers generator by current time (see the documentation of srand for further help)...

/* Function sets canvas size (drawing area) in pixels...
 *  that is what dimensions (x and y) your game will have
 *  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
 * */
void SetCanvasSize(int width, int height) {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}


class Position {
	private:
		int x;
		int y;
	public:
		Position(int x_coordinate = 0, int y_coordinate = 0): x(x_coordinate), y(y_coordinate) {}
	
		void setX(int x_coordinate) {
			x = x_coordinate; 
		}

		void setY(int y_coordinate) {
			y = y_coordinate; 
		}
	
		int getX() const {
			return x;
		}

		int getY() const {
			return y; 
		}
	
		void moveBy(int dx, int dy) {
			x += dx;
			y += dy;
		}
	
		bool operator==(Position const &other) const {
			return x == other.x && y == other.y;
		}
	};

class Entity {
	protected:
	   Position p;
	   bool visible;
	public:
	   Entity (int x = 0, int y = 0, bool v = true) : p(x, y), visible(v) {}

	   virtual ~Entity () = default;  // virtual destructor to avoid memory leaks

	   virtual void render() const = 0; // each class should its own drawing objects 

	   void setPosition (int x, int y) {
		p = Position (x, y);
	   }

	   Position getPosition () const {
		return p;
	   }

	   void toggleVisibility () {
		visible = !visible;
	   }

	   void setVisible (bool v) {
		visible = v;
	   }

	   bool isVisible () const {
		return visible;
	   }

	   virtual int getPoints() const = 0;

};

class Car : public Entity {
	protected:
	   string color;
	   int speed;
	public: 
	   Car (int x = 0, int y = 0, const string &col = "", int sp = 0) : Entity(x, y), color(col), speed(sp) {}

	   virtual ~Car () = default;
	   virtual void move () = 0; //each car move differently
	   virtual void render () const override = 0;
	   virtual int getPoints() const override {
		return 0;
	   }

	   void setSpeed (int sp) {
		speed  = sp;
	   }

	   int getSpeed () const {
		return speed;
	   }

	   void setColor (string col) {
		color = col;
	   }

	   string getColor () const {
		return color;
	   }
};


class Passenger : public Entity {
	private: 
	   Position destination;
	   bool isPickedUP;
	   int fare;
	public: 
	   Passenger (int initial_x = 0, int initial_y = 0, int desX = 0, int desY = 0, int getfare = 0) :
	   Entity (initial_x, initial_y), destination(desX, desY), isPickedUP(false), fare(getfare) {} 

	   ~Passenger () override = default;

	   void render() const override {
		if (!visible) {
			return;
		}
		DrawCircle(p.getX() + cell_size/2, p.getY() + cell_size/2 + 7, 5, colors[BLACK]);
        DrawLine(p.getX() + cell_size/2, p.getY() + cell_size/2 + 2, p.getX() + cell_size/2, p.getY() + cell_size/2 - 8, 2, colors[BLACK]);
        DrawLine(p.getX() + cell_size/2, p.getY() + cell_size/2, p.getX() + cell_size/2 - 6, p.getY() + cell_size/2 - 2, 2, colors[BLACK]);
        DrawLine(p.getX() + cell_size/2, p.getY() + cell_size/2, p.getX() + cell_size/2 + 6, p.getY() + cell_size/2 - 2, 2, colors[BLACK]);
        DrawLine(p.getX() + cell_size/2, p.getY() + cell_size/2 - 8, p.getX() + cell_size/2 - 4, p.getY() + cell_size/2 - 16, 2, colors[BLACK]);
        DrawLine(p.getX() + cell_size/2, p.getY() + cell_size/2 - 8, p.getX() + cell_size/2 + 4, p.getY() + cell_size/2 - 16, 2, colors[BLACK]);
	}
	

	   int getPoints () const override {
		if (isPickedUP) {
			return 0;
		}
		else return fare;
	   }

	   void pickup () {
		if (!isPickedUP) {
			isPickedUP = true;
			visible = false;
		}
	   }

	   void dropoff () {
		if (isPickedUP) {
			isPickedUP = false;
			visible = false;   //remove passenger from the board
		}
	   }

	   Position getDest() const {
		return destination;
	   }

	   int getFare () const {
		return fare;
	   }

};

class DeliveryPackage : public Entity {
	private: 
	   Position destination;
	   bool isPickedUP;
	   int deliveryFee;
	public: 
	   DeliveryPackage (int initial_x, int initial_y, int desX, int desY, int fee) :
	   Entity (initial_x, initial_y), destination (desX, desY), deliveryFee(fee) {}

	   ~DeliveryPackage() override = default;

	   void render() const override {
		if (!visible) {
			return;
		}

        DrawSquare(p.getX() - 10, p.getY() - 10, 20, colors[BROWN]);
        DrawLine(p.getX() - 10, p.getY() - 3, p.getX() + 10, p.getY() - 3, 1, colors[BLACK]);
        DrawLine(p.getX() - 3, p.getY() - 10, p.getX() - 3, p.getY() + 10, 1, colors[BLACK]);
	   }

	   int getPoints () const override {
		if (isPickedUP) {
			return 0;
		}
		else return deliveryFee;
	   }

	   void pickUp () {
		if (!isPickedUP) {
			isPickedUP = true;
			visible = false;
		}
	   }

	   void dropoff () {
		if (isPickedUP) {
			isPickedUP = false;
			visible = false;
		}
	   }

	   Position getDest () const {
		return destination;
	   }

	   int getFee () const {
		return deliveryFee;
	   }

};

class FuelStations : public Entity {
	public:
	   FuelStations(int x = 0, int y = 0) : Entity(x,y) {}
	   
	   void render() const override {
		// Skip rendering if not visible
		if (!visible) {
			return;
		}
		DrawSquare(p.getX() - 15, p.getY() - 15, 30, colors[RED]);
		DrawRectangle(p.getX() - 10, p.getY() - 5, 20, 10, colors[DARK_GRAY]);
		DrawRectangle(p.getX() - 5, p.getY() + 5, 3, 8, colors[BLACK]);
		DrawRectangle(p.getX() + 2, p.getY() + 5, 3, 8, colors[BLACK]);
		DrawRectangle(p.getX() - 15 - 5, p.getY() - 15 - 5, 30 + 10, 5, colors[YELLOW]);
	}

	   int getPoints() const override {
        return 0;
    }

};

void DrawCar(int x, int y, int bodyW, int bodyH, int wheelR, float* bodyColor, float* wheelColor) {

    int sx = x;
    int sy = y + wheelR;  
    DrawRectangle(sx, sy, bodyW, bodyH, bodyColor);

    int leftWheelX = x + wheelR;           
    int wheelY     = y;                 
    DrawCircle(leftWheelX, wheelY, wheelR, wheelColor);

    int rightWheelX = x + bodyW - wheelR; 
    DrawCircle(rightWheelX, wheelY, wheelR, wheelColor);
}

class Tree : public Entity {
	public: 
	Tree (int x = 0, int y = 0) : Entity(x,y) {}

	void render () const override {
		if (!visible) {
			return;
		}
		DrawRectangle (p.getX() - 3, p.getY() -10, 6, 10, colors[BROWN]);
		DrawCircle (p.getX(), p.getY()+5, 10, colors[GREEN]);
	}

	int getPoints () const override {
		return 0;
	}
	
};

class PlayerCar : public Car {
	private:
	   int vx;
	   int vy;
	   bool MovingLeft;
	   bool MovingRight;
	   bool MovingUp;
	   bool MovingDown;

	public: 
	   PlayerCar (int x = 0, int y = 0, const string &color = "green", int speed = 5):
	   Car(x, y, color, speed), vx(0), vy(0), MovingLeft(false), MovingRight(false),
	   MovingUp(false), MovingDown(false) {}
	   
	void move () override {
		int newx = p.getX() + vx;
		int newy = p.getY() + vy;

		if (newx >= 0 && newx <= board_width - 60) { //ensuring car stays within the board
			p.setX(newx);
		}

		if (newy >= 0 && newy <= board_height - 25) { //ensuring car stays within the board
			p.setY(newy);
		}
	}

	bool isMoving() const {
		return MovingLeft || MovingRight || MovingUp || MovingDown;
	}

	void handleInput (int key) {
		if (key == GLUT_KEY_UP) {
			MovingUp = true;
			vy = speed;
		}
		else if (key == GLUT_KEY_DOWN) {
			MovingDown = true;
			vy = -speed;
		}
		else if (key == GLUT_KEY_LEFT) {
			MovingLeft = true;
			vx = -speed;
		}
		else if (key == GLUT_KEY_RIGHT) {
			MovingRight = true;
			vx = speed;
		}
	}

	void stopCar (int key) {   //resets the velocity if key is released
		if (key == GLUT_KEY_UP) {
			MovingUp = false;
			if (!MovingDown) {
				vy = 0;
			}
		}
		else if (key == GLUT_KEY_DOWN) {
			MovingDown = false;
			if (!MovingUp) {
				vy = 0;
			}
		}
		else if (key == GLUT_KEY_RIGHT) {
			MovingRight = false;
			if (!MovingLeft) {
				vx = 0;
			}
		}
		else if (key == GLUT_KEY_LEFT) {
			MovingLeft = false;
			if (!MovingRight) {
				vx = 0;
			}
		}
	}

	void render () const override {
		DrawCar (p.getX(), p.getY(), 36, 20, 4, colors[GREEN], colors[BLACK]);
	}
};


class OtherCar : public Car {
	private: 
	   int direction; //0 = vertical & 1 = horizontal
	   int vx;
	   int vy;
	   int dir_counter;
	public: 
	   OtherCar (int x = 0, int y = 0, const string &color = "red", int speed = 3):
	   Car(x, y, color, speed), direction(rand() % 2), dir_counter(0) {

	if (direction == 0) {
		if (rand() % 2 == 0) {
			vy = speed;
		}
		else vy = -speed;
		vx = 0;
	}

	else if (direction == 1) {
		if (rand() % 2 == 0) {
			vx = speed;
		}
		else vx = -speed;
		vy = 0;
	}
}

    void move () override {
		dir_counter++;

		if (dir_counter > 100) { //after 100 frames, change the direction of car
		direction = rand () % 2;

		if (direction == 0) {
			if (rand() % 2 == 0) {
				vy = speed;
			}
			else vy = -speed;
			vx = 0;
		}
	
		else if (direction == 1) {
			if (rand() % 2 == 0) {
				vx = speed;
			}
			else vx = -speed;
			vy = 0;
		}
		dir_counter = 0;
	}
	    p.moveBy (vx, vy);

		if (p.getX() < 0) {
			p.setX(0);
		}
		if (p.getX() > board_width - 100) {
			p.setX(board_width-100); 
		}
		if (p.getY() < 0) {
			p.setY(0);
		}
		if (p.getY() > board_height - 100) {
			p.setY(board_height-100); 
		}
    }

	void render () const override {
		DrawCar (p.getX(), p.getY(), 36, 20, 4, colors[RED], colors[BLACK]);
	}

};

class Board {
	private:
	   int grid[grid_width][grid_height];
	   bool initialized;  //to track if board is initialized or not
	   int passengerCount;
	   int packageCount;

	public: 
	   Board () : initialized(false), passengerCount(no_ofPassengers), packageCount(delivery_pkg) {
		for (int i = 0; i < grid_width; i++) {
			for (int j = 0; j < grid_height; j++) {
				grid[i][j] = 0;  // roads
			}
		}
	   }

	   

	   void generateBoard () {
		if (initialized) {
			return;
		}
		//creatig the buildings
		createBuilding(2, 8, 15, 18);
		createBuilding(30, 34, 5, 15);
		createBuilding(12, 17, 3, 6);
		createBuilding(16, 19, 3, 8);
		createBuilding(28, 32, 16, 19);
		createBuilding(3, 6, 8, 13);
		createBuilding(3, 8, 12, 14);
		createBuilding(18, 23, 9, 11);
		createBuilding(20, 23, 7, 13);
		createBuilding(25, 30, 3, 6);
		createBuilding(27, 29, 6, 10);
		createBuilding(22, 26, 15, 18);
		createBuilding(24, 27, 12, 15);
			
			initialized = true;
		}

		void InitializeBoard (Passenger *passengers[], FuelStations *stations[fuel_stations], 
		DeliveryPackage *packages[], Tree *trees[]) {
			generateBoard();

			// Initialize the trees
			for (int i = 0; i < 10; i++) {
				Position treePos = findRandomRoadPosition();
				trees[i] = new Tree(treePos.getX(), treePos.getY());
			}

			 // Initializing the passengers
			 for (int i = 0; i < no_ofPassengers; i++) {
				Position passengerPos = findRandomRoadPosition();
				Position destPos = findValidDeliveryPosition();
				int fare = 50 + rand() % 100; // Random fare between 50 and 150
				passengers[i] = new Passenger(passengerPos.getX(), passengerPos.getY(), destPos.getX(), destPos.getY(), fare);
			}

			// fuel stations
			for (int i = 0; i < fuel_stations; i++) {
				Position stationPos = findRandomRoadPosition();
				stations[i] = new FuelStations(stationPos.getX(), stationPos.getY());
			}

			 //  packages
			 for (int i = 0; i < delivery_pkg; i++) {
				Position packagePos = findValidDeliveryPosition();
				Position destPos = findValidDeliveryPosition();
				int fee = 30 + rand() % 70; // Random fee between 30 and 100
				packages[i] = new DeliveryPackage(packagePos.getX(), packagePos.getY(), destPos.getX(), destPos.getY(), fee);
			}
	
		}
		
		void createBuilding(int startX, int endX, int startY, int endY) {
			for (int i = startX; i < endX; i++) {
				for (int j = startY; j < endY; j++) {
					if (i < grid_width && j < grid_height) {
						grid[i][j] = 1;
						
				}
			}
		}

	   }

	   bool isBuilding(int gridX, int gridY) const {
		if (gridX < 0 || gridX >= grid_width || gridY < 0 || gridY >= grid_height) {
			return false;
		}
		return grid[gridX][gridY] == 1;
	}

	   void render() {
		// roads
		DrawRectangle(0, 0, board_width, board_height, colors[WHITE]);
		
		// lines
		for (int i = 0; i <= grid_width; i++) {
			DrawLine(i * cell_size, 0, i * cell_size, board_height, 1, colors[GRAY]);
			//DrawLine(0, i * cell_size, board_width, i * cell_size, 1, colors[GRAY]);
		}
		
		//buildings
		for (int i = 0; i < grid_width; i++) {
			for (int j = 0; j < grid_height; j++) {
				if (grid[i][j] == 1) {
					DrawSquare(i * cell_size, j * cell_size, cell_size, colors[BLACK]);
				}
			}
		}
	}

	bool isPositionValid(int x, int y) const {
		int gridX = x / cell_size;
		int gridY = y / cell_size;
		
		if (gridX < 0 || gridX >= grid_width || gridY < 0 || gridY >= grid_height)
			return false;
			
		return grid[gridX][gridY] == 0; // 0 indicates roads
	}

	// Find a valid position near a building for delivery points
	Position findValidDeliveryPosition() const {
		int px, py;
		bool foundspot = false;
		
		for (int attempts = 0; attempts < 100 && !foundspot; attempts++) {
			px = 20 + rand() % (board_width - 40);
			py = 20 + rand() % (board_height - 40);
			
			int gridx = px / cell_size;
			int gridy = py / cell_size;
			
			if (grid[gridx][gridy] == 0) { // Road
				// Find a spot near to a building
				if ((gridx > 0 && grid[gridx-2][gridy] == 1) || 
					(gridx < grid_width-1 && grid[gridx+2][gridy] == 1) || 
					(gridy > 0 && grid[gridx][gridy-2] == 1) ||
					(gridy < grid_height-1 && grid[gridx][gridy+2] == 1)) {
					foundspot = true;
				}
			}
		}
		
		return Position(px, py);
	}

	// Find a random valid position on the road
	Position findRandomRoadPosition() const {
		int px, py;
		do {
			px = 20 + rand() % (board_width - 40);
			py = 20 + rand() % (board_height - 40);
		} while (!isPositionValid(px, py));
		
		return Position(px, py);
	}

};

class Garage : public Entity {
	public: 
	   Garage (int x = 0, int y = 0) : Entity (x, y) {}

	   void render () const override {
		DrawTriangle(p.getX(), p.getY() + 20, p.getX() - 20, p.getY() - 10, p.getX() + 20,
		 p.getY() - 10, colors[GRAY]);
	   }

	   int getPoints() const override {
		return 0;
	}

	   bool isClicked (int mx, int my) {
		return (mx >= p.getX() && mx <= p.getX() + 150 && my >= p.getY() && my <= p.getY() + 100);
	   }
	   

};


class Game {
	private: 
	   string role;
       string playername;
       Board gameBoard;
	   Tree *trees[10];
       Passenger *passengers[no_ofPassengers];
       FuelStations *stations[fuel_stations];
	   DeliveryPackage *packages[delivery_pkg];
	   OtherCar *bots[other_cars];
	   PlayerCar *playerCar;
	   Garage *garage;
	   int score;
	   int wallet;
	   bool isPkgPick;
	   int currentpkgPos;
	   bool dest_Mark;
	   Position dest_Pos;
	   bool isPassPick;
	   int currentPassPos;
	   bool pass_collision[no_ofPassengers];
	   bool tree_collision[10];
	   bool building_collision;
	   bool car_collision[other_cars];
	   int jobCounter;
	   float fuel;
	   float maxFuel;
	   float consumption_rate;
	   int game_time;
	   int start_time;
	   bool game_over;
	   string game_status;

	   struct ScoreEntry {
		char name[50];
		int score;
	   };

	   ScoreEntry LB[10];
	   int no_ofEntries;
	   bool entering_name;
	   bool isLBUpdated;
	   
	public: 
	bool isMenuActive = true;
    bool isGameActive = false;
	bool isLBActive = false;
	
	   Game () : score(0), wallet(0), isPkgPick(false), currentpkgPos(-1), dest_Mark(false),
	   isPassPick(false), currentPassPos(-1), building_collision(false), maxFuel(100), jobCounter(0),
	   consumption_rate(0.25), game_time(180), start_time(0), game_over(false),
	   game_status(""),  no_ofEntries(0), entering_name(false), isLBUpdated(false) {
	  

		for (int i = 0; i < 10; i++) {
			trees[i] = nullptr;
		}

		for (int i = 0; i < no_ofPassengers; i++) {
			passengers[i] = nullptr;
		}

		for (int i = 0; i < fuel_stations; i++) {
			stations[i] = nullptr;
		}

		for (int i = 0; i < delivery_pkg; i++) {
			packages[i] = nullptr;
		}
		playerCar = new PlayerCar (10, 1000);
		garage = new Garage(20,20);
		
		for (int i = 0; i < other_cars; i++) {
			int x = rand() % (board_width-60);
			int y = rand() % (board_height-25);
			bots[i] = new OtherCar(x, y);
		}

		for (int i = 0; i < no_ofPassengers; i++) {
			pass_collision[i] = false;
		}
		for (int i = 0; i < 10; i++) {
			tree_collision[i] = false;
		}
		for (int i = 0; i < other_cars; i++) {
			car_collision[i] = false;
		}
		loadLB();
	   }


	   void destructor () {
		for (int i = 0; i < 10; i++) {
			if (trees[i] != nullptr) {
				delete trees[i];
				trees[i] = nullptr;
			}
		}

		for (int i = 0; i < no_ofPassengers; i++) {
			if (passengers[i] != nullptr) {
				delete passengers[i];
				passengers[i] = nullptr;
			}
		}

		for (int i = 0; i < fuel_stations; i++) {
			if (stations[i] != nullptr) {
				delete stations[i];
				stations[i] = nullptr;
			}
		}

		for (int i = 0; i < delivery_pkg; i++) {
			if (packages[i] != nullptr) {
				delete packages[i];
				packages[i] = nullptr;
			}
		}
		delete playerCar;
		delete garage;

		for (int i = 0; i < other_cars; i++) {
			if (bots[i] != nullptr) {
				delete bots[i];
				bots[i] = nullptr;
			}
		}
	   }

	   void displayMenu () {
		if (isMenuActive) {
		    glClearColor(0.1, 0.1, 0.2, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);

			DrawString(600, 1000, "Rush Hour Game", colors[WHITE]);  //title
	
			//menu
			DrawString(600, 800, "1. Start Game as Taxi Driver", colors[WHITE]);
			DrawString(600, 750, "2. Start Game as Delivery Driver", colors[WHITE]);
			DrawString(600, 700, "3. View Leaderboard", colors[WHITE]);
			DrawString(600, 600, "Press ESC to Exit", colors[WHITE]);


			if (entering_name) {
			DrawRectangle(580, 520, 400, 40, colors[GRAY]);  
			DrawString(590, 530, playername, colors[BLACK]);       
			DrawString(600, 570, "Enter your name:", colors[WHITE]); 
		}
			
	
			if (role == "") {
				DrawString(600, 650, "Choose your role (1 or 2 or 3):", colors[WHITE]);
			}
	
			if (role != "") {
				DrawString(600, 450, "Your Role: " + role, colors[WHITE]);
				DrawString(600, 400, "Press ENTER to start game", colors[WHITE]);
			}
			
		}
	   }

	   void loadLB () {
		ifstream file("highscores.txt", ios::binary);
		if (file) {
			ScoreEntry entry;
			no_ofEntries = 0;
			while (file.read((char*)&entry, sizeof(entry)) && no_ofEntries < 10) {
					LB[no_ofEntries++] = entry;
			}
			file.close();
		}
		else {
			no_ofEntries = 0;
		}
	   }

	   void saveLB() {
		ofstream file("highscores.txt", ios::binary | ios::trunc);
		if (file) {
			for (int i = 0; i < no_ofEntries; ++i) {
				file.write((char*)&LB[i], sizeof(ScoreEntry));
			}
			file.close();
		}
	}

	void sortLB() {
		for (int i = 0; i < no_ofEntries - 1; ++i) {
			for (int j = 0; j < no_ofEntries- i - 1; ++j) {
				if (LB[j].score < LB[j + 1].score) {
					ScoreEntry temp = LB[j];
                    LB[j]    = LB[j+1];
                    LB[j+1]  = temp;

				}
			}
		}
	}

	void checkAndUpdateLB() {
		if (score <= 0 || playername.empty()) {
			return;
		}

		ScoreEntry newEntry;  //make the entry for player name
		int maxCopy = playername.length() < 49 ? playername.length() : 49;
		for (int i = 0; i < maxCopy; ++i) {
			newEntry.name[i] = playername[i];
		}
		newEntry.name[maxCopy] = '\0';
		newEntry.score = score;
	
		bool added = false;
	
		if (no_ofEntries < 10) {
			LB[no_ofEntries] = newEntry;
			++no_ofEntries;
			added = true;
		} 
		else {
			// Replace the 10th record if this score is higher
			if (newEntry.score > LB[9].score) {
				LB[9] = newEntry;
				added = true;
			}
		}
	
		if (added) {
			sortLB();
			saveLB();
		}
	}

	void renderLB() {
		glClearColor(0.1, 0.1, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
		DrawString(600, 800, "LEADERBOARD (TOP 10)", colors[WHITE]);
		
		int yPos = 700;
		for (int i = 0; i < no_ofEntries; ++i) {
			string entry = to_string(i+1) + ". " + LB[i].name + " - " + to_string(LB[i].score);
			DrawString(600, yPos, entry, colors[WHITE]);
			yPos -= 50;
		}
		if (no_ofEntries == 0) {
			DrawString(600, 500, "No scores yet!", colors[WHITE]);
		}
		DrawString(600, 200, "Press ESC to return", colors[WHITE]);
	}


	   void startGame () {
		   if (role == "Taxi Driver" || role == "Delivery Driver") {
			   isMenuActive = false;
			   isGameActive = true;

			   score = 0;
			   wallet = 0;
			   fuel = maxFuel;
			   game_over = false;
			   game_status = "";
			   start_time = 0;

			   gameBoard.InitializeBoard(passengers, stations, packages, trees);			   
		   } 
	   }

	   void PickingUp () {
		Position carPos = playerCar->getPosition();

		if (role == "Delivery Driver") {
			if (!isPkgPick) {
			   for (int i = 0; i < delivery_pkg; i++) {
				  if (packages[i] != nullptr && packages[i]->isVisible()) {
					 Position pkgPos = packages[i]->getPosition();

					 int dx = carPos.getX() - pkgPos.getX(); //check if car is close to pickup
					 int dy = carPos.getY() - pkgPos.getY();
					 double distance = sqrt(dx*dx + dy*dy);

					 if (distance < 100) {
						// Pick up the package
						packages[i]->pickUp();
						isPkgPick = true;
						currentpkgPos = i;
						
						// Show destination circle
						dest_Mark = true;
						dest_Pos = packages[i]->getDest();
						break;
					 }
				  }
			   }
			}

			else {
			   if (currentpkgPos >= 0 && currentpkgPos < delivery_pkg) { //dropping off the package
				  Position destPos = packages[currentpkgPos]->getDest();
				  
				  int dx = carPos.getX() - destPos.getX();
				  int dy = carPos.getY() - destPos.getY();
				  double distance = sqrt(dx*dx + dy*dy);
				  
				  if (distance < 50) {
					Difficulty();
					int pkgIdx = currentpkgPos;

					 packages[currentpkgPos]->dropoff();
					 isPkgPick = false;
					 score += 10;
					 int deliveryFee = packages[currentpkgPos]->getFee();
				     wallet += deliveryFee;
					
					 dest_Mark = false;
					 currentpkgPos = -1;

					 Position packagePos = gameBoard.findValidDeliveryPosition(); //generating new pkg
					 Position destPos = gameBoard.findValidDeliveryPosition();
					 int fee = 30 + rand() % 70; // Random fee between 30 and 100

					 delete packages[pkgIdx];
					 packages[pkgIdx] = new DeliveryPackage(packagePos.getX(), packagePos.getY(), 
					 destPos.getX(), destPos.getY(), fee);
				  }
			   }
			}
		}

		else if (role == "Taxi Driver") {
			if (!isPassPick) {
			   for (int i = 0; i < no_ofPassengers; i++) {
				  if (passengers[i] != nullptr && passengers[i]->isVisible()) {
					 Position PassPos = passengers[i]->getPosition();

					 int dx = carPos.getX() - PassPos.getX();
					 int dy = carPos.getY() - PassPos.getY();
					 double distance = sqrt(dx*dx + dy*dy);

					 if (distance < 100) {
						passengers[i]->pickup();
						isPassPick = true;
						currentPassPos = i;
						
						dest_Mark = true;
						dest_Pos = passengers[i]->getDest();
						break;
					 }
				  }
			   }
			}

			else {
				if (currentPassPos >= 0 && currentPassPos < no_ofPassengers) {
					Position destPos = passengers[currentPassPos]->getDest();
					
					int dx = carPos.getX() - destPos.getX();
					int dy = carPos.getY() - destPos.getY();
					double distance = sqrt(dx*dx + dy*dy);
					
					if (distance < 50) {
						Difficulty();
						int passIdx = currentPassPos;

						passengers[currentPassPos]->dropoff();
						isPassPick = false;
						score += 10;
						int passengerFare = passengers[currentPassPos]->getFare();
					    wallet += passengerFare;
						
						dest_Mark = false;
						currentPassPos = -1;

						Position passengerPos = gameBoard.findRandomRoadPosition();
					    Position destPos = gameBoard.findValidDeliveryPosition();
					    int fare = 50 + rand() % 100; // Random fare between 50 and 150

						delete passengers[passIdx];
					    passengers[passIdx] = new Passenger(passengerPos.getX(), passengerPos.getY(), 
						destPos.getX(), destPos.getY(), fare);
					}
				}
			}
		}	
	}

	void Difficulty() {
		jobCounter++;
		if (jobCounter % 2 == 0) {
			int newSpeed = playerCar->getSpeed() + 1;
			playerCar->setSpeed(newSpeed);
			for (int i = 0; i < other_cars; ++i)
				if (bots[i]) bots[i]->setSpeed(newSpeed);
		}
	}

	void updateFuel() {
		if(playerCar->getSpeed() > 0 && (playerCar->isMoving())) {
			fuel -= consumption_rate;
		}
		
		if(fuel <= 0) {
			fuel = 0;
			game_over = true;
			game_status = "lose";
		}
	}

	void updateTime() {
		if(!game_over) {
			int current_time = glutGet(GLUT_ELAPSED_TIME) / 1000;
			if(start_time == 0) {
				start_time = current_time;
			}

			int elapsed = current_time - start_time;
			int remaining = game_time - elapsed;
			
			if(remaining <= 0) {
				if(score >= 100) {
					game_over = true;
					game_status = "win";
				} 
				else {
					game_over = true;
					game_status = "lose";
				}
			}
		}
	}

	void checkStatus() {
		if(score < 0) {
			game_over = true;
			game_status = "lose";
		}
		if(score >= 100) {
			game_over = true;
			game_status = "win";
		}
	}

	void refill() {
		Position carPos = playerCar->getPosition();
		
		for(int i = 0; i < fuel_stations; i++) {
			if(stations[i] != nullptr) {
				Position stationPos = stations[i]->getPosition();
				
				int dx = carPos.getX() - stationPos.getX();
				int dy = carPos.getY() - stationPos.getY();
				double distance = sqrt(dx*dx + dy*dy);
				
				if(distance < 100) {
					if(wallet >= 100 && fuel < maxFuel) {
						wallet -= 100; 
						fuel = maxFuel;
					}
					break;
				}
			}
		}
	}	

	void PrintableKeys(unsigned char key, int x, int y) {
		if (key == 27) {  // ASCII 27 = ESC
			if (isLBActive) {
				isLBActive = false;
				isMenuActive = true;
			} 
			else {
				destructor();  // clean up
				exit(0);
			}
			glutPostRedisplay();
			return;
		}
	
		if (isMenuActive) {
			if (entering_name) {
				if (key == 13) {  // ENTER
					entering_name = false;
					startGame();  
				}
				else if (key == 8) {  // BACKSPACE
					int L = (int)playername.length();
					if (L > 0) {
						// remove last char manually
						string temp;
						for (int i = 0; i < L - 1; ++i) {
							temp += playername[i];
						}
						playername = temp;
					}
				}
				else {
					bool correct = false;
					if (key >= '0' && key <= '9') correct = true;
					if (key >= 'A' && key <= 'Z') correct = true;
					if (key >= 'a' && key <= 'z') correct = true;
					if (correct && playername.length() < 31) {
						string temp = playername;
						temp += key;
						playername = temp;
					}
				}
			}
			else {
				if (key == '1') {
					role = "Taxi Driver";
					entering_name= true;
					playername = "";  
				}
				else if (key == '2') {
					role = "Delivery Driver";
					entering_name = true;
					playername = "";
				}
				else if (key == '3') {
					loadLB();
					isLBActive = true;
					isMenuActive = false;
				}
			}
		}

		else if (isLBActive) {
			return;
		}
		else if (isGameActive && !game_over) {
			if (key == 32) {  // SPACE
				PickingUp();
			}
			else if (key == 'f' || key == 'F') {
				refill();
			}
		}
		else if (game_over) {
			return;
		}
	
		glutPostRedisplay();
	}
	

	bool isCollision (const Position &p1, const Position &p2, int radius = 20) {
		int car_width = 36;
		int car_height = 20;

		int carLeft = p1.getX();
		int carRight = p1.getX() + car_width;
		int carTop = p1.getY() + car_height;
		int carBottom = p1.getY();

		int centre_x = p2.getX();
		int centre_y = p2.getY();

		int closestX = centre_x;
		int closestY = centre_y;

		if (centre_x < carLeft) {
			closestX = carLeft;
		} 
		else if (centre_x > carRight) {
			closestX = carRight;
		}
		
		if (centre_y < carBottom) {
			closestY = carBottom;
		} 
		else if (centre_y > carTop) {
			closestY = carTop;
		}

		int dx = closestX - centre_x;
		int dy = closestY - centre_y;
		double distance = sqrt(dx*dx + dy*dy);  //calculate distance from closest point to radius

		return distance < radius;
	}

	void CheckingCollision () {
		if (score < 0) {
			cout << "Score is below zero: " << score << ", game_over = " << (game_over ? "true" : "false") << endl;
		}
		Position carPos = playerCar->getPosition();
	if (role == "Taxi Driver") {
		for (int i = 0; i < no_ofPassengers; i++) { //collision with passengers
			if (passengers[i] != nullptr && passengers[i]->isVisible()) {
				Position passPos = passengers[i]->getPosition();
				bool currentCollision = isCollision(carPos, passPos);
				
				if (currentCollision && !pass_collision[i]) { //deduct score once per collision
				   score -= 5;
				   cout << "Hit a passenger! -5 points. Score: " << score << endl;
				   pass_collision[i] = true;
				}
				else if (!currentCollision && pass_collision[i]) { //resetting the state of collision
				   pass_collision[i] = false;
				}
			 }
		}
	}

		for (int i = 0; i < 10; i++) { //tree collision
			if (trees[i] != nullptr && trees[i]->isVisible()) {
			   Position treePos = trees[i]->getPosition();
			   bool currentCollision = isCollision(carPos, treePos);
			   
			   if (currentCollision && !tree_collision[i]) {
				  score -= 2;
				  cout << "Hit a tree! -2 points. Score: " << score << endl;
				  tree_collision[i] = true;
			   }
			   else if (!currentCollision && tree_collision[i]) {
				  tree_collision[i] = false;
			   }
			}
		 }

		bool currentBuildingCol = false;
		int car_width = 36; //collision with buildings
		int car_height = 20;

		int carLeft = carPos.getX();
		int carRight = carPos.getX() + car_width;
		int carTop = carPos.getY() + car_height;
		int carBottom = carPos.getY();

		int leftGridX = carLeft / cell_size;
		int rightGridX = carRight / cell_size;
		int topGridY = carTop / cell_size;
		int bottomGridY = carBottom / cell_size;

		// Check if any occupied grid cell is a building
		for (int gx = leftGridX; gx <= rightGridX && !currentBuildingCol; gx++) {
			for (int gy = bottomGridY; gy <= topGridY && !currentBuildingCol; gy++) {
				if (gx >= 0 && gx < grid_width && gy >= 0 && gy < grid_height) {
					if (gameBoard.isBuilding(gx, gy)) {
						currentBuildingCol = true;
						break;
					}
				}
				if (currentBuildingCol) break;
			}
		}

		if (currentBuildingCol && !building_collision) {
			score -= 2;
			cout << "Hit a building! -2 points. Score: " << score << endl;
			building_collision = true;
		 }
		 else if (!currentBuildingCol && building_collision) {
			building_collision = false;
		 }

		 for (int i = 0; i < other_cars; i++) { //cars collision
			if (bots[i] != nullptr) {
			   Position botPos = bots[i]->getPosition();
			   bool currentCollision = isCollision(carPos, botPos, 5);
			   
			   if (currentCollision && !car_collision[i]) {
				  score -= 3;
				  cout << "Hit another car! -3 points. Score: " << score << endl;
				  car_collision[i] = true;
			   }
			   else if (!currentCollision && car_collision[i]) {
				  car_collision[i] = false;
			   }
			}
		 }
	}


	void gameRender () {
		if (game_over) {
			renderGameOver();
			return;
		}

		if (isGameActive) {
			gameBoard.render();
			garage->render();
				
			playerCar->move();
			playerCar->render();

			DrawString(800, board_height - 30, "Current Role: " + role, colors[BLACK]);
			DrawString(1750, board_height - 30, "Score: " + to_string(score), colors[BLACK]);
			DrawString(1750, board_height - 60, "Wallet: $" + to_string(wallet), colors[BLACK]);

			DrawString(100, board_height - 30, "Fuel: ", colors[BLACK]);
            DrawRectangle(150, board_height - 35, 200, 20, colors[GRAY]);
            DrawRectangle(150, board_height - 35, 200 * (fuel / maxFuel), 20, colors[GREEN]);

			int current_time = glutGet(GLUT_ELAPSED_TIME) / 1000;
            int elapsed = (start_time > 0) ? (current_time - start_time) : 0;
            int remaining = game_time - elapsed;
            if(remaining < 0) {
			   remaining = 0;
			}
        
            int minutes = remaining / 60;
            int seconds = remaining % 60;
            string time_str = to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
            DrawString(400, board_height - 30, "Time: " + time_str, colors[BLACK]);

			if (dest_Mark) {
				DrawCircle(dest_Pos.getX(), dest_Pos.getY(), 15, colors[GREEN]);
			 }

			if (role == "Taxi Driver") {
			for (int i = 0; i < 10; i++) {
				if (trees[i] != nullptr) {
					trees[i]->render();
				}
			}

			for (int i = 0; i < no_ofPassengers; i++) {
				if (passengers[i] != nullptr) {
					passengers[i]->render();
				}
			}

			for (int i = 0; i < fuel_stations; i++) {
				if (stations[i] != nullptr) {
					stations[i]->render();
				}
			}

			for (int i = 0; i < other_cars; i++) {
				bots[i]->move();
				bots[i]->render();
			}
		}
		
		else if (role == "Delivery Driver") {

			for (int i = 0; i < 10; i++) {
				if (trees[i] != nullptr) {
					trees[i]->render();
				}
			}

			for (int i = 0; i < delivery_pkg; i++) {
				if (packages[i] != nullptr) {
					packages[i]->render();
				}
			}

			for (int i = 0; i < fuel_stations; i++) {
				if (stations[i] != nullptr) {
					stations[i]->render();
				}
			}

			for (int i = 0; i < other_cars; i++) {
				bots[i]->move();
				bots[i]->render();
			}
	    }
			
		}

		
	}

	PlayerCar* getPlayerCar() { 
		return playerCar; 
	}

	void moveBotCars() {
		if (isGameActive) {
			for (int i = 0; i < other_cars; i++) {
				if (bots[i] != nullptr) {
					bots[i]->move();
				}
			}
		}
	}

	void switchRole() {
		if (role == "Taxi Driver") {
			role = "Delivery Driver";
		} 
		else {
			role = "Taxi Driver";
		}
		
		// Reset player car position to the top
		playerCar->setPosition(10,1000);
		score = 0;
		wallet = 0;
		start_time = glutGet(GLUT_ELAPSED_TIME) / 1000;
		fuel = 100;
	}
	
	bool isGarageClicked(int mouseX, int mouseY) {
		if (garage != nullptr) {
			return garage->isClicked(mouseX, mouseY);
		}
		return false;
	}
	
	Garage* getGarage() { 
		return garage; 
	}
	
	string getRole() const {
		return role;
	}

	bool isgameover () {
		return game_over;
	}

	void renderGameOver () {
		glClearColor(0.1, 0.1, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

		string message;
		if (game_status == "win") {
			message = "CONGRATULATIONS! YOU WON!";
			DrawString(board_width/2 - 200, board_height/2 + 50, message, colors[GREEN]);
			DrawString(board_width/2 - 200, board_height/2, "Final Score: " + to_string(score), colors[WHITE]);
		} 
		else {
			message = "GAME OVER";
			DrawString(board_width/2 - 100, board_height/2 + 50, message, colors[RED]);
			DrawString(board_width/2 - 200, board_height/2, "Final Score: " + to_string(score), colors[WHITE]);
			
			string reason;
			if (fuel <= 0) {
				reason = "You ran out of fuel!";
			} 
			else if (score < 0) {
				reason = "Your score dropped below zero!";
			} 
			else {
				reason = "Time's up!";
			}
			DrawString(board_width/2 - 200, board_height/2 - 50, reason, colors[WHITE]);
		}
		if (!isLBUpdated) {
			checkAndUpdateLB();
			isLBUpdated = true;
		}
		DrawString(board_width/2 - 200, board_height/2 - 100, "Press ESC to exit", colors[WHITE]);
	}
	
	    
};

Game game;

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
 *
 * You will have to add the necessary code here for shooting, etc.
 *
 * This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
 * x & y that tells the coordinate of current position of move mouse
 *
 * */
void MouseClicked(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // dealing only with left button
			{
				int gameY = board_height- y;
		       if (game.isGameActive) {
				if (game.isGarageClicked(x, gameY)) {
					game.switchRole();
				}
			   }

	} else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
			{
			cout<<"Right Button Pressed"<<endl;

	}
	glutPostRedisplay();
}





int xI = 400, yI = 400;

/*
 * Main Canvas drawing function.
 * */
	void printableKeysWrapper (unsigned char key, int x, int y) {
	game.PrintableKeys(key, x, y);
	}

void GameDisplay()/**/{
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim rid and 1 means pure red and so on.

	glClearColor(0/*Red Component*/, 0,	//148.0/255/*Green Component*/,
			0.0/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear (GL_COLOR_BUFFER_BIT); //Update the colors

	if (game.isLBActive) {
		game.renderLB();
	}
	else if (game.isMenuActive) {
        game.displayMenu();
    } 
	else {
        game.gameRender();
    }
	glutSwapBuffers(); // do not modify this line..

}



/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
 * is pressed from the keyboard
 *
 * You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
 *
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 *
 * */

 void NonPrintableKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		game.getPlayerCar()->handleInput(GLUT_KEY_LEFT);
	}
	else if (key == GLUT_KEY_RIGHT) {
		game.getPlayerCar()->handleInput(GLUT_KEY_RIGHT);
	}
	else if (key == GLUT_KEY_UP) {
		game.getPlayerCar()->handleInput(GLUT_KEY_UP);
	}
	else if (key == GLUT_KEY_DOWN) {
		game.getPlayerCar()->handleInput(GLUT_KEY_DOWN);
	}


	glutPostRedisplay();
}


void SpecialKeyUp(int key, int x, int y) {
		// Stop the car when the key is released
		game.getPlayerCar()->stopCar(key);
		glutPostRedisplay();
	}


/*
 * This function is called after every 1000.0/FPS milliseconds
 * (FPS is defined on in the beginning).
 * You can use this function to animate objects and control the
 * speed of different moving objects by varying the constant FPS.
 *
 * */
void Timer(int m) {

	// implement your functionality here
	if (game.isGameActive && !game.isgameover()) {
	for (int i = 0; i < other_cars; i++) {
		game.moveBotCars();
	}

	game.CheckingCollision();
	game.updateFuel();
	game.updateTime();
	game.checkStatus();
    }

	glutPostRedisplay();

	// once again we tell the library to call our Timer function after next 1000/FPS
	glutTimerFunc(100, Timer, 0);
}

/*This function is called (automatically) whenever your mouse moves witin inside the game window
 *
 * You will have to add the necessary code here for finding the direction of shooting
 *
 * This function has two arguments: x & y that tells the coordinate of current position of move mouse
 *
 * */
void MousePressedAndMoved(int x, int y) {
	// cout << x << " " << y << endl;
	// glutPostRedisplay();
}
void MouseMoved(int x, int y) {
	// //cout << x << " " << y << endl;
	// glutPostRedisplay();
}



/*
 * our gateway main function
 * */
int main(int argc, char*argv[]) {

	int width = 1920, height = 1080; // i have set my window size to be 800 x 600

	InitRandomizer(); // seed the random number generator...
	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("OOP Project"); // set the title of our game window
	SetCanvasSize(width, height); // set the number of pixels...

	srand((unsigned)time(NULL));

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	//glutDisplayFunc(display); // tell library which function to call for drawing Canvas.

	glutDisplayFunc(GameDisplay); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutSpecialUpFunc(SpecialKeyUp);
	glutKeyboardFunc(printableKeysWrapper); // tell library which function to call for printable ASCII characters
	// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0, Timer, 0);

	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse
	glutMotionFunc(MousePressedAndMoved); // Mouse

	// now handle the control to library and it will call our registered functions when
	// it deems necessary...
	glutMainLoop();
	return 1;
}
#endif /* RushHour_CPP_ */
