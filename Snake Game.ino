// Authors : Dhruvin Savla, Jaydatt Dholakiya
// Dt : 30 - 10 - 2023

// Importing Libraries for Led Matrix and LCD display
#include <LedControl.h>
#include <LiquidCrystal.h>

// Defining Class for Generation of Apple
class Apple {
public:
  int x;
  int y;
};

// Creating a Class for the Nodes of Linked List
class Node {
public:
  int x, y;
  Node *next;
  Node *prev;
  Node() {
    x = y = 0;
    next = NULL;
    prev = NULL;
  }
  Node(int x, int y) {
    this->x = x;
    this->y = y;
    this->next = NULL;
    this->prev = NULL;
  }
};
// Creating a Doubly LinkedList --> Snake
class Linkedlist {
public:
  Node *head;
  Node *tail;
  Linkedlist() {
    head = NULL;
    tail = NULL;
  }
  void add(int, int);
  void addlast(int, int);
  void remove();
};

// To add a Node in the list from front
void Linkedlist::add(int x, int y) {
  Node *new_node = new Node(x, y);
  new_node->next = head;
  new_node->prev = NULL;

  if (head != NULL) {
    head->prev = new_node;
    head = new_node;
  } else {
    head = new_node;
    tail = new_node;
  }
}

// To add a Node in the list from last
void Linkedlist::addlast(int x, int y) {
  Node *new_node = new Node(x, y);
  new_node->next = NULL;
  new_node->prev = tail;
  tail->next = new_node;
  tail = tail->next;
}

//To remove a Node
void Linkedlist::remove() {
  tail = tail->prev;
  Node *tmp = tail->next;
  tail->next = tail->next->x = tail->next->y = tail->next->next = NULL;
  free(tmp);
}

//Defining Pins
int x_pin = A3;
int y_pin = A1;
int DIN = 12;
int CS = 10;
int CLK = 11;
int sw = 2;

int start = 0;
int direction = 2;
int score = 0;

LedControl lc = LedControl(DIN, CLK, CS, 1);

LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

// Creating the list
Linkedlist *Snake = new Linkedlist();  

//Creating Apple Object
Apple *apple = new Apple();

void setup() {
  // Initial message on the lcd display
  lcd.begin(16, 2);
  lcd.print("Press to start");

  // Randomly Generating the Apple
  apple->x = (int)random(0, 8);
  apple->y = (int)random(0, 8);
  lc.setLed(0, apple->x, apple->y, true);

  // Initial Snake
  Snake->add(0, 0);
  Snake->add(0, 1);
  lc.setLed(0, 0, 0, true);
  lc.setLed(0, 0, 1, true);

  // Input from the Joystick
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);
  pinMode(sw, INPUT);
  digitalWrite(sw, HIGH);

  // Matrix Setup
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
}

void loop() {
restart:                                 // Redirect here when game over to Start Again
  start = max(start, !digitalRead(sw));  // Start of the Game
  if (start) {
    if (collision()) {
      lcd.print("Game Over!");
      lcd.setCursor(0, 1);
      lcd.print("Final Score :");
      lcd.setCursor(13, 1);
      lcd.print(score);
      byte sadFace[8] = { B11111111, B10000001, B10100101, B10001001, B10001001, B10100101, B10000001, B11111111 };
      for (int i = 0; i < 8; i++) {
        lc.setRow(0, i, sadFace[i]);
      }
      delay(5000);
      lc.clearDisplay(0);
      lcd.clear();
      lcd.print("Press to start");
      reset();
      score = 0;
      start = 0;
      goto restart;
    }

    eatAppleChecker();

    lcd.print("Score : ");
    lcd.setCursor(7, 0);
    lcd.print(score);
    lc.setLed(0, apple->x, apple->y, true);

    // Displaying the snake
    Node *body = Snake->head;
    while (body != NULL) {
      lc.setLed(0, body->x, body->y, true);
      body = body->next;
    }

    joystick();

    delay(250);
    lc.clearDisplay(0);
    lcd.clear();
  }
}

// If Ate the Apple Increase Size and Score
void eatAppleChecker() {
  if ((Snake->head->x == apple->x) && (Snake->head->y == apple->y)) {
    lc.setLed(0, apple->x, apple->y, false);
    apple->x = (int)random(0, 8);
    apple->y = (int)random(0, 8);
    Snake->addlast(Snake->tail->x, Snake->tail->y);
    score++;
  }
}

// Detection of Collision
bool collision() {
  // Traverse the List and Check whether next node of Head is equal to the body
  Node *tmp = Snake->head->next;
  int chk_x = Snake->head->x, chk_y = Snake->head->y;
  while (tmp != NULL) {
    if (tmp->x == chk_x && tmp->y == chk_y) {
      return 1;
    }
    tmp = tmp->next;
  }
  return 0;
}
// If collision occurs reset the game
void reset() {
  //Clear the list
  while (Snake->head != NULL) {
    Node *tmp = Snake->head->next;
    Node *clr = Snake->head;
    Snake->head->x = Snake->head->y = Snake->head->prev = NULL;
    Snake->head = tmp;
    free(clr);  // Clear the Memory
  }
  Snake->tail = NULL;
  // Reset to Default Settings
  direction = 2;

  apple->x = (int)rand() % 8;
  apple->y = (int)rand() % 8;
  lc.setLed(0, apple->x, apple->y, true);

  Snake->add(0, 0);
  Snake->add(0, 1);
  lc.setLed(0, 0, 0, true);
  lc.setLed(0, 0, 1, true);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
}

// Controlling the Joystick
void joystick() {
  int x_data = analogRead(x_pin);
  int y_data = analogRead(y_pin);

  if (x_data <= 200) {
    if (direction != 1) {
      direction = 2;
    }
  } else if (x_data >= 800) {
    if (direction != 2) {
      direction = 1;
    }
  } else if (y_data <= 200) {
    if (direction != 4) {
      direction = 3;
    }
  } else if (y_data >= 800) {
    if (direction != 3) {
      direction = 4;
    }
  }
  chooseDirection(direction);
}

void chooseDirection(int data) {
  if (data == 1) {
    moveUp();
  } else if (data == 2) {
    moveDown();
  } else if (data == 3) {
    moveLeft();
  } else {
    moveRight();
  }
}

// Snake Movement Functions
void moveDown() {
  int x = Snake->head->x;
  int y = Snake->head->y + 1;
  y %= 8;  // To avoid the points to go out of matrix
  Snake->add(x, y);
  Snake->remove();
}

void moveLeft() {
  int x = Snake->head->x + 1;
  int y = Snake->head->y;
  x %= 8;
  Snake->add(x, y);
  Snake->remove();
}

void moveRight() {
  int x = Snake->head->x + 7;
  int y = Snake->head->y;
  x %= 8;
  Snake->add(x, y);
  Snake->remove();
}

void moveUp() {
  int x = Snake->head->x;
  int y = Snake->head->y + 7;
  y %= 8;
  Snake->add(x, y);
  Snake->remove();
}