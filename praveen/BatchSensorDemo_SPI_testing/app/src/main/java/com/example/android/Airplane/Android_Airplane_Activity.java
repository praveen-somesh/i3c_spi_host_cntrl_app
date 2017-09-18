package com.example.android.Airplane;

import android.app.ActionBar;
import android.app.AlertDialog;
import android.app.KeyguardManager;
import android.app.ListActivity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ExpandableListView;
import android.widget.ListView;
import android.widget.SimpleExpandableListAdapter;
import android.widget.TextView;
import android.widget.Toast;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import min3d.core.Object3dContainer;

/**
 * For a given BLE device, this Activity provides the user interface to connect, display data,
 * and display GATT services and characteristics supported by the device.  The Activity
 * communicates with {@code BluetoothLeService}, which in turn interacts with the
 * Bluetooth LE API.
 */
public class Android_Airplane_Activity extends ListActivity {
    //private final static String TAG = Android_Airplane_Activity.class.getSimpleName();

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";
//////////////////////////////////////////////////////////////////////////////////////

    public TextView ble_status;
    private Button bt2;
    private int step_counters =0;
    String status = "IDLE";
    int loop;
    public static TextView status_log;
    public static int detection_mode = 0;

    private final int CONTEXTID_VIEWFILE = 0;
    private final int CONTEXTID_CANCEL = 1;
    static int BLE_MAX_BYTES_PER_TRANSMISSION = 20;
    public int configure_true = 1;
    private final static String TAG = "";
    //private static final String TAG = "Android Airplane Proc Confg";
    public static boolean mConnected = false;
    private TextView mConnectionState;
    private TextView mDataField;
    private String mDeviceName;
    private String mDeviceAddress;
    private ExpandableListView mGattServicesList;
    public static BluetoothLeService mBluetoothLeService;
    // private BluetoothLeService mBluetoothLeService;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
            new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    // private boolean mConnected = false;
    private BluetoothGattCharacteristic mNotifyCharacteristic;
    Button b1;
    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";

    private int TIME = 500;
    private int TIME_1 = 2000;
    private int test_start = 0;
    public int connection_state=0;
    public int app_start=1;

    //�������̹�����
    KeyguardManager km = null;
    //����������
    private KeyguardManager.KeyguardLock kl = null;
    //������Դ������
    private PowerManager pm;
    private PowerManager.WakeLock wl;
    public static int detection_mode_exit = 0;




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
    //@Override
    //public boolean onKeyDown(int keyCode, KeyEvent event) {
    //   if (keyCode == KeyEvent.KEYCODE_BACK) {
    //	Log.d(TAG, "======================Detected Back Button===============================");
    //android.os.Process.killProcess(android.os.Process.myPid());// Kill the app on click of back button.
    //     System.exit(0);			// This kills the app on exit
    //finish();
    // }
    //  return super.onKeyDown(keyCode, event);
    //  }

    // Code to manage Service lifecycle.
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


    // Handles various events fired by the Service.
    // ACTION_GATT_CONNECTED: connected to a GATT server.
    // ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
    // ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
    // ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read
    //                        or notification operations.
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                Log.i("ON GATT CONNECTED 1", "OK");
                mConnected = true;
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
                ble_status=(TextView)findViewById(R.id.ble_status);
                ble_status.setText("BLE CONNECTED");
                //  Toast.makeText(getApplicationContext(),"BLE Connected",Toast.LENGTH_LONG).show();

                Log.i("ON GATT CONNECTED 2", "OK");
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                updateConnectionState(R.string.disconnected);
                invalidateOptionsMenu();
                //  Toast.makeText(getApplicationContext(),"BLE DisConnected",Toast.LENGTH_LONG).show();
                detection_mode_exit = 1;
                ble_status=(TextView)findViewById(R.id.ble_status);
                ble_status.setText("BLE DISCONNECTED");
                // clearUI();
                Log.i("ON GATT DISCONNECTED", "OK");
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.

                mBluetoothLeService.enableTXNotification();
                //Log.i("ON GATT SERVICE DISCOVERED", "OK");
                //   displayGattServices(mBluetoothLeService.getSupportedGattServices());
                Log.i("ON DISPLAYED SERVICES", "OK");
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                Log.i("ON GATT DATA AVAILABLE", "OK");
                displayData(intent.getStringExtra(BluetoothLeService.EXTRA_DATA));
            }
        }
    };

    // If a given GATT characteristic is selected, check for supported features.  This sample
    // demonstrates 'Read' and 'Notify' features.  See
    // http://d.android.com/reference/android/bluetooth/BluetoothGatt.html for the complete
    // list of supported characteristic features.
    /*private final ExpandableListView.OnChildClickListener servicesListClickListner =
            new ExpandableListView.OnChildClickListener() {
                @Override
                public boolean onChildClick(ExpandableListView parent, View v, int groupPosition,
                                            int childPosition, long id) {
                    if (mGattCharacteristics != null) {
                        final BluetoothGattCharacteristic characteristic =
                                mGattCharacteristics.get(groupPosition).get(childPosition);
                        final int charaProp = characteristic.getProperties();
                        if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                            // If there is an active notification on a characteristic, clear
                            // it first so it doesn't update the data field on the user interface.
                            if (mNotifyCharacteristic != null) {
                                mBluetoothLeService.setCharacteristicNotification(
                                        mNotifyCharacteristic, false);
                                mNotifyCharacteristic = null;
                            }
                            mBluetoothLeService.readCharacteristic(characteristic);
                        }
                        if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                            mNotifyCharacteristic = characteristic;
                            mBluetoothLeService.setCharacteristicNotification(
                                    characteristic, true);
                        }
                        return true;
                    }
                    return false;
                }
            };*/

    private void clearUI() {
        mGattServicesList.setAdapter((SimpleExpandableListAdapter) null);
        mDataField.setText(R.string.no_data);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActionBar actionBar;
        actionBar = getActionBar();
        ColorDrawable colorDrawable = new ColorDrawable(Color.parseColor("#0080ff"));
        actionBar.setBackgroundDrawable(colorDrawable);
        Log.e(TAG, "start Airplane Activity Activity");
        final Intent intent = getIntent();
        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);
        //getActionBar().setTitle(mDeviceName);
        // getActionBar().setDisplayHomeAsUpEnabled(true);
        //  ActionBar bar=getActionBar();
        //  ActionBar bar=getActionBar();
        // bar.setBackgroundDrawable(new ColorDrawable("");
        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
        setContentView(R.layout.activity_android__airplane_);
        startService(gattServiceIntent);


        bt2=(Button)findViewById(R.id.button2);

        bt2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(BluetoothLeService.ble_state==1) {
                    //      BluetoothLeService.data_resp=0;//pk
                    int t;
                    //    Android_Airplane_Activity.reg_rd((byte) 0x01, (byte) 0x01, (byte) 0x00);//reading the version register


                    //     long start_time = System.currentTimeMillis(); //fetch starting time
                    //      long wait_time = 250;
                    //      long end_time = start_time + wait_time;

                    //     while (BluetoothLeService.data_resp == 0 && System.currentTimeMillis() < end_time) {
                    //delay(1);
                    //      }
                    //      BluetoothLeService.data_resp = 0;

                    //      byte version;
                    //      version = BluetoothLeService.read_value;
                    //       Log.e(TAG, "debug version register 0x0100 value is:" + version);
                    Intent intent2 = new Intent(getApplicationContext(), ReadSensorData.class);
                    startActivity(intent2);
                }
                else {
                    Toast toast = Toast.makeText(Android_Airplane_Activity.this,"No Bluetooth Connection ", Toast.LENGTH_LONG);
                    toast.setGravity(Gravity.CENTER, 0, 0);
                    toast.show();
                    //Toast.makeText(getApplicationContext(),"Please Connect The Bluetooth",Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
           // final boolean result = mBluetoothLeService.connect(mDeviceAddress);
           // Log.d(TAG, "Connect request result=" + result);
        }else{
            Log.e(TAG, "Outisde of  Activity: onResume");
        }

    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);

        loop=0;
        if(detection_mode==1){
            // Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR enable
        }
        Log.e(TAG, "on-pause status");

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        //indService(mServiceConnection);
        //   mBluetoothLeService = null;

        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
           // final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            //Log.d(TAG, "Connect request result=" + result);
        }
        Android_Airplane_Activity.reg_wr_byte_spi((byte) 0x00, (byte) 0x07, (byte) 0x00);//global GSR disable
        finish();//
        // Android_Airplane_Activity.reg_wr_byte((byte) 0x00, (byte) 0x07, (byte) 0x01);//global GSR disable
        try {
            Thread.sleep(100);
        }catch (Exception e){
            e.printStackTrace();
        }

    }





    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_activity, menu);
        if (mConnected) {
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
                mBluetoothLeService.connect(mDeviceAddress);

                return true;
            case R.id.menu_disconnect:

                mBluetoothLeService.disconnect();
                return true;
            case android.R.id.home:
                onBackPressed();
                return true;
            case R.id.help:
                final Intent help_intent = new Intent(this, Help.class);
                startActivity(help_intent);
                return true;
            case R.id.about:
                //  final Intent about_intent = new Intent(this, Help.class);
                //  startActivity(about_intent);
                AlertDialog.Builder builder = new AlertDialog.Builder(this, R.style.Theme_MyDialog);
                builder.setTitle("Application version");
                builder.setMessage("SensorBuffer 1.0.6 ");

                builder.setPositiveButton("OK", null);

                // builder.setNegativeButton("Cancel", null);
                builder.show();
                return true;
            case R.id.debug_mode:
                final Intent debug_intent = new Intent(this, Airplane_Debug.class);
                startActivity(debug_intent);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void updateConnectionState(final int resourceId) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(mConnectionState!=null){
                    mConnectionState.setText(resourceId);}

            }
        });
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

    }  public static void SendData(byte dataArray[], int var_length)
    {
        byte [] data_array = new byte [var_length];
        Log.e(TAG, "sendata flow \n");

        for (int i = 0; i < var_length; i ++) {
            data_array[i] = dataArray[i];
            Log.e(TAG, String.format("%02x ",data_array[i] ));
        }
        int groupPosition1 = 2;
        int childPosition1 = 1;

        mBluetoothLeService.writeRXCharacteristic(data_array);
    }

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }
    public static String replaceBlank(String str) {
        String dest = "";
        if (str!=null) {
            Pattern p = Pattern.compile("\\s*|\t|\r|\n|^\\s*|\\s*$");
            Matcher m = p.matcher(str);
            dest = m.replaceAll("");
        }
        return dest;
    }

    public void reg_wr(int len, byte addrh, byte addrl, byte data[]) {
        byte [] data_array = new byte [len+5];
        int  i;

        data_array[0] = (byte) 0x00;
        data_array[1] =  addrh;
        data_array[2] =  addrl;
        data_array[3] = (byte) ( (len & 0xff00) >> 8 );
        data_array[4] = (byte) (len & 0xff) ;

        for(i=0; i<len; i++) {
            data_array[5+i] = data[i];
        }

        writedata(data_array, len+5);

    }
    public static void reg_wr_byte( byte addrh, byte addrl, byte data) {
        byte [] data_array = new byte [6];

        data_array[0] = (byte) 0x00;
        data_array[1] =  addrh;
        data_array[2] =  addrl;
        data_array[3] = (byte) 0x00;
        data_array[4] = (byte) 0x01;
        data_array[5] = data;

        writedata(data_array, 6);

    }
    public static void reg_wr_byte_spi( byte addrh, byte addrl, byte data) {
        byte [] data_array = new byte [6];


        data_array[0] =  'W';
        data_array[1] =  'R';
        data_array[2] =  addrh;
        data_array[3] =  addrl;
        data_array[4] = (byte) 0x00;
        data_array[5] = data;

        writedata(data_array, 6);

    }
    public static void reg_wr_fixed_addr(int len, byte addrh, byte addrl, byte data[]) {
        byte [] data_array = new byte [len+5];
        int  i;

        data_array[0] = (byte) 0x02;
        data_array[1] =  addrh;
        data_array[2] =  addrl;
        data_array[3] = (byte) ( (len & 0xff00) >> 8 );
        data_array[4] = (byte) (len & 0xff) ;

        for(i=0; i<len; i++) {
            data_array[5+i] = data[i];
        }

        writedata(data_array, len+5);

    } public static void reg_rd(int len, byte addrh, byte addrl) {
        byte [] data_array = new byte [5];

        data_array[0] = (byte) 0x01;
        data_array[1] =  addrh;
        data_array[2] =  addrl;
        data_array[3] = (byte) ((len & 0xff00) >> 8);
        data_array[4] = (byte) (len & 0xff);


        writedata(data_array, 5);

    }
    public static void reg_rd_spi(byte addrh, byte addrl) {
        byte [] data_array = new byte [7];

        data_array[0] = (char) 'R';
        data_array[1] = (char) 'D';

        data_array[2] =  addrh;
        data_array[3] =  addrl;
        data_array[4] = (byte) 0x03;
        data_array[5] = (byte) 0x00;
        data_array[6] = (byte) 0x00;

        writedata(data_array, 7);

    }

    public static void reg_rd_byte( byte addrh, byte addrl) {
        byte [] data_array = new byte [5];

        data_array[0] = (byte) 0x01;
        data_array[1] =  addrh;
        data_array[2] =  addrl;
        data_array[3] = (byte) (0x00);
        data_array[4] = (byte) (0x01);


        writedata(data_array, 5);

    }

    public static void reg_rd_fixed_addr(int len, byte addrh, byte addrl) {
        byte [] data_array = new byte [5];

        data_array[0] = (byte) 0x0b;//repeated reg group(0x0b)//reg addr (0x03),//auto(0x01)  mode header packet///msd
        data_array[1] =  addrh;
        data_array[2] =  addrl;
        // data_array[3] = (byte) ( (len & 0xff00) >> 8 );
        data_array[3] = (byte)0x20;
        data_array[4] = (byte) (len & 0xff);

        writedata(data_array, 5);

    }
    public static void reg_rd_fixed_addr_spi( byte addrh, byte addrl) {
        byte [] data_array = new byte [20];

        data_array[0] = (char) 'R';
        data_array[1] = (char) 'D';
        data_array[2] =  addrh;
        data_array[3] =  addrl;
        data_array[4] = (byte) 0x0b;//repeated reg group(0x0b)//reg addr (0x03),//auto(0x01)  mode header packet///msd

        for(int i=5;i<20;i++){
            data_array[i] = (byte) 0x00;
        }
        writedata(data_array, 20);
    }

    public static void reg_rd_fixed_addr_spi_remainbytes( byte addrh, byte addrl) {
        byte [] data_array = new byte [14];

        data_array[0] = (char) 'R';
        data_array[1] = (char) 'D';
        data_array[2] =  addrh;
        data_array[3] =  addrl;
        data_array[4] = (byte) 0x0b;//repeated reg group(0x0b)//reg addr (0x03),//auto(0x01)  mode header packet///msd

        for(int i=5;i<14;i++){
            data_array[i] = (byte) 0x00;
        }
        writedata(data_array, 14);
    }


    public static void reg_rd_fixed_addr_begin(int len, byte addrh, byte addrl) {
        byte [] data_array = new byte [5];

        data_array[0] = (byte) 0x03;//repeated reg group(0x0b)//reg addr (0x03),auto(0x01)  mode header packet///msd
        data_array[1] =  addrh;
        data_array[2] =  addrl;
        // data_array[3] = (byte) ( (len & 0xff00) >> 8 );
        data_array[3] = (byte)0x00;     //reg_group size
        data_array[4] = (byte) (len & 0xff);
        writedata(data_array, 5);
    }

    public static void reg(int len, byte addrh, byte addrl) {
        byte [] data_array = new byte [8];

        data_array[0] = (byte) 0xff;
        data_array[1] = (byte) 0xff;
        data_array[2] = (byte) 0x06;
        data_array[3] =  addrh;
        data_array[4] =  addrl;
        data_array[5] = (byte)( (len & 0xff00) >> 8 );
        data_array[6] = (byte) (len & 0xff);
        data_array[7] = (byte) 0x00;

        writedata(data_array, 8);

    }


    public static void reg_wr_burst(int len, byte addrh, byte addrl, byte data[]) { //len = data_num + 7
        byte [] data_array = new byte [len+7];
        int  i;

        data_array[0] = (byte) 0xff;
        data_array[1] = (byte) 0xff;
        data_array[2] = (byte) 0x05;
        data_array[3] =  addrh;
        data_array[4] =  addrl;
        data_array[5] = (byte)( (len & 0xff00) >> 8 );
        data_array[6] = (byte) (len & 0xff);

        for(i=0; i<len; i++) {
            data_array[7+i] = data[i];
        }

        writedata(data_array, len+7);
    }
    public static void writedata(byte data[],int var_length){
        byte [] data_array = new byte [BLE_MAX_BYTES_PER_TRANSMISSION];
        int index;
	    	/* BLE can send maximum of 20 bytes of data per transmission.
	    	 * This part of code sends complete 20 bytes of data. If total data bytes are less
	    	 * than 20 bytes, then data will be sent using if condition below
	    	 */

	    	/* loop count for sending 20 bytes of data */
        int loopCount = var_length / BLE_MAX_BYTES_PER_TRANSMISSION;

        for (int x = 0; x < loopCount; x++) {
            for(int z=0; z<20; z++) {
                index = ((x * BLE_MAX_BYTES_PER_TRANSMISSION) + z);
                data_array[z] = data[index];
//	    		    Log.e(TAG, "write data value "+String.format("%02x ", index)+String.format("%02x \n", data_array[z]));
            }

	    	    /* send the 20 bytes of data */
            SendData(data_array, BLE_MAX_BYTES_PER_TRANSMISSION);

        }


	    	/* To send the remaining bytes if any */
        if ((var_length % BLE_MAX_BYTES_PER_TRANSMISSION) != 0) {

            int remaining_bytes = var_length - (loopCount * BLE_MAX_BYTES_PER_TRANSMISSION);
            for(int z=0; z<remaining_bytes; z++) {
                index = ((loopCount * BLE_MAX_BYTES_PER_TRANSMISSION) + z);
                data_array[z] = data[index];
//	        		Log.e(TAG, "write data value "+String.format("%02x ", index)+String.format("%02x \n", data_array[z]));
            }

	        	/* Send the remaining bytes */
            SendData(data_array, remaining_bytes);
        }
    }
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_BACK) {
            // do something
            Toast.makeText(getApplicationContext(),"Restart The Application",
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