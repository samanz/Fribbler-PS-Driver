/**
 * program1.cpp (19jan09/sklar+cummins)
 *
 *
 *
 **/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

#include <termios.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define debugging_connection 0
#define debugging 0

int errno;

const int PACKET_LEN = 9;
const int MAX_TRIES = 20;
const int SET_MOTORS_OFF = 108;
const int SET_MOTORS_ON  = 109;
const int SET_SPEAKER    = 113;


// sensors
#define SENSOR_LEFT_IR      0
#define SENSOR_RIGHT_IR     1
#define SENSOR_LEFT_LIGHT   2
#define SENSOR_CENTER_LIGHT 3
#define SENSOR_RIGHT_LIGHT  4
#define SENSOR_LEFT_LINE    5
#define SENSOR_RIGHT_LINE   6
#define SENSOR_STALL        7
#define NUM_SENSORS         8

unsigned char   mLeftIR;
unsigned char   mRightIR;
unsigned int    mLeftLight;
unsigned int    mCenterLight;
unsigned int    mRightLight;
unsigned char   mLineLeft;
unsigned char   mLineRight;
unsigned char   mStall;

int mInSync;



/**
 * initRobot()
 *
 * this function opens a serial connection to the robot.  the port
 * name for the robot is contained in the argument string "portname".
 * if the connection is opened successfully, then the function returns
 * the file descriptor for the serial port connection. 
 * otherwise, the function throws an exception.
 *
 */
int initRobot( char *portname ) {
  int robot;
  struct termios attr;
  // open serial connection to robot
  if (( robot = open( portname, O_RDWR | O_NOCTTY | O_NONBLOCK )) < 0 ) {
    fprintf( stderr, "\n**error> unable to open serial port '%s': %s\n", portname, strerror( errno ));
    throw errno;
  }
  // get default attributes set on open serial connection
  if ( tcgetattr( robot, &attr ) < 0 ) {
    fprintf( stderr, "\n**error> call to tcgetattr failed: %s\n", strerror( errno ));
    throw errno;
  }
  // set up attribute values for scribbler
  attr.c_cflag |= ( CLOCAL | CREAD );
  attr.c_lflag &= ~( ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN );
  attr.c_oflag &= ~( OPOST );
  // input speed
  attr.c_iflag &= ~( INLCR | IGNCR | ICRNL | IGNBRK );
  if ( cfsetispeed( &attr, B38400 ) != 0 ) {
    fprintf( stderr, "\n**error> unable to cfsetispeed: %s\n", strerror( errno ));
    throw errno;
  }
  // output speed
  if ( cfsetospeed( &attr, B38400 ) != 0 ) {
    fprintf( stderr, "\n**error> unable to cfsetospeed: %s\n", strerror( errno ));
    throw errno;
  }
  // remaining attributes
  attr.c_cflag &= ~( CSIZE );
  attr.c_cflag |= CS8;
  attr.c_cflag &= ~( CSTOPB );
  attr.c_iflag &= ~( INPCK | ISTRIP );
  attr.c_cflag &= ~( PARENB | PARODD );
  attr.c_iflag &= ~( IXON | IXOFF | IXANY );
  attr.c_cflag &= ~( CRTSCTS );
  attr.c_cc[VMIN] = 0;
  attr.c_cc[VTIME] = 1;
  if ( tcsetattr( robot, /*TCSANOW*/ TCSAFLUSH, &attr ) < 0 ) {
    fprintf( stderr, "\n**error> call to tcsetattr failed: %s\n", strerror( errno ));
    throw errno;
  }
  // make it blocking...
  fcntl( robot, F_SETFL, fcntl( robot, F_GETFL, 0) & ~O_NONBLOCK );
#if debugging_connection
  printf( "\n\ndisplaying attributes that we just set:\n" );
  if ( tcgetattr( robot, &attr ) < 0 ) {
    fprintf( stderr, "call to tcgetattr failed: %s\n", strerror( errno ));
  }
  else {
    printf( "got attributes again a-okay\n" );
    printf( "   c_iflag=%ld\n", attr.c_iflag );
    printf( "   c_oflag=%ld\n", attr.c_oflag );
    printf( "   c_cflag=%ld\n", attr.c_cflag );
    printf( "   c_lflag=%ld\n", attr.c_lflag );
    printf( "   c_ispeed=%ld\n", attr.c_ispeed );
    printf( "   c_ospeed=%ld\n", attr.c_ospeed );
    printf( "   c_cc=[" );
    for ( int i=0; i<NCCS; i++ ) {
      printf( " 0x%x", attr.c_cc[i], attr.c_cc[i] );
    }
    printf( "]\n" );
  }
#endif
  mInSync = 0;
  return robot;
} // end of initRobot()



/**
 * getChar()
 *
 * this function reads a single character from the robot. if there is
 * a problem reading, then it tries again, up to MAX_TRIES times.
 * the function returns the character read, or throws an exception if
 * no character is read after trying MAX_TRIES times.
 *
 */
unsigned char getChar( int robot ) {
  bool unread=true;
  unsigned char ch;
  int numtries=0;
  while ( unread ) {
    if ( read( robot, &ch, 1 ) != 1 ) {
      if ( numtries < MAX_TRIES ) {
	numtries++;
      }
      else {
	throw( errno );
      }
    }
    else {
      unread = false;
    }
  }
#if debugging
  printf( "<-- 0x%x\n", ch);
#endif
  return ch;
} // end of getChar()



/**
 * putChar()
 *
 * this function outputs a character to the robot.  if there is an
 * error writing to the robot, then the function throws an exception
 * and returns false.  otherwise, the function returns true.
 *
 */
bool putChar( int robot, unsigned char ch ) {
  if ( write( robot, &ch, 1 ) != 1 ) {
    throw( errno );
  }
  return true;
} // end of putChar()



/**
 * getAck()
 *
 */
void getAck( int robot, unsigned char data[], int datalen ) {
  unsigned char ch;
  try {
    ch = getChar( robot );
    if ( ch != data[0] ) {
#if debugging
      fprintf( stderr, "**error> bad ack: read [0x%x] but wanted [0x%x]\n", ch, data[0] );
#endif
    }
    for ( int i=1; i<datalen; i++ ) {
      try {
	ch = getChar( robot );
	/* note that fluke messes with SET_MOTORS_ON... */
	if (( ch != data[i] ) && ( data[0] != SET_MOTORS_ON )) {
#if debugging
	  fprintf( stderr, "**error> bad ack byte %d: read [0x%x] but wanted [0x%x]\n", i, ch, data[i] );
#endif
	}
      }
      catch ( int err ) {
	fprintf( stderr, "**error> reading ack byte=%d: %s\n", i, strerror( err ));
	throw( err );
      }
      mInSync = 1;
    }
  }
  catch ( int err ) {
    fprintf( stderr, "**error> reading ack: %s\n", strerror( err ));
    throw( err );
  }
} // end of getAck()




/**
 * sendData()
 *
 */
void sendData( int robot, unsigned char data[], int datalen ) {
#if debugging
  printf( "sending: [" );
  for ( int i=0; i<datalen; i++ ) {
    printf( " 0x%x", data[i] );
  }
  printf( " ]\n" );
#endif
  unsigned char ch;
  mInSync = 0;
  for ( int i=0; i<datalen; i++ ) {
    ch = data[i] & 0x00FF;
    try {
      putChar( robot, ch );
    }
    catch( int err ) {
      fprintf( stderr, "\n**error> writing character to robot: %s\n", strerror( err ));
      throw( err );
    }
  }
  try {
    getAck( robot, data, datalen );
  }
  catch( int err ) {
    throw( err );
  }
} // end of sendData()




/**
 * readSensors()
 *
 */
void readSensors( int robot ) {
  try {
    mLeftIR      = getChar( robot );
    mRightIR     = getChar( robot );
    mLeftLight   = getChar( robot );
    mLeftLight   <<= 8;
    mLeftLight   += getChar( robot );
    mCenterLight = getChar( robot );
    mCenterLight <<= 8;
    mCenterLight += getChar( robot );
    mRightLight  = getChar( robot );
    mRightLight  <<= 8;
    mRightLight  += getChar( robot ); 
    mLineLeft    = getChar( robot );
    mLineRight   = getChar( robot );
    mStall       = getChar( robot );
  }
  catch ( int err ) {
    fprintf( stderr, "**error> reading sensors: %s\n", strerror( err ));
    throw( err );
  }
#if debugging
  printf( "sensors %3d, %3d, %5d, %5d, %5d, %3d, %3d, %3d\n",
	  mLeftIR, mRightIR, mLeftLight, mCenterLight, mRightLight,
	  mLineLeft, mLineRight, mStall );
#endif
} // end of readSensors()




/**
 * beep()
 *
 */
void beep( int robot, int freq, int duration ) {
  unsigned char data[PACKET_LEN];
  memset( data, 0, PACKET_LEN );
  data[0] = SET_SPEAKER;
  data[1] = 3;   //duration >> 8;
  data[2] = 232; //duration & 0x00FF;
  data[3] = 1;   //freq >> 8;
  data[4] = 184; //freq & 0x00FF;
  try {
    sendData( robot, data, PACKET_LEN );
    readSensors( robot );
    getAck( robot, &data[0], 1 );
  }
  catch( int err ) {
  }
} // end of beep()




/**
 * stop()
 *
 */
void stop( int robot ) {
  unsigned char data[PACKET_LEN];
  memset( data, 0, PACKET_LEN );
  data[0] = SET_MOTORS_OFF;
  try {
    sendData( robot, data, PACKET_LEN );
    readSensors( robot );
    getAck( robot, &data[0], 1 );
  }
  catch( int err ) {
  }
} // end of stop()




/**
 * drive()
 *
 */
void drive( int robot, int left, int right, int duration ) {
  unsigned char data[PACKET_LEN];
  memset( data, 0, PACKET_LEN );
  data[0] = SET_MOTORS_ON;
  data[1] = left;
  data[2] = right;
  try {
    sendData( robot, data, PACKET_LEN );
    readSensors( robot );
    getAck( robot, &data[0], 1 );
    if ( duration ) {
      usleep( duration ); // microseconds
      stop( robot );
    }
  }
  catch( int err ) {
  }
} // end of drive()




/**
 * main()
 *
 */
int main( int argc, char **argv ) {

  int robot;

  printf( "\nhello!\n" );
  printf( "this program should make the robot beep and then move forward for a short time.\n" );

  fprintf( stdout,"please wait while i connect to the robot..." );
  fflush( stdout );
  robot = initRobot( "/dev/tty.scribbler" );
  fprintf( stdout," connected a-okay!\n" );

  printf( "now i'm telling the robot to 'beep'.\n" );
  beep( robot, 440, 1 );
  
  printf( "now i'm telling the robot to move forward.\n" );
  drive( robot, 10, 10, 1000 );

  printf( "now i'm telling the robot to stop moving.\n" );
  stop( robot );

  printf( "now i'm closing my connection to the robot.\n" );
  close( robot );

  printf( "\nthat's all folks. bye!\n\n" );

} // end of main()
