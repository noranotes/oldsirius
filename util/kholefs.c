/*

    Este programa é voltado ao desenvolvimento de Sistemas Operacionais







*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#define FAT_ATTR_READ_ONLY	0x01
#define FAT_ATTR_HIDDEN		0x02
#define FAT_ATTR_SYSTEM		0x04
#define FAT_ATTR_VOLUME_ID	0x08
#define FAT_ATTR_DIRECTORY	0x10
#define FAT_ATTR_ARCHIVE		0x20
#define FAT_ATTR_LONG_NAME	(FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN\
 				| FAT_ATTR_SYSTEM| FAT_ATTR_VOLUME_ID)

#define FAT_DIR_ENTRY_SIZE	32

#define FAT12 "FAT12   "
#define FAT16 "FAT16   "
#define FAT32 "FAT32   "


typedef struct fat_bpb {

	char BS_jmpBoot[3];
	char BS_OEMName[8];
	unsigned short BPB_BytsPerSec;
	unsigned char BPB_SecPerClus;
	unsigned short BPB_RsvdSecCnt;
	unsigned char BPB_NumFATs;
	unsigned short BPB_RootEntCnt;
	unsigned short BPB_TotSec16;
	unsigned char BPB_Media;
	unsigned short BPB_FATSz16;
	unsigned short BPB_SecPertrk;
	unsigned short BPB_NumHeads;
	unsigned int BPB_HiddSec;
	unsigned int BPB_TotSec32;	
	unsigned char BS_DrvNum;
    	unsigned char BS_Reserved1;
	unsigned char BS_BootSig;
	unsigned int BS_VolID;
	char BS_VolLab[11];
	char BS_FilSysType[8];
	char code[512-62];

}__attribute__ ((packed)) fat12_16_t;



unsigned int disk_size;
unsigned int total_sectors;
unsigned int FATSz;
unsigned int data_sectors;
unsigned int root_dir_sectors;
unsigned int count_of_clusters;
unsigned int TYPE;

unsigned int  TmpVal1,TmpVal2; 



static size_t file_size_bytes(FILE *fp);

fat12_16_t buffer_boot_record;


int main(int argc, char *argv[]){
	

    int boot_record_size;

	unsigned char* dados =(unsigned char*)malloc(4096);		

    struct stat st;

    FILE *fp1;
    FILE *fp2;


    if(argv[1]!=0){
	if((fp1=fopen(argv[1],"r+b"))==NULL){
	printf("Erro ao abrir o arquivo \"argv[1]\"");
	exit(1); 
	}


    stat (argv[1],&st);
    
    boot_record_size = st.st_size;
    
	
	if((fread(&buffer_boot_record,sizeof(char),boot_record_size,fp1))!=(sizeof(buffer_boot_record))){
	printf("Erro ao copiar o sector de boot");
	exit(1);	
	}
	
    printf("Boot Record Copiado %i Bytes\n",boot_record_size);

    }else{
             printf("Erro! nao ha boot record");
             exit(1);
    }


    if(argv[2]!=0){
	if((fp2=fopen(argv[2],"r+b"))==NULL){
	printf("Erro ao abrir o lear a imagem \"argv[2]\"");
	exit(1); 
	}

	/*Calculando o total de sectores*/

    stat (argv[2],&st);

    disk_size = st.st_size/buffer_boot_record.BPB_BytsPerSec;

   

    if(disk_size > 65535){
                buffer_boot_record.BPB_TotSec32 = disk_size;
                buffer_boot_record.BPB_TotSec16 = 0;
              }
     else buffer_boot_record.BPB_TotSec16 = disk_size;


    /*Calculando a FATSz*/
    root_dir_sectors = ((buffer_boot_record.BPB_RootEntCnt * 32) 
                     + (buffer_boot_record.BPB_BytsPerSec -1))
                     /buffer_boot_record.BPB_BytsPerSec;

    TmpVal1 = disk_size - (buffer_boot_record.BPB_RsvdSecCnt + root_dir_sectors);
    TmpVal2 = (256 * buffer_boot_record.BPB_SecPerClus) + buffer_boot_record.BPB_NumFATs;

    FATSz = (TmpVal1 + (TmpVal2 -1))/TmpVal2;



    total_sectors = (buffer_boot_record.BPB_TotSec16 == 0)? buffer_boot_record.BPB_TotSec32 : buffer_boot_record.BPB_TotSec16;

    /* determina se FAT12 ou FAT16 */
    data_sectors = total_sectors 
                 - (buffer_boot_record.BPB_RsvdSecCnt + (buffer_boot_record.BPB_NumFATs * FATSz)
                 + root_dir_sectors);
    
    count_of_clusters = data_sectors/buffer_boot_record.BPB_SecPerClus;

    if (count_of_clusters < 4085){

            /*FAT12 */

             TYPE = 12;

             buffer_boot_record.BPB_FATSz16 = FATSz &0xFFF;

             strncpy(buffer_boot_record.BS_FilSysType,FAT12,8);
    
    }else if (count_of_clusters < 65525){
            /*FAT16 */
        
            TYPE = 16;

            buffer_boot_record.BPB_FATSz16 = FATSz &0xFFFF;
            
            strncpy(buffer_boot_record.BS_FilSysType,FAT16,8);

    }else{
            /*FAT32*/
        
            printf("O volume deve ser FAT32 ou exFAT");
            exit(1);


        }

    puts("Status Disk:\n\n");    
    printf("\tFileSystem Type: %s\n",buffer_boot_record.BS_FilSysType);
    printf("\tBytsPerSec: %i\n",buffer_boot_record.BPB_BytsPerSec); 
    printf("\tBytsPerClus: %i\n",buffer_boot_record.BPB_SecPerClus);  

 

   

	/*Escrevendo o sector de boot*/
	if((fwrite(&buffer_boot_record,sizeof(char),boot_record_size,fp2))!=(sizeof(buffer_boot_record))){
	printf("Erro ao escrever o sector de boot");
	exit(1);	
	}else{ fclose(fp1);}
    }else{
        printf("Erro, no disk imagem");
        exit(1);
}

    
	printf("Sucesso!\n");
	
	free(dados);
	fclose(fp2);
	exit(0);
	return 0;


}


static size_t file_size_bytes(FILE *fp){

	size_t count = -1; 
	do{ 
		fgetc(fp); 
		count++;

	}while(!feof(fp));
	rewind(fp);

	return count;
}
