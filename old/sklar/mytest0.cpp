/**
 * mytest0.cpp (03jan09/sklar)
 *
 * this is a trivial program that connects to the scribbler robot and
 * sends it one command string that makes it beep once.
 *

def beep(duration=.5, frequency1=None, frequency2=None):
... myro.globvars.robot.beep(duration, f1, f2)

def forward(speed=1, seconds=None):
... myro.globvars.robot.forward(speed, seconds)
...... self.move(speed, 0)
---> SET_MOTORS 1 1

def backward(speed=1, seconds=None):
... myro.globvars.robot.backward(speed, seconds)
...... self.move(-speed, 0)
---> SET_MOTORS -1 -1

def turn(direction, amount = .8, seconds=None):
... myro.globvars.robot.turn(direction, amount, seconds)
        if type(direction) in [float, int]:
            retval = self.move(0, direction)
        else:
            direction = direction.lower()
            if direction == "left":
                retval = self.move(0, value)
            elif direction == "right":
                retval = self.move(0, -value)
            elif direction in ["straight", "center"]:
                retval = self.move(0, 0) # aka, stop!
            else:
                retval = "error"

def turnLeft(speed=1, seconds=None):
... myro.globvars.robot.turnLeft(speed, seconds)
...... retval = self.move(0, speed)

def turnRight(speed=1, seconds=None):
... myro.globvars.robot.turnRight(speed, seconds)
...... retval = self.move(0, -speed)

def stop():
... myro.globvars.robot.stop()
...... self.move(0, 0)

 def move(self, translate, rotate):
        self._lastTranslate = translate
        self._lastRotate = rotate
        self._adjustSpeed()

 def _adjustSpeed(self):
        left  = min(max(self._lastTranslate - self._lastRotate, -1), 1)
        right  = min(max(self._lastTranslate + self._lastRotate, -1), 1)
        leftPower = (left + 1.0) * 100.0
        rightPower = (right + 1.0) * 100.0
        self._set(Scribbler.SET_MOTORS, rightPower, leftPower)



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

#define debugging 1

const int SET_SPEAKER=113;
const int SET_MOTORS_OFF=108;
const int SET_MOTORS_ON=109;



// sensors
unsigned char   mLeftIR;
unsigned char   mRightIR;
unsigned int    mLeftLight;
unsigned int    mCenterLight;
unsigned int    mRightLight;
unsigned char   mLineLeft;
unsigned char   mLineRight;
unsigned char   mStall;

int		mInSync;		// is object syncronized with robot?


unsigned char getChar( int fd ) {
  char unsigned ch;
  int ret;
  ret = read( fd, &ch, 1 );
  if ( ret != 1 ) {
    throw( ret );
  }
  return ch;
} // end of getChar()


unsigned char getCharBlocking( int fd ) {
  bool trying=true;
  unsigned char ch;
  int numtries=0;
  while ( trying ) {
    try {
      ch = getChar( fd );
    }
    catch( int err ) {
      if ( numtries < 20 ) {
	numtries++;
      }
      else {
	trying = false;
	cerr << "throwing error in getCharBlocking(), error=" << err << endl;
	throw numtries;
      }
    }
  }
#if debugging
  printf( "<-- 0x%x\n", ch);
#endif
  return ch;
} // end of getCharBlocking()


int putChar( int fd, unsigned char ch ) {
  if ( write( fd, &ch, 1 ) != 1 ) {
    return 0;
  }
  return 1;
} // end of putChar()


void clearBuffer( int fd ) {
  unsigned char ch;
  try {
    while ( getChar( ch )) {
    }
  }
  catch( int err ) {
    cerr << "error=" << err << endl;
  }
} // end of clearBuffer()



int getAck( int fd, unsigned char cmd, unsigned char data[] ) {
  unsigned char ch;
  try {
    ch = getCharBlocking( fd );
    if ( ch != cmd ) {
#if debugging
      cerr << "bad first ack: should be " << cmd << " is " << ch << endl;
#endif
    }
    for ( int i=0; i<8; i++ ) {
      ch = getCharBlocking( fd );
      /* N.B. Fluke messes with SET_MOTORS_ON */
      if ( ch != data[i] && cmd != SET_MOTORS_ON ) {
#if debugging
	fprintf( stderr, "bad command %d echo of byte %d", cmd, i );
	fprintf( stderr, "   should be 0x%x is 0x%x\n", data[i], ch );
#endif
      }
    }
    //    mInSync = 1;
  }
  catch ( int err ) {
    return 0;
  }
  return 1;
} // end of getAck()


void send( int fd, int unsigned ch ) {
  unsigned char c = ch & 0x00FF;
  if ( ! putChar( fd, c )) {
    fprintf( stderr, "send(%d) failed\n", ch );
  }
} // end of send()


int sendCommand( int fd, unsigned char cmd, unsigned char data[8] ) {
  printf( "sending command: [0x%x] data=[", cmd );
  for ( int i=0; i<8; i++ ) {
    printf( " 0x%x", data[i] );
  }
  printf( " ]\n" );
  unsigned char ch;
  //  clearBuffer( fd );
  //  mInSync = 0;
  send( fd, cmd );
  for ( int i=0; i<8; i++ ) {
    send( fd, data[i] );
  }
  return( getAck( fd, cmd, data ));
} // end of sendCommand()



int getFinalAck( int fd, unsigned char cmd ) {
  printf( "getting final ack...\n" );
  unsigned char ch;
  try {
    ch = getCharBlocking( fd );
  }
  catch ( int err )  {
    cerr << "DataError in getFinalAck\n";
    return 0;
  }
  if ( ch != cmd ) {
#if debugging
    cerr << "bad final ack: should be " << cmd << " is " << ch << endl;
#endif
    return 0;
  }
  return 1;
} // end of getFinalAck()


int loadAllSensors( int fd ) {
  printf( "loading all sensors...\n" );
  try {
    mLeftIR      = getCharBlocking( fd );
    mRightIR     = getCharBlocking( fd );
    mLeftLight   = getCharBlocking( fd );
    mLeftLight   <<= 8;
    mLeftLight   += getCharBlocking( fd );
    mCenterLight = getCharBlocking( fd );
    mCenterLight <<= 8;
    mCenterLight += getCharBlocking( fd );
    mRightLight  = getCharBlocking( fd );
    mRightLight  <<= 8;
    mRightLight  += getCharBlocking( fd ); 
    mLineLeft    = getCharBlocking( fd );
    mLineRight   = getCharBlocking( fd );
    mStall       = getCharBlocking( fd );
  }
  catch ( int err ) {
    cerr << "data error in loadAllSensors\n";
    return 0;
  }
#if debugging
  fprintf( stderr, "Sensors %3d, %3d, %5d, %5d, %5d, %3d, %3d, %3d\n",
	   mLeftIR, mRightIR, mLeftLight, mCenterLight, mRightLight,
	   mLineLeft, mLineRight, mStall );
#endif
  return 1;
} // end of loadAllSensors()



int beep( int fd, int freq, int duration ) {
  printf( "beeping...\n" );
  unsigned char data[8];
  //  memset( data, '*', 8 );
  memset( data, 0, 8 );
  data[0] = 3;   //duration >> 8;
  data[1] = 232; //duration & 0x00FF;
  data[2] = 1;   //freq >> 8;
  data[3] = 184; //freq & 0x00FF;
  if ( ! sendCommand( fd, SET_SPEAKER, data ))
    return 0;
  if ( ! loadAllSensors( fd ) )
    return 0;
  return getFinalAck( fd, SET_SPEAKER );
} // end of beep()



int stop( int fd ) {
  unsigned char data[8];
  memset( data, 0, 8 );
  if ( ! sendCommand( fd, SET_MOTORS_OFF, data ))
    return 0;
  if ( ! loadAllSensors( fd ) )
    return 0;
  return getFinalAck( fd, SET_MOTORS_OFF );
} // end of stop()



int drive( int fd, int left, int right, int duration ) {
  //  int usleep( long );
  unsigned char data[8];
  memset( data, 0, 8 );
  data[0] = left;
  data[1] = right;
  if ( ! sendCommand( fd, SET_MOTORS_ON, data ))
    return 0;
  if ( ! loadAllSensors( fd ) )
    return 0;
  if ( ! getFinalAck( fd, SET_MOTORS_ON ))
    return 0;
  if ( duration ) {
      printf( "generated stop\n" );
      usleep( duration ); // microseconds
      return stop( fd );
  }
  return 1;
} // end of drive()





/**
 * main()
 *
 */
int main( int argc, char **argv ) {

  int fd;
  struct termios attr;
  int vmin, vtime, nread;
  const int packetlen = 9;
  const int maxbuf=1024;
  unsigned char buf[maxbuf];


  // command string to make the robot beep
  // SET_SPEAKER = 113
  // duration >> 8
  // duration % 256
  // frequency >> 8
  // frequency % 256
  // >>> beep(1,440)
  //              _write: ['0x71', '0x3', '0xe8', '0x1', '0xb8', '0x0', '0x0', '0x0', '0x0']
  unsigned char beepdata[packetlen] = { 113,    3,     232,    1,      184,   0,     0,     0,     0 };

  // command string to make the robot go forward
  // SET_MOTORS = 109
  // left
  // right
  // >>> forward()
  //               _write: ['0x6d', '0xc8', '0xc8', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0']
  // 0xa 0xa
  unsigned char fwddata[packetlen] = { 109,    10,    10,    0,     0,     0,     0,     0,     0 };
  unsigned char stopdata[packetlen] = { 108,    0,    0,    0,     0,     0,     0,     0,     0 };


  printf( "\nhello!\n\n" );

  // open serial connection to scribbler
  printf( "trying to open serial connection to scribbler... " );
  if (( fd = open( "/dev/tty.scribbler", O_RDWR | O_NOCTTY | O_NONBLOCK )) < 0 ) {
    fprintf( stderr, "\n**error> unable to open serial port '%s': %s\n", "/dev/tty.scribbler", strerror( errno ));
    exit( 2 );
  }
  printf( " a-okay!\n" );

  /*
   * default values for scribbler, according to myro interface:
   * baudrate=38400 
   * timeout=10  
   * bytesize=8 
   * stopbits=1 
   * parity=N   
   * flowctl=0  
   * rtscts=0   
   * c_iflag=0 (same as john)
   * c_oflag=0 (same as john)
   * c_cflag=51968 (john has 35584)
   * c_lflag=0 (same as john)
   * c_ispeed=38400 (john has 115200)
   * c_ospeed=38400 (john has 115200)
   * c_cc=
   *          0    1    2    3    4    5    6    7   8    9   10   11   12   13   14  15  16  17   18   19
   * myro=[ 0x4 0xff 0xff 0x7f 0x17 0x15 0x12 0xff 0x3 0x1c 0x1a 0x19 0x11 0x13 0x16 0xf 0x0 0x0 0x14 0xff]
   * john=[ --- ---- ---- ---- ---- ---- ---- ---- --- ---- ---- ---- ---- ---- ---- --- --- 0x1 ---- ----]
   *
   * difference is value of vtime = 17
   * john's value works.
   *
   */

  // get attributes set on open serial connection
  printf( "reading attributes on serial connection... " );
  vmin = vtime = 0;
  if ( tcgetattr( fd, &attr ) < 0 ) {
    fprintf( stderr, "\n**error> call to tcgetattr failed: %s\n", strerror( errno ));
    exit( 3 );
  }
  printf( "  a-okay!\n" );

#if debugging
  printf( "attribute values are:\n" );
  printf( "   c_iflag=%ld\n", attr.c_iflag );
  printf( "   c_oflag=%ld\n", attr.c_oflag );
  printf( "   c_cflag=%ld\n", attr.c_cflag );
  printf( "   c_lflag=%ld\n", attr.c_lflag );
  printf( "   c_ispeed=%ld\n", attr.c_ispeed );
  printf( "   c_ospeed=%ld\n", attr.c_ospeed );
  printf( "   c_cc=[" );
  for ( int i=0; i<NCCS; i++ ) {
    printf( " (%c %x)", attr.c_cc[i], attr.c_cc[i] );
  }
  printf( "]\n" );
#endif

  // set up attribute values for what scribbler needs (according to myro...)
  // set up raw mode / no echo / binary
  attr.c_cflag |= ( CLOCAL | CREAD );

  attr.c_lflag &= ~( ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN );

  attr.c_oflag &= ~( OPOST );

  // set input speed
  printf( "setting input speed... " ); 
  attr.c_iflag &= ~( INLCR | IGNCR | ICRNL | IGNBRK );
  if ( cfsetispeed( &attr, B38400 ) != 0 ) {
    fprintf( stderr, "\n**error> unable to cfsetispeed: %s\n", strerror( errno ));
    exit( 4 );
  }
  printf( " a-okay!\n" );

  // set output speed
  printf( "setting output speed... " ); 
  if ( cfsetospeed( &attr, B38400 ) != 0 ) {
    fprintf( stderr, "\n**error> unable to cfsetospeed: %s\n", strerror( errno ));
    exit( 5 );
  }
  printf( " a-okay!\n" );

  // set up character length
  attr.c_cflag &= ~( CSIZE );
  attr.c_cflag |= CS8;

  // set up stopbits
  attr.c_cflag &= ~( CSTOPB );

  // set up parity
  attr.c_iflag &= ~( INPCK | ISTRIP );
  attr.c_cflag &= ~( PARENB | PARODD );

  // set up flow control
  // xonxoff
  attr.c_iflag &= ~( IXON | IXOFF | IXANY);
  // rtscts
  attr.c_cflag &= ~( CRTSCTS );
  // buffer

  // set up vmin (minimal number of characters to be read. = for non blocking)
  if (( vmin < 0 ) || ( vmin > 255 )) {
    fprintf( stderr, "**error> invalid vmin: %d\n", vmin );
  }
  else {
    attr.c_cc[VMIN] = vmin;
  }

  // set up vtime
  if (( vtime < 0 ) || ( vtime > 255 )) {
    fprintf( stderr, "**error> invalid vtime: %d\n", vtime );
  }
  else {
    attr.c_cc[VTIME] = 1; // john has 1; myro has 0; ---> 1 seems to work better...
  }

  // now set all these attributes
  printf( "setting attributes... " );
  if ( tcsetattr( fd, /*TCSANOW*/ TCSAFLUSH, &attr ) < 0 ) {
    fprintf( stderr, "\n**error> call to tcsetattr failed: %s\n", strerror( errno ));
    exit( 6 );
  }
  printf( " a-okay!\n" );

  /* o.k. now let's make it blocking */
  fcntl( fd, F_SETFL, fcntl( fd, F_GETFL, 0) & ~O_NONBLOCK );

#if debugging
  printf( "\n\ndisplaying attributes that we just set:\n" );
  if ( tcgetattr( fd, &attr ) < 0 ) {
    fprintf( stderr, "call to tcgetattr failed: %s\n", strerror( errno ));
    exit( 3 );
  }
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
#endif

  printf( "\nJOHN'S COMMANDS-----\n" );
  beep( fd, 440, 1 );
  drive( fd, 10, 10, 500 );
  stop( fd );

  printf( "\nBETSY'S COMMANDS-----\n" );


  // now send data string to make robot beep
  printf( "writing data...\n" );
#if debugging
  printf( "beep: length=%d  [", packetlen );
  for ( int i=0; i<packetlen; i++ ) {
    printf( " 0x%x ",beepdata[i] );
  }
  printf( "]\n" );
#endif
  // "beep"
  write( fd,beepdata,packetlen );
  getAck( fd,beepdata[0],&(beepdata[1]) );
  // "load all sensors"
  nread = read( fd, buf, 11 );
  printf( "nread=%d buf=[",nread );
  for ( int i=0; i<nread; i++ ) {
    printf( " 0x%x", buf[i] );
  }
  printf( " ]\n" );
  // get final ack
  nread = read( fd, buf, 1 );
  if ( buf[0] != beepdata[0] ) {
    fprintf( stderr,"error reading final ack: read [0x%x] but wanted [0x%x]\n", buf[0], beepdata[0] );
  }


  // now send data string to make robot go forward
  printf( "writing data...\n" );
#if debugging
  printf( "forward: length=%d  [", packetlen );
  for ( int i=0; i<packetlen; i++ ) {
    printf( " 0x%x ",fwddata[i] );
  }
  printf( "]\n" );
#endif
  // "drive"
  write( fd,fwddata,packetlen );
  getAck( fd,fwddata[0],&(fwddata[1]) );
  try {
    unsigned char ch = getCharBlocking( fd );
    if ( ch != fwddata[0] ) {
      cerr << "bad first ack: should be " << fwddata[0] << " is " << ch << endl;
    }
    for ( int i=0; i<8; i++ ) {
      ch = getCharBlocking( fd );
      /* N.B. Fluke messes with SET_MOTORS */
      if ( ch != fwddata[i+1] && fwddata[0] != 109 ) {
	fprintf( stderr, "bad command %d echo of byte %d", fwddata[0], i );
	fprintf( stderr, "   should be 0x%x is 0x%x\n", fwddata[i+1], ch );
      }
    }
  }
  catch ( int err ) {
    cerr << "error=" << err << endl;
  }
  // "load all sensors"
  nread = read( fd, buf, 11 );
  printf( "nread=%d buf=[",nread );
  for ( int i=0; i<nread; i++ ) {
    printf( " 0x%x", buf[i] );
  }
  printf( " ]\n" );
  // get final ack
  nread = read( fd, buf, 1 );
  if ( buf[0] != fwddata[0] ) {
    fprintf( stderr,"error reading final ack: read [0x%x] but wanted [0x%x]\n", buf[0], fwddata[0] );
  }


  // now send data string to make robot stop
  printf( "writing data...\n" );
#if debugging
  printf( "stop: length=%d  [", packetlen );
  for ( int i=0; i<packetlen; i++ ) {
    printf( " 0x%x ",stopdata[i] );
  }
  printf( "]\n" );
#endif
  // "stop"
  write( fd,stopdata,packetlen );
  getAck( fd,stopdata[0],&(stopdata[1]) );



  // close connection
  printf( "closing connection...\n" );
  close( fd );

  printf( "\nbye!\n\n" );

} // end of main()
