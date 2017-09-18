package com.example.android.Airplane;

import android.app.ActionBar;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Toast;

import java.util.Arrays;

import min3d.Shared;
import min3d.Utils;
import min3d.core.Object3dContainer;
import min3d.core.RendererActivity;
import min3d.parser.IParser;
import min3d.parser.Parser;
import min3d.vos.Light;

public class LoadObjFile extends RendererActivity {

    static String str = "";

    static String str_temp = "";

    private Object3dContainer objModel;
    private int x, y =0, swing;

    /* All related to sensor data */
    private SensorManager mSensorManager;
    private Sensor mAccelerometer, mGyroscope, mCompass;
    public static float[] gravity = new float[4];

    private static final String TAG = "";
    //made below all  has staic
    static int accel_x = 0, accel_y = 0, accel_z = 0, i = 0, k = 0, j = 0, p = 0, gyrox_disp = 0, gyroy_disp = 0, gyroz_disp = 0, rotatex = 0;
    static long dt1, dt0, dt0_acc = 0, dt1_acc = 0;
    static float dt = 0, dt2 = 0, gyro_x = 0, gyro_y = 0, gyro_z = 0, dT = 0.01f, gyro_pitch = 0, gyro_roll = 0, gyro_yaw = 0, driftx = 0, drifty = 0, driftz = 0;
    static float gyrox_dt = 0, gyroy_dt = 0, gyroz_dt = 0, gyro_pitch_old = 0, gyro_roll_old = 0, gyro_yaw_old = 0;
    static float accelx_2 = 0, accely_2 = 0, accelz_2 = 3, accel_pitch = 0, accel_roll = 0, accel_yaw = 0;
    static double result=0, result1=0;
    static int a = 0, dt3 = 0, dt4 = 0, comp_x = 0, comp_y = 0, comp_z = 0, record = 0, play = 0, play_save = 0, record_cnt = 0, play_cnt = 0, compare = 0;

    static float dt_acc = 0, dt2_acc = 0, accelx_dt = 0, accely_dt = 0, accelz_dt = 0, velx_dt = 0, vely_dt = 0, velz_dt = 0, accel_z_tmp = 0;
    static  int ax, ay, az, gx, gy, gz, g=0, ARRAY_LENGTH = 16, sma_x = 0, sma_y = 0, sma_z = 0, s =0, sg =0, angle_compass = 0, angle_compass_calib = 0;
    static  float sma_gyro_x = 0, sma_gyro_y = 0, sma_gyro_z = 0, compass_x=0, compass_y=0, compass_z=0;
    public static int accelx_offset = 0, accely_offset = 0, accelz_offset = 0, gyrox_offset = 0, gyroy_offset = 0, gyroz_offset = 0;

    public static float roll_right_mul = 0, roll_left_mul = 0, pitch_fwd_mul = 0, pitch_back_mul = 0, yaw_right_mul = 0, yaw_left_mul = 0, compass_x_calib=0;
    static  int sma_rotatex = 0, rs = 0, sma_compassx = 0, sma_compassy = 0, sma_compassz = 0;

    /* These arrays are for SMA filter */
    static  int [ ] accel_x_array = new int[ 20 ];
    static  int [ ] accel_y_array = new int[ 20 ];
    static  int [ ] accel_z_array = new int[ 20 ];

    static float [ ] gyro_x_array = new float[ 20 ];
    static  float [ ] gyro_y_array = new float[ 20 ];
    static  float [ ] gyro_z_array = new float[ 20 ];

    static int [ ] rotatex_array = new int[ 20 ];
    static int [ ] compassx_array = new int[ 20 ];
    static  int [ ] compassy_array = new int[ 20 ];

    ///sensor demo
    static int air_count=0,intr_enable = 0, up=-1,display_status=0,read=0;
    static float  accx,accy,accz,gyrox,gyroy,gyroz,compx,compy,compz;
    static float airplane[]= new float[900];
    static float display[] = new float[900];
    static int calib_status=0;
    static int sen_data_count=0;
    final static byte  sensor_data[] = new byte[1910];
    static int ret=0;

    //spi semnsor buffer
    public static byte sensor_fifo_aiprlane_spi[] = new byte[1801];
    public static int buf_count_spi=0;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ActionBar actionBar;
        actionBar = getActionBar();
        ColorDrawable colorDrawable = new ColorDrawable(Color.parseColor("#e6f7ff"));
        actionBar.setBackgroundDrawable(colorDrawable);
        System.out.println("sensor data on create .............debug..");
        //intr_enable = 0;
        BluetoothLeService.interrupt = 0;
        /*
        BluetoothLeService.data_resp=0;
        System.out.println("Reading version register .............debug..");
        Android_Airplane_Activity.reg_rd((byte) 0x01, (byte) 0x01, (byte) 0x00);//reading the version register
        while(BluetoothLeService.data_resp == 0){
            delay(1);
        }
        BluetoothLeService.data_resp=0;
        byte version;

        version = BluetoothLeService.read_value;   //praveen
        Log.e(TAG, "debug version register 0x0100 value is:" + version);
        */
    }
    public static void calibration(){
        int calib_count=0;
        //intr_enable = 0;
        BluetoothLeService.interrupt = 0;
        Log.e(TAG, "Calibration process started.............debug");

        while(calib_count<=3) {
            capture_sensors_data();
            air_count=0;
            calib_count++;
            if(intr_enable==0)calib_count--;
        }
        if(calib_count==4) {
            calib_status = 1;
            air_count=0;
            ReadSensorData.btn_calib=0;
            Log.e(TAG, "Calibration Done......Calibration Done............Calibration Done....................Calibration Done.............debug");
        }
        System.out.println("Accelerometer x axis offset ......   =: " + accelx_offset);
        System.out.println("Accelerometer y axis offset ......   =: " + accely_offset);
        System.out.println("Accelerometer z axis offset ......   =: " + accelz_offset);
        System.out.println("Gyroscope x axis offset ......   =: " + gyrox_offset);
        System.out.println("Gyroscope y axis offset ......   =: " + gyroy_offset);
        System.out.println("Gyroscope z axis offset ......   =: " + gyroz_offset);
        System.out.println("compass x recorded.....  =: " + compass_x_calib);

    }
    public static void read1800bytes_sensor_data(){
        System.out.println("sensor data on create .............debug..");
        i=j=p=0;
        sen_data_count=0;
        buf_count_spi=0;
        BluetoothLeService.datain_count=0;
        BluetoothLeService.status_read_done=0;
        gyro_pitch_old = gyro_roll_old = gyro_yaw_old = 0;
        intr_enable = 0;
///////////////
        //  sma_gyro_x = sma_gyro_y = sma_gyro_z =0;//if uncmtd airplane is moving 2nt time unstable unless all array is reset
        ////  sma_x = sma_y =sma_z = 0;
        //  sma_compassx=sma_compassy=sma_compassz=0;
        //  gyro_x = gyro_y = gyro_z =0;

        /////////
        accel_x = accel_y = accel_z =  i =  k =  j =  p =  gyrox_disp =  gyroy_disp =  gyroz_disp =  rotatex = 0;
        dt1= dt0= dt0_acc = dt1_acc = 0;
        dT = 0.01f;
        dt =  dt2 =  gyro_x =  gyro_y =  gyro_z = gyro_pitch =  gyro_roll =  gyro_yaw =  driftx =  drifty =  driftz = 0;
        gyrox_dt = gyroy_dt =  gyroz_dt =  gyro_pitch_old =  gyro_roll_old =  gyro_yaw_old = 0;
        accelz_2 = 3;
        accelx_2 = accely_2 =   accel_pitch =  accel_roll = accel_yaw = 0;
        result= result1=0;
        a =  dt3 =  dt4 =  comp_x = comp_y = comp_z =  record =  play =  play_save =  record_cnt =  play_cnt = compare = 0;
        dt_acc = dt2_acc =  accelx_dt =  accely_dt = accelz_dt =  velx_dt =  vely_dt =  velz_dt =  accel_z_tmp = 0;
        ARRAY_LENGTH = 16;
        ax= ay= az= gx= gy= gz= g=  0;s = sg = angle_compass =  angle_compass_calib = 0;
        sma_gyro_x =  sma_gyro_y =  sma_gyro_z = 0;
        sma_x =  sma_y =  sma_z =  0;//if uncmtd 2 nd time struks at the previous place unless arrays are  resets
        compass_x= compass_y= compass_z=0;
        accelx_offset =  accely_offset =  accelz_offset =  gyrox_offset =  gyroy_offset = gyroz_offset = 0;
        roll_right_mul = roll_left_mul =  pitch_fwd_mul =  pitch_back_mul =  yaw_right_mul =  yaw_left_mul =  compass_x_calib=0;
        sma_rotatex =  rs =  sma_compassx = sma_compassy =  sma_compassz = 0;


        Arrays.fill(accel_x_array, 0);
        Arrays.fill(accel_y_array, 0);
        Arrays.fill(accel_z_array, 0);
        Arrays.fill(gyro_x_array, 0);
        Arrays.fill(gyro_y_array, 0);
        Arrays.fill(gyro_z_array, 0);
        Arrays.fill(rotatex_array, 0);
        Arrays.fill(compassx_array, 0);
        Arrays.fill(compassy_array, 0);
        ///sensor demo
        air_count=intr_enable =display_status=read=0;
        up=-1;
        accx=accy=accz=gyrox=gyroy=gyroz=compx=compy=compz=0;
        //  airplane[]= new float[900];
        //float display[] = new float[900];
        calib_status=0;
        ///////////////
        accel_x = accel_y =accel_z = 0;
        compass_x_calib=0;
        rotatex=0;
        BluetoothLeService.interrupt = 0;
        Log.e(TAG, " started Fetching Data...............debug");
        calib_status=1;

        capture_sensors_data();
        if(air_count==900){
            System.out.println("captured accel/gyro/magnet sensors data completed....debug..");
            display();
            System.out.println("captured sensor data mapping to airplane is done.......debug..");
            display_status=1;
            ReadSensorData.interrupt=5;
        }
        air_count=0;
    }
    private static void display() {
        System.out.println("start displaying airplane.............debug..");
        air_count=0;
        while (air_count<=899){
            display[air_count]=airplane[air_count];
          //  Log.e(TAG, String.format("display[%d]:%f ",air_count, display[air_count]));
            air_count++;
            display[air_count]=airplane[air_count];
         //   Log.e(TAG, String.format("display[%d]:%f ",air_count, display[air_count]));
            air_count++;
            display[air_count]=airplane[air_count];
         //   Log.e(TAG, String.format("display[%d]:%f ",air_count, display[air_count]));
            air_count++;
            //delay(100);
        }
        System.out.println("displaying airplane completed.............debug..");
    }
    protected static int  airplane_cals()
    {
      //  if (calib_status == 1) {
            Log.e(TAG, String.format("gyro_pitch........debug1:%f ", gyro_pitch));
            Log.e(TAG, String.format("gyro_roll........debug1:%f ", gyro_roll));
            Log.e(TAG, String.format("gyro_yaw........debug1:%f ", gyro_yaw));
            float x = accx;
            float y = accy;
            float z = accz;
            ax = (int) x;
            ay = (int) y;
            az = (int) z;
            j++;
            if (j != 1) {
                dt0_acc = dt1_acc;
            }
            dt1_acc = System.currentTimeMillis();
            if (j == 1) {
                dt0_acc = dt1_acc;
            }
            dt2_acc = dt1_acc - dt0_acc;
            dt_acc = dt2_acc / 1000;
            accel_x = (int) x;
            accel_y = (int) y;
            accel_z = (int) z;
            // Subtract the Sensor Offset values to get actual values
            accel_x = accel_x - accelx_offset;
            accel_y = accel_y - accely_offset;
            accel_z = accel_z - accelz_offset;
                                        /* Simple Moving Average Filter */
            sma_x = sma_x - (accel_x_array[ARRAY_LENGTH - 1] / ARRAY_LENGTH) + (accel_x / ARRAY_LENGTH);
            sma_y = sma_y - (accel_y_array[ARRAY_LENGTH - 1] / ARRAY_LENGTH) + (accel_y / ARRAY_LENGTH);
            sma_z = sma_z - (accel_z_array[ARRAY_LENGTH - 1] / ARRAY_LENGTH) + (accel_z / ARRAY_LENGTH);

            for (s = ARRAY_LENGTH - 1; s > 0; s--) {
                accel_x_array[s] = accel_x_array[s - 1];
                accel_y_array[s] = accel_y_array[s - 1];
                accel_z_array[s] = accel_z_array[s - 1];
            }
            accel_x_array[0] = accel_x;
            accel_y_array[0] = accel_y;
            accel_z_array[0] = accel_z;
            // Convert to acceleration due to gravity
            accel_x = (int) Math.round(((double) sma_x) / 1671.84f);
            accel_y = (int) Math.round(((double) sma_y) / 1671.84f);
            accel_z = (int) Math.round(((double) sma_z) / 1671.84f);
                                        /* Removing gravity value from Z axis */
            accel_z_tmp = accel_z - 10;
            /* Providing tolerance limit for all the 3 axes*/
            if (accel_x > 1 || accel_x < -1) {
                //	            distance_x();
            }
            if (accel_y > 1 || accel_y < -1) {
                //	            distance_y();
            }
            if (accel_z > 1 || accel_z < -1) {
                //	            distance_z();
            }
            accelx_2 = accel_x * accel_x;
            accely_2 = accel_y * accel_y;
            accelz_2 = accel_z * accel_z;
            result = accely_2 + accelz_2;
            result1 = (float) (accel_x / Math.sqrt(result));
            accel_roll = (float) (Math.atan(result1)) * (180 / 3.14159f);
            result = accelx_2 + accelz_2;
            result1 = (float) (accel_y / Math.sqrt(result));
            accel_pitch = (float) (Math.atan(result1)) * (180 / 3.14159f);
            result = accelx_2 + accely_2;
            result1 = (float) (accel_z / Math.sqrt(result));
            accel_yaw = (float) (Math.atan(result1)) * (180 / 3.14159f);
                i++;
                float x2 = gyrox;
                float y2 = gyroy;
                float z2 = gyroz;
                gyro_x = x2;
                gyro_y = y2;
                gyro_z = z2;
                gx = (int) x2;
                gy = (int) y2;
                gz = (int) z2;
            //	        Subtract the Sensor Offset values to get actual values
                gyro_x = gyro_x - (gyrox_offset);
                gyro_y = gyro_y - (gyroy_offset);
                gyro_z = gyro_z - (gyroz_offset);
                                        /* Simple Moving Average Filter */
                sma_gyro_x = sma_gyro_x - (gyro_x_array[ARRAY_LENGTH - 1] / ARRAY_LENGTH) + (gyro_x / ARRAY_LENGTH);
                sma_gyro_y = sma_gyro_y - (gyro_y_array[ARRAY_LENGTH - 1] / ARRAY_LENGTH) + (gyro_y / ARRAY_LENGTH);
                sma_gyro_z = sma_gyro_z - (gyro_z_array[ARRAY_LENGTH - 1] / ARRAY_LENGTH) + (gyro_z / ARRAY_LENGTH);
                for (sg = ARRAY_LENGTH - 1; sg > 0; sg--) {
                    gyro_x_array[sg] = gyro_x_array[sg - 1];
                    gyro_y_array[sg] = gyro_y_array[sg - 1];
                    gyro_z_array[sg] = gyro_z_array[sg - 1];
                }
                gyro_x_array[0] = gyro_x;
                gyro_y_array[0] = gyro_y;
                gyro_z_array[0] = gyro_z;
                if (i != 1) {
                    dt0 = dt1;
                }
                dt1 = System.currentTimeMillis();
                if (i == 1) {
                    dt0 = dt1;
                }
                dt2 = dt1 - dt0;
                dt = dt2 / 1000;
                gyro_x = sma_gyro_x * (0.005493164f);  //  Multiplying with 180/32738 (32738 is the full scale value for 2G)
                gyro_y = sma_gyro_y * (0.005493164f);
                gyro_z = gyro_z * (0.005493164f);
                                        /* Using 1.5 as Hard coded multiplication factor for LSM330DLC */


                gyrox_dt = (gyro_x * dt) * 1.5f;// pkoya commented and uncomtd
                gyroy_dt = (gyro_y * dt) * 1.5f;
                gyroz_dt = (gyro_z * dt) * 1.5f;

        //////////////*****************pkoya commented below all related to gyrox_dt,gyroy_dt,gyroz_dt and tested :result-->all roll,pith,yaw are:0's///////////******************

                gyro_roll = gyro_roll_old + gyrox_dt;        //(gyro_x * dt);
                gyro_pitch = gyro_pitch_old + gyroy_dt;    //(gyro_y * dt);
                gyro_yaw = gyro_yaw_old + gyroz_dt;        //(gyro_z * dt);
                driftx = gyro_pitch_old - gyro_pitch;
                drifty = gyro_roll_old - gyro_roll;
                driftz = gyro_yaw_old - gyro_yaw;

                if (accel_x == 0 && accel_y == 0) {
                    gyro_pitch = 0;
                }
                if (accel_y == 0 && accel_x == 0) {
                    gyro_roll = 0;
                }
                System.out.println("compass x calibration value .....=: " + compass_x_calib);
                if (rotatex < (compass_x_calib + 1) && rotatex > (compass_x_calib - 1) && accel_x == 0 && accel_y == 0) {
                    gyro_yaw = 0;

                }
                gyro_pitch_old = gyro_pitch;
                gyro_roll_old = gyro_roll;
                gyro_yaw_old = gyro_yaw;

                System.out.println("gyro roll..storing in old....debug2.  =: " + gyro_roll);
                System.out.println("gyro pitch.storing in old....debug2  =: " + gyro_pitch);
                System.out.println("gyro yaw...storing in old.. .debug2 =: " + gyro_yaw);


                gyrox_disp = (int) gyro_roll;
                gyroy_disp = (int) gyro_pitch;
                gyroz_disp = (int) gyro_yaw;

                comp_x = (int) (((0.9f) * gyrox_disp) + (0.1f * (-accel_pitch)));
                comp_y = (int) (((0.9f) * gyroy_disp) + (0.1f * (-accel_roll)));
                comp_z = (int) (((0.9f) * gyroz_disp) + (0.1f * (-accel_yaw)));

            p++;
            compass_x = compx;
            compass_y = compy;
            compass_z = compz;
                                    /* Simple Moving Average Filter */
            sma_compassx = (int) (sma_compassx - (compassx_array[ARRAY_LENGTH - 1] / ARRAY_LENGTH) + (compass_x / ARRAY_LENGTH));
            sma_compassy = (int) (sma_compassy - (compassy_array[ARRAY_LENGTH - 1] / ARRAY_LENGTH) + (compass_y / ARRAY_LENGTH));

            for (rs = ARRAY_LENGTH - 1; rs > 0; rs--) {
                compassx_array[rs] = compassx_array[rs - 1];
                compassy_array[rs] = compassy_array[rs - 1];
            }
            compassx_array[0] = (int) compass_x;
            compassy_array[0] = (int) compass_y;

            if ((compass_x != ax) && (compass_x != gx) && (compass_y != ay) && (compass_y != gy)) {

                if (p < 5) {

                    angle_compass = (int) (Math.atan2(sma_compassx, sma_compassy) * (180 / 3.14));
                    rotatex = (int) ((angle_compass + 180) % 360);                                    //Compute x value
                    rotatex = 360 - rotatex;
                    compass_x_calib = rotatex;
                }
                angle_compass = (int) (Math.atan2(sma_compassx, sma_compassy) * (180 / 3.14));
                rotatex = (int) ((angle_compass + 180) % 360);                                    //Compute x value
                rotatex = 360 - rotatex;
            }
            ret=0;
            int k = 0;
            while (k < 3) {
                ret=0;
                Log.e(TAG, String.format("gyro_pitch:%f ", gyro_pitch));
                Log.e(TAG, String.format("gyro_roll:%f ", gyro_roll));
                Log.e(TAG, String.format("gyro_yaw:%f ", gyro_yaw));
                airplane[air_count] = gyro_pitch;
                Log.e(TAG, String.format("airplane[%d]..gyro_pitch:%f ", air_count, airplane[air_count]));
                air_count++;
                airplane[air_count] = gyro_roll;
                Log.e(TAG, String.format("airplane[%d]..gyro_roll:%f ", air_count, airplane[air_count]));
                air_count++;
                airplane[air_count] = gyro_yaw;
                Log.e(TAG, String.format("airplane[%d]..gyro_yaw:%f ", air_count, airplane[air_count]));
                air_count++;
                k++;
            }
            ret=1;
        try {
            Thread.sleep(0,1);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
       // }
    return 0;
    }
    public static   void capture_sensors_data() {
        System.out.println("global GSR disabled .............debug..");
        //Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disable
        // delay(50);
        System.out.println("global GSR enabled .............debug..");
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR enable
        delay(500);
        System.out.println("global GSR enabled.............debug..");
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR en-able  (soft reset)
        delay(500);
        System.out.println("global GSR disabled .............debug..");
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disable
        // delay(50);
        System.out.println("buffer setup sample rate .............debug..");
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x01, (byte) 0x01, (byte) 0x00);//buffer setup sample rate
        // delay(50);
        System.out.println("buffer setup capture time  .............debug..");
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x01, (byte) 0x02, (byte) 0x00);//buffer setup capture time
        //delay(50);
        System.out.println("Intr_mask setup .done............debug..");
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x00, (byte) 0x06, (byte) 0xfe);//Intr_mask setup
        // delay(50);
        System.out.println("data capture enable .............debug..");
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x01, (byte) 0x03, (byte) 0x01);//data capture enable
        // delay(50);
        for (int i = 0; i < 1; i++) {
            System.out.println("waiting for Interrupt....debug..");
          /*
          t = 350;
          while (BluetoothLeService.interrupt == 0 && (t-- > 0)) {
          delay(1);
          }
          */
            long start_time = System.currentTimeMillis(); //fetch starting time
            long wait_time = 10000;
            long end_time = start_time + wait_time;

            while (BluetoothLeService.interrupt == 0 && System.currentTimeMillis() < end_time) {
                //delay(1);
            }
            if (BluetoothLeService.interrupt == 1) {
                BluetoothLeService.interrupt = 0;
                intr_enable = 1;
                ReadSensorData.interrupt = 0;
                System.out.println("Interrupt came....data capture dis-able .done............debug..");
                break;
            } else {
                intr_enable = 0;
                ReadSensorData.interrupt = 1;
            }
        }
        // BluetoothLeService.interrupt = 0;
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x01, (byte) 0x03, (byte) 0x00);//data capture disable once interrupt came
        // delay(20);
        if(intr_enable==1) {
            System.out.println("reading status resgister............debug..");
            BluetoothLeService.data_resp = 0;
            Android_Airplane_Activity.reg_rd_spi((byte) 0x00, (byte) 0x04);//reading the status register
           /*
            t = 250;
            while (BluetoothLeService.data_resp == 0 && (t-- > 0)) {
            delay(1);
            }
            */
            long start_time = System.currentTimeMillis(); //fetch starting time
            long wait_time = 250;
            long end_time = start_time + wait_time;

            while (BluetoothLeService.data_resp == 0 && System.currentTimeMillis() < end_time) {
                //delay(1);
            }
            BluetoothLeService.data_resp = 0;
        }
        byte status = BluetoothLeService.read_value;
        BluetoothLeService.status_read_done=1;
        BluetoothLeService.count_200=0;
        BluetoothLeService.datain_count=0;
        Log.e(TAG, " status register value is...debug:" + status);
        if (status == 0x01 && intr_enable==1)
        {
            Log.e(TAG, " status is true..int_l is set.........debug");
            for (read = 0; read <129; read++) {
                BluetoothLeService.data_resp = 0;
                if(read==128){
                    Log.e(TAG, "Reading remaninig bytes...........debug");
                    Android_Airplane_Activity.reg_rd_fixed_addr_spi_remainbytes((byte) 0x01, (byte) 0x20);
                    while(BluetoothLeService.count_200<8)
                    // while(BluetoothLeService.data_resp==0)
                    {
                        ;
                    }
                }
                else {
                    Android_Airplane_Activity.reg_rd_fixed_addr_spi((byte) 0x01, (byte) 0x20);//read accel/gyro/magnet data //rbop is set only,when data length > 4096.
                    while(BluetoothLeService.count_200<14)
                    // while(BluetoothLeService.data_resp==0)
                    {
                        ;
                    }
                }

                BluetoothLeService.count_200=0;
                BluetoothLeService.data_resp=0;
               // BluetoothLeService.count_200=0;
                long start_time = System.currentTimeMillis(); //fetch starting time
                long wait_time = 10;
                long end_time = start_time + wait_time;

                while (BluetoothLeService.data_resp == 0 && System.currentTimeMillis() < end_time) {
                    //delay(1);
                }
                BluetoothLeService.data_resp = 0;
            /*
             t=500;
             while(BluetoothLeService.dataresponse == 0 && (t-- > 0) ) {
             delay(1);

                long start_time = System.currentTimeMillis(); //fetch starting time
                long wait_time = 10;
                long end_time = start_time + wait_time;

                while (BluetoothLeService.data_resp == 0 && System.currentTimeMillis() < end_time) {
                    //   System.out.println("wating for response............debug..");
                    //delay(1);
                }
                BluetoothLeService.data_resp = 0;*/
            }
            buf_count_spi=0;
            int while_count=0,sc=1;
            while(while_count++<100)
            {
                short value;
                ////////////////x-co-ordinates////////////////
                // accx= ((data[sc]& 0xff)|(data[sc+3]<<8) );
                value= (short) ((sensor_fifo_aiprlane_spi[sc]& 0xff)|(sensor_fifo_aiprlane_spi[sc+3]<<8));
                if(calib_status==1) {
                   // String hex = Integer.toHexString(value & 0xffff);
                  //  Log.e(TAG, String.format("debug..kps..acc_x short in hex...is:%s ", hex));
                    accx = value;
                   //  Log.e(TAG, String.format("debug..kps..acce_x merged_[%d-%d]:%f ", sc, sc + 3, accx));
                }
                if(calib_status==0) {
                    AutoCalibration.ax = (int)value;
                   //  Log.e(TAG, String.format("debug....acce_x merged_[%d-%d]:%d ", sc, sc + 3, AutoCalibration.ax));
                }
                sc++;
                value= (short) ((sensor_fifo_aiprlane_spi[sc]& 0xff)|(sensor_fifo_aiprlane_spi[sc+3]<<8));
                if(calib_status==1) {
                   // String hex = Integer.toHexString(value & 0xffff);
                  //  Log.e(TAG, String.format("debug..kps..gyro_x short in hex...is:%s ", hex));
                    gyrox = value;
                  //    Log.e(TAG, String.format("debug..kps..gyro_x merged_[%d-%d]:%f ", sc, sc + 3, gyrox));
                }
                if(calib_status==0) {
                    AutoCalibration.gx = (int)value;
                    // Log.e(TAG, String.format("debug....gyro_x merged_[%d-%d]:%d ", sc, sc + 3, AutoCalibration.gx));
                }
                sc++;
                value= (short) ((sensor_fifo_aiprlane_spi[sc+3]& 0xff)|(sensor_fifo_aiprlane_spi[sc]<<8));
                if(calib_status==1) {
                   // String hex = Integer.toHexString(value & 0xffff);
                   // Log.e(TAG, String.format("debug..kps..comp_x short in hex...is:%s ", hex));
                    compx = value;
                     Log.e(TAG, String.format("debug..kps..compas_x merged_[%d-%d]:%f ", sc, sc + 3, compx));
                }
                if(calib_status==0) {
                    AutoCalibration.compass_x1 = value;
                    // Log.e(TAG, String.format("debug....compas_x merged_[%d-%d]:%f ", sc, sc + 3, AutoCalibration.compass_x1));
                }
                sc+=4;
                ///////////////////////////////y co-ordinates/////////////////////////////////////
                value= (short) ((sensor_fifo_aiprlane_spi[sc]& 0xff)|(sensor_fifo_aiprlane_spi[sc+3]<<8));
                if(calib_status==1) {
                   // String hex = Integer.toHexString(value & 0xffff);
                  //  Log.e(TAG, String.format("debug..kps..acc_y short in hex...is:%s ", hex));
                    accy = value;
                   //  Log.e(TAG, String.format("debug..kps..acce_y merged_[%d-%d]:%f ", sc, sc + 3, accy));
                }
                if(calib_status==0) {
                    AutoCalibration.ay = (int)value;
                    // Log.e(TAG, String.format("debug....acce_y merged_[%d-%d]:%d ", sc, sc + 3, AutoCalibration.ay));
                }
                sc++;
                value= (short) ((sensor_fifo_aiprlane_spi[sc]& 0xff)|(sensor_fifo_aiprlane_spi[sc+3]<<8));
                if(calib_status==1) {
                   // String hex = Integer.toHexString(value & 0xffff);
                   // Log.e(TAG, String.format("debug..kps..gyro_y short in hex...is:%s ", hex));
                    gyroy = value;
                   //  Log.e(TAG, String.format("debug..kps..gyro_y merged_[%d-%d]:%f ", sc, sc + 3, gyroy));
                }
                if(calib_status==0) {
                    AutoCalibration.gy = (int)value;
                    // Log.e(TAG, String.format("debug....gyro_y merged_[%d-%d]:%d ", sc, sc + 3, AutoCalibration.gy));
                }
                sc++;
                value= (short) ((sensor_fifo_aiprlane_spi[sc+3]& 0xff)|(sensor_fifo_aiprlane_spi[sc]<<8));
                if(calib_status==1) {
                  //  String hex = Integer.toHexString(value & 0xffff);
                   // Log.e(TAG, String.format("debug..kps..comp_y short in hex...is:%s ", hex));
                    compy = value;
                   // Log.e(TAG, String.format("debug..kps..compas_y merged_[%d-%d]:%f ", sc, sc + 3, compy));
                }
                if(calib_status==0) {
                    AutoCalibration.compass_y1 = value;
                    // Log.e(TAG, String.format("debug....compas_y merged_[%d-%d]:%f ", sc, sc + 3, AutoCalibration.compass_y1));
                }
                sc+=4;
                /////////////////////////////////////////////z -co ordinates ////////////////////////////////////
                value= (short) ((sensor_fifo_aiprlane_spi[sc]& 0xff)|(sensor_fifo_aiprlane_spi[sc+3]<<8));
                if(calib_status==1) {
                   // String hex = Integer.toHexString(value & 0xffff);
                  //  Log.e(TAG, String.format("debug..kps..acc_z short in hex...is:%s ", hex));
                    accz = value;
                   //  Log.e(TAG, String.format("debug..kps..acce_z merged_[%d-%d]:%f ", sc, sc + 3, accz));
                }
                if(calib_status==0) {
                    AutoCalibration.az = (int)value;
                    // Log.e(TAG, String.format("debug....acce_z merged_[%d-%d]:%d ", sc, sc + 3, AutoCalibration.az));
                }
                sc++;
                value= (short) ((sensor_fifo_aiprlane_spi[sc]& 0xff)|(sensor_fifo_aiprlane_spi[sc+3]<<8));
                if(calib_status==1) {
                  //  String hex = Integer.toHexString(value & 0xffff);
                  //  Log.e(TAG, String.format("debug..kps..gyro_x short in hex...is:%s ", hex));
                    gyroz = value;
                   //  Log.e(TAG, String.format("debug..kps..gyro_z merged_[%d-%d]:%f ", sc, sc + 3, gyroz));
                }
                if(calib_status==0) {
                    AutoCalibration.gz = (int)value;
                    //Log.e(TAG, String.format("debug....gyro_z merged_[%d-%d]:%d ", sc, sc + 3, AutoCalibration.gz));
                }
                sc++;
                value= (short) ((sensor_fifo_aiprlane_spi[sc+3]& 0xff)|(sensor_fifo_aiprlane_spi[sc]<<8));
                if(calib_status==1) {
                  //  String hex = Integer.toHexString(value & 0xffff);
                  //  Log.e(TAG, String.format("debug..kps..comp_z short in hex...is:%s ", hex));
                    compz = value;
                   //Log.e(TAG, String.format("debug..kps..compas_z merged_[%d-%d]:%f ", sc, sc + 3, compz));
                }
                if(calib_status==0) {
                    AutoCalibration.compass_z1 = value;
                    //Log.e(TAG, String.format("debug....compas_z merged_[%d-%d]:%f ", sc, sc + 3, AutoCalibration.compass_z1));
                }
                sc+=4;
                airplane_cals();
            }
        }
        else {
            Log.e(TAG, "Status Reguster is bit not En-abled ");
        }
    }
    public static void delay(int time)
    {
        try {
            Thread.sleep(time);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    @Override
    public void initScene() {
        // scene.backgroundColor().setAll(0x000000ff);
        scene.backgroundColor().setAll(0x0099CCFF);
        scene.lights().add(new Light());
        Light mylight=new Light();
        mylight.position.setZ(150);
        Bitmap bitmap= Utils.makeBitmapFromResourceId(R.drawable.blue_sky);
        Shared.textureManager().addTextureId(bitmap, "blue_sky", false);
        bitmap.recycle();
        IParser parser = Parser.createParser(Parser.Type.OBJ, getResources(),"com.example.android.bluetoothlegatt:raw/mdn3_obj", true);  //aplane2_obj
        parser.parse();
        objModel = parser.getParsedObject();
        objModel.scale().x = objModel.scale().y = objModel.scale().z = 1.0f;	 //.00100f;	//	.0050f; objModel.scale().x = objModel.scale().y = objModel.scale().z = .1f; ///origanal
        //   objModel.scale().x = objModel.scale().y = objModel.scale().z = 0.0f;//test
        //  objModel.scale().x = objModel.scale().y = objModel.scale().z = 0.909f;//test
        scene.addChild(objModel);
    }

    @Override
    public void updateScene() {
        //  objModel.rotation().x = (-gyro_pitch);
        //  objModel.rotation().y = gyro_roll;
        //  objModel.rotation().z = gyro_yaw;
        //test block
        // if(intr_enable==0) {
        // }
        if( display_status==1 && up<900) {

            // System.out.println("update scene if condtn............debug..");
            objModel.rotation().y = -display[++up];
            objModel.rotation().x = display[++up];
            objModel.rotation().z = -display[++up];
            delay(10);//20

            // objModel.rotation().x = -display[++up];
            //objModel.rotation().y = display[++up ];
            // objModel.rotation().z = display[++up ];
//            objModel.rotation().y = display[up + 1];
            // objModel.rotation().z = display[up + 2];
            //  up++;

            if(up==899){//899
                display_status=0;
                up=-1;
            }
        }
        else {
            //System.out.println("update scene else condtn............debug..");
            // objModel.rotation().x = 0;
            // objModel.rotation().y = 0;
            //  objModel.rotation().z = 0;
            // clearchache();
            finish();
        }
        // super.updateScene();
    }
    public  static  void clearchache(){

    }
    @Override
    protected void onDestroy() {
        //  Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x03, (byte) 0x00);//
        //  Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disa-able
        //  Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR enaable
        //  Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disa-able
        air_count=0;
        finish();//
        super.onDestroy();
    }
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            Intent a = new Intent(this,ReadSensorData.class);
            a.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(a);
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }


} /* End of LoadObjFile and angle calculation */