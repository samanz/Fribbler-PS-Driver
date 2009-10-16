/******************************************************************************
 *
 * Grabs a camera frame
 * 
 * This is pretty black magic, but the code was empirically tuned to sync 
 * up with the cameras clock. Because of the latency reading the IO port 
 * (8-9 instruction cycles)  we can't read every pixel.  Even though the 
 * camera is set to VG mode (640 x 320) we actually get a 256 wide image. 
 *	
 * Parameters:	
 *  R0 - img buffer pointer (256 x 192)
 *  R1 - delay around pclk - tuning parameter
 *
 * Temp Use:
 *  R2 - VSYNC mask
 *  R3-  PCLK synching	 
 *  R4 - IOPIN Address
 *  R5 - width/pin-17 mask
 *  R6 - pixel counter
 *  R7 - PCLK mask
 *  R8 - IOPIN value
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

.file "grab_frame.s"
.arm
.align
.globl grab_frame
.type grab_frame, %function
.section .data,"ax",%progbits
grab_frame:	
	STMFD SP!, {R0-R12, LR}

        /* Disable IRQ */
        MRS R5, CPSR
        ORR R5, R5, #0x80
        MSR CPSR, R5
        
	MOV R3, R1
	LDR R5,=304            /* width */
	/*MUL R6,R1,R5*/       /* total number of pixels = width * height */
	LDR R6,=192
	LDR R4,=0xE0028000      /* IOPIN */
	LDR R2,=0x00008000      /* mask out VSYNC */
	LDR R7,=0x00010000      /* mask out PCLK */
	LDR R5,=0x00020000      /* mask out pin 17 */
	/*LDR R3,=16808*/       /* vsync to href counter */
	/*LDR R3,=17013*/
	/*LDR R3,=21594*/
	
vsync_low:
	/* load IOPIN into R8 */
	LDR R8, [R4]	
	/* compare VSYNC to IOPIN */
	ANDS R8, R8, R2
	/* loop while VSYNC is low */	
	BNE vsync_low

pclk_sync1:
	 /* load IOPIN into R8 */	
	LDR R8, [R4]	
	NOP
	NOP	
	NOP
	NOP
	NOP
	/* compare PCLK to IOPIN */
	ANDS R8, R8, R7
	SUB R3, R3, #3
	/* loop while PCLK is low */
	NOP
	NOP
	BEQ pclk_sync1

pclk_sync2:
	 /* load IOPIN into R8 */	
	LDR R8, [R4]
	NOP
	NOP	
	NOP
	NOP
	NOP
	/* compare PCLK to IOPIN */
	ANDS R8, R8, R7
	SUB R3, R3, #3
	/* loop while PCLK is low */
	NOP
	NOP	
	BEQ pclk_sync2

pclk_sync3:
	 /* load IOPIN into R8 */	
	LDR R8, [R4]
	NOP
	NOP	
	NOP
	NOP
	NOP
	/* compare PCLK to IOPIN */
	ANDS R8, R8, R7
	SUB R3, R3, #3
	/* loop while PCLK is low */
	NOP
	NOP
	BEQ pclk_sync3

pclk_delay:
	SUBS R3, R3, #1 /*1*/
	NOP             /*2*/
	NOP             /*3*/
	BNE pclk_delay  /*4, 4-6*/
	NOP /*5*/
	NOP /*6*/
	
	NOP /*1*/
	NOP /*2*/
	NOP /*3*/	
	NOP /*4*/
	
	/* load top 8 bits of IOPIN into R8  */
	LDRB R8, [R4, #3] /*5-12*/
	SUBS R8, R8, #16  /*13*/
	NOP               /*14*/
	
	/* 256 pixels in row */
	MOV R3, #256      /*15*/
	NOP		  /*16*/
	NOP              /*17*/
	
	BEQ  col          /*16, 16-18*/
	NOP
	NOP
	
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	
col:
	NOP
	NOP

	/* load top 8 bits of IOPIN into R8  */
	LDRB R8, [R4, #3]		
	/* store R8 into imgbuf and increment imgbuf */
	STRB R8, [R0], #1
	/* debug off */
	STR R5, [R4, #12]	
	/* subtract one from pixel count */
	SUBS R3, R3, #1
	/* debug off */	
	STR R5, [R4, #12]
	
	BNE col
	NOP

	ANDS R3, R6, #1
	LDRNE R2,=1774 /*(1776 -4 (above) - 2) : 1 rows*/
	LDREQ R2,=3302 /*(3304 -4 (above) - 2) : 2 rows*/
	LDR R3,=256
	NOP
	NOP
	NOP
	NOP
	
dummy:
	SUBS R2, R2, #1
	NOP
	NOP
	BNE dummy
	NOP
	SUBS R6, R6, #1
	
	BNE col

	/* turn debug led on */
	STR R5, [R4, #4]	

	/* delay for a while so we can see the led*/
	LDR R6,=50000
leddelay:	
	SUBS R6, R6, #1	
	BNE leddelay
	
	/* debug led off */
	STR R5, [R4, #12]	

	
        /* Enable IRQ */
        MRS R5, CPSR
        BIC R5, R5, #0x80
        MSR CPSR, R5
        
	/* return */
	LDMFD SP!, {R0-R12, PC}
