/**
 * mytest0.c (03jan09/sklar)
 *
 * this is a trivial program that connects to the scribbler robot and
 * sends it one command string that makes it beep once.
 *
 **/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define debugging 0



int main( int argc, char **argv ) {

  int fd;
  struct termios attr;
  int vmin, vtime;
  const int datalen = 9;
  // command string to make the robot beep
  char data[9] = { 113,     1,     44,     3,     16,     0,     0,     0,     0 };
  int i;

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
  printf( "   c_cc=[" );
  for ( i=0; i<NCCS; i++ ) {
    printf( " (%c %x)", attr.c_cc[i], attr.c_cc[i] );
  }
  printf( "]\n" );
  printf( "   c_ispeed=%ld\n", attr.c_ispeed );
  printf( "   c_ospeed=%ld\n", attr.c_ospeed );
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
    attr.c_cc[VTIME] = vtime;
  }

  // now set all these attributes
  printf( "setting attributes... " );
  if ( tcsetattr( fd, TCSANOW, &attr ) < 0 ) {
    fprintf( stderr, "\n**error> call to tcsetattr failed: %s\n", strerror( errno ));
    exit( 6 );
  }
  printf( " a-okay!\n" );

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
  printf( "   c_cc=[" );
  for ( i=0; i<NCCS; i++ ) {
    printf( " (%c %x)", attr.c_cc[i], attr.c_cc[i] );
  }
  printf( "]\n" );
  printf( "   c_ispeed=%ld\n", attr.c_ispeed );
  printf( "   c_ospeed=%ld\n", attr.c_ospeed );
#endif

#if debugging
  // display data string  
  printf( "data: length=%d  [", datalen );
  for ( i=0; i<datalen; i++ ) {
    printf( " %d ",data[i] );
  }
  printf( "]\n" );
#endif

  // now send data string
  printf( "writing data...\n" );
  write( fd,data,datalen );

  // close connection
  printf( "closing connection...\n" );
  close( fd );

  printf( "\nbye!\n\n" );

} // end of main()
