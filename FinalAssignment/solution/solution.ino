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
    char text[number_of_muxpos]; 
    int displayed_pos;
    const char special_char = 'd';
    void displayChar( char ch) {
      byte glyph = EMPTY_GLYPH;
      if ( isAlpha( ch ) ) { //checkujem ci char ktory chcem zobrazit je pismeno abecedy
        glyph = LETTER_GLYPH[ ch - ( isUpperCase(ch) ? 'A' : 'a' ) ];
      }
      else {  //ak nie chcem zobrazovat cislicu medzi 1 a 9
        int int_digit = ch - '0';
        if ( ( int_digit >= 0 ) && ( int_digit <= 9 ) ){
          glyph = digits[int_digit];
        }
      }
      digitalWrite( latch_pin, LOW );
      shiftOut( data_pin, clock_pin, MSBFIRST, glyph );
      shiftOut( data_pin, clock_pin, MSBFIRST, 1 << displayed_pos );
      digitalWrite( latch_pin, HIGH );
    }
    void pos_aritmetic() {
      displayed_pos  = ( displayed_pos + 1 ) % number_of_muxpos;
    }
  public:
    void set( const char* setter ) {
      int i = 0;
      while( i < number_of_muxpos ){
        text[i] = setter[i];
        i++;
      }
    }
    void show_text() {
      displayChar( text[displayed_pos]);
      pos_aritmetic();
    }
    char get_special_symbol(){
      return special_char;
    }
};

class Generator {
  private:
    const int throws_modulo = 10;
    const int sides_of_poly[7] = { 4, 6, 8, 10, 12, 20, 100 }; 
    const int choices_of_sides = sizeof( sides_of_poly ) / sizeof( sides_of_poly[0] );
    int index_poly;
    int number_of_throws = 1;
    int result;
    bool normal_mode = false;
  public:
    void generate_result() {
      result = 0;
      for(int i = 0; i < number_of_throws; i++) {
        result += random( 1, sides_of_poly[index_poly] + 1 );
      }
    }
    bool is_normal_mode() {
      return normal_mode;
    }
    int get_result() {
      return result;
    }
    int get_sides(){
      return sides_of_poly[index_poly];
    }

    int get_throws(){
      return number_of_throws;
    }
    void increase_sides(){
      if ( !normal_mode ){  //pridavat chcem len ak je v konfiguracnom mode
        index_poly = ( index_poly + 1 ) % choices_of_sides;
      }
    }
    void increase_throws(){
      if ( !normal_mode ){
        number_of_throws = ( number_of_throws % ( throws_modulo - 1 ) ) + 1;
      }
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
    bool is_held_down() {
      bool current_state = digitalRead( pin );
      if( current_state == false ){
        return true;
      }
      return false;
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

int get_number_length(int number){
  int length = 0;
  while ( number != 0){
    number /= 10;
    length++;
  }
  return length;
}
int get_digit( int number, int pos ){
  return number / pos;
}
int raise_ten( int lenght ){
  int i = 0;
  int raised = 10;
  while( i < lenght){
    raised = raised * 10;
  }
}
void fill_setter_result( int result, char* setter ){
  int length = get_number_length( result );
  Serial.println(length);
  int i = 0;
  int pos = raise_ten( length );
  while( i < length ){
    pos = pos / 10;
    int digit = get_digit( result, pos );
    result = result - (digit * pos);
    char chr_res = result;
    setter[i] = chr_res - '0';
    i++;
  }
}
void fill_setter_config( int throws, int sides, char* setter ){
  ;
}

void check_1_button(){
  if( buttons_array[0].was_pressed() ) {
    generator_instance.switch_to_normal();
  }
  if( buttons_array[0].is_held_down() ) {
    char setter[number_of_muxpos];
    generator_instance.generate_result();
    Serial.println("dole");
    fill_setter_result( generator_instance.get_result(), setter);
    display_instance.set( setter );
    // generujem a prenastavujem display
  }
}

void check_2_button(){
  if( buttons_array[1].was_pressed() ) {
    char setter[number_of_muxpos];
    generator_instance.increase_throws();
    generator_instance.switch_to_config();
    fill_setter_config( generator_instance.get_throws(), generator_instance.get_sides(), setter );
    display_instance.set( setter );
   // nastavim display na config hodnoty
  }
}

void check_3_button(){
  if( buttons_array[2].was_pressed() ) {
    char setter[number_of_muxpos];
    generator_instance.increase_sides();
    generator_instance.switch_to_config();
    fill_setter_config( generator_instance.get_throws(), generator_instance.get_sides(), setter );
    display_instance.set( setter );
    // nastavim display na config hodnoty
  }
}



void setup() {
  Serial.begin(9600);
  pinMode( latch_pin, OUTPUT );
  pinMode( clock_pin, OUTPUT );
  pinMode( data_pin, OUTPUT );
  for( int i = 0; i < number_of_buttons; i++ ) {
    buttons_array[i].pin = button_pins[i];
    pinMode( buttons_array[i].pin, INPUT );
  }
  set_OUTPUT_pins();  
  char setter[number_of_muxpos];
  fill_setter_config( generator_instance.get_throws() , generator_instance.get_throws(), setter);
  display_instance.set(setter);
}

void loop() {
  check_1_button();
  check_2_button();
  check_3_button();
  display_instance.show_text();
}
