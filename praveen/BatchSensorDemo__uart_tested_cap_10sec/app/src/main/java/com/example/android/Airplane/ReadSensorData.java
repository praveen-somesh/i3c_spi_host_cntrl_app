package com.example.android.Airplane;




import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.KeyguardManager;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.media.Rating;
import android.os.AsyncTask;
import android.os.Bundle;

import android.os.Handler;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Toast;

import java.util.Timer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import min3d.Shared;
import min3d.Utils;
import min3d.core.Object3dContainer;
import min3d.parser.IParser;
import min3d.parser.Parser;
import min3d.vos.Light;

public class ReadSensorData extends Activity {

    private int step_counters =0;
    ProgressBar progressBar,progressBar2;
    Button buttonStartProgress;

    private String mDeviceName;

    //�������̹�����
    KeyguardManager km = null;
    //����������
    private KeyguardManager.KeyguardLock kl = null;
    //������Դ������
    private PowerManager pm;
    private PowerManager.WakeLock wl;
    public static int detection_mode_exit = 0;
    private int test_start = 0;
    String status = "IDLE";
    Button btn2,button_Back;
    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";
    //TextView calib_text,done_text;
    private final static String TAG = "ReadSensorData";
    static String str = "";
    int count=0;
    private TextView mDataField;
    public static boolean Connected = false;
    private TextView mConnectionState;
    public int app_start=1;
    public static String mDeviceAddress;
    static String str_temp = "";
    Spinner myspinner;
    public static TextView status_log,text;
    int x1= 0, progress_val = 0, progress_val_old = 0;
    ProgressBar ProgressBar_calib;
    //   public static boolean mConnected = false;
    private int TIME = 10;
    public static int detection_mode = 0;
    public  static  int interrupt=44,btn_calib=0;
    // public static BluetoothLeService mBluetoothLeService;
    public TextView ble_status1;
    public static BluetoothLeService mBluetoothLeService;
    static int update=0;

    private void updateConnectionState(final int resourceId) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(mConnectionState!=null){
                    mConnectionState.setText(resourceId);}

            }
        });
    }
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            mBluetoothLeService.initialize();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            if(mBluetoothLeService.connect(mDeviceAddress)){
                Log.e(TAG, "BL initialization done");
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                //  Log.i("ON GATT CONNECTED 1", "OKK _read");
                Connected = true;
                // Android_Airplane_Activity.mConnected = true;

                updateConnectionState(R.string.connected);
                invalidateOptionsMenu();
                if(app_start!=1){
                    try {
                        Thread.sleep(4000);
                    }catch (InterruptedException e){
                        e.printStackTrace();
                    }
                }
                else
                    app_start=0;
                TextView ble_status=(TextView)findViewById(R.id.ble_status1);
                ble_status.setText("CONNECTED");
                //   Android_Airplane_Activity.state=1;
                //  Toast.makeText(getApplicationContext(),"BLE Connected..",Toast.LENGTH_LONG).show();

                //  Log.i("ON GATT CONNECTED 2", "OKK");
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                Connected = false;
                //    Android_Airplane_Activity.mConnected = false;
                updateConnectionState(R.string.disconnected);
                invalidateOptionsMenu();
                // Android_Airplane_Activity.state=2;
                //Toast.makeText(getApplicationContext(),"BLE DisConnected..",Toast.LENGTH_SHORT).show();
                //detection_mode_exit = 1;
                // clearUI();
                TextView ble_status=(TextView)findViewById(R.id.ble_status1);
                ble_status.setText("DISCONNECTED");
                // Log.i("ON GATT DISCONNECTED", "OKK_read");
            }
            else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.

                //mBluetoothLeService.enableTXNotification();
                //  Log.i("ON GATT SERVICE DISCOVERED", "OK");
                // displayGattServices(mBluetoothLeService.getSupportedGattServices());
                Log.i("ON DISPLAYED SERVICES", "OK");
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                Log.i("ON GATT DATA AVAILABLE", "OK");
                displayData(intent.getStringExtra(BluetoothLeService.EXTRA_DATA));
            }
        }

    };
    public static String replaceBlank(String str) {
        String dest = "";
        if (str!=null) {
            Pattern p = Pattern.compile("\\s*|\t|\r|\n|^\\s*|\\s*$");
            Matcher m = p.matcher(str);
            dest = m.replaceAll("");
        }
        return dest;
    }
    private void displayData(String data) {
        if (data != null) {
            mDataField.setText(data);
        }
        data=replaceBlank(data);
        String[] data_element=data.split("[|]");

        if (data != "" && test_start == 1) {

            // Log.e(TAG, "the display data"+data+"end");
            // Log.e(TAG, "the data length"+data_element.length);

            for(int i=0; i< data_element.length;i++){
                //Log.e(TAG, "the data elements:"+data_element[i]);
            }

            int data_value = Integer.parseInt(data_element[0]);

            if( (data_value & 0x01) != 0 ) {
                step_counters += 1;
                // Log.e(TAG, "stepcounter:"+step_counters);
            }

            if( (data_value & 0x02) != 0 ) {
                step_counters += 4;
                //  Log.e(TAG, "stepcounter:"+step_counters);
            }

            if( (data_value ^ 0x00) == 0 ) {
                status = "IDLE";
            }
            if ((data_value & 0x04) != 0 && (data_value & 0x08) == 0 ) {
                status = "Walking";
            }
            if ((data_value & 0x08) != 0 && (data_value & 0x04) == 0 ) {
                status = "Running";
            }

            if(step_counters == 10)  {
                Log.e("JULIA", "Note::���뻷���ж�");
                wakeAndUnlock(true);
            }

        }

    }
    private void wakeAndUnlock(boolean b)

    {
        if(b)
        {
            //��ȡ��Դ����������
            pm=(PowerManager) getSystemService(Context.POWER_SERVICE);

            Log.e("JULIA", "Get Power Mangere Service");
            //��ȡPowerManager.WakeLock���󣬺���Ĳ���|��ʾͬʱ��������ֵ�������ǵ����õ�Tag
            wl = pm.newWakeLock(PowerManager.ACQUIRE_CAUSES_WAKEUP | PowerManager.SCREEN_DIM_WAKE_LOCK, "bright");
            Log.e("JULIA", "before acquire");
            //������Ļ
            wl.acquire();

            Log.e("JULIA", "��Ļ��������");
            //�õ����������������
            km= (KeyguardManager)getSystemService(Context.KEYGUARD_SERVICE);
            kl = km.newKeyguardLock("unLock");

            //����
            kl.disableKeyguard();
            Log.e("JULIA", "���̽���");
        }
        else
        {
            //����
            kl.reenableKeyguard();

            //�ͷ�wakeLock���ص�
            wl.release();
        }

    }
    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_activity, menu);
        //  menu.findItem(R.id.menu_connect).setVisible(false);

        if (Connected) {
            menu.findItem(R.id.menu_connect).setVisible(false);
            menu.findItem(R.id.menu_disconnect).setVisible(true);
        } else {
            menu.findItem(R.id.menu_connect).setVisible(true);
            menu.findItem(R.id.menu_disconnect).setVisible(false);
        }
        return true;

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch(item.getItemId()) {
            case R.id.menu_connect:
                mBluetoothLeService.connect(ReadSensorData.mDeviceAddress);

                return true;
            case R.id.menu_disconnect:

                mBluetoothLeService.disconnect();
                return true;

            case android.R.id.home:
                onBackPressed();
                return true;

            case R.id.debug_mode:

                final Intent menu_intent = new Intent(this, Airplane_Debug.class);
                startActivity(menu_intent);

                return true;
            case R.id.help:
                final Intent help_intent = new Intent(this, Help.class);
                startActivity(help_intent);
                //    mBluetoothLeService.disconnect();
                return true;

            case R.id.about:
                AlertDialog.Builder builder = new AlertDialog.Builder(this, R.style.Theme_MyDialog);
                builder.setTitle("Application version");
                builder.setMessage("SensorBuffer 1.0.8-B");
                builder.setPositiveButton("OK", null);
                builder.show();
                return  true;
        }
        return super.onOptionsItemSelected(item);
    }
    public class BackgroundAsyncTask extends AsyncTask<Void, Integer, Void> {
        ProgressDialog progress1 = new ProgressDialog(ReadSensorData.this,R.style.Theme_MyDialog);
        //  private final Context mContext;
        int myProgress1;
        Button button;
        TextView status_log= (TextView) findViewById(R.id.textView3);

        @Override
        protected void onPostExecute(Void result) {

            // TODO Auto-generated method stub
            status_log.setText("Calibration is Done Press Airplane Button  ");
            btn2.setClickable(true);
            progress1.dismiss();
            btn2.setVisibility(View.GONE);
            // status_log.setText("Calibration is Done Press Airplane Button  ");
        }

        @Override
        protected void onPreExecute() {
            // TODO Auto-generated method stub
            myProgress1 = 0;
            progress1.setMessage("CALIBRATING SENSOR DATA PLEASE WAIT...");
            progress1.show();
            progress1.setCanceledOnTouchOutside(false);

        }

        @Override
        protected void onCancelled() {

            progress1.dismiss();
        }

        @Override
        protected Void doInBackground(Void... params) {


            LoadObjFile.calibration();

            progress1.dismiss();
            // TODO Auto-generated method stub
            //  while (myProgress1 <1) {

            //    myProgress1++;
            publishProgress(myProgress1);
            //    SystemClock.sleep(10);
            //    }
            return null;
        }
        @Override
        protected void onProgressUpdate(Integer... values) {
            // TODO Auto-generated method stub
            progressBar.setProgress(values[0]);
            status_log.setText("Calibration is Done Press Airplane Button  ");
        }
    }
    public class BackgroundAsyncTask1 extends AsyncTask<Void, Integer, Void> {
        ProgressDialog progress = new ProgressDialog(ReadSensorData.this,R.style.Theme_MyDialog);
        int myProgress;
        Button button;
        @Override
        protected void onPostExecute(Void result) {
            // TODO Auto-generated method stub
            buttonStartProgress.setClickable(true);
            progress.dismiss();
        }
        @Override
        protected void onPreExecute() {
            // TODO Auto-generated method stub
            //capture_accel_data();
            myProgress = 0;
            // progress.setTitle("Proccessing...");
            //if(LoadObjFile.intr_enable==1)
            progress.setMessage("Aligning of Sensors Data Please Wait...  ");
            progress.show();
            progress.setCanceledOnTouchOutside(false);
            //progress.dismiss();
        }
        @Override
        protected Void doInBackground(Void... params) {
            LoadObjFile.read1800bytes_sensor_data();
            // TODO Auto-generated method stub
            // while (myProgress <1) {
            /// myProgress++;
            publishProgress(myProgress);
            ///  SystemClock.sleep(10);
            // }
            if(LoadObjFile.intr_enable==1) {
                Intent i = new Intent(getApplicationContext(), LoadObjFile.class);
                startActivity(i);
            }
            else {

                // Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR ena-able
                // Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disable
                //  Intent j = new Intent(getApplicationContext(), ReadSensorData.class);
                //startActivity(j);
                progress.dismiss();

            }
            return null;
        }
        @Override
        protected void onProgressUpdate(Integer... values) {
            // TODO Auto-generated method stub
            if(LoadObjFile.intr_enable==1) {
                //Toast.makeText(getApplicationContext(), "Interrupt not generated ", Toast.LENGTH_LONG).show();
            }
            else{
                Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x03, (byte) 0x00);//
                Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disa-able
                Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR enaable
                // Toast.makeText(getApplicationContext(), "Interrupt not generated ", Toast.LENGTH_LONG).show();
                Toast toast = Toast.makeText(ReadSensorData.this,"Interrupt not generated ", Toast.LENGTH_LONG);
                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
            }
            progressBar.setProgress(values[0]);
        }
    }
    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        ActionBar actionBar;
        actionBar = getActionBar();
        ColorDrawable colorDrawable = new ColorDrawable(Color.parseColor("#0080ff"));
        actionBar.setBackgroundDrawable(colorDrawable);


        final Intent intent = getIntent();
        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);



        Intent gattServiceIntent = new Intent(this, Android_Airplane_Activity.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);


        setContentView(R.layout.activity_android__airplane_);
        startService(gattServiceIntent);




        setContentView(R.layout.loadobjfile);
        btn2 = (Button) findViewById(R.id.button_calib);
        progressBar2 = (ProgressBar) findViewById(R.id.progressbar_Horizontal2);
        progressBar2.setProgress(0);
        btn2.setOnClickListener(new Button.OnClickListener(){
            @Override
            public void onClick(View v) {
                if(BluetoothLeService.ble_state==1) {
                    progressBar2.setProgress(0);
                    // TODO Auto-generated method stub
                    new BackgroundAsyncTask().execute();
                    btn2.setClickable(false);
                }else {
                    Toast.makeText(getApplicationContext(),"Please Connect The Bluetooth",Toast.LENGTH_SHORT).show(); // commented
                    Toast toast = Toast.makeText(ReadSensorData.this,"No Bluetooth Connection ", Toast.LENGTH_LONG);
                    toast.setGravity(Gravity.CENTER, 0, 0);
                    toast.show();
                }
            }
        });
        buttonStartProgress = (Button) findViewById(R.id.startprogress);

        progressBar = (ProgressBar) findViewById(R.id.progressbar_Horizontal);
        progressBar.setProgress(0);

        buttonStartProgress.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(BluetoothLeService.ble_state==1) {
                    // progressBar.setProgress(0);
                    // TODO Auto-generated method stub
                    new BackgroundAsyncTask1().execute();
                    buttonStartProgress.setClickable(false);
                }else {
                    Toast toast = Toast.makeText(ReadSensorData.this,"No Bluetooth Connection ", Toast.LENGTH_LONG);// commented
                    toast.setGravity(Gravity.CENTER, 0, 0);
                    toast.show();
                    //Toast.makeText(getApplicationContext(),"Please Connect The Bluetooth",Toast.LENGTH_SHORT).show();
                }
            }
        });
        final Button button_Exit=(Button)findViewById(R.id.button_exit);
        button_Exit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //  Intent intent1=new Intent(getApplicationContext(),Android_Airplane_Activity.class);
                //  startActivity(intent1);
                finish();
            }
        });
    }

    //@Override
    //protected void onStart() {
    //     super.onStart();
    //   registerReceiver(mGattUpdateReceiver, new IntentFilter(makeGattUpdateIntentFilter()));
    // }

    @Override
    protected void onPause() {
        super.onPause();

        unregisterReceiver(mGattUpdateReceiver);
    }
    @Override
    protected void onResume() {

        registerReceiver(mGattUpdateReceiver, new IntentFilter(makeGattUpdateIntentFilter()));
        if (mBluetoothLeService != null) {
            // final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            //Log.d(TAG, "Connect request result=" + result);
        }
        super.onResume();
        if(interrupt==5) {
            Log.e(TAG, "on-Resume status");
            // Toast.makeText(getApplicationContext(),"Mapping Sensor_data to airplane Done ",Toast.LENGTH_LONG).show();
            Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x03, (byte) 0x00);//
            Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disa-able
            Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR enaable
        }
        else {
            if (interrupt == 1) {
                // Toast.makeText(getApplicationContext(), "Interrupt not generated ", Toast.LENGTH_LONG).show();
                //TextView text= (TextView) findViewById(R.id.textView4);
                //text.setText("Interrupt not coming press Airplane ");
                // if(ReadSensorData.btn_calib==0){
                //  btn2.setVisibility(View.GONE);
                //}
            }

        }
    }
    @Override
    protected void onDestroy() {
        Log.e(TAG, "on-destroy status");
        Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x03, (byte) 0x00);//dis-able capture
        Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR ena-able
        Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disable
        //  finish();//
        // Intent i=new Intent(getApplication(),ReadSensorData.class);

        //  i.putExtra("android.speech.extra.DICTATION_MODE", false);
        super.onDestroy();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            //final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            //Log.d(TAG, "Connect request result=" + result);
        };
        // unregisterReceiver(mGattUpdateReceiver);
    }
    // super.updateScene();
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_BACK) {
            // do something
            Toast.makeText(getApplicationContext(),"Press Back",
                    Toast.LENGTH_SHORT).show();
            // Intent a = new Intent(this,Android_Airplane_Activity.class);
            // a.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            // startActivity(a);
            // return true;
            return false;

        }

        return super.onKeyDown(keyCode, event);
    }
}





