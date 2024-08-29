#include"header.h"
#define FLASH_SECTOR 14
#define FLASH_SECTOR_BASE  0x00038000  // Address of sector 14
#define FLAG_ADDRESS (FLASH_SECTOR_BASE)
#define COUNT_ADDRESS (FLASH_SECTOR_BASE+1024)


typedef void (*IAP)(unsigned int[], unsigned int[]);
#define IAP_LOCATION 0x7FFFFFF1
IAP iap_entry = (IAP) IAP_LOCATION;

void iap_command(unsigned int command[], unsigned int result[]) {
    IAP iap_entry = (IAP) IAP_LOCATION;
    iap_entry(command, result);
}

void erase_sector(int sector) {
    unsigned int command[5];
    unsigned int result[4];

    command[0] = 50;  // prepare sector command
    command[1] = sector;
    command[2] = sector;
   // command[3] = 60;

    iap_command(command, result);
    // Check result[0] for success
}
void write_flash( unsigned int address, unsigned int data) {
    unsigned int command[5];
    unsigned int result[4];

    command[0] = 51;  // Write command from RAM to ROM
    command[1] = address;
    command[2] = (unsigned int)&data;
    command[3] = 1024;  // Number of bytes
    command[4] = 60000;

    iap_command(command, result);
    // Check result[0] for success
}
void set_lock_flag() {
    erase_sector(FLASH_SECTOR);
    write_flash(FLAG_ADDRESS, 1);
}
void increment_failed_attempts() {
    unsigned int attempts = *((unsigned int*)COUNT_ADDRESS);
    ++attempts;				     // when provided attempts++ not working, WHY?????!!!!!
	erase_sector(FLASH_SECTOR);
    write_flash(COUNT_ADDRESS, attempts);
		uart0_tx_string("\r\n");
			uart0_tx_integer(attempts);
				uart0_tx_string("\r\n"); 

		uart0_tx_integer(attempts);
    if (attempts >= 3) {
        set_lock_flag();
		uart0_tx_string("SYSTEM LOCKED2\r\n"); 
				while(1)
				{
								// LOCKED
				}
    }
}
int main()
	{
	signed int c=0,fail=0,i=0;
	char temp;
	int flag=1;
	char og[11]="vector";	   //original password
	char psw[12]="";			      // user entering password
	uart0_init(9600);
//	erase_sector(FLASH_SECTOR);

			uart0_tx_string("\r\nFlash upon reset:");
			uart0_tx_integer(*((unsigned int*)FLASH_SECTOR_BASE));
			uart0_tx_string("\r\n");


//    write_flash(FLAG_ADDRESS, 2);
//	 *(volatile unsigned int*)(FLASH_SECTOR_BASE)=1;
	if(*( unsigned int*)(FLASH_SECTOR_BASE)==0)	
	{
			uart0_tx_string("SYSTEM LOCKED1\r\n"); 
				while(1)
				{
								// LOCKED
				}

	}
	L1:
	c=i=0;
	if(fail==0)
	uart0_tx_string("\r\nPassword is case sensitive and less <= 10 character\r\n");
	if(fail==1)
	uart0_tx_string("2 More attempts\r\n");
	if(fail==2)
	uart0_tx_string("!!!Last attempt!!!\r\n");
	uart0_tx_string("Enter Password:");
	while(1)
	{	
		temp=uart0_rx();
		if(temp!='\b' && temp!='\r')//ANY CHARACTER other than backspace and enter key
		{
		flag=1;	  //=>	setting flag despite the charector length 				      
		c++;
		if(c<=10)
		uart0_tx('*'); //loopback  '*' (Only if entered charecter is <= 10 )
		else
		{
		flag=0;     // if length Exceeds more than 10, flag set to 0. Thus it will not update the array
		c--;
		}
		}
		if(temp=='\b')	//enters only if its backspace
		{
		if(c!=0)				//IF USER PRESS BACKSPACE IN FIRST ATTEMPT ITSELF then this skips
		{
		i--;		   //decrementing array index.
		c--;		   //decrementing the charecter count
		psw[i]=0;			//DELETING the the actual array charecter
		uart0_tx('\b');		//1)displaying the backspace
		uart0_tx(0);        //2)deleting the already displayed charecter
		}
		continue;  //Eat 5 star do nothing.(only if C==0).
		}
		if(temp=='\r')		 //ENTER key
		{
		psw[i++]=0;	 //Explicitely adding the null charecter upon enter
		break;
		}
		if(flag==1) //array updated only if the flag==1 => number of char entered is less than 10, 
		psw[i++]=temp; //if more than 10 char it wont reflect neither in the array nor in the loopback display. 
	}	
	if(strcmp(psw,og)==0)
	uart0_tx_string("\r\nWelcome\r\n");	  //actual password check
	else
	{
	uart0_tx_string("\r\nWrong password \r\n");
	increment_failed_attempts();	
		fail++;				  // resetting for safer side
		c=i=0;				  // resetting for safer side
		set_lock_flag();
	//	uart0_tx_string("\r\nNO MORE Attempts\r\n"); // failed last attemp
	//	uart0_tx_string("SYSTEM LOCKED\r\n"); 
		goto L1;	
	}
	
}
































