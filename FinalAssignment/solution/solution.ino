#include "funshield.h"
constexpr int button_pins[3] = { button1_pin, button2_pin, button3_pin }; 
constexpr byte EMPTY_GLYPH = 0b11111111;
constexpr int number_of_muxpos = sizeof( digit_muxpos ) / sizeof( digit_muxpos[0] );
constexpr int  pins [3] = { latch_pin, clock_pin, data_pin };
constexpr int number_of_pins = sizeof( pins ) / sizeof( pins[0] );

constexpr byte LETTER_GLYPH[] {
  0b10001000,   // A
  0b10000011,   // b
  0b11000110,   // C
  0b10100001,   // dx
  0b10000110,   // E
  0b10001110,   // F
  0b10000010,   // G
  0b10001001,   // H
  0b11111001,   // I
  0b11100001,   // J
  0b10000101,   // K
  0b11000111,   // L
  0b11001000,   // M
  0b10101011,   // n
  0b10100011,   // o
  0b10001100,   // P
  0b10011000,   // q
  0b10101111,   // r
  0b10010010,   // S
  0b10000111,   // t
  0b11000001,   // U
  0b11100011,   // v
  0b10000001,   // W
  0b10110110,   // ksi
  0b10010001,   // Y
  0b10100100,   // Z
};

void set_OUTPUT_pins() {
  for ( int i = 0; i < number_of_pins; i++ ) { 
    pinMode( pins[i], OUTPUT );
  }
}

class Display{
  private:
    char *text; //text [number_of_muxpos];
    int displayed_pos;

    void displayChar( char ch, byte pos ) {
      byte glyph = EMPTY_GLYPH;
      if (isAlpha(ch)) {
        glyph = LETTER_GLYPH[ ch - (isUpperCase(ch) ? 'A' : 'a') ];
      }
      else {
        int int_digit = ch - '0';
        if ( (int_digit >= 0 ) && (int_digit <= 9) ){
          glyph = digits[int_digit];
        }
      }
      digitalWrite(latch_pin, LOW);
      shiftOut(data_pin, clock_pin, MSBFIRST, glyph);
      shiftOut(data_pin, clock_pin, MSBFIRST, 1 << pos);
      digitalWrite(latch_pin, HIGH);
    }

    void pos_aritmetic() {
      displayed_pos  = ( displayed_pos+number_of_muxpos-1 ) % number_of_muxpos; 
    }
  public:
    void set( const char* message, int how_many_to_display ) {
      text = message;
      //for( int i = 0; ( i < how_many_to_display ) &&  ( i < number_of_muxpos ); i++ ) {
        //text[i] = message[i];
      //}
    }
    void print_text(){
      for ( int i = 0; i < number_of_muxpos; i++ ){
        Serial.println(text[i]);
      }
    }
    void show_text() {
      displayChar( text[displayed_pos], displayed_pos );
      pos_aritmetic();
    }
};

class Generator {
  private:
    int throws_modulo = 10;
    int sides_of_poly[7] = { 4, 6, 8, 10, 12, 20, 100 }; 
    int choices_of_sides = sizeof( sides_of_poly ) / sizeof( sides_of_poly[0] );
    int index_poly;
    int number_of_throws = 1;
    char special_char = 'd';
    bool normal_mode = true;
  public:
    int get_sides(){
      return sides_of_poly[index_poly];
    }
    char get_special_char(){
      return special_char;
    }
    int get_throws(){
      return number_of_throws;
    }
    void increase_sides(){
      if ( !normal_mode ){
        index_poly = index_poly % ( choices_of_sides-1 ) + 1;
      }
      Serial.println( sides_of_poly[index_poly] );
    }
    void increase_throws(){
      if ( !normal_mode ){
        number_of_throws = (number_of_throws % ( throws_modulo - 1 ) ) + 1;
      }
      Serial.println( number_of_throws );
    }
    void switch_to_normal(){
      normal_mode = true;
    }
    void switch_to_config(){
      normal_mode = false;
    }

};

class Button {
  private:
    unsigned long previousPress;
    bool previous_state;
  public:
    int pin;
    bool is_held_down(){
      bool current_state = digitalRead( pin );
      if( current_state == false){
        Serial.println("som dole");
      }
    }
    bool was_pressed() {
      bool current_state = digitalRead( pin );
      if( current_state != previous_state ) {
        previous_state = current_state;
        if( !current_state ) {
          return true;
        }
      }
      return false;
    }
  };

Button buttons_array[3];
constexpr int number_of_buttons = sizeof( buttons_array ) / sizeof( buttons_array[0] );
Display display_instance;
Generator generator_instance;

void check_1_button(){
  if( buttons_array[0].was_pressed() ){
    generator_instance.switch_to_normal();
  }
  if( buttons_array[0].is_held_down() ){
    ;
  }
}

void check_2_button(){
  if(buttons_array[1].was_pressed()){
   generator_instance.increase_throws();
   generator_instance.switch_to_config();
  }
}

void check_3_button(){
  if(buttons_array[2].was_pressed()){
    generator_instance.increase_sides();
    generator_instance.switch_to_config();
  }
}
int get_number_length(int number){
  int length = 0;
  while ( number != 0){
    number /= 10;
    length++;
  }
  return length;
}

void fill_setter( int what_to_set[3], char *setter ){
  int setter_index;
  for(int i = 0; i < 3; i++){
    if( i != 1 ) { //ak sa nejedna o special sign 
      int num = what_to_set[i];
      int index_in_number;
      int number_length = get_number_length(num);
      while( index_in_number < number_length){
        int digit_to_set = what_to_set[i] / ; 
        setter[setter_index] = ;
        index_in_number++;
        setter_index++;
      }
    }
    else{
      setter[setter_index] = what_to_set[i];
    }
    setter_index++;

   
  }
  
}

void setup() {
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  Serial.begin(9600);
  for( int i = 0; i < number_of_buttons; i++ ) {
    buttons_array[i].pin = button_pins[i];
    pinMode( buttons_array[i].pin, INPUT );
  }
  set_OUTPUT_pins();  
}

void loop() {
  check_1_button();
  check_2_button();
  check_3_button();
  //char setter[number_of_muxpos];
  //fill_setter(generator_instance., second, third);
  char setter[number_of_muxpos];
  int what_to_set[3] = {generator_instance.get_throws(), generator_instance.get_special_char(), generator_instance.get_sides() };
  fill_setter( what_to_set ,setter );
  display_instance.set(setter, number_of_muxpos);
  display_instance.show_text();
  //clear_setter(setter);
}
