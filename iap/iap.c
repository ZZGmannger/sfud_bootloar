#include "iap.h"
#include <sfud.h>
#include <string.h>

#define IAP_ASSERT(x)   do{ if(x==NULL)while(1);}while(0)
#define IAP_LOG(fmt,...)    printf("IAP:"fmt,__VA_ARGS__)


void sfud_test(void)
{
//	sfud_err result = SFUD_SUCCESS;
//  
//	const sfud_flash *flash = sfud_get_device_table();
//	char read[50];
//	char write[32] = "sfud test--------------------->";

//    /* erase test */
//    result = sfud_erase(flash, 0, 10);
//    /* write test */
//    result = sfud_write(flash, 0, strlen(write),(uint8_t*)write);
//    /* read test */
//    result = sfud_read(flash, 0, 32, (uint8_t*)read);
//    if (result == SFUD_SUCCESS)
//    {
//        printf("read out: %s\r\n",read);
//    }
	void iap_test(void);
	iap_test();
}

#define FW_START_ADDR1    (0)
#define FW_START_ADDR2    ()


typedef struct 
{
	char name[32];
	char date[64];
	char ver[32];
	
	uint32_t length;
	uint32_t crc32;
	
	uint32_t jump_already;
	uint32_t jump_success;
}fw_header_t;

#define SECTOR_SIZE   (4*1024)
#define BLOCK_SZIE    (16*SECTOR_SIZE)

#define SECTOR0_ADDR  (0*SECTOR_SIZE)
#define SECTOR1_ADDR  (1*SECTOR_SIZE)
#define SECTOR2_ADDR  (2*SECTOR_SIZE)
#define SECTOR3_ADDR  (3*SECTOR_SIZE)

#define BLOCK0_ADDR   (0*BLOCK_SZIE)
#define BLOCK1_ADDR   (1*BLOCK_SZIE)
#define BLOCK2_ADDR   (2*BLOCK_SZIE)
#define BLOCK3_ADDR   (3*BLOCK_SZIE)

#define BLOCK4_ADDR   (4*BLOCK_SZIE)
#define BLOCK5_ADDR   (5*BLOCK_SZIE)
#define BLOCK6_ADDR   (6*BLOCK_SZIE)
#define BLOCK7_ADDR   (7*BLOCK_SZIE)

/*0-3 block -------> 4*64K = 256K for data*/

/*4-5 block -------->2*64K = 128K for firmware1*/
#define FW1_START_ADDR      (BLOCK4_ADDR)
#define FW1_MAX_SIZE		(128*1024)
/*6-7 block -------->2*64K = 128K for firmware1*/
#define FW2_START_ADDR      (BLOCK6_ADDR)
#define FW2_MAX_SIZE 		(128*1024)




int fw_header_read(uint32_t addr , fw_header_t* hdr);
int fw_header_write(uint32_t addr ,const fw_header_t* hdr);

static fw_header_t fw_wr;
static fw_header_t fw_rd;


#define  MEMBER_INDEX(memb)  (uint32_t)(&(((fw_header_t*)0)->memb))

void iap_test(void)
{
	const sfud_flash *flash = sfud_get_device_table();
//	sfud_erase(flash, SECTOR0_ADDR, SECTOR_SIZE);
//	
//	fw_wr.crc32 = 0xe5741238;
//	fw_wr.length = 115463;
//	fw_wr.jump_already = FLAG_INIT_VALUE;
//	fw_wr.jump_success = FLAG_INIT_VALUE;
//	strcpy(fw_wr.date,__DATE__""__TIME__);
//	strcpy(fw_wr.ver,"hwer: v1.1 , sver: 1.2.1");
//	strcpy(fw_wr.name,"rhf0m003ut2.bin");
//	fw_header_write(0 ,&fw_wr);
		
	fw_header_read(FW_START_ADDR1 ,&fw_rd);
	IAP_LOG("read out--------------------------->%d\r\n",0);
	IAP_LOG("%s\r\n",fw_rd.ver);
	IAP_LOG("%s\r\n",fw_rd.name);
	IAP_LOG("%s\r\n",fw_rd.date);
	IAP_LOG("length = %d Byte\r\n",fw_rd.length);
	IAP_LOG("crc = 0x%08X\r\n",fw_rd.crc32);
	IAP_LOG("jump already: 0x%08X\r\n",fw_rd.jump_already);
	IAP_LOG("jump success: 0x%08X\r\n",fw_rd.jump_success);
	
	if(fw_rd.jump_already == 0xffffffff)
	{
		uint32_t a = 0xeeeeeeee;
		sfud_write(flash,0+ MEMBER_INDEX(jump_already),4,(uint8_t*)&a);
	}
	
}

int fw_header_read(uint32_t addr , fw_header_t* hdr)
{
	IAP_ASSERT(hdr);
	
	const sfud_flash *flash = sfud_get_device_table();
	uint8_t sfud_ret = sfud_read(flash, addr, sizeof(fw_header_t), (uint8_t*)hdr);
 
	if(sfud_ret != SFUD_SUCCESS)
	{
		IAP_LOG("read err:%d",sfud_ret); 
	}
	return sfud_ret;
}
int fw_header_write(uint32_t addr ,const fw_header_t* hdr)
{
	IAP_ASSERT(hdr);
	
	const sfud_flash *flash = sfud_get_device_table();
	uint8_t sfud_ret = sfud_write(flash, addr, sizeof(fw_header_t), (uint8_t*)hdr);
 
	if(sfud_ret != SFUD_SUCCESS)
	{
		IAP_LOG("write err:%d",sfud_ret); 
	}
	return sfud_ret;
}

#define JUMP_ALREAY_MAGIC  (0x6F34E923)
#define JUMP_SUCCESS_MAGIC (0x19EF5B2A)

#define FLAG_INIT_VALUE    (0xFFFFFFFF)

int set_jump_already(void)
{

}
int set_jump_success(void)
{
	
}
/*
1.将固件app和bootloader打包成同一的一个hex文件 ,通过stlink或者其他工具烧录到mcu空片
2.mcu烧录完成后，复位，进入bootloader。判断当前两个备份区header都为空
3.将当前mcu运行的app程序拷贝到第一个备份区，判断拷贝后固件完整性，填充header，并将
两个标志位置位，然后跳转到APP程序，开始正常运行。
3.当收到OTA升级命令，app程序将判断两个备份区的header ，找出header为空的，或者老的固件。
  擦除对应的备份域，将新的固件写入，并填充header信息（不操作标志位）。进行复位
4.复位后，bootloader 找到header信息完整，标志均未置位的的备份区新固件 ， 进行完整性判断
 判断无误后（有误则直接销毁新接收的固件，然后跳转到APP运行），擦除当前mcu中app固件，
将新接收到的固件写入到mcu flash，读出固件再次进行完整性判断（不通过再次擦除重写）。
通过后，置位已经跳转标志，打开看门狗，并跳转到app固件运行。
5.若app固件正常启动运行，将喂狗，将对应备份域的跳转完成标志置位。正常工作，等待下一的
更新操作（将重复3，4过程）
6.若APP不能正常启动，在看门狗的作用下，产生复位，再次进入bootloader , 系统检测到备份域存在
某个备份域  已跳转标志置位，但跳转完成标志未置位，则说明跳转失败，系统将擦除当前备份区无
法使用的固件，并擦除mcu flash当前固件，并将另一个备份区的固件写入mcu flash , 然后校验完整性
并跳转。
s
*/
void jump_check(void)
{
/*
	1.check two header
	    a. if two header null
		copy the fw from mcu flash
	2.
*/
}

