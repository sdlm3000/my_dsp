//*---------------------�Լ�Ĵ���----------------------------------------------*/
#define SELF_TEST_X 0x0D
#define SELF_TEST_Y 0x0E
#define SELF_TEST_Z 0x0F
#define SELF_TEST_A 0x10

//*---------------------���ƼĴ���----------------------------------------------*/
#define SMPLRT_DIV  0x19    //*�����ʷ�Ƶ������ֵ��0x07(125Hz) */
#define CONFIG   0x1A       // *��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz) */
#define GYRO_CONFIG  0x1B   // *�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s) */
#define ACCEL_CONFIG 0x1C  // *���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz) */
#define FIFO_R_W 0X74

#define INT_PIN_CFG 0x37
#define INT_ENALE 0x38
#define INT_STATUS 0x37
#define SIGNAL_PATH_RESET 0x68
#define USER_CTRL 0x6A
#define INT_PIN_CFG 0x37


#define PWR_MGMT_1  0x6B // *��Դ��������ֵ��0x00(��������) */
#define WHO_AM_I  0x75 //*IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��) */

//*---------------------���ݼĴ���----------------------------------------------*/
#define ACCEL_XOUT_H 0x3B  // �洢�����X�ᡢY�ᡢZ����ٶȸ�Ӧ���Ĳ���ֵ */
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
 
#define TEMP_OUT_H  0x41   // �洢������¶ȴ������Ĳ���ֵ */
#define TEMP_OUT_L  0x42  //READ ONLY
 
#define GYRO_XOUT_H  0x43 // �洢�����X�ᡢY�ᡢZ�������Ǹ�Ӧ���Ĳ���ֵ */
#define GYRO_XOUT_L  0x44 
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48

#define FIFO_COUNTH 0X72  //* �洢FIFO�ļ���ֵ*/
#define FIFO_COUNTL 0X73

//*--------------------DMP    �Ĵ���----------------------------------------------*/




