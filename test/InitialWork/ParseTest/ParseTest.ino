/**
  @file example.c
  @author James Gregson (james.gregson@gmail.com)
  @brief example usage of the expression parser, see expression_parser.h for license details. demonstrates using the variable and function callbacks, as well as direct initialization of the data structures and customized error handling.
*/
#include<stdio.h>
#include<string.h>
#include"expression_parser.h"

#include <SPI.h>
#include "RF24.h"

#define NUM_WAVES 40
double all_args[NUM_WAVES][3];
float x = 0.0;
float y = 0.0;
float drum_loc[4][2] = {{1.0, 3.0}, {3.4, 5.2}, {100.3, 54.3}, {14.5, 60.3}}; // Dummy variables to simulate calculation
float displacement_contrib[NUM_WAVES];
int current_wave;
long timer;

/**
  @brief user-defined variable callback function. see expression_parser.h for more details.
  @param[in] user_data pointer to any user-defined state data that is required, none in this case
  @param[in] name name of the variable to look up the value of
  @param[out] value output point to double that holds the variable value on completion
  @return true if the variable exists and the output value was set, false otherwise
*/
int variable_callback( void *user_data, const char *name, double *value ) {
  // look up the variables by name
  bool set = false;
  if ( strcmp( name, "ws" ) == 0 ) {
    // set return value, return true
    *value = all_args[current_wave][1];
    set = true;
  } else if ( strcmp( name, "wd" ) == 0 ) {
    // set return value, return true
    *value = get_dist_from_wave(get_dist_from_src(current_wave), current_wave);
    set = true;
  } else if ( strcmp( name, "dt" ) == 0 ) {
    // set return value, return true
    *value = get_delta_t(current_wave);
    set = true;
  } else if ( strcmp( name, "wl" ) == 0 ) {
    // set return value, return true
    *value = 3.0;
    set = true;
  } else if ( strcmp( name, "e" ) == 0 ) {
    *value = 2.71;
    set = true;
  } else if ( strcmp( name, "d" ) == 0 ) {
    *value = 0.5;
    set = true;
  }
  else if ( strcmp( name, "var0" ) == 0 ) {
    *value = 0.0;
    set = true;
  }
  else if ( strcmp( name, "var1" ) == 0 ) {
    *value = 1.0;
    set = true;
  }
  else if ( strcmp( name, "var2" ) == 0 ) {
    *value = 2.0;
    set = true;
  }
  else if ( strcmp( name, "var3" ) == 0 ) {
    *value = 3.0;
    set = true;
  }

  // failed to find variable, return false
  if (set) return PARSER_TRUE;
  return PARSER_FALSE;
}

/**
  @brief user-defined function callback. see expression_parser.h for more details.
  @param[in] user_data input pointer to any user-defined state variables needed.  in this case, this pointer is the maximum number of arguments allowed to the functions (as a contrived example usage).
  @param[in] name name of the function to evaluate
  @param[in] num_args number of arguments that were parsed in the function call
  @param[in] args list of parsed arguments
  @param[out] value output evaluated result of the function call
  @return true if the function exists and was evaluated successfully with the result stored in value, false otherwise.
*/
int function_callback( void *user_data, const char *name, const int num_args, const double *args, double *value ) {
  int i, max_args;
  double tmp;
  bool set = false;

  // example to show the user-data parameter, sets the maximum number of
  // arguments allowed for the following functions from the user-data function
  max_args = *((int*)user_data);

  if ( strcmp( name, "max_value") == 0 && num_args >= 2 && num_args <= max_args ) {
    // example 'maximum' function, returns the largest of the arguments, this and
    // the min_value function implementation below allow arbitrary number of arguments
    tmp = args[0];
    for ( i = 1; i < num_args; i++ ) {
      tmp = args[i] >= tmp ? args[i] : tmp;
    }
    // set return value and return true
    *value = tmp;
    set = true;
  } else if ( strcmp( name, "min_value" ) == 0 && num_args >= 2 && num_args <= max_args ) {
    // example 'minimum' function, returns the smallest of the arguments
    tmp = args[0];
    for ( i = 1; i < num_args; i++ ) {
      tmp = args[i] <= tmp ? args[i] : tmp;
    }
    // set return value and return true
    *value = tmp;
    set = true;
  }
  // failed to evaluate function, return false
  if (set) return PARSER_TRUE;
  return PARSER_FALSE;
}

void init_args(double args[][3]) {
  for (int i = 0; i < NUM_WAVES; i++) {
    double analogrd = (double) analogRead(A0);
    if (analogrd < 200) args[i][0] = 0;
    else if (analogrd < 400) args[i][1] = 1;
    else if (analogrd < 700) args[i][1] = 2;
    else args[i][1] = 3;
    args[i][1] = analogrd;
    args[i][2] = millis();
  }
}


long get_delta_t(int wave_num) {
  return millis() - (long) all_args[wave_num][2];
}

float get_dist_from_src(int wave_num) {
  int drum = (int) all_args[wave_num][0];
  return sqrt(sq(x - drum_loc[drum][0]) + sq(y - drum_loc[drum][1]));
}

float get_dist_from_wave(float dist, int wave_num) {
  return abs(dist - all_args[wave_num][2]);
}

void setup() {
  double value;
  int num_arguments = 6;
  //  const char *expr0 = "e^2";
  const char *expr0 = "ws*e^(-d*wd*dt^3)*(wl*dt-wd)";
  //  const char *expr3 = "5.0*( max_value( var0, max_value( var1, var2 ) )/2 + min_value( var1, var2, var3 )/2 )";
  parser_data pd;

  Serial.begin(115200);
  pinMode(A0, INPUT);
  init_args(all_args);

  Serial.println("READY");

  // Test for the sscanf function, which does not work for floats on Arduino.
  //  char *s = " 8.649292 " ;
  //  double d = atof(s);
  //  sscanf( s, "f %lf ", &d ) ;
  //  Serial.println(d);

  // should succeed, and print results. the success or failure of the expression parser
  // can be tested by testing the return value for equality with itself, which always
  // fails for nan (Not a Number), provided strict floating point behaviour is followed
  // by the compiler.

  //  parser_data_init( &pd, expr0, variable_callback, function_callback, &num_arguments );
  //  //    value = parse_expression_with_callbacks( expr0, variable_callback, function_callback, &num_arguments );
  //  value = parser_parse(&pd);
  //  if ( value == value ) {
  //    Serial.print( expr0); Serial.print(" = "); Serial.print( value ); Serial.println("\n\n");
  //  }
  //  else Serial.println( pd.error );

  long start = millis();
  for (int i = 0; i < NUM_WAVES; i++) {
    parser_data_init( &pd, expr0, variable_callback, function_callback, &num_arguments );
    //  value = parse_expression_with_callbacks( expr0, variable_callback, function_callback, &num_arguments );
    value = parser_parse(&pd);
    current_wave++;
  }
  Serial.print(F("Total time taken: ")); Serial.println(millis() - start);

  start = millis();
  current_wave = 0;
  for (int i = 0; i < NUM_WAVES; i++) {
    test_raw_c();    
    current_wave++;
  }
  Serial.print(F("Total time taken for raw function: ")); Serial.println(millis() - start);
}

void loop() {
}

void test_raw_c() {
  //"ws*e^(-d*wd*dt^3)*(wl*dt-wd)"
  double ws = all_args[current_wave][1];
  double wd = get_dist_from_wave(get_dist_from_src(current_wave), current_wave);
  double dt = get_delta_t(current_wave);
  double wl = 3.0;
  double e = 2.71;
  double d = 0.5;
  double meow = ws*pow(e, (-d*wd*pow(dt, 3)))*(wl*dt-wd);
}

