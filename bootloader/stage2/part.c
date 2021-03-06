/* Khole OS v0.3
 * Copyright (C) 2017,2018 (Nelson Sapalo da Silva Cole)
 */
#include <io.h>
#include <string.h>
#include <fs/fs.h>
#include <part.h>


//TODO esta variavel foi erdada do boot
extern uint8_t bootdevice_num;
uint32_t part_next_id  = 1;

// d0:, d1:, d2:, d10:, ... dn:

const char *dID[] ={
"d0: ","d1: ","d2: ","d3: ","d4: ","d5: ","d6: ","d7: ","d8: ","d9: ",
"d10:","d11:","d12:","d13:","d14:","d15:","d16:","d17:","d18:","d19:",
};

static uint32_t MoutDisk(int dev,int type)
{
	char *buffer =(char*)malloc(0x1000);

	uint32_t retval = 0;

	if(type) {
		if((read_sector(buffer,16,1,dev))!=0) {
			printf("Read disk%d error\n",dev);
			retval = 0;
		}

		if(!(strncmp(buffer +1,"CD001",5)) && (buffer[0] == 1)) {

			strncpy(part[0].volumeID,dID[0],4);
			part[0].p = 1;
			part[0].type = PART_CD001;
			part[0].devnum = dev;
			printf("Partition Type: CDFS (ISO 9660 Volume Primary) Volume ID %s\n",part[0].volumeID);
			retval = PART_CD001;
		}

    	
	}
	else if(!type) {

		if((read_sector(buffer,0,1,dev))!=0) {
			printf("Read disk%d error\n",dev);
			retval = 0;
		}
		
		//MBR

		printf ("Partition Type: (MBR), not found\n");

		while(true);

	}
	free(buffer);

	return (retval);
}

static int MoutPart(int dev, int part)
{

	
}

void part_initialize()
{

	uint8_t bootdev =0;
	uint8_t bootdevtype = 0;

	printf("Partition initialize\n");
      	memset(&part,0,32);

			bootdev = (mboot_device() >>24 &0xff);
			bootdevtype =( mboot_device () &0xff);

	MoutDisk(bootdev,bootdevtype);
	// Check partition d0
	if(!(part[0].p))
		printf("Partition (d0:), not present\n");


}

stpart_t indexpart(char *src)
{
	stpart_t retval;

	if((src[0] == 'd') && (src[2] == ':')) {
		retval.index = (src[1] - 48);
		retval.pathstart = 3;
		retval.error = 0;
	}
	else if((src[0] == 'd') && (src[3] == ':')) {
		retval.index = ((src[1] - 48) * 10) + ((src[2] - 48));
		retval.pathstart = 4;
		retval.error = 0;
	}
	else { 

		retval.error = 1; // error
	}
	return retval;
}
