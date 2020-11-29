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
1.���̼�app��bootloader�����ͬһ��һ��hex�ļ� ,ͨ��stlink��������������¼��mcu��Ƭ
2.mcu��¼��ɺ󣬸�λ������bootloader���жϵ�ǰ����������header��Ϊ��
3.����ǰmcu���е�app���򿽱�����һ�����������жϿ�����̼������ԣ����header������
������־λ��λ��Ȼ����ת��APP���򣬿�ʼ�������С�
3.���յ�OTA�������app�����ж�������������header ���ҳ�headerΪ�յģ������ϵĹ̼���
  ������Ӧ�ı����򣬽��µĹ̼�д�룬�����header��Ϣ����������־λ�������и�λ
4.��λ��bootloader �ҵ�header��Ϣ��������־��δ��λ�ĵı������¹̼� �� �����������ж�
 �ж������������ֱ�������½��յĹ̼���Ȼ����ת��APP���У���������ǰmcu��app�̼���
���½��յ��Ĺ̼�д�뵽mcu flash�������̼��ٴν����������жϣ���ͨ���ٴβ�����д����
ͨ������λ�Ѿ���ת��־���򿪿��Ź�������ת��app�̼����С�
5.��app�̼������������У���ι��������Ӧ���������ת��ɱ�־��λ�������������ȴ���һ��
���²��������ظ�3��4���̣�
6.��APP���������������ڿ��Ź��������£�������λ���ٴν���bootloader , ϵͳ��⵽���������
ĳ��������  ����ת��־��λ������ת��ɱ�־δ��λ����˵����תʧ�ܣ�ϵͳ��������ǰ��������
��ʹ�õĹ̼���������mcu flash��ǰ�̼���������һ���������Ĺ̼�д��mcu flash , Ȼ��У��������
����ת��
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

