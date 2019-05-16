#ifndef _INCL_APP
#define _INCL_APP
	void app_test_1ms(void);
	void app_test_10ms(void);
	void app_test_100ms(void);
	void app_test_1000ms(void);
    void app_test_BG(void);
    inline void app_ini(void);
    
     //  service supported in bootloader , so to update flash of eep jump to bootloader update make application valid, reset done
    #define WR_FLH 1 
    #define WR_EEP (WR_FLH+1)
    #define RD_FLH 3      
    #define RD_EEP (RD_FLH +1)

     //  service supported in application code
    #define PING 5
    #define RSTCMD 6
    #define FLASH 7  // command supported by application , to jump bootloader , amkes application invalid
    #define PING_APP_RESP 8 // on PING command Application send 8 as responce while BL send 5 as responce

     //  service supported in bootloader
    #define READ_RAM 0x0A
    #define MAST_AUTH 0x0B   // master load encripted mac befor to configure 
    
    // test service supported for application
    #define CLR_GPIO_0 0x10         // Relay 0 // PWM 
	#define SET_GPIO_0 0x11
	#define READ_GPIO_0 0x12
	
	#define CLR_GPIO_1 0x20        // Relay 1 // ADC
	#define SET_GPIO_1 0x21
	#define READ_GPIO_1 0x22
	
	#define CLR_GPIO_2 0x30         // FDR button
	#define SET_GPIO_2 0x31
	#define READ_GPIO_2 0x32
	
	#define CLR_GPIO_3 0x40         // Status LED
	#define SET_GPIO_3 0x41
	#define READ_GPIO_3 0x42
	
   #define  FCTORY_RESET 0x50
	// remaining are tx and rx for BLE
	
    #define TEN_SEC 10
    #define FIFTEEN_SEC 15
    
#endif