package com.example.android.Airplane;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;


public class AutoCalibration extends Activity  {
    private SensorManager mSensorManager;
    private Sensor mAccelerometer, mGyroscope, mCompass;
    static int ax, ay, az, gx, gy, gz, i, j, k, done_a, done_g, done_c, kill, accel_x1, accel_y1, accel_z1;
    static int ax_high, ax_low, ay_high, ay_low, az_high, az_low, gx_high, gx_low, gy_high, gy_low, gz_high, gz_low;
    static int ax_prev, ay_prev, az_prev, gx_prev, gy_prev, gz_prev;
    static int roll_right, roll_left, pitch_fwd, pitch_back, yaw_right, yaw_left;
    static float gyro_x1, gyro_y1, gyro_z1, compass_x1, compass_y1, compass_z1, compassx_low, compassx_high, ax1, ay1, az1;
    static int ARRAY_LENGTH1 = 16, sma_x1 = 0, sma_y1 = 0, sma_z1 = 0, s1 = 0, sg1 = 0;
    static int accelx_21, accely_21, accelz_21, result1, result11, accel_roll1, accel_pitch1, accel_yaw1;
    float sma_gyro_x1 = 0, sma_gyro_y1 = 0, sma_gyro_z1 = 0, dt1 = 0, dt21 = 0, gyrox_dt1 = 0, gyroy_dt1 = 0, gyroz_dt1 = 0, gyro_roll_old1 = 0, gyro_pitch_old1 = 0, gyro_yaw_old1 = 0, gyro_roll1 = 0, gyro_pitch1 = 0, gyro_yaw1 = 0;
    long dt11, dt01 ;
    int x= 0, progress_val = 0, progress_val_old = 0;
    ProgressBar ProgressBar_calib;

    float [ ] gyro_x_array1 = new float[ 20 ];
    float [ ] gyro_y_array1 = new float[ 20 ];
    float [ ] gyro_z_array1 = new float[ 20 ];

    Button roll_right_button, roll_left_button, pitch_fwd_button, pitch_back_button, yaw_right_button, yaw_left_button;
    TextView offset_done, roll_right_text, roll_left_text, pitch_fwd_text, pitch_back_text, yaw_right_text, yaw_left_text;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.auto_calib_activity);

        ActionBar actionBar;
        actionBar = getActionBar();
        ColorDrawable colorDrawable = new ColorDrawable(Color.parseColor("#0080ff"));
        actionBar.setBackgroundDrawable(colorDrawable);

        //  Calibration_button = (Button) findViewById(R.id.auto_offset_button);
        roll_right_button = (Button) findViewById(R.id.roll_right_button);
        roll_left_button = (Button) findViewById(R.id.roll_left_button);
        pitch_fwd_button = (Button) findViewById(R.id.pitch_fwd_button);
        pitch_back_button = (Button) findViewById(R.id.pitch_back_button);
        yaw_right_button = (Button) findViewById(R.id.yaw_right_button);
        yaw_left_button = (Button) findViewById(R.id.yaw_left_button);

        //   Calibration_button.setOnClickListener(calibrationbuttonlistener);
        roll_right_button.setOnClickListener(roll_right_button_listener);
        roll_left_button.setOnClickListener(roll_left_button_listener);
        pitch_fwd_button.setOnClickListener(pitch_fwd_button_listener);
        pitch_back_button.setOnClickListener(pitch_back_button_listener);
        yaw_right_button.setOnClickListener(yaw_right_button_listener);
        yaw_left_button.setOnClickListener(yaw_left_button_listener);

        offset_done = (TextView) findViewById(R.id.auto_offset_text);
        roll_right_text = (TextView) findViewById(R.id.roll_right_text);
        roll_left_text = (TextView) findViewById(R.id.roll_left_text);
        pitch_fwd_text = (TextView) findViewById(R.id.pitch_fwd_text);
        pitch_back_text = (TextView) findViewById(R.id.pitch_back_text);
        yaw_right_text = (TextView) findViewById(R.id.yaw_right_text);
        yaw_left_text = (TextView) findViewById(R.id.yaw_left_text);

        ProgressBar_calib = (ProgressBar) findViewById(R.id.calib_progress);


        offset_done.setText("Offset calibration in progress");

    }
    public static void do_calibration() {



        System.out.println("Inside on do calibration java java java.........");

        if (kill == 0){

            System.out.println("if kill==0.....");

            //	if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER){//we are getting ax,ay,az values from ice40aero..class
            {
                //  ax = (int) event.values[0];
                //   ay = (int) event.values[1];
                //   az = (int) event.values[2];

                System.out.println("accel_x in int..java calib...  =: " + ax);
                System.out.println("accel_y in int..java calib....  =: " + ay);
                System.out.println("accel_z in int..java calib....  =: " + az);

                System.out.println(Integer.toHexString(ax));
                System.out.println(Integer.toHexString(ay));
                System.out.println(Integer.toHexString(az));
                i++;
//
                System.out.println("i count   =: " + i);
                if (i== 199){
                    ax_low = ax;
                    ax_high = ax;
                    ay_low = ay;
                    ay_high = ay;
                    az_low = az;
                    az_high = az;
                }
                if (i > 200 && i < 1200){
                    if (ax < ax_low){
                        ax_low = ax;
                    } else if (ax > ax_high){
                        ax_high = ax;
                    }

                    if (ay < ay_low){
                        ay_low = ay;
                    } else if (ay > ay_high){
                        ay_high = ay;
                    }

                    if (az < az_low){
                        az_low = az;
                    } else if (az > az_high){
                        az_high = az;
                    }
                    //	progress_val = (i/10)-20;
                    //	ProgressBar_calib.incrementProgressBy(-progress_val_old);
                    //	ProgressBar_calib.incrementProgressBy(progress_val);
                    //	progress_val_old = progress_val;
                }
                  if (i == 1200){
                LoadObjFile.accelx_offset = (ax_high + ax_low )/2;
                LoadObjFile.accely_offset = (ay_high + ay_low )/2;
                LoadObjFile.accelz_offset = (az_high + az_low )/2;
                LoadObjFile.accelz_offset = LoadObjFile.accelz_offset - 16384;
                done_a = 1;
                 }
            }


            //Offset calculation for gyroscope
            //	if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE)
            {
                //	gx = (int) event.values[0];
                //   gy = (int) event.values[1];
                //   gz = (int) event.values[2];


                System.out.println("gx in int..java calib....  =: " + gx);
                System.out.println("gy in int..java calib....  =: " + gy);
                System.out.println("gz in int..java calib....  =: " + gz);
                System.out.println(Integer.toHexString(gx));
                System.out.println(Integer.toHexString(gy));
                System.out.println(Integer.toHexString(gz));

                j++;
                if (j== 199){
                    gx_low = gx;
                    gx_high = gx;
                    gy_low = gy;
                    gy_high = gy;
                    gz_low = gz;
                    gz_high = gz;
                }
                if (j > 200 && j < 1200){
                    if (gx < gx_low){  // use low
                        gx_low = gx;
                    } else if (gx > gx_high){
                        gx_high = gx;
                    }

                    if (gy < gy_low){
                        gy_low = gy;
                    } else if (gy > gy_high){
                        gy_high = gy;
                    }

                    if (gz < gz_low){
                        gz_low = gz;
                    } else if (gz > gz_high){
                        gz_high = gz;
                    }
                }
                 if (j == 1200){
                LoadObjFile.gyrox_offset = (gx_high + gx_low )/2;
                LoadObjFile.gyroy_offset = (gy_high + gy_low )/2;
                LoadObjFile.gyroz_offset = (gz_high + gz_low )/2;
                done_g = 1;
                  }
            } //  End of gyroscope data

            //	if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
            {
                //	compass_x1 = event.values[0];
                //	compass_y1 = event.values[1];
                //	compass_z1 = event.values[2];

                System.out.println("compass_x1 in float..java calib....  =: " + compass_x1);
                System.out.println("compass_y1 in float..java calib....  =: " + compass_y1);
                System.out.println("compass_z1 in float..java calib....  =: " + compass_z1);

                System.out.println(Float.toHexString(compass_x1));
                System.out.println(Float.toHexString(compass_y1));
                System.out.println(Float.toHexString(compass_z1));

                if ((compass_x1 != ax)  && (compass_x1 != gx)&&(k<501)){											//added k<501
                    if ((compass_x1 > -1000) && (compass_x1 < 1000)){
                        k++;
//					        System.out.println("compass x.................................  =: " + compass_x1);
                        if (k== 199){
                            compassx_low = compass_x1;
                            compassx_high = compass_x1;
                        }
                        if (k > 200 && k < 500){
                            if (compass_x1 < compassx_low){  // use low
                                compassx_low = compass_x1;
                            } else if (compass_x1 > compassx_high){
                                compassx_high = compass_x1;
                            }
                        }
                        if (k == 500){
                            //LoadObjFile.compass_x_calib = (compassx_high + compassx_low )/2;
                            done_c = 1;
                        }
                    } // End of compass < 1000
                }
            } 			// End of compass data

            if (done_a == 1 && done_g == 1 && done_c == 1){
                // Calibration done
                //offset_done.setText("Offset calibration done");
                i = 0;
                j = 0;
                kill = 1;
                System.out.println("Calibration done    done_a=1 done_g=1 and done_c=1.........");

                //   System.out.println("Accelerometer x axis offset ......   =: " + LoadObjFile.accelx_offset);
                //   System.out.println("Accelerometer y axis offset ......   =: " + LoadObjFile.accely_offset);
                //   System.out.println("Accelerometer z axis offset ......   =: " + LoadObjFile.accelz_offset);
                //   System.out.println("Gyroscope x axis offset ......   =: " + LoadObjFile.gyrox_offset);
                //  System.out.println("Gyroscope y axis offset ......   =: " + LoadObjFile.gyroy_offset);
                //   System.out.println("Gyroscope z axis offset ......   =: " + LoadObjFile.gyroz_offset);
                //  System.out.println("compass x recorded.....  =: " + LoadObjFile.compass_x_calib);
            }
            //  System.out.println("i ......   =: " + i);
            //   System.out.println("j ......   =: " + j);
            //  System.out.println("k ......   =: " + k);
            //  System.out.println("Accelerometer x axis offset ......   =: " + LoadObjFile.accelx_offset);
            //  System.out.println("Accelerometer y axis offset ......   =: " + LoadObjFile.accely_offset);
            //  System.out.println("Accelerometer z axis offset ......   =: " + LoadObjFile.accelz_offset);
            //  System.out.println("Gyroscope x axis offset ......   =: " + LoadObjFile.gyrox_offset);
            //  System.out.println("Gyroscope y axis offset ......   =: " + LoadObjFile.gyroy_offset);
            //  System.out.println("Gyroscope z axis offset ......   =: " + LoadObjFile.gyroz_offset);
            //  System.out.println("compass x recorded.....  =: " + LoadObjFile.compass_x_calib);

        }  // End of kill 0 (Offset Calibration)

        //  Multiplication factor calculation for the 6 sides using gyroscope (Currently not used)
        //deleted the code///
    } //End of sensor event listener



    private Button.OnClickListener calibrationbuttonlistener =
            new View.OnClickListener() {
                public void onClick(View v) {


                }
            };

    private Button.OnClickListener roll_right_button_listener =
            new Button.OnClickListener() {
                public void onClick(View v) {
                    roll_right = 1;
                    Toast.makeText(getApplicationContext(),"ON click roll_right",Toast.LENGTH_SHORT).show();

                }
            };

    private Button.OnClickListener roll_left_button_listener =
            new Button.OnClickListener() {
                public void onClick(View v) {
                    roll_left = 1;
                }
            };

    private Button.OnClickListener pitch_fwd_button_listener =
            new Button.OnClickListener() {
                public void onClick(View v) {
                    pitch_fwd = 1;
                }
            };

    private Button.OnClickListener pitch_back_button_listener =
            new Button.OnClickListener() {
                public void onClick(View v) {
                    pitch_back = 1;
                }
            };

    private Button.OnClickListener yaw_right_button_listener =
            new Button.OnClickListener() {
                public void onClick(View v) {
                    yaw_right = 1;
                }
            };

    private Button.OnClickListener yaw_left_button_listener =
            new Button.OnClickListener() {
                public void onClick(View v) {
                    yaw_left = 1;
                }
            };


} /* End of Autocalibration class */

